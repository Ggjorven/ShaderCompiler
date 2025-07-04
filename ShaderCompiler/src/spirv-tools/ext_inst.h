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

#ifndef SOURCE_EXT_INST_H_
#define SOURCE_EXT_INST_H_

#include "spirv-tools/table.h"
#include "spirv-tools/libspirv.h"

// Gets the type of the extended instruction set with the specified name.
spv_ext_inst_type_t spvExtInstImportTypeGet(const char* name);

// Returns true if the extended instruction set is non-semantic
bool spvExtInstIsNonSemantic(const spv_ext_inst_type_t type);

// Returns true if the extended instruction set is debug info
bool spvExtInstIsDebugInfo(const spv_ext_inst_type_t type);

#endif  // SOURCE_EXT_INST_H_
