#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <d3d11.h>
#include <tchar.h>
#include <algorithm>
#include <intrin.h>

#undef max
#undef min

// DirectXTex
#include "../vendor/include/directxtex/BC.h"
#include "../vendor/include/directxtex/filters.h"
#include "../vendor/include/directxtex/DirectXTexP.h"

// Imgui
#include "../vendor/include/imgui/imgui.h"
#include "../vendor/include/imgui/imgui_impl_win32.h"
#include "../vendor/include/imgui/imgui_impl_dx11.h"

// Detours
#include "../vendor/include/detours/detver.h"
#include "../vendor/include/detours/detours.h"

// Modules
#include "../config.h"
#include "../log.h"
#include "../utils.h"
#include "hook.h"
#include "dx11_detour.h"
#include "overlay.h"
#include "gui.h"
#include "window_proc_hook.h"
#include "window_detour.h"
#include "state.h"
#include "frame.h"

#ifndef _WINDLL
#include "test/dx11.h"
#endif

#ifdef _WINDLL
  #define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

  typedef BOOL(WINAPI* tUnhookWindowsHookEx)(HHOOK);
  LRESULT CALLBACK GetMessageProc(int code, WPARAM wparam, LPARAM lparam);
#endif