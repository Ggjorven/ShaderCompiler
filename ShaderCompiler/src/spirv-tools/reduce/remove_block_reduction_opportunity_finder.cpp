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

#include "spirv-tools/reduce/remove_block_reduction_opportunity_finder.h"

#include "spirv-tools/reduce/remove_block_reduction_opportunity.h"

namespace spvtools {
namespace reduce {

std::string RemoveBlockReductionOpportunityFinder::GetName() const {
  return "RemoveBlockReductionOpportunityFinder";
}

std::vector<std::unique_ptr<ReductionOpportunity>>
RemoveBlockReductionOpportunityFinder::GetAvailableOpportunities(
    opt::IRContext* context, uint32_t target_function) const {
  std::vector<std::unique_ptr<ReductionOpportunity>> result;

  // Consider every block in every relevant function.
  for (auto* function : GetTargetFunctions(context, target_function)) {
    for (auto bi = function->begin(); bi != function->end(); ++bi) {
      if (IsBlockValidOpportunity(context, function, &bi)) {
        result.push_back(MakeUnique<RemoveBlockReductionOpportunity>(
            context, function, &*bi));
      }
    }
  }
  return result;
}

bool RemoveBlockReductionOpportunityFinder::IsBlockValidOpportunity(
    opt::IRContext* context, opt::Function* function,
    opt::Function::iterator* bi) {
  assert(*bi != function->end() && "Block iterator was out of bounds");

  // Don't remove first block; we don't want to end up with no blocks.
  if (*bi == function->begin()) {
    return false;
  }

  // Don't remove blocks with references.
  if (context->get_def_use_mgr()->NumUsers((*bi)->id()) > 0) {
    return false;
  }

  // Don't remove blocks whose instructions have outside references.
  if (!BlockInstructionsHaveNoOutsideReferences(context, *bi)) {
    return false;
  }

  return true;
}

bool RemoveBlockReductionOpportunityFinder::
    BlockInstructionsHaveNoOutsideReferences(
        opt::IRContext* context, const opt::Function::iterator& bi) {
  // Get all instructions in block.
  std::unordered_set<uint32_t> instructions_in_block;
  for (const opt::Instruction& instruction : *bi) {
    instructions_in_block.insert(instruction.unique_id());
  }

  // For each instruction...
  for (const opt::Instruction& instruction : *bi) {
    // For each use of the instruction...
    bool no_uses_outside_block = context->get_def_use_mgr()->WhileEachUser(
        &instruction, [&instructions_in_block](opt::Instruction* user) -> bool {
          // If the use is in this block, continue (return true). Otherwise, we
          // found an outside use; return false (and stop).
          return instructions_in_block.find(user->unique_id()) !=
                 instructions_in_block.end();
        });

    if (!no_uses_outside_block) {
      return false;
    }
  }

  return true;
}

}  // namespace reduce
}  // namespace spvtools
