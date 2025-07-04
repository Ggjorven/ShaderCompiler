// Copyright (c) 2020 André Perez Maselco
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

#include "spirv-tools/fuzz/fuzzer_pass_toggle_access_chain_instruction.h"

#include "spirv-tools/fuzz/fuzzer_util.h"
#include "spirv-tools/fuzz/instruction_descriptor.h"
#include "spirv-tools/fuzz/transformation_toggle_access_chain_instruction.h"

namespace spvtools {
namespace fuzz {

FuzzerPassToggleAccessChainInstruction::FuzzerPassToggleAccessChainInstruction(
    opt::IRContext* ir_context, TransformationContext* transformation_context,
    FuzzerContext* fuzzer_context,
    protobufs::TransformationSequence* transformations,
    bool ignore_inapplicable_transformations)
    : FuzzerPass(ir_context, transformation_context, fuzzer_context,
                 transformations, ignore_inapplicable_transformations) {}

void FuzzerPassToggleAccessChainInstruction::Apply() {
  auto context = GetIRContext();
  // Iterates over the module's instructions and checks whether it is
  // OpAccessChain or OpInBoundsAccessChain. In this case, the transformation is
  // probabilistically applied.
  context->module()->ForEachInst([this,
                                  context](opt::Instruction* instruction) {
    spv::Op opcode = instruction->opcode();
    if ((opcode == spv::Op::OpAccessChain ||
         opcode == spv::Op::OpInBoundsAccessChain) &&
        GetFuzzerContext()->ChoosePercentage(
            GetFuzzerContext()->GetChanceOfTogglingAccessChainInstruction())) {
      auto instructionDescriptor =
          MakeInstructionDescriptor(context, instruction);
      auto transformation =
          TransformationToggleAccessChainInstruction(instructionDescriptor);
      ApplyTransformation(transformation);
    }
  });
}

}  // namespace fuzz
}  // namespace spvtools
