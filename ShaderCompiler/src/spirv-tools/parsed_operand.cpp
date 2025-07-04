// Copyright (c) 2016 Google Inc.
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

// This file contains utility functions for spv_parsed_operand_t.

#include "spirv-tools/parsed_operand.h"

#include <cassert>
#include "spirv-tools/util/hex_float.h"

namespace spvtools {

void EmitNumericLiteral(std::ostream* out, const spv_parsed_instruction_t& inst,
                        const spv_parsed_operand_t& operand) {
  if (operand.type != SPV_OPERAND_TYPE_LITERAL_INTEGER &&
      operand.type != SPV_OPERAND_TYPE_LITERAL_FLOAT &&
      operand.type != SPV_OPERAND_TYPE_TYPED_LITERAL_NUMBER &&
      operand.type != SPV_OPERAND_TYPE_OPTIONAL_LITERAL_INTEGER &&
      operand.type != SPV_OPERAND_TYPE_OPTIONAL_TYPED_LITERAL_INTEGER)
    return;
  if (operand.num_words < 1) return;
  // TODO(dneto): Support more than 64-bits at a time.
  if (operand.num_words > 2) return;

  const uint32_t word = inst.words[operand.offset];
  if (operand.num_words == 1) {
    switch (operand.number_kind) {
      case SPV_NUMBER_SIGNED_INT:
        *out << int32_t(word);
        break;
      case SPV_NUMBER_UNSIGNED_INT:
        *out << word;
        break;
      case SPV_NUMBER_FLOATING:
        switch (operand.fp_encoding) {
          case SPV_FP_ENCODING_IEEE754_BINARY16:
            *out << spvtools::utils::FloatProxy<spvtools::utils::Float16>(
                uint16_t(word & 0xFFFF));
            break;
          case SPV_FP_ENCODING_IEEE754_BINARY32:
            *out << spvtools::utils::FloatProxy<float>(word);
            break;
          case SPV_FP_ENCODING_FLOAT8_E4M3:
            *out << spvtools::utils::FloatProxy<spvtools::utils::Float8_E4M3>(
                uint8_t(word & 0xFF));
            break;
          case SPV_FP_ENCODING_FLOAT8_E5M2:
            *out << spvtools::utils::FloatProxy<spvtools::utils::Float8_E5M2>(
                uint8_t(word & 0xFF));
            break;
          // TODO Bfloat16
          case SPV_FP_ENCODING_UNKNOWN:
            switch (operand.number_bit_width) {
              case 16:
                *out << spvtools::utils::FloatProxy<spvtools::utils::Float16>(
                    uint16_t(word & 0xFFFF));
                break;
              case 32:
                *out << spvtools::utils::FloatProxy<float>(word);
                break;
            }
          default:
            break;
        }
        break;
      default:
        break;
    }
  } else if (operand.num_words == 2) {
    // Multi-word numbers are presented with lower order words first.
    uint64_t bits =
        uint64_t(word) | (uint64_t(inst.words[operand.offset + 1]) << 32);
    switch (operand.number_kind) {
      case SPV_NUMBER_SIGNED_INT:
        *out << int64_t(bits);
        break;
      case SPV_NUMBER_UNSIGNED_INT:
        *out << bits;
        break;
      case SPV_NUMBER_FLOATING:
        // Assume only 64-bit floats.
        *out << spvtools::utils::FloatProxy<double>(bits);
        break;
      default:
        break;
    }
  }
}
}  // namespace spvtools
