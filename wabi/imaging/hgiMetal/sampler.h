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
#ifndef WABI_IMAGING_HGI_METAL_SAMPLER_H
#define WABI_IMAGING_HGI_METAL_SAMPLER_H

#include "wabi/imaging/hgi/sampler.h"

#include "wabi/imaging/hgiMetal/api.h"


WABI_NAMESPACE_BEGIN

class HgiMetal;

///
/// \class HgiMetalSampler
///
/// Metal implementation of HgiSampler
///
class HgiMetalSampler final : public HgiSampler
{
 public:

  HGIMETAL_API
  HgiMetalSampler(HgiMetal *hgi, HgiSamplerDesc const &desc);

  HGIMETAL_API
  ~HgiMetalSampler() override;

  HGIMETAL_API
  uint64_t GetRawResource() const override;

  HGIMETAL_API
  MTL::SamplerState *GetSamplerId() const;

 private:

  HgiMetalSampler() = delete;
  HgiMetalSampler &operator=(const HgiMetalSampler &) = delete;
  HgiMetalSampler(const HgiMetalSampler &) = delete;

 private:

  MTL::SamplerState *_samplerId;
  NS::String *_label;
};


WABI_NAMESPACE_END

#endif
