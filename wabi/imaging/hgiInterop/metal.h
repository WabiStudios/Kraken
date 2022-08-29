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
#ifndef WABI_IMAGING_HGIINTEROP_HGIINTEROPMETAL_H
#define WABI_IMAGING_HGIINTEROP_HGIINTEROPMETAL_H

#include "wabi/imaging/garch/glApi.h"

#include "wabi/imaging/hgiMetal/Foundation/Foundation.hpp"
#include "wabi/imaging/hgiMetal/Metal/Metal.hpp"
#include "wabi/imaging/hgiMetal/QuartzCore/QuartzCore.hpp"

#include "wabi/wabi.h"
#include "wabi/base/gf/vec4i.h"
#include "wabi/imaging/hgi/texture.h"
#include "wabi/imaging/hgiInterop/api.h"


WABI_NAMESPACE_BEGIN

class Hgi;
class HgiMetal;
class VtValue;

/// \class HgiInteropMetal
///
/// Provides Metal/GL interop
///
class HgiInteropMetal final
{
 public:

  HGIINTEROP_API
  HgiInteropMetal(Hgi *hgi);

  HGIINTEROP_API
  ~HgiInteropMetal();

  /// Copy/Present provided color (and optional depth) textures to app.
  HGIINTEROP_API
  void CompositeToInterop(HgiTextureHandle const &color,
                          HgiTextureHandle const &depth,
                          VtValue const &framebuffer,
                          GfVec4i const &compRegion);

 private:

  HgiInteropMetal() = delete;

  enum
  {
    ShaderContextColor,
    ShaderContextColorDepth,

    ShaderContextCount
  };

  struct ShaderContext
  {
    uint32_t program;
    uint32_t vao;
    uint32_t vbo;
    int32_t posAttrib;
    int32_t texAttrib;
    int32_t samplerColorLoc;
    int32_t samplerDepthLoc;
    uint32_t blitTexSizeUniform;
  };

  struct VertexAttribState
  {
    int32_t enabled;
    int32_t size;
    int32_t type;
    int32_t normalized;
    int32_t stride;
    int32_t bufferBinding;
    void *pointer;
  };

  void _BlitToOpenGL(VtValue const &framebuffer, GfVec4i const &compRegion, int shaderIndex);
  void _FreeTransientTextureCacheRefs();
  void _CaptureOpenGlState();
  void _RestoreOpenGlState();
  void _CreateShaderContext(int32_t vertexSource, int32_t fragmentSource, ShaderContext &shader);
  void _SetAttachmentSize(int width, int height);
  void _ValidateGLContext();

  HgiMetal *_hgiMetal;

  MTL::Device *_device;
  MTL::CommandBuffer *_cmdBuffer;

  MTL::Texture *_mtlAliasedColorTexture;
  MTL::Texture *_mtlAliasedDepthRegularFloatTexture;

  MTL::Library *_defaultLibrary;
  MTL::Function *_computeDepthCopyProgram;
  MTL::Function *_computeColorCopyProgram;
  MTL::ComputePipelineState *_computePipelineStateColor;
  MTL::ComputePipelineState *_computePipelineStateDepth;

  MTL::Buffer *_pixelBuffer;
  MTL::Buffer *_depthBuffer;
  MTL::Drawable *_cvmtlTextureCache;
  MTL::Texture *_cvmtlColorTexture;
  MTL::Texture *_cvmtlDepthTexture;

  MTL::Drawable *_cvglTextureCache;
  MTL::Texture *_cvglColorTexture;
  MTL::Texture *_cvglDepthTexture;

  uint32_t _glColorTexture;
  uint32_t _glDepthTexture;

  ShaderContext _shaderProgramContext[ShaderContextCount];

  int32_t _restoreDrawFbo;
  int32_t _restoreVao;
  int32_t _restoreVbo;
  bool _restoreDepthTest;
  bool _restoreDepthWriteMask;
  bool _restoreStencilWriteMask;
  bool _restoreCullFace;
  int32_t _restoreFrontFace;
  int32_t _restoreDepthFunc;
  int32_t _restoreViewport[4];
  bool _restoreblendEnabled;
  int32_t _restoreColorOp;
  int32_t _restoreAlphaOp;
  int32_t _restoreColorSrcFnOp;
  int32_t _restoreAlphaSrcFnOp;
  int32_t _restoreColorDstFnOp;
  int32_t _restoreAlphaDstFnOp;
  bool _restoreAlphaToCoverage;
  int32_t _restorePolygonMode;
  int32_t _restoreActiveTexture;
  int32_t _restoreTexture[2];
  VertexAttribState _restoreVertexAttribState[2];
  int32_t _restoreProgram;
};

WABI_NAMESPACE_END

#endif
