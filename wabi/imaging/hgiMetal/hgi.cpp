//
// Copyright 2020 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "wabi/base/arch/defines.h"

#include "wabi/imaging/hgiMetal/hgi.h"
#include "wabi/imaging/hgiMetal/buffer.h"
#include "wabi/imaging/hgiMetal/blitCmds.h"
#include "wabi/imaging/hgiMetal/computeCmds.h"
#include "wabi/imaging/hgiMetal/computePipeline.h"
#include "wabi/imaging/hgiMetal/capabilities.h"
#include "wabi/imaging/hgiMetal/conversions.h"
#include "wabi/imaging/hgiMetal/diagnostic.h"
#include "wabi/imaging/hgiMetal/graphicsCmds.h"
#include "wabi/imaging/hgiMetal/graphicsPipeline.h"
#include "wabi/imaging/hgiMetal/resourceBindings.h"
#include "wabi/imaging/hgiMetal/sampler.h"
#include "wabi/imaging/hgiMetal/shaderFunction.h"
#include "wabi/imaging/hgiMetal/shaderProgram.h"
#include "wabi/imaging/hgiMetal/texture.h"

#include "wabi/base/trace/trace.h"

#include "wabi/base/tf/getenv.h"
#include "wabi/base/tf/registryManager.h"
#include "wabi/base/tf/type.h"

WABI_NAMESPACE_BEGIN

TF_REGISTRY_FUNCTION(TfType)
{
  TfType t = TfType::Define<HgiMetal, TfType::Bases<Hgi>>();
  t.SetFactory<HgiFactory<HgiMetal>>();
}

HgiMetal::HgiMetal(MTL::Device *device)
  : _device(device),
    _currentCmds(nullptr),
    _frameDepth(0),
    _workToFlush(false)
{
  if (!_device) {
    if (TfGetenvBool("HGIMETAL_USE_INTEGRATED_GPU", false)) {
      NS::Array *devices = MTL::CopyAllDevices();
      for (NS::UInteger dix = 0 ; dix < devices->count() ; dix++) {
        MTL::Device *d = devices->object<MTL::Device>(dix);
        if (d->lowPower()) {
          _device = d;
          break;
        }
      }
    }

    if (!_device) {
      _device = MTL::CreateSystemDefaultDevice();
    }
  }

  static int const commandBufferPoolSize = 256;

  _commandQueue = _device->newCommandQueue(commandBufferPoolSize);
  _commandBuffer = _commandQueue->commandBuffer();
  _commandBuffer->retain();

  _capabilities.reset(new HgiMetalCapabilities(_device));

  MTL::ArgumentDescriptor *argumentDescBuffer = MTL::ArgumentDescriptor::alloc()->init();
  argumentDescBuffer->setDataType(MTL::DataTypePointer);
  _argEncoderBuffer = _device->newArgumentEncoder(NS::Array::array(argumentDescBuffer));
  argumentDescBuffer->release();

  MTL::ArgumentDescriptor *argumentDescSampler = MTL::ArgumentDescriptor::alloc()->init();
  argumentDescSampler->setDataType(MTL::DataTypeSampler);
  _argEncoderSampler = _device->newArgumentEncoder(NS::Array::array(argumentDescSampler));
  argumentDescSampler->release();

  MTL::ArgumentDescriptor *argumentDescTexture = MTL::ArgumentDescriptor::alloc()->init();
  argumentDescTexture->setDataType(MTL::DataTypeTexture);
  _argEncoderTexture = _device->newArgumentEncoder(NS::Array::array(argumentDescTexture));
  argumentDescTexture->release();

  HgiMetalSetupMetalDebug();

  _captureScopeFullFrame = MTL::CaptureManager::sharedCaptureManager()->newCaptureScope(_device);
  _captureScopeFullFrame->setLabel(NS::String::string("Full Hydra Frame", NS::UTF8StringEncoding));

  MTL::CaptureManager::sharedCaptureManager()->setDefaultCaptureScope(_captureScopeFullFrame);

#if !__has_feature(objc_arc)
  _pool = nil;
#endif
}

HgiMetal::~HgiMetal()
{
  _commandBuffer->commit();
  _commandBuffer->waitUntilCompleted();
  _commandBuffer->release();
  _captureScopeFullFrame->release();
  _commandQueue->release();
  _argEncoderBuffer->release();
  _argEncoderTexture->release();

  while (_freeArgBuffers.size()) {
    _freeArgBuffers.top()->release();
    _freeArgBuffers.pop();
  }
}

