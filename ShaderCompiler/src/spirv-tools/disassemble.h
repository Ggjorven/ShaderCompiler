// Copyright (c) 2018 Google Inc.
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

#ifndef SOURCE_DISASSEMBLE_H_
#define SOURCE_DISASSEMBLE_H_

#include <ios>
#include <sstream>
#include <string>

#include "spirv-tools/name_mapper.h"
#include "spirv-tools/libspirv.h"

namespace spvtools {

// Decodes the given SPIR-V instruction binary representation to its assembly
// text. The context is inferred from the provided module binary. The options
// parameter is a bit field of spv_binary_to_text_options_t (note: the option
// SPV_BINARY_TO_TEXT_OPTION_PRINT will be ignored). Decoded text will be
// stored into *text. Any error will be written into *diagnostic if diagnostic
// is non-null.
std::string spvInstructionBinaryToText(const spv_target_env env,
                                       const uint32_t* inst_binary,
                                       const size_t inst_word_count,
                                       const uint32_t* binary,
                                       const size_t word_count,
                                       const uint32_t options);

namespace disassemble {

// Shared code with other tools (than the disassembler) that might need to
// output disassembly. An InstructionDisassembler instance converts SPIR-V
// binary for an instruction to its assembly representation.
class InstructionDisassembler {
 public:
  InstructionDisassembler(std::ostream& stream, uint32_t options,
                          NameMapper name_mapper);

  // Emits the assembly header for the module.
  void EmitHeaderSpirv();
  void EmitHeaderVersion(uint32_t version);
  void EmitHeaderGenerator(uint32_t generator);
  void EmitHeaderIdBound(uint32_t id_bound);
  void EmitHeaderSchema(uint32_t schema);

  // Emits the assembly text for the given instruction.
  void EmitInstruction(const spv_parsed_instruction_t& inst,
                       size_t inst_byte_offset);
  // Same as EmitInstruction, but only for block instructions (including
  // OpLabel) and useful for nested indentation.  If nested indentation is not
  // desired, EmitInstruction can still be used for block instructions.
  void EmitInstructionInBlock(const spv_parsed_instruction_t& inst,
                              size_t inst_byte_offset, uint32_t block_indent);

  // Emits a comment between different sections of the module.
  void EmitSectionComment(const spv_parsed_instruction_t& inst,
                          bool& inserted_decoration_space,
                          bool& inserted_debug_space,
                          bool& inserted_type_space);

  // Resets the output color, if color is turned on.
  void ResetColor();
  // Set the output color, if color is turned on.
  void SetGrey();
  void SetBlue();
  void SetYellow();
  void SetRed();
  void SetGreen();

 private:
  void ResetColor(std::ostream& stream) const;
  void SetGrey(std::ostream& stream) const;
  void SetBlue(std::ostream& stream) const;
  void SetYellow(std::ostream& stream) const;
  void SetRed(std::ostream& stream) const;
  void SetGreen(std::ostream& stream) const;

  void EmitInstructionImpl(const spv_parsed_instruction_t& inst,
                           size_t inst_byte_offset, uint32_t block_indent,
                           bool is_in_block);

  // Emits an operand for the given instruction, where the instruction
  // is at offset words from the start of the binary.
  void EmitOperand(std::ostream& stream, const spv_parsed_instruction_t& inst,
                   uint16_t operand_index) const;

  // Emits a mask expression for the given mask word of the specified type.
  void EmitMaskOperand(std::ostream& stream, spv_operand_type_t type,
                       uint32_t word) const;

  // Generate part of the instruction as a comment to be added to
  // |id_comments_|.
  void GenerateCommentForDecoratedId(const spv_parsed_instruction_t& inst);

  std::ostream& stream_;
  const bool print_;  // Should we also print to the standard output stream?
  const bool color_;  // Should we print in colour?
  const int indent_;  // How much to indent. 0 means don't indent
  const bool nested_indent_;     // Whether indentation should indicate nesting
  const int comment_;            // Should we comment the source
  const bool show_byte_offset_;  // Should we print byte offset, in hex?
  spvtools::NameMapper name_mapper_;

  // Some comments are generated as instructions (such as OpDecorate) are
  // visited so that when the instruction with that result id is visited, the
  // comment can be output.
  std::unordered_map<uint32_t, std::ostringstream> id_comments_;
  // Align the comments in consecutive lines for more readability.
  uint32_t last_instruction_comment_alignment_;
};

}  // namespace disassemble
}  // namespace spvtools

#endif  // SOURCE_DISASSEMBLE_H_
