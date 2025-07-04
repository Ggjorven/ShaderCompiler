// Copyright (c) 2019 Google LLC
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

#include "spirv-tools/reduce/remove_selection_reduction_opportunity.h"

#include "spirv-tools/opt/basic_block.h"
#include "spirv-tools/opt/ir_context.h"

namespace spvtools {
namespace reduce {

bool RemoveSelectionReductionOpportunity::PreconditionHolds() { return true; }

void RemoveSelectionReductionOpportunity::Apply() {
  auto merge_instruction = header_block_->GetMergeInst();
  merge_instruction->context()->KillInst(merge_instruction);
}

}  // namespace reduce
}  // namespace spvtools
