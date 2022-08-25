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

// FFmpeg
extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libavutil/avutil.h>
  #include <libavutil/time.h>
  #include <libavutil/opt.h>
  #include <libswscale/swscale.h>
}

// stb
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

// Imgui
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

// Detours
#include <detours/detver.h>
#include <detours/detours.h>

// Modules
#include "../config.h"
#include "../log.h"
#include "../utils.h"
#include "window_manager.h"
#include "hook.h"
#include "video_manager.h"
#include "graphics_manager.h"
#include "overlay.h"
#include "gui.h"
#include "state.h"

#ifndef _WINDLL
  #include "test/dx11.h"
#endif

#ifdef _WINDLL
  #define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

  typedef BOOL(WINAPI* tUnhookWindowsHookEx)(HHOOK);
  LRESULT CALLBACK GetMessageProc(int code, WPARAM wparam, LPARAM lparam);
#endif