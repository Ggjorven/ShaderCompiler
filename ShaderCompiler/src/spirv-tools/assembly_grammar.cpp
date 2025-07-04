// Copyright (c) 2015-2016 The Khronos Group Inc.
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

#include "spirv-tools/assembly_grammar.h"

#include <algorithm>
#include <cassert>
#include <cstring>

#include "spirv-tools/ext_inst.h"
#include "spirv-tools/opcode.h"
#include "spirv-tools/operand.h"
#include "spirv-tools/spirv_target_env.h"
#include "spirv-tools/table.h"
#include "spirv-tools/table2.h"

namespace spvtools {
namespace {

/// @brief Parses a mask expression string for the given operand type.
///
/// A mask expression is a sequence of one or more terms separated by '|',
/// where each term a named enum value for the given type.  No whitespace
/// is permitted.
///
/// On success, the value is written to pValue.
///
/// @param[in] type of the operand
/// @param[in] textValue word of text to be parsed
/// @param[out] pValue where the resulting value is written
///
/// @return result code
spv_result_t spvTextParseMaskOperand(const spv_operand_type_t type,
                                     const char* textValue, uint32_t* pValue) {
  if (textValue == nullptr) return SPV_ERROR_INVALID_TEXT;
  size_t text_length = strlen(textValue);
  if (text_length == 0) return SPV_ERROR_INVALID_TEXT;
  const char* text_end = textValue + text_length;

  // We only support mask expressions in ASCII, so the separator value is a
  // char.
  const char separator = '|';

  // Accumulate the result by interpreting one word at a time, scanning
  // from left to right.
  uint32_t value = 0;
  const char* begin = textValue;  // The left end of the current word.
  const char* end = nullptr;  // One character past the end of the current word.
  do {
    end = std::find(begin, text_end, separator);

    const spvtools::OperandDesc* entry = nullptr;
    if (auto error =
            spvtools::LookupOperand(type, begin, end - begin, &entry)) {
      return error;
    }
    value |= entry->value;

    // Advance to the next word by skipping over the separator.
    begin = end + 1;
  } while (end != text_end);

  *pValue = value;
  return SPV_SUCCESS;
}

// Associates an opcode with its name.
struct SpecConstantOpcodeEntry {
  spv::Op opcode;
  const char* name;
};

// All the opcodes allowed as the operation for OpSpecConstantOp.
// The name does not have the usual "Op" prefix. For example opcode
// spv::Op::IAdd is associated with the name "IAdd".
//
// clang-format off
#define CASE(NAME) { spv::Op::Op##NAME, #NAME }
const SpecConstantOpcodeEntry kOpSpecConstantOpcodes[] = {
    // Conversion
    CASE(SConvert),
    CASE(FConvert),
    CASE(ConvertFToS),
    CASE(ConvertSToF),
    CASE(ConvertFToU),
    CASE(ConvertUToF),
    CASE(UConvert),
    CASE(ConvertPtrToU),
    CASE(ConvertUToPtr),
    CASE(GenericCastToPtr),
    CASE(PtrCastToGeneric),
    CASE(Bitcast),
    CASE(QuantizeToF16),
    // Arithmetic
    CASE(SNegate),
    CASE(Not),
    CASE(IAdd),
    CASE(ISub),
    CASE(IMul),
    CASE(UDiv),
    CASE(SDiv),
    CASE(UMod),
    CASE(SRem),
    CASE(SMod),
    CASE(ShiftRightLogical),
    CASE(ShiftRightArithmetic),
    CASE(ShiftLeftLogical),
    CASE(BitwiseOr),
    CASE(BitwiseAnd),
    CASE(BitwiseXor),
    CASE(FNegate),
    CASE(FAdd),
    CASE(FSub),
    CASE(FMul),
    CASE(FDiv),
    CASE(FRem),
    CASE(FMod),
    // Composite
    CASE(VectorShuffle),
    CASE(CompositeExtract),
    CASE(CompositeInsert),
    // Logical
    CASE(LogicalOr),
    CASE(LogicalAnd),
    CASE(LogicalNot),
    CASE(LogicalEqual),
    CASE(LogicalNotEqual),
    CASE(Select),
    // Comparison
    CASE(IEqual),
    CASE(INotEqual),
    CASE(ULessThan),
    CASE(SLessThan),
    CASE(UGreaterThan),
    CASE(SGreaterThan),
    CASE(ULessThanEqual),
    CASE(SLessThanEqual),
    CASE(UGreaterThanEqual),
    CASE(SGreaterThanEqual),
    // Memory
    CASE(AccessChain),
    CASE(InBoundsAccessChain),
    CASE(PtrAccessChain),
    CASE(InBoundsPtrAccessChain),
    CASE(CooperativeMatrixLengthNV),
    CASE(CooperativeMatrixLengthKHR)
};

// The 60 is determined by counting the opcodes listed in the spec.
static_assert(61 == sizeof(kOpSpecConstantOpcodes)/sizeof(kOpSpecConstantOpcodes[0]),
              "OpSpecConstantOp opcode table is incomplete");
#undef CASE
// clang-format on

const size_t kNumOpSpecConstantOpcodes =
    sizeof(kOpSpecConstantOpcodes) / sizeof(kOpSpecConstantOpcodes[0]);

}  // namespace

CapabilitySet AssemblyGrammar::filterCapsAgainstTargetEnv(
    const spv::Capability* cap_array, uint32_t count) const {
  CapabilitySet cap_set;
  const auto version = spvVersionForTargetEnv(target_env_);
  for (uint32_t i = 0; i < count; ++i) {
    const spvtools::OperandDesc* entry = nullptr;
    if (SPV_SUCCESS ==
        spvtools::LookupOperand(SPV_OPERAND_TYPE_CAPABILITY,
                                static_cast<uint32_t>(cap_array[i]), &entry)) {
      // This token is visible in this environment if it's in an appropriate
      // core version, or it is enabled by a capability or an extension.
      if ((version >= entry->minVersion && version <= entry->lastVersion) ||
          entry->extensions_range.count() > 0u ||
          entry->capabilities_range.count() > 0u) {
        cap_set.insert(cap_array[i]);
      }
    }
  }
  return cap_set;
}

const char* AssemblyGrammar::lookupOperandName(spv_operand_type_t type,
                                               uint32_t operand) const {
  const spvtools::OperandDesc* desc = nullptr;
  if (spvtools::LookupOperand(type, operand, &desc) != SPV_SUCCESS || !desc) {
    return "Unknown";
  }
  return desc->name().data();
}

spv_result_t AssemblyGrammar::lookupSpecConstantOpcode(const char* name,
                                                       spv::Op* opcode) const {
  const auto* last = kOpSpecConstantOpcodes + kNumOpSpecConstantOpcodes;
  const auto* found =
      std::find_if(kOpSpecConstantOpcodes, last,
                   [name](const SpecConstantOpcodeEntry& entry) {
                     return 0 == strcmp(name, entry.name);
                   });
  if (found == last) return SPV_ERROR_INVALID_LOOKUP;
  *opcode = found->opcode;
  return SPV_SUCCESS;
}

spv_result_t AssemblyGrammar::lookupSpecConstantOpcode(spv::Op opcode) const {
  const auto* last = kOpSpecConstantOpcodes + kNumOpSpecConstantOpcodes;
  const auto* found =
      std::find_if(kOpSpecConstantOpcodes, last,
                   [opcode](const SpecConstantOpcodeEntry& entry) {
                     return opcode == entry.opcode;
                   });
  if (found == last) return SPV_ERROR_INVALID_LOOKUP;
  return SPV_SUCCESS;
}

spv_result_t AssemblyGrammar::parseMaskOperand(const spv_operand_type_t type,
                                               const char* textValue,
                                               uint32_t* pValue) const {
  return spvTextParseMaskOperand(type, textValue, pValue);
}

void AssemblyGrammar::pushOperandTypesForMask(
    const spv_operand_type_t type, const uint32_t mask,
    spv_operand_pattern_t* pattern) const {
  spvPushOperandTypesForMask(type, mask, pattern);
}

}  // namespace spvtools
