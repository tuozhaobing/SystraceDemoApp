/**
 * Copyright 2004-present, Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "plthooks.h"

#include <stdint.h>
#include <unistd.h>
#include <vector>

using HookId = uintptr_t;

namespace facebook {
namespace plthooks {
namespace hooks {

struct HookInfo {
  HookId out_id;
  uintptr_t got_address;
  void* new_function;
  void* previous_function;
  bool no_chaining;
};

enum HookResult {
  UNKNOWN_HOOK = -2,
  WRONG_HOOK_INFO = -1,
  NEW_HOOK = 1,
  ALREADY_HOOKED_APPENDED = 2,
  REMOVED_STILL_HOOKED = 3,
  REMOVED_TRIVIAL = 4, // only one item left in the run list
  REMOVED_FULLY = 5,
  HOOK_CANNOT_BE_CHAINED = 6,
};

HookResult add(HookInfo&);

/**
 * Only uses new_function and got_address from HookInfo.
 * This call fills previous_function with the *original* value of the slot.
 *
 * If the run list only contains one function and this calls removes it,
 * all knowledge of the hook is erased.
 *
 * Returns REMOVED_STILL_HOOKED if the run list contains more than one item.
 * Returns REMOVED_TRIVIAL if only one item is left in the run list.
 * Returns REMOVED_FULLY if all information about this hook has been removed.
 * Returns WRONG_HOOK_INFO otherwise.
 */
HookResult remove(HookInfo&);

bool is_hooked(uintptr_t got_address);

ssize_t list_size(HookId);

std::vector<void*> get_run_list(HookId id);

/**
 * Hooks all the shared libraries loaded at the time of calling this function.
 * Installs hooks for every {func_name, hook} pair in <functionHooks>,
 * avoiding hooking libs that have been already hooked or that the client
 * doesn't want to hook.
 */
    void hookLoadedLibs(
            std::vector<plt_hook_spec>& functionHooks,
            AllowHookingLibCallback allowHookingCb,
            void* data);

    void unhookLoadedLibs(std::vector<plt_hook_spec>& functionHooks);

} // namespace hooks
} // namespace plthooks
} // namespace facebook
