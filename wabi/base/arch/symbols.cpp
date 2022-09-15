//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//

#include "wabi/wabi.h"
#include "wabi/base/arch/fileSystem.h"
#include "wabi/base/arch/symbols.h"
#include "wabi/base/arch/defines.h"
#if defined(ARCH_OS_LINUX)
#  include <dlfcn.h>
#elif defined(ARCH_OS_DARWIN)
#  include <dlfcn.h>
#elif defined(ARCH_OS_WINDOWS)
#  include <Windows.h>
#  include <DbgHelp.h>
#  include <Psapi.h>
#  include <vector>
#  include <locale>
#endif

WABI_NAMESPACE_BEGIN

bool ArchGetAddressInfo(void *address,
                        std::string *objectPath,
                        void **baseAddress,
                        std::string *symbolName,
                        void **symbolAddress)
{
#if defined(_GNU_SOURCE) || defined(ARCH_OS_DARWIN)

  Dl_info info;
  if (dladdr(address, &info)) {
    if (objectPath) {
      // The object filename may be a relative path if, for instance,
      // the given address comes from an executable that was invoked
      // with a relative path, or from a shared library that was
      // dlopen'd with a relative path. We want to always return
      // absolute paths, so do the resolution here.
      //
      // This may be incorrect if the current working directory was
      // changed after the source object was loaded.
      *objectPath = ArchAbsPath(info.dli_fname);
    }
    if (baseAddress) {
      *baseAddress = info.dli_fbase;
    }
    if (symbolName) {
      *symbolName = info.dli_sname ? info.dli_sname : "";
    }
    if (symbolAddress) {
      *symbolAddress = info.dli_saddr;
    }
    return true;
  }
  return false;

#elif defined(ARCH_OS_WINDOWS)

  if (!address) {
    return false;
  }

  HMODULE module = nullptr;
  std::string stemp(reinterpret_cast<LPCSTR>(address));
  std::wstring atemp(stemp.begin(), stemp.end());

  if (!::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                             GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(atemp.c_str()),
                           &module)) {
    return false;
  }

  if (objectPath) {
    wchar_t buffer[MAX_PATH] = {0};
    if (GetModuleFileName(module, buffer, MAX_PATH)) {
      int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &buffer[0], sizeof(buffer), NULL, 0, NULL, NULL);
      std::string strTo(sizeNeeded, 0);
      WideCharToMultiByte(CP_UTF8, 0, &buffer[0], sizeof(buffer), &strTo[0], sizeNeeded, NULL, NULL);

      objectPath->assign(strTo.c_str());
    }
  }

  if (baseAddress || symbolName || symbolAddress) {
    DWORD displacement;
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);

    // Symbol
    ULONG64 symBuffer[(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) + sizeof(ULONG64) - 1) /
                      sizeof(ULONG64)];
    SYMBOL_INFO *symbol = (SYMBOL_INFO *)symBuffer;
    symbol->MaxNameLen = MAX_SYM_NAME;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    // Line
    IMAGEHLP_LINE64 line = {0};
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    DWORD64 dwAddress = (DWORD64)address;
    SymFromAddr(process, dwAddress, NULL, symbol);
    if (!SymGetLineFromAddr64(process, dwAddress, &displacement, &line)) {
      return false;
    }

    if (baseAddress) {
      MODULEINFO moduleInfo = {0};
      if (!GetModuleInformation(process, module, &moduleInfo, sizeof(moduleInfo))) {
        return false;
      }
      *baseAddress = moduleInfo.lpBaseOfDll;
    }

    if (symbolName) {
      *symbolName = symbol->Name ? symbol->Name : "";
    }

    if (symbolAddress) {
      *symbolAddress = (void *)symbol->Address;
    }
  }
  return true;

#else

  return false;

#endif
}

WABI_NAMESPACE_END
