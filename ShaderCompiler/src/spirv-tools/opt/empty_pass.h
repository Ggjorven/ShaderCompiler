// Copyright (c) 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SOURCE_OPT_EMPTY_PASS_H_
#define SOURCE_OPT_EMPTY_PASS_H_

#include "spirv-tools/opt/pass.h"

namespace spvtools {
namespace opt {

// Documented in optimizer.hpp
class EmptyPass : public Pass {
 public:
  EmptyPass() {}

  const char* name() const override { return "empty-pass"; }

  Status Process() override { return Status::SuccessWithoutChange; }
};

}  // namespace opt
}  // namespace spvtools

#endif  // SOURCE_OPT_EMPTY_PASS_H_
