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

#ifndef SOURCE_OPT_PASSES_H_
#define SOURCE_OPT_PASSES_H_

// A single header to include all passes.

#include "spirv-tools/opt/aggressive_dead_code_elim_pass.h"
#include "spirv-tools/opt/amd_ext_to_khr.h"
#include "spirv-tools/opt/analyze_live_input_pass.h"
#include "spirv-tools/opt/block_merge_pass.h"
#include "spirv-tools/opt/canonicalize_ids_pass.h"
#include "spirv-tools/opt/ccp_pass.h"
#include "spirv-tools/opt/cfg_cleanup_pass.h"
#include "spirv-tools/opt/code_sink.h"
#include "spirv-tools/opt/combine_access_chains.h"
#include "spirv-tools/opt/compact_ids_pass.h"
#include "spirv-tools/opt/convert_to_half_pass.h"
#include "spirv-tools/opt/convert_to_sampled_image_pass.h"
#include "spirv-tools/opt/copy_prop_arrays.h"
#include "spirv-tools/opt/dead_branch_elim_pass.h"
#include "spirv-tools/opt/dead_insert_elim_pass.h"
#include "spirv-tools/opt/dead_variable_elimination.h"
#include "spirv-tools/opt/desc_sroa.h"
#include "spirv-tools/opt/eliminate_dead_constant_pass.h"
#include "spirv-tools/opt/eliminate_dead_functions_pass.h"
#include "spirv-tools/opt/eliminate_dead_io_components_pass.h"
#include "spirv-tools/opt/eliminate_dead_members_pass.h"
#include "spirv-tools/opt/eliminate_dead_output_stores_pass.h"
#include "spirv-tools/opt/empty_pass.h"
#include "spirv-tools/opt/fix_func_call_arguments.h"
#include "spirv-tools/opt/fix_storage_class.h"
#include "spirv-tools/opt/flatten_decoration_pass.h"
#include "spirv-tools/opt/fold_spec_constant_op_and_composite_pass.h"
#include "spirv-tools/opt/freeze_spec_constant_value_pass.h"
#include "spirv-tools/opt/graphics_robust_access_pass.h"
#include "spirv-tools/opt/if_conversion.h"
#include "spirv-tools/opt/inline_exhaustive_pass.h"
#include "spirv-tools/opt/inline_opaque_pass.h"
#include "spirv-tools/opt/interface_var_sroa.h"
#include "spirv-tools/opt/interp_fixup_pass.h"
#include "spirv-tools/opt/invocation_interlock_placement_pass.h"
#include "spirv-tools/opt/licm_pass.h"
#include "spirv-tools/opt/local_access_chain_convert_pass.h"
#include "spirv-tools/opt/local_redundancy_elimination.h"
#include "spirv-tools/opt/local_single_block_elim_pass.h"
#include "spirv-tools/opt/local_single_store_elim_pass.h"
#include "spirv-tools/opt/loop_fission.h"
#include "spirv-tools/opt/loop_fusion_pass.h"
#include "spirv-tools/opt/loop_peeling.h"
#include "spirv-tools/opt/loop_unroller.h"
#include "spirv-tools/opt/loop_unswitch_pass.h"
#include "spirv-tools/opt/merge_return_pass.h"
#include "spirv-tools/opt/modify_maximal_reconvergence.h"
#include "spirv-tools/opt/null_pass.h"
#include "spirv-tools/opt/opextinst_forward_ref_fixup_pass.h"
#include "spirv-tools/opt/private_to_local_pass.h"
#include "spirv-tools/opt/reduce_load_size.h"
#include "spirv-tools/opt/redundancy_elimination.h"
#include "spirv-tools/opt/relax_float_ops_pass.h"
#include "spirv-tools/opt/remove_dontinline_pass.h"
#include "spirv-tools/opt/remove_duplicates_pass.h"
#include "spirv-tools/opt/remove_unused_interface_variables_pass.h"
#include "spirv-tools/opt/replace_desc_array_access_using_var_index.h"
#include "spirv-tools/opt/replace_invalid_opc.h"
#include "spirv-tools/opt/resolve_binding_conflicts_pass.h"
#include "spirv-tools/opt/scalar_replacement_pass.h"
#include "spirv-tools/opt/set_spec_constant_default_value_pass.h"
#include "spirv-tools/opt/simplification_pass.h"
#include "spirv-tools/opt/split_combined_image_sampler_pass.h"
#include "spirv-tools/opt/spread_volatile_semantics.h"
#include "spirv-tools/opt/ssa_rewrite_pass.h"
#include "spirv-tools/opt/strength_reduction_pass.h"
#include "spirv-tools/opt/strip_debug_info_pass.h"
#include "spirv-tools/opt/strip_nonsemantic_info_pass.h"
#include "spirv-tools/opt/struct_packing_pass.h"
#include "spirv-tools/opt/switch_descriptorset_pass.h"
#include "spirv-tools/opt/trim_capabilities_pass.h"
#include "spirv-tools/opt/unify_const_pass.h"
#include "spirv-tools/opt/upgrade_memory_model.h"
#include "spirv-tools/opt/vector_dce.h"
#include "spirv-tools/opt/workaround1209.h"
#include "spirv-tools/opt/wrap_opkill.h"

#endif  // SOURCE_OPT_PASSES_H_
