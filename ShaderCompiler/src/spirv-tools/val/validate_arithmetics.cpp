// Copyright (c) 2017 Google Inc.
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

// Performs validation of arithmetic instructions.

#include <vector>

#include "spirv-tools/opcode.h"
#include "spirv-tools/val/instruction.h"
#include "spirv-tools/val/validate.h"
#include "spirv-tools/val/validation_state.h"

namespace spvtools {
namespace val {

// Validates correctness of arithmetic instructions.
spv_result_t ArithmeticsPass(ValidationState_t& _, const Instruction* inst) {
  const spv::Op opcode = inst->opcode();
  const uint32_t result_type = inst->type_id();

  switch (opcode) {
    case spv::Op::OpFAdd:
    case spv::Op::OpFSub:
    case spv::Op::OpFMul:
    case spv::Op::OpFDiv:
    case spv::Op::OpFRem:
    case spv::Op::OpFMod:
    case spv::Op::OpFNegate: {
      bool supportsCoopMat =
          (opcode != spv::Op::OpFMul && opcode != spv::Op::OpFRem &&
           opcode != spv::Op::OpFMod);
      bool supportsCoopVec =
          (opcode != spv::Op::OpFRem && opcode != spv::Op::OpFMod);
      if (!_.IsFloatScalarType(result_type) &&
          !_.IsFloatVectorType(result_type) &&
          !(supportsCoopMat && _.IsFloatCooperativeMatrixType(result_type)) &&
          !(opcode == spv::Op::OpFMul &&
            _.IsCooperativeMatrixKHRType(result_type) &&
            _.IsFloatCooperativeMatrixType(result_type)) &&
          !(supportsCoopVec && _.IsFloatCooperativeVectorNVType(result_type)))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected floating scalar or vector type as Result Type: "
               << spvOpcodeString(opcode);

      for (size_t operand_index = 2; operand_index < inst->operands().size();
           ++operand_index) {
        if (supportsCoopVec && _.IsCooperativeVectorNVType(result_type)) {
          const uint32_t type_id = _.GetOperandTypeId(inst, operand_index);
          if (!_.IsCooperativeVectorNVType(type_id)) {
            return _.diag(SPV_ERROR_INVALID_DATA, inst)
                   << "Expected arithmetic operands to be of Result Type: "
                   << spvOpcodeString(opcode) << " operand index "
                   << operand_index;
          }
          spv_result_t ret =
              _.CooperativeVectorDimensionsMatch(inst, type_id, result_type);
          if (ret != SPV_SUCCESS) return ret;
        } else if (supportsCoopMat &&
                   _.IsCooperativeMatrixKHRType(result_type)) {
          const uint32_t type_id = _.GetOperandTypeId(inst, operand_index);
          if (!_.IsCooperativeMatrixKHRType(type_id) ||
              !_.IsFloatCooperativeMatrixType(type_id)) {
            return _.diag(SPV_ERROR_INVALID_DATA, inst)
                   << "Expected arithmetic operands to be of Result Type: "
                   << spvOpcodeString(opcode) << " operand index "
                   << operand_index;
          }
          spv_result_t ret =
              _.CooperativeMatrixShapesMatch(inst, result_type, type_id, false);
          if (ret != SPV_SUCCESS) return ret;
        } else if (_.GetOperandTypeId(inst, operand_index) != result_type)
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Expected arithmetic operands to be of Result Type: "
                 << spvOpcodeString(opcode) << " operand index "
                 << operand_index;
      }
      break;
    }

    case spv::Op::OpUDiv:
    case spv::Op::OpUMod: {
      bool supportsCoopMat = (opcode == spv::Op::OpUDiv);
      bool supportsCoopVec = (opcode == spv::Op::OpUDiv);
      if (!_.IsUnsignedIntScalarType(result_type) &&
          !_.IsUnsignedIntVectorType(result_type) &&
          !(supportsCoopMat &&
            _.IsUnsignedIntCooperativeMatrixType(result_type)) &&
          !(supportsCoopVec &&
            _.IsUnsignedIntCooperativeVectorNVType(result_type)))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected unsigned int scalar or vector type as Result Type: "
               << spvOpcodeString(opcode);

      for (size_t operand_index = 2; operand_index < inst->operands().size();
           ++operand_index) {
        if (supportsCoopVec && _.IsCooperativeVectorNVType(result_type)) {
          const uint32_t type_id = _.GetOperandTypeId(inst, operand_index);
          if (!_.IsCooperativeVectorNVType(type_id)) {
            return _.diag(SPV_ERROR_INVALID_DATA, inst)
                   << "Expected arithmetic operands to be of Result Type: "
                   << spvOpcodeString(opcode) << " operand index "
                   << operand_index;
          }
          spv_result_t ret =
              _.CooperativeVectorDimensionsMatch(inst, type_id, result_type);
          if (ret != SPV_SUCCESS) return ret;
        } else if (supportsCoopMat &&
                   _.IsCooperativeMatrixKHRType(result_type)) {
          const uint32_t type_id = _.GetOperandTypeId(inst, operand_index);
          if (!_.IsCooperativeMatrixKHRType(type_id) ||
              !_.IsUnsignedIntCooperativeMatrixType(type_id)) {
            return _.diag(SPV_ERROR_INVALID_DATA, inst)
                   << "Expected arithmetic operands to be of Result Type: "
                   << spvOpcodeString(opcode) << " operand index "
                   << operand_index;
          }
          spv_result_t ret =
              _.CooperativeMatrixShapesMatch(inst, result_type, type_id, false);
          if (ret != SPV_SUCCESS) return ret;
        } else if (_.GetOperandTypeId(inst, operand_index) != result_type)
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Expected arithmetic operands to be of Result Type: "
                 << spvOpcodeString(opcode) << " operand index "
                 << operand_index;
      }
      break;
    }

    case spv::Op::OpISub:
    case spv::Op::OpIAdd:
    case spv::Op::OpIMul:
    case spv::Op::OpSDiv:
    case spv::Op::OpSMod:
    case spv::Op::OpSRem:
    case spv::Op::OpSNegate: {
      bool supportsCoopMat =
          (opcode != spv::Op::OpIMul && opcode != spv::Op::OpSRem &&
           opcode != spv::Op::OpSMod);
      bool supportsCoopVec =
          (opcode != spv::Op::OpSRem && opcode != spv::Op::OpSMod);
      if (!_.IsIntScalarType(result_type) && !_.IsIntVectorType(result_type) &&
          !(supportsCoopMat && _.IsIntCooperativeMatrixType(result_type)) &&
          !(opcode == spv::Op::OpIMul &&
            _.IsCooperativeMatrixKHRType(result_type) &&
            _.IsIntCooperativeMatrixType(result_type)) &&
          !(supportsCoopVec && _.IsIntCooperativeVectorNVType(result_type)))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected int scalar or vector type as Result Type: "
               << spvOpcodeString(opcode);

      const uint32_t dimension = _.GetDimension(result_type);
      const uint32_t bit_width = _.GetBitWidth(result_type);

      for (size_t operand_index = 2; operand_index < inst->operands().size();
           ++operand_index) {
        const uint32_t type_id = _.GetOperandTypeId(inst, operand_index);

        if (supportsCoopVec && _.IsCooperativeVectorNVType(result_type)) {
          if (!_.IsCooperativeVectorNVType(type_id)) {
            return _.diag(SPV_ERROR_INVALID_DATA, inst)
                   << "Expected arithmetic operands to be of Result Type: "
                   << spvOpcodeString(opcode) << " operand index "
                   << operand_index;
          }
          spv_result_t ret =
              _.CooperativeVectorDimensionsMatch(inst, type_id, result_type);
          if (ret != SPV_SUCCESS) return ret;
        }

        if (supportsCoopMat && _.IsCooperativeMatrixKHRType(result_type)) {
          if (!_.IsCooperativeMatrixKHRType(type_id) ||
              !_.IsIntCooperativeMatrixType(type_id)) {
            return _.diag(SPV_ERROR_INVALID_DATA, inst)
                   << "Expected arithmetic operands to be of Result Type: "
                   << spvOpcodeString(opcode) << " operand index "
                   << operand_index;
          }
          spv_result_t ret =
              _.CooperativeMatrixShapesMatch(inst, result_type, type_id, false);
          if (ret != SPV_SUCCESS) return ret;
        }

        if (!type_id ||
            (!_.IsIntScalarType(type_id) && !_.IsIntVectorType(type_id) &&
             !(supportsCoopMat && _.IsIntCooperativeMatrixType(result_type)) &&
             !(opcode == spv::Op::OpIMul &&
               _.IsCooperativeMatrixKHRType(result_type) &&
               _.IsIntCooperativeMatrixType(result_type)) &&
             !(supportsCoopVec && _.IsIntCooperativeVectorNVType(result_type))))
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Expected int scalar or vector type as operand: "
                 << spvOpcodeString(opcode) << " operand index "
                 << operand_index;

        if (_.GetDimension(type_id) != dimension)
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Expected arithmetic operands to have the same dimension "
                 << "as Result Type: " << spvOpcodeString(opcode)
                 << " operand index " << operand_index;

        if (_.GetBitWidth(type_id) != bit_width)
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Expected arithmetic operands to have the same bit width "
                 << "as Result Type: " << spvOpcodeString(opcode)
                 << " operand index " << operand_index;
      }
      break;
    }

    case spv::Op::OpDot: {
      if (!_.IsFloatScalarType(result_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float scalar type as Result Type: "
               << spvOpcodeString(opcode);

      if (_.IsBfloat16ScalarType(result_type)) {
        if (!_.HasCapability(spv::Capability::BFloat16DotProductKHR)) {
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "OpDot Result Type <id> " << _.getIdName(result_type)
                 << "requires BFloat16DotProductKHR be declared.";
        }
      }

      uint32_t first_vector_num_components = 0;

      for (size_t operand_index = 2; operand_index < inst->operands().size();
           ++operand_index) {
        const uint32_t type_id = _.GetOperandTypeId(inst, operand_index);

        if (!type_id || !_.IsFloatVectorType(type_id))
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Expected float vector as operand: "
                 << spvOpcodeString(opcode) << " operand index "
                 << operand_index;

        const uint32_t component_type = _.GetComponentType(type_id);
        if (component_type != result_type)
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Expected component type to be equal to Result Type: "
                 << spvOpcodeString(opcode) << " operand index "
                 << operand_index;

        const uint32_t num_components = _.GetDimension(type_id);
        if (operand_index == 2) {
          first_vector_num_components = num_components;
        } else if (num_components != first_vector_num_components) {
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Expected operands to have the same number of components: "
                 << spvOpcodeString(opcode);
        }
      }
      break;
    }

    case spv::Op::OpVectorTimesScalar: {
      if (!_.IsFloatVectorType(result_type) &&
          !_.IsFloatCooperativeVectorNVType(result_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float vector type as Result Type: "
               << spvOpcodeString(opcode);

      const uint32_t vector_type_id = _.GetOperandTypeId(inst, 2);
      if (result_type != vector_type_id)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected vector operand type to be equal to Result Type: "
               << spvOpcodeString(opcode);

      const uint32_t component_type = _.GetComponentType(vector_type_id);

      const uint32_t scalar_type_id = _.GetOperandTypeId(inst, 3);
      if (component_type != scalar_type_id)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected scalar operand type to be equal to the component "
               << "type of the vector operand: " << spvOpcodeString(opcode);

      break;
    }

    case spv::Op::OpMatrixTimesScalar: {
      if (!_.IsFloatMatrixType(result_type) &&
          !(_.IsCooperativeMatrixType(result_type)))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float matrix type as Result Type: "
               << spvOpcodeString(opcode);

      const uint32_t matrix_type_id = _.GetOperandTypeId(inst, 2);
      if (result_type != matrix_type_id)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected matrix operand type to be equal to Result Type: "
               << spvOpcodeString(opcode);

      const uint32_t component_type = _.GetComponentType(matrix_type_id);

      const uint32_t scalar_type_id = _.GetOperandTypeId(inst, 3);
      if (component_type != scalar_type_id)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected scalar operand type to be equal to the component "
               << "type of the matrix operand: " << spvOpcodeString(opcode);

      break;
    }

    case spv::Op::OpVectorTimesMatrix: {
      const uint32_t vector_type_id = _.GetOperandTypeId(inst, 2);
      const uint32_t matrix_type_id = _.GetOperandTypeId(inst, 3);

      if (!_.IsFloatVectorType(result_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float vector type as Result Type: "
               << spvOpcodeString(opcode);

      const uint32_t res_component_type = _.GetComponentType(result_type);

      if (!vector_type_id || !_.IsFloatVectorType(vector_type_id))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float vector type as left operand: "
               << spvOpcodeString(opcode);

      if (res_component_type != _.GetComponentType(vector_type_id))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected component types of Result Type and vector to be "
               << "equal: " << spvOpcodeString(opcode);

      uint32_t matrix_num_rows = 0;
      uint32_t matrix_num_cols = 0;
      uint32_t matrix_col_type = 0;
      uint32_t matrix_component_type = 0;
      if (!_.GetMatrixTypeInfo(matrix_type_id, &matrix_num_rows,
                               &matrix_num_cols, &matrix_col_type,
                               &matrix_component_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float matrix type as right operand: "
               << spvOpcodeString(opcode);

      if (res_component_type != matrix_component_type)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected component types of Result Type and matrix to be "
               << "equal: " << spvOpcodeString(opcode);

      if (matrix_num_cols != _.GetDimension(result_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected number of columns of the matrix to be equal to "
               << "Result Type vector size: " << spvOpcodeString(opcode);

      if (matrix_num_rows != _.GetDimension(vector_type_id))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected number of rows of the matrix to be equal to the "
               << "vector operand size: " << spvOpcodeString(opcode);

      break;
    }

    case spv::Op::OpMatrixTimesVector: {
      const uint32_t matrix_type_id = _.GetOperandTypeId(inst, 2);
      const uint32_t vector_type_id = _.GetOperandTypeId(inst, 3);

      if (!_.IsFloatVectorType(result_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float vector type as Result Type: "
               << spvOpcodeString(opcode);

      uint32_t matrix_num_rows = 0;
      uint32_t matrix_num_cols = 0;
      uint32_t matrix_col_type = 0;
      uint32_t matrix_component_type = 0;
      if (!_.GetMatrixTypeInfo(matrix_type_id, &matrix_num_rows,
                               &matrix_num_cols, &matrix_col_type,
                               &matrix_component_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float matrix type as left operand: "
               << spvOpcodeString(opcode);

      if (result_type != matrix_col_type)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected column type of the matrix to be equal to Result "
                  "Type: "
               << spvOpcodeString(opcode);

      if (!vector_type_id || !_.IsFloatVectorType(vector_type_id))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float vector type as right operand: "
               << spvOpcodeString(opcode);

      if (matrix_component_type != _.GetComponentType(vector_type_id))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected component types of the operands to be equal: "
               << spvOpcodeString(opcode);

      if (matrix_num_cols != _.GetDimension(vector_type_id))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected number of columns of the matrix to be equal to the "
               << "vector size: " << spvOpcodeString(opcode);

      break;
    }

    case spv::Op::OpMatrixTimesMatrix: {
      const uint32_t left_type_id = _.GetOperandTypeId(inst, 2);
      const uint32_t right_type_id = _.GetOperandTypeId(inst, 3);

      uint32_t res_num_rows = 0;
      uint32_t res_num_cols = 0;
      uint32_t res_col_type = 0;
      uint32_t res_component_type = 0;
      if (!_.GetMatrixTypeInfo(result_type, &res_num_rows, &res_num_cols,
                               &res_col_type, &res_component_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float matrix type as Result Type: "
               << spvOpcodeString(opcode);

      uint32_t left_num_rows = 0;
      uint32_t left_num_cols = 0;
      uint32_t left_col_type = 0;
      uint32_t left_component_type = 0;
      if (!_.GetMatrixTypeInfo(left_type_id, &left_num_rows, &left_num_cols,
                               &left_col_type, &left_component_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float matrix type as left operand: "
               << spvOpcodeString(opcode);

      uint32_t right_num_rows = 0;
      uint32_t right_num_cols = 0;
      uint32_t right_col_type = 0;
      uint32_t right_component_type = 0;
      if (!_.GetMatrixTypeInfo(right_type_id, &right_num_rows, &right_num_cols,
                               &right_col_type, &right_component_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float matrix type as right operand: "
               << spvOpcodeString(opcode);

      if (!_.IsFloatScalarType(res_component_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float matrix type as Result Type: "
               << spvOpcodeString(opcode);

      if (res_col_type != left_col_type)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected column types of Result Type and left matrix to be "
               << "equal: " << spvOpcodeString(opcode);

      if (res_component_type != right_component_type)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected component types of Result Type and right matrix to "
                  "be "
               << "equal: " << spvOpcodeString(opcode);

      if (res_num_cols != right_num_cols)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected number of columns of Result Type and right matrix "
                  "to "
               << "be equal: " << spvOpcodeString(opcode);

      if (left_num_cols != right_num_rows)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected number of columns of left matrix and number of "
                  "rows "
               << "of right matrix to be equal: " << spvOpcodeString(opcode);

      assert(left_num_rows == res_num_rows);
      break;
    }

    case spv::Op::OpOuterProduct: {
      const uint32_t left_type_id = _.GetOperandTypeId(inst, 2);
      const uint32_t right_type_id = _.GetOperandTypeId(inst, 3);

      uint32_t res_num_rows = 0;
      uint32_t res_num_cols = 0;
      uint32_t res_col_type = 0;
      uint32_t res_component_type = 0;
      if (!_.GetMatrixTypeInfo(result_type, &res_num_rows, &res_num_cols,
                               &res_col_type, &res_component_type))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float matrix type as Result Type: "
               << spvOpcodeString(opcode);

      if (left_type_id != res_col_type)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected column type of Result Type to be equal to the type "
               << "of the left operand: " << spvOpcodeString(opcode);

      if (!right_type_id || !_.IsFloatVectorType(right_type_id))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected float vector type as right operand: "
               << spvOpcodeString(opcode);

      if (res_component_type != _.GetComponentType(right_type_id))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected component types of the operands to be equal: "
               << spvOpcodeString(opcode);

      if (res_num_cols != _.GetDimension(right_type_id))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected number of columns of the matrix to be equal to the "
               << "vector size of the right operand: "
               << spvOpcodeString(opcode);

      break;
    }

    case spv::Op::OpIAddCarry:
    case spv::Op::OpISubBorrow:
    case spv::Op::OpUMulExtended:
    case spv::Op::OpSMulExtended: {
      std::vector<uint32_t> result_types;
      if (!_.GetStructMemberTypes(result_type, &result_types))
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected a struct as Result Type: "
               << spvOpcodeString(opcode);

      if (result_types.size() != 2)
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected Result Type struct to have two members: "
               << spvOpcodeString(opcode);

      if (opcode == spv::Op::OpSMulExtended) {
        if (!_.IsIntScalarType(result_types[0]) &&
            !_.IsIntVectorType(result_types[0]))
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Expected Result Type struct member types to be integer "
                    "scalar "
                 << "or vector: " << spvOpcodeString(opcode);
      } else {
        if (!_.IsUnsignedIntScalarType(result_types[0]) &&
            !_.IsUnsignedIntVectorType(result_types[0]))
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Expected Result Type struct member types to be unsigned "
                 << "integer scalar or vector: " << spvOpcodeString(opcode);
      }

      if (result_types[0] != result_types[1])
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected Result Type struct member types to be identical: "
               << spvOpcodeString(opcode);

      const uint32_t left_type_id = _.GetOperandTypeId(inst, 2);
      const uint32_t right_type_id = _.GetOperandTypeId(inst, 3);

      if (left_type_id != result_types[0] || right_type_id != result_types[0])
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected both operands to be of Result Type member type: "
               << spvOpcodeString(opcode);

      break;
    }

    case spv::Op::OpCooperativeMatrixMulAddNV: {
      const uint32_t D_type_id = _.GetOperandTypeId(inst, 1);
      const uint32_t A_type_id = _.GetOperandTypeId(inst, 2);
      const uint32_t B_type_id = _.GetOperandTypeId(inst, 3);
      const uint32_t C_type_id = _.GetOperandTypeId(inst, 4);

      if (!_.IsCooperativeMatrixNVType(A_type_id)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected cooperative matrix type as A Type: "
               << spvOpcodeString(opcode);
      }
      if (!_.IsCooperativeMatrixNVType(B_type_id)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected cooperative matrix type as B Type: "
               << spvOpcodeString(opcode);
      }
      if (!_.IsCooperativeMatrixNVType(C_type_id)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected cooperative matrix type as C Type: "
               << spvOpcodeString(opcode);
      }
      if (!_.IsCooperativeMatrixNVType(D_type_id)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected cooperative matrix type as Result Type: "
               << spvOpcodeString(opcode);
      }

      const auto A = _.FindDef(A_type_id);
      const auto B = _.FindDef(B_type_id);
      const auto C = _.FindDef(C_type_id);
      const auto D = _.FindDef(D_type_id);

      std::tuple<bool, bool, uint32_t> A_scope, B_scope, C_scope, D_scope,
          A_rows, B_rows, C_rows, D_rows, A_cols, B_cols, C_cols, D_cols;

      A_scope = _.EvalInt32IfConst(A->GetOperandAs<uint32_t>(2));
      B_scope = _.EvalInt32IfConst(B->GetOperandAs<uint32_t>(2));
      C_scope = _.EvalInt32IfConst(C->GetOperandAs<uint32_t>(2));
      D_scope = _.EvalInt32IfConst(D->GetOperandAs<uint32_t>(2));

      A_rows = _.EvalInt32IfConst(A->GetOperandAs<uint32_t>(3));
      B_rows = _.EvalInt32IfConst(B->GetOperandAs<uint32_t>(3));
      C_rows = _.EvalInt32IfConst(C->GetOperandAs<uint32_t>(3));
      D_rows = _.EvalInt32IfConst(D->GetOperandAs<uint32_t>(3));

      A_cols = _.EvalInt32IfConst(A->GetOperandAs<uint32_t>(4));
      B_cols = _.EvalInt32IfConst(B->GetOperandAs<uint32_t>(4));
      C_cols = _.EvalInt32IfConst(C->GetOperandAs<uint32_t>(4));
      D_cols = _.EvalInt32IfConst(D->GetOperandAs<uint32_t>(4));

      const auto notEqual = [](std::tuple<bool, bool, uint32_t> X,
                               std::tuple<bool, bool, uint32_t> Y) {
        return (std::get<1>(X) && std::get<1>(Y) &&
                std::get<2>(X) != std::get<2>(Y));
      };

      if (notEqual(A_scope, B_scope) || notEqual(A_scope, C_scope) ||
          notEqual(A_scope, D_scope) || notEqual(B_scope, C_scope) ||
          notEqual(B_scope, D_scope) || notEqual(C_scope, D_scope)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Cooperative matrix scopes must match: "
               << spvOpcodeString(opcode);
      }

      if (notEqual(A_rows, C_rows) || notEqual(A_rows, D_rows) ||
          notEqual(C_rows, D_rows)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Cooperative matrix 'M' mismatch: "
               << spvOpcodeString(opcode);
      }

      if (notEqual(B_cols, C_cols) || notEqual(B_cols, D_cols) ||
          notEqual(C_cols, D_cols)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Cooperative matrix 'N' mismatch: "
               << spvOpcodeString(opcode);
      }

      if (notEqual(A_cols, B_rows)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Cooperative matrix 'K' mismatch: "
               << spvOpcodeString(opcode);
      }
      break;
    }

    case spv::Op::OpCooperativeMatrixMulAddKHR: {
      const uint32_t D_type_id = _.GetOperandTypeId(inst, 1);
      const uint32_t A_type_id = _.GetOperandTypeId(inst, 2);
      const uint32_t B_type_id = _.GetOperandTypeId(inst, 3);
      const uint32_t C_type_id = _.GetOperandTypeId(inst, 4);

      if (!_.IsCooperativeMatrixAType(A_type_id)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Cooperative matrix type must be A Type: "
               << spvOpcodeString(opcode);
      }
      if (!_.IsCooperativeMatrixBType(B_type_id)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Cooperative matrix type must be B Type: "
               << spvOpcodeString(opcode);
      }
      if (!_.IsCooperativeMatrixAccType(C_type_id)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Cooperative matrix type must be Accumulator Type: "
               << spvOpcodeString(opcode);
      }
      if (!_.IsCooperativeMatrixKHRType(D_type_id)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Expected cooperative matrix type as Result Type: "
               << spvOpcodeString(opcode);
      }

      const auto A = _.FindDef(A_type_id);
      const auto B = _.FindDef(B_type_id);
      const auto C = _.FindDef(C_type_id);
      const auto D = _.FindDef(D_type_id);

      std::tuple<bool, bool, uint32_t> A_scope, B_scope, C_scope, D_scope,
          A_rows, B_rows, C_rows, D_rows, A_cols, B_cols, C_cols, D_cols;

      A_scope = _.EvalInt32IfConst(A->GetOperandAs<uint32_t>(2));
      B_scope = _.EvalInt32IfConst(B->GetOperandAs<uint32_t>(2));
      C_scope = _.EvalInt32IfConst(C->GetOperandAs<uint32_t>(2));
      D_scope = _.EvalInt32IfConst(D->GetOperandAs<uint32_t>(2));

      A_rows = _.EvalInt32IfConst(A->GetOperandAs<uint32_t>(3));
      B_rows = _.EvalInt32IfConst(B->GetOperandAs<uint32_t>(3));
      C_rows = _.EvalInt32IfConst(C->GetOperandAs<uint32_t>(3));
      D_rows = _.EvalInt32IfConst(D->GetOperandAs<uint32_t>(3));

      A_cols = _.EvalInt32IfConst(A->GetOperandAs<uint32_t>(4));
      B_cols = _.EvalInt32IfConst(B->GetOperandAs<uint32_t>(4));
      C_cols = _.EvalInt32IfConst(C->GetOperandAs<uint32_t>(4));
      D_cols = _.EvalInt32IfConst(D->GetOperandAs<uint32_t>(4));

      const auto notEqual = [](std::tuple<bool, bool, uint32_t> X,
                               std::tuple<bool, bool, uint32_t> Y) {
        return (std::get<1>(X) && std::get<1>(Y) &&
                std::get<2>(X) != std::get<2>(Y));
      };

      if (notEqual(A_scope, B_scope) || notEqual(A_scope, C_scope) ||
          notEqual(A_scope, D_scope) || notEqual(B_scope, C_scope) ||
          notEqual(B_scope, D_scope) || notEqual(C_scope, D_scope)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Cooperative matrix scopes must match: "
               << spvOpcodeString(opcode);
      }

      if (notEqual(A_rows, C_rows) || notEqual(A_rows, D_rows) ||
          notEqual(C_rows, D_rows)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Cooperative matrix 'M' mismatch: "
               << spvOpcodeString(opcode);
      }

      if (notEqual(B_cols, C_cols) || notEqual(B_cols, D_cols) ||
          notEqual(C_cols, D_cols)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Cooperative matrix 'N' mismatch: "
               << spvOpcodeString(opcode);
      }

      if (notEqual(A_cols, B_rows)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Cooperative matrix 'K' mismatch: "
               << spvOpcodeString(opcode);
      }
      break;
    }

    case spv::Op::OpCooperativeMatrixReduceNV: {
      if (!_.IsCooperativeMatrixKHRType(result_type)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Result Type must be a cooperative matrix type: "
               << spvOpcodeString(opcode);
      }

      const auto result_comp_type_id =
          _.FindDef(result_type)->GetOperandAs<uint32_t>(1);

      const auto matrix_id = inst->GetOperandAs<uint32_t>(2);
      const auto matrix = _.FindDef(matrix_id);
      const auto matrix_type_id = matrix->type_id();
      if (!_.IsCooperativeMatrixKHRType(matrix_type_id)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Matrix must have a cooperative matrix type: "
               << spvOpcodeString(opcode);
      }
      const auto matrix_type = _.FindDef(matrix_type_id);
      const auto matrix_comp_type_id = matrix_type->GetOperandAs<uint32_t>(1);
      if (matrix_comp_type_id != result_comp_type_id) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Result Type and Matrix type must have the same component "
                  "type: "
               << spvOpcodeString(opcode);
      }
      if (_.FindDef(result_type)->GetOperandAs<uint32_t>(2) !=
          matrix_type->GetOperandAs<uint32_t>(2)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Result Type and Matrix type must have the same scope: "
               << spvOpcodeString(opcode);
      }

      if (!_.IsCooperativeMatrixAccType(result_type)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Result Type must have UseAccumulator: "
               << spvOpcodeString(opcode);
      }
      if (!_.IsCooperativeMatrixAccType(matrix_type_id)) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Matrix type must have UseAccumulator: "
               << spvOpcodeString(opcode);
      }

      const auto reduce_value = inst->GetOperandAs<uint32_t>(3);

      if ((reduce_value &
           uint32_t(
               spv::CooperativeMatrixReduceMask::CooperativeMatrixReduce2x2)) &&
          (reduce_value & uint32_t(spv::CooperativeMatrixReduceMask::Row |
                                   spv::CooperativeMatrixReduceMask::Column))) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Reduce 2x2 must not be used with Row/Column: "
               << spvOpcodeString(opcode);
      }

      std::tuple<bool, bool, uint32_t> result_rows, result_cols, matrix_rows,
          matrix_cols;
      result_rows =
          _.EvalInt32IfConst(_.FindDef(result_type)->GetOperandAs<uint32_t>(3));
      result_cols =
          _.EvalInt32IfConst(_.FindDef(result_type)->GetOperandAs<uint32_t>(4));
      matrix_rows = _.EvalInt32IfConst(matrix_type->GetOperandAs<uint32_t>(3));
      matrix_cols = _.EvalInt32IfConst(matrix_type->GetOperandAs<uint32_t>(4));

      if (reduce_value &
          uint32_t(
              spv::CooperativeMatrixReduceMask::CooperativeMatrixReduce2x2)) {
        if (std::get<1>(result_rows) && std::get<1>(result_cols) &&
            std::get<1>(matrix_rows) && std::get<1>(matrix_cols) &&
            (std::get<2>(result_rows) != std::get<2>(matrix_rows) / 2 ||
             std::get<2>(result_cols) != std::get<2>(matrix_cols) / 2)) {
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "For Reduce2x2, result rows/cols must be half of matrix "
                    "rows/cols: "
                 << spvOpcodeString(opcode);
        }
      }
      if (reduce_value == uint32_t(spv::CooperativeMatrixReduceMask::Row)) {
        if (std::get<1>(result_rows) && std::get<1>(matrix_rows) &&
            std::get<2>(result_rows) != std::get<2>(matrix_rows)) {
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "For ReduceRow, result rows must match matrix rows: "
                 << spvOpcodeString(opcode);
        }
      }
      if (reduce_value == uint32_t(spv::CooperativeMatrixReduceMask::Column)) {
        if (std::get<1>(result_cols) && std::get<1>(matrix_cols) &&
            std::get<2>(result_cols) != std::get<2>(matrix_cols)) {
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "For ReduceColumn, result cols must match matrix cols: "
                 << spvOpcodeString(opcode);
        }
      }

      const auto combine_func_id = inst->GetOperandAs<uint32_t>(4);
      const auto combine_func = _.FindDef(combine_func_id);
      if (!combine_func || combine_func->opcode() != spv::Op::OpFunction) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "CombineFunc must be a function: " << spvOpcodeString(opcode);
      }
      const auto function_type_id = combine_func->GetOperandAs<uint32_t>(3);
      const auto function_type = _.FindDef(function_type_id);
      if (function_type->operands().size() != 4) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "CombineFunc must have two parameters: "
               << spvOpcodeString(opcode);
      }
      for (uint32_t i = 0; i < 3; ++i) {
        // checks return type and two params
        const auto param_type_id = function_type->GetOperandAs<uint32_t>(i + 1);
        if (param_type_id != matrix_comp_type_id) {
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "CombineFunc return type and parameters must match matrix "
                    "component type: "
                 << spvOpcodeString(opcode);
        }
      }

      break;
    }

    default:
      break;
  }

  return SPV_SUCCESS;
}

}  // namespace val
}  // namespace spvtools
