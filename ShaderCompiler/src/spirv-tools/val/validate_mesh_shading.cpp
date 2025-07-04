// Copyright (c) 2022 The Khronos Group Inc.
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

// Validates ray query instructions from SPV_KHR_ray_query

#include "spirv-tools/opcode.h"
#include "spirv-tools/spirv_target_env.h"
#include "spirv-tools/val/instruction.h"
#include "spirv-tools/val/validate.h"
#include "spirv-tools/val/validation_state.h"

namespace spvtools {
namespace val {

bool IsInterfaceVariable(ValidationState_t& _, const Instruction* inst,
                         spv::ExecutionModel model) {
  bool foundInterface = false;
  for (auto entry_point : _.entry_points()) {
    const auto* models = _.GetExecutionModels(entry_point);
    if (models->find(model) == models->end()) return false;
    for (const auto& desc : _.entry_point_descriptions(entry_point)) {
      for (auto interface : desc.interfaces) {
        if (inst->id() == interface) {
          foundInterface = true;
          break;
        }
      }
    }
  }
  return foundInterface;
}

spv_result_t MeshShadingPass(ValidationState_t& _, const Instruction* inst) {
  const spv::Op opcode = inst->opcode();
  switch (opcode) {
    case spv::Op::OpEmitMeshTasksEXT: {
      _.function(inst->function()->id())
          ->RegisterExecutionModelLimitation(
              [](spv::ExecutionModel model, std::string* message) {
                if (model != spv::ExecutionModel::TaskEXT) {
                  if (message) {
                    *message =
                        "OpEmitMeshTasksEXT requires TaskEXT execution model";
                  }
                  return false;
                }
                return true;
              });

      const uint32_t group_count_x = _.GetOperandTypeId(inst, 0);
      if (!_.IsUnsignedIntScalarType(group_count_x) ||
          _.GetBitWidth(group_count_x) != 32) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Group Count X must be a 32-bit unsigned int scalar";
      }

      const uint32_t group_count_y = _.GetOperandTypeId(inst, 1);
      if (!_.IsUnsignedIntScalarType(group_count_y) ||
          _.GetBitWidth(group_count_y) != 32) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Group Count Y must be a 32-bit unsigned int scalar";
      }

      const uint32_t group_count_z = _.GetOperandTypeId(inst, 2);
      if (!_.IsUnsignedIntScalarType(group_count_z) ||
          _.GetBitWidth(group_count_z) != 32) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Group Count Z must be a 32-bit unsigned int scalar";
      }

      if (inst->operands().size() == 4) {
        const auto payload = _.FindDef(inst->GetOperandAs<uint32_t>(3));
        if (payload->opcode() != spv::Op::OpVariable) {
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Payload must be the result of a OpVariable";
        }
        if (payload->GetOperandAs<spv::StorageClass>(2) !=
            spv::StorageClass::TaskPayloadWorkgroupEXT) {
          return _.diag(SPV_ERROR_INVALID_DATA, inst)
                 << "Payload OpVariable must have a storage class of "
                    "TaskPayloadWorkgroupEXT";
        }
      }
      break;
    }

    case spv::Op::OpSetMeshOutputsEXT: {
      _.function(inst->function()->id())
          ->RegisterExecutionModelLimitation(
              [](spv::ExecutionModel model, std::string* message) {
                if (model != spv::ExecutionModel::MeshEXT) {
                  if (message) {
                    *message =
                        "OpSetMeshOutputsEXT requires MeshEXT execution model";
                  }
                  return false;
                }
                return true;
              });

      const uint32_t vertex_count = _.GetOperandTypeId(inst, 0);
      if (!_.IsUnsignedIntScalarType(vertex_count) ||
          _.GetBitWidth(vertex_count) != 32) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Vertex Count must be a 32-bit unsigned int scalar";
      }

      const uint32_t primitive_count = _.GetOperandTypeId(inst, 1);
      if (!_.IsUnsignedIntScalarType(primitive_count) ||
          _.GetBitWidth(primitive_count) != 32) {
        return _.diag(SPV_ERROR_INVALID_DATA, inst)
               << "Primitive Count must be a 32-bit unsigned int scalar";
      }

      break;
    }

    case spv::Op::OpWritePackedPrimitiveIndices4x8NV: {
      // No validation rules (for the moment).
      break;
    }
    case spv::Op::OpVariable: {
      if (_.HasCapability(spv::Capability::MeshShadingEXT)) {
        bool meshInterfaceVar =
            IsInterfaceVariable(_, inst, spv::ExecutionModel::MeshEXT);
        bool fragInterfaceVar =
            IsInterfaceVariable(_, inst, spv::ExecutionModel::Fragment);

        const spv::StorageClass storage_class =
            inst->GetOperandAs<spv::StorageClass>(2);
        bool storage_output = (storage_class == spv::StorageClass::Output);
        bool storage_input = (storage_class == spv::StorageClass::Input);

        if (_.HasDecoration(inst->id(), spv::Decoration::PerPrimitiveEXT)) {
          if (fragInterfaceVar && !storage_input) {
            return _.diag(SPV_ERROR_INVALID_DATA, inst)
                   << "PerPrimitiveEXT decoration must be applied only to "
                      "variables in the Input Storage Class in the Fragment "
                      "Execution Model.";
          }

          if (meshInterfaceVar && !storage_output) {
            return _.diag(SPV_ERROR_INVALID_DATA, inst)
                   << _.VkErrorID(4336)
                   << "PerPrimitiveEXT decoration must be applied only to "
                      "variables in the Output Storage Class in the "
                      "Storage Class in the MeshEXT Execution Model.";
          }
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