bool HgiMetal::IsBackendSupported() const
{
  // Want Metal 2.0 and Metal Shading Language 2.2 or higher.
#ifdef ARCH_OS_MACOS
  if (NS::ProcessInfo::processInfo()->isOperatingSystemAtLeastVersion(
        NS::OperatingSystemVersion{10, 15, 0})) {
#elif defined(ARCH_OS_IOS)
  if (NS::ProcessInfo::processInfo()->isOperatingSystemAtLeastVersion(
        NS::OperatingSystemVersion{13, 0, 0})) {
#else  /* ARCH_OS_IOS */
  if (false) {
#endif /* ARCH_OS_MACOS */
    // Only support devices with barycentrics.
    return _capabilities->IsSet(HgiDeviceCapabilitiesBitsBuiltinBarycentrics);
  }

  return false;
}

MTL::Device *HgiMetal::GetPrimaryDevice() const
{
  return _device;
}

HgiGraphicsCmdsUniquePtr HgiMetal::CreateGraphicsCmds(HgiGraphicsCmdsDesc const &desc)
{
  HgiMetalGraphicsCmds *gfxCmds(new HgiMetalGraphicsCmds(this, desc));
  return HgiGraphicsCmdsUniquePtr(gfxCmds);
}

HgiComputeCmdsUniquePtr HgiMetal::CreateComputeCmds()
{
  HgiComputeCmds *computeCmds = new HgiMetalComputeCmds(this);
  if (!_currentCmds) {
    _currentCmds = computeCmds;
  }
  return HgiComputeCmdsUniquePtr(computeCmds);
}

HgiBlitCmdsUniquePtr HgiMetal::CreateBlitCmds()
{
  HgiMetalBlitCmds *blitCmds = new HgiMetalBlitCmds(this);
  if (!_currentCmds) {
    _currentCmds = blitCmds;
  }
  return HgiBlitCmdsUniquePtr(blitCmds);
}

HgiTextureHandle HgiMetal::CreateTexture(HgiTextureDesc const &desc)
{
  return HgiTextureHandle(new HgiMetalTexture(this, desc), GetUniqueId());
}

void HgiMetal::DestroyTexture(HgiTextureHandle *texHandle)
{
  _TrashObject(texHandle);
}

HgiTextureViewHandle HgiMetal::CreateTextureView(HgiTextureViewDesc const &desc)
{
  if (!desc.sourceTexture) {
    TF_CODING_ERROR("Source texture is null");
  }

  HgiTextureHandle src = HgiTextureHandle(new HgiMetalTexture(this, desc), GetUniqueId());
  HgiTextureView *view = new HgiTextureView(desc);
  view->SetViewTexture(src);
  return HgiTextureViewHandle(view, GetUniqueId());
}

void HgiMetal::DestroyTextureView(HgiTextureViewHandle *viewHandle)
{
  // Trash the texture inside the view and invalidate the view handle.
  HgiTextureHandle texHandle = (*viewHandle)->GetViewTexture();

  if (_workToFlush) {
    _commandBuffer->addCompletedHandler([texHandle](MTL::CommandBuffer *cmdBuffer) -> void {
      delete texHandle.Get();
    });
  } else {
    _TrashObject(&texHandle);
  }
  (*viewHandle)->SetViewTexture(HgiTextureHandle());
  delete viewHandle->Get();
  *viewHandle = HgiTextureViewHandle();
}

HgiSamplerHandle HgiMetal::CreateSampler(HgiSamplerDesc const &desc)
{
  return HgiSamplerHandle(new HgiMetalSampler(this, desc), GetUniqueId());
}

void HgiMetal::DestroySampler(HgiSamplerHandle *smpHandle)
{
  _TrashObject(smpHandle);
}

HgiBufferHandle HgiMetal::CreateBuffer(HgiBufferDesc const &desc)
{
  return HgiBufferHandle(new HgiMetalBuffer(this, desc), GetUniqueId());
}

void HgiMetal::DestroyBuffer(HgiBufferHandle *bufHandle)
{
  _TrashObject(bufHandle);
}

HgiShaderFunctionHandle HgiMetal::CreateShaderFunction(HgiShaderFunctionDesc const &desc)
{
  return HgiShaderFunctionHandle(new HgiMetalShaderFunction(this, desc), GetUniqueId());
}

void HgiMetal::DestroyShaderFunction(HgiShaderFunctionHandle *shaderFunctionHandle)
{
  _TrashObject(shaderFunctionHandle);
}

HgiShaderProgramHandle HgiMetal::CreateShaderProgram(HgiShaderProgramDesc const &desc)
{
  return HgiShaderProgramHandle(new HgiMetalShaderProgram(desc), GetUniqueId());
}

void HgiMetal::DestroyShaderProgram(HgiShaderProgramHandle *shaderProgramHandle)
{
  _TrashObject(shaderProgramHandle);
}


HgiResourceBindingsHandle HgiMetal::CreateResourceBindings(HgiResourceBindingsDesc const &desc)
{
  return HgiResourceBindingsHandle(new HgiMetalResourceBindings(desc), GetUniqueId());
}

void HgiMetal::DestroyResourceBindings(HgiResourceBindingsHandle *resHandle)
{
  _TrashObject(resHandle);
}

HgiGraphicsPipelineHandle HgiMetal::CreateGraphicsPipeline(HgiGraphicsPipelineDesc const &desc)
{
  return HgiGraphicsPipelineHandle(new HgiMetalGraphicsPipeline(this, desc), GetUniqueId());
}

void HgiMetal::DestroyGraphicsPipeline(HgiGraphicsPipelineHandle *pipeHandle)
{
  _TrashObject(pipeHandle);
}

HgiComputePipelineHandle HgiMetal::CreateComputePipeline(HgiComputePipelineDesc const &desc)
{
  return HgiComputePipelineHandle(new HgiMetalComputePipeline(this, desc), GetUniqueId());
}

void HgiMetal::DestroyComputePipeline(HgiComputePipelineHandle *pipeHandle)
{
  _TrashObject(pipeHandle);
}

TfToken const &HgiMetal::GetAPIName() const
{
  return HgiTokens->Metal;
}

HgiMetalCapabilities const *HgiMetal::GetCapabilities() const
{
  return _capabilities.get();
}

void HgiMetal::StartFrame()
{
#if !__has_feature(objc_arc)
  _pool = NS::AutoreleasePool::alloc()->init();
#endif

  if (_frameDepth++ == 0) {
    _captureScopeFullFrame->beginScope();

    if (MTL::CaptureManager::sharedCaptureManager()->isCapturing()) {
      // We need to grab a new command buffer otherwise the previous one
      // (if it was allocated at the end of the last frame) won't appear in
      // this frame's capture, and it will confuse us!
      CommitPrimaryCommandBuffer(CommitCommandBuffer_NoWait, true);
    }
  }
}

void HgiMetal::EndFrame()
{
  if (--_frameDepth == 0) {
    _captureScopeFullFrame->endScope();
  }

#if !__has_feature(objc_arc)
  if (_pool) {
    _pool->drain();
    _pool = nil;
  }
#endif
}

MTL::CommandQueue *HgiMetal::GetQueue() const
{
  return _commandQueue;
}

MTL::CommandBuffer *HgiMetal::GetPrimaryCommandBuffer(HgiCmds *requester, bool flush)
{
  if (_workToFlush) {
    if (_currentCmds && requester != _currentCmds) {
      return nil;
    }
  }
  if (flush) {
    _workToFlush = true;
  }
  return _commandBuffer;
}

MTL::CommandBuffer *HgiMetal::GetSecondaryCommandBuffer()
{
  MTL::CommandBuffer *commandBuffer = _commandQueue->commandBuffer();
  commandBuffer->retain();
  return commandBuffer;
}

int HgiMetal::GetAPIVersion() const
{
  return GetCapabilities()->GetAPIVersion();
}

void HgiMetal::CommitPrimaryCommandBuffer(CommitCommandBufferWaitType waitType,
                                          bool forceNewBuffer)
{
  if (!_workToFlush && !forceNewBuffer) {
    return;
  }

  CommitSecondaryCommandBuffer(_commandBuffer, waitType);
  _commandBuffer->release();
  _commandBuffer = _commandQueue->commandBuffer();
  _commandBuffer->retain();

  _workToFlush = false;
}

void HgiMetal::CommitSecondaryCommandBuffer(MTL::CommandBuffer *commandBuffer,
                                            CommitCommandBufferWaitType waitType)
{
  commandBuffer->commit();
  if (waitType == CommitCommandBuffer_WaitUntilScheduled) {
    commandBuffer->waitUntilScheduled();
  } else if (waitType == CommitCommandBuffer_WaitUntilCompleted) {
    commandBuffer->waitUntilCompleted();
  }
}

void HgiMetal::ReleaseSecondaryCommandBuffer(MTL::CommandBuffer *commandBuffer)
{
  commandBuffer->release();
}

MTL::ArgumentEncoder *HgiMetal::GetBufferArgumentEncoder() const
{
  return _argEncoderBuffer;
}

MTL::ArgumentEncoder *HgiMetal::GetSamplerArgumentEncoder() const
{
  return _argEncoderSampler;
}

MTL::ArgumentEncoder *HgiMetal::GetTextureArgumentEncoder() const
{
  return _argEncoderTexture;
}

MTL::Buffer *HgiMetal::GetArgBuffer()
{
  MTL::ResourceOptions options = _capabilities->defaultStorageMode;
  MTL::Buffer *buffer;

  {
    std::lock_guard<std::mutex> lock(_freeArgMutex);
    if (_freeArgBuffers.empty()) {
      buffer = _device->newBuffer(4096, options);
    } else {
      buffer = _freeArgBuffers.top();
      _freeArgBuffers.pop();
      memset(buffer->contents(), 0x00, buffer->length());
    }
  }

  if (!_commandBuffer) {
    TF_CODING_ERROR("_commandBuffer is null");
  }

  _commandBuffer->addCompletedHandler([&](MTL::CommandBuffer *cmdBuffer) -> void {
    std::lock_guard<std::mutex> lock(_freeArgMutex);
    _freeArgBuffers.push(buffer);
  });

  return buffer;
}

bool HgiMetal::_SubmitCmds(HgiCmds *cmds, HgiSubmitWaitType wait)
{
  TRACE_FUNCTION();

  if (cmds) {
    _workToFlush = Hgi::_SubmitCmds(cmds, wait);
    if (cmds == _currentCmds) {
      _currentCmds = nullptr;
    }
  }

  return _workToFlush;
}

WABI_NAMESPACE_END
