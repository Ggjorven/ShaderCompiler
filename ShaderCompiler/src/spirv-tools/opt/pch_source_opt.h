// Copyright (c) 2018 The Khronos Group Inc.
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

#include <algorithm>
#include <functional>
#include <map>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "spirv-tools/opt/basic_block.h"
#include "spirv-tools/opt/decoration_manager.h"
#include "spirv-tools/opt/def_use_manager.h"
#include "spirv-tools/opt/ir_context.h"
#include "spirv-tools/opt/mem_pass.h"
#include "spirv-tools/opt/module.h"
#include "spirv-tools/opt/pass.h"
#include "spirv-tools/util/hex_float.h"
#include "spirv-tools/util/make_unique.h"
