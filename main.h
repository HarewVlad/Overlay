#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <d3d11.h>
#include <tchar.h>

// Imgui
#include "vendor/include/imgui/imgui.h"
#include "vendor/include/imgui/imgui_impl_win32.h"
#include "vendor/include/imgui/imgui_impl_dx11.h"

// Detours
#include "vendor/include/detours/detver.h"
#include "vendor/include/detours/detours.h"

// Modules
#include "log.h"
#include "utils.h"
#include "hook.h"

#ifndef _WINDLL
// Test enviroments
#include "test/directx11.h"
#endif

#ifdef _WINDLL
  #define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

  typedef BOOL(WINAPI* tUnhookWindowsHookEx)(HHOOK);
  LRESULT CALLBACK GetMessageProc(int code, WPARAM wparam, LPARAM lparam);
#endif