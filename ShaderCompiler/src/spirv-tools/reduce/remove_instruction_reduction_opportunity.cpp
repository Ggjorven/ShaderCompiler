// Copyright (c) 2018 Google LLC
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

#include "spirv-tools/reduce/remove_instruction_reduction_opportunity.h"

#include "spirv-tools/opt/ir_context.h"

namespace spvtools {
namespace reduce {

bool RemoveInstructionReductionOpportunity::PreconditionHolds() { return true; }

void RemoveInstructionReductionOpportunity::Apply() {
  const uint32_t kNumEntryPointInOperandsBeforeInterfaceIds = 3;
  for (auto& entry_point : inst_->context()->module()->entry_points()) {
    opt::Instruction::OperandList new_entry_point_in_operands;
    for (uint32_t index = 0; index < entry_point.NumInOperands(); index++) {
      if (index >= kNumEntryPointInOperandsBeforeInterfaceIds &&
          entry_point.GetSingleWordInOperand(index) == inst_->result_id()) {
        continue;
      }
      new_entry_point_in_operands.push_back(entry_point.GetInOperand(index));
    }
    entry_point.SetInOperands(std::move(new_entry_point_in_operands));
  }
  inst_->context()->KillInst(inst_);
}

}  // namespace reduce
}  // namespace spvtools
