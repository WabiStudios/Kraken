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
#ifndef WABI_IMAGING_HGI_METAL_SHADERPROGRAM_H
#define WABI_IMAGING_HGI_METAL_SHADERPROGRAM_H

#include "wabi/imaging/hgi/shaderProgram.h"

#include "wabi/imaging/hgiMetal/api.h"
#include "wabi/imaging/hgiMetal/shaderFunction.h"

#include <vector>

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

WABI_NAMESPACE_BEGIN


///
/// \class HgiMetalShaderProgram
///
/// Metal implementation of HgiShaderProgram
///
class HgiMetalShaderProgram final : public HgiShaderProgram
{
 public:

  HGIMETAL_API
  ~HgiMetalShaderProgram() override;

  HGIMETAL_API
  bool IsValid() const override;

  HGIMETAL_API
  std::string const &GetCompileErrors() override;

  HGIMETAL_API
  HgiShaderFunctionHandleVector const &GetShaderFunctions() const override;

  HGIMETAL_API
  size_t GetByteSizeOfResource() const override;

  HGIMETAL_API
  uint64_t GetRawResource() const override;

  HGIMETAL_API
  MTL::Function *GetVertexFunction() const
  {
    return _vertexFunction;
  }

  HGIMETAL_API
  MTL::Function *GetFragmentFunction() const
  {
    return _fragmentFunction;
  }

  HGIMETAL_API
  MTL::Function *GetComputeFunction() const
  {
    return _computeFunction;
  }

  HGIMETAL_API
  MTL::Function *GetPostTessVertexFunction() const
  {
    return _postTessVertexFunction;
  }

  HGIMETAL_API
  MTL::Function *GetPostTessControlFunction() const
  {
    return _postTessControlFunction;
  }

 protected:

  friend class HgiMetal;

  HGIMETAL_API
  HgiMetalShaderProgram(HgiShaderProgramDesc const &desc);

 private:

  HgiMetalShaderProgram() = delete;
  HgiMetalShaderProgram &operator=(const HgiMetalShaderProgram &) = delete;
  HgiMetalShaderProgram(const HgiMetalShaderProgram &) = delete;

 private:

  std::string _errors;

  MTL::Function *_vertexFunction;
  MTL::Function *_fragmentFunction;
  MTL::Function *_computeFunction;
  MTL::Function *_postTessVertexFunction;
  MTL::Function *_postTessControlFunction;
};


WABI_NAMESPACE_END

#endif
