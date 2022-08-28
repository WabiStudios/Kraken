/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright 2022, Wabi Animation Studios, Ltd. Co.
 */

/**
 * @file
 * ⚓︎ Anchor.
 * Bare Metal.
 */

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#  define _CRT_SECURE_NO_WARNINGS
#endif

#include "ANCHOR_rect.h"
#include "ANCHOR_api.h"
#include "ANCHOR_event.h"
#include "ANCHOR_event_consumer.h"
#include "ANCHOR_event_manager.h"
#include "ANCHOR_system.h"
#include "ANCHOR_window.h"

#ifndef ANCHOR_DEFINE_MATH_OPERATORS
#  define ANCHOR_DEFINE_MATH_OPERATORS
#endif
#include "ANCHOR_internal.h"

// System includes
#include <ctype.h>                         // toupper
#include <stdio.h>                         // vsnprintf, sscanf, printf
#if defined(_MSC_VER) && _MSC_VER <= 1500  // MSVC 2008 or earlier
#  include <stddef.h>                      // intptr_t
#else
#  include <stdint.h>  // intptr_t
#endif

// [Windows] OS specific includes (optional)
#if defined(_WIN32) && defined(ANCHOR_DISABLE_DEFAULT_FILE_FUNCTIONS) && \
  defined(ANCHOR_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS) &&           \
  defined(ANCHOR_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS) && !defined(ANCHOR_DISABLE_WIN32_FUNCTIONS)
#  define ANCHOR_DISABLE_WIN32_FUNCTIONS
#endif
#if defined(_WIN32) && !defined(ANCHOR_DISABLE_WIN32_FUNCTIONS)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  ifndef __MINGW32__
#    include <Windows.h>  // _wfopen, OpenClipboard
#  else
#    include <windows.h>
#  endif
#  if defined(WINAPI_FAMILY) && \
    (WINAPI_FAMILY == WINAPI_FAMILY_APP)  // UWP doesn't have all Win32 functions
#    define ANCHOR_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
#    define ANCHOR_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
#  endif
#endif

// [Apple] OS specific includes
#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#  pragma warning(disable : 4127)  // condition expression is constant
#  pragma warning( \
    disable : 4996)  // 'This function or variable may be unsafe': strcpy, strdup, \
                                               // sprintf, vsnprintf, sscanf, fopen
#  if defined(_MSC_VER) && _MSC_VER >= 1922  // MSVC 2019 16.2 or later
#    pragma warning( \
      disable : 5054)  // operator '|': deprecated between enumerations of different types
#  endif
#  pragma warning(disable : 26451)  // [Static Analyzer] Arithmetic overflow : Using operator \
                                      // 'xxx' on a 4 byte value and then casting the result to a 8 \
                                      // byte value. Cast the value to the wider type before \
                                      // calling operator 'xxx' to avoid overflow(io.2).
#  pragma warning( \
    disable : 26495)  // [Static Analyzer] Variable 'XXX' is uninitialized. Always \
                                      // initialize a member variable (type.6).
#  pragma warning( \
    disable : 26812)  // [Static Analyzer] The enum type 'xxx' is unscoped. Prefer \
                                      // 'enum class' over 'enum' (Enum.3).
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#  if __has_warning("-Wunknown-warning-option")
#    pragma clang diagnostic ignored \
      "-Wunknown-warning-option"  // warning: unknown warning group 'xxx' // not all warnings \
                                    // are known by all Clang versions and they tend to be \
                                    // rename-happy.. so ignoring warnings triggers new warnings \
                                    // on some configuration. Great!
#  endif
#  pragma clang diagnostic ignored "-Wunknown-pragmas"  // warning: unknown warning group 'xxx'
#  pragma clang diagnostic ignored \
    "-Wold-style-cast"  // warning: use of old-style cast // yes, \
                                                          // they are more terse.
#  pragma clang diagnostic ignored \
    "-Wfloat-equal"  // warning: comparing floating point with == or != is unsafe // storing \
                       // and comparing against same constants (typically 0.0f) is ok.
#  pragma clang diagnostic ignored \
    "-Wformat-nonliteral"  // warning: format string is not a string literal            // \
                             // passing non-literal to vsnformat(). yes, user passing incorrect \
                             // format strings can crash the code.
#  pragma clang diagnostic ignored \
    "-Wexit-time-destructors"  // warning: declaration requires an exit-time destructor \
                                 // // exit-time destruction order is undefined. if \
                                 // MemFree() leads to users code that has been disabled \
                                 // before exit it might cause problems. ANCHOR coding \
                                 // style welcomes static/globals.
#  pragma clang diagnostic ignored \
    "-Wglobal-constructors"  // warning: declaration requires a global destructor         // \
                                                          // similar to above, not sure what the exact difference is.
#  pragma clang diagnostic ignored \
    "-Wsign-conversion"  // warning: implicit conversion changes signedness
#  pragma clang diagnostic ignored \
    "-Wformat-pedantic"  // warning: format specifies type 'void *' but the argument has type \
                                                                   // 'xxxx *' // unreasonable, would lead to casting every %p arg to \
                                                                   // void*. probably enabled by -pedantic.
#  pragma clang diagnostic ignored \
    "-Wint-to-void-pointer-cast"  // warning: cast to 'void *' from smaller \
                                                                   // integer type 'int'
#  pragma clang diagnostic ignored \
    "-Wzero-as-null-pointer-constant"  // warning: zero as null pointer constant // some \
                                         // standard header variations use #define NULL 0
#  pragma clang diagnostic ignored \
    "-Wdouble-promotion"  // warning: implicit conversion from 'float' to 'double' when passing \
                            // argument to function  // using printf() is a misery with this as \
                            // C++ va_arg ellipsis changes float to double.
#  pragma clang diagnostic ignored \
    "-Wimplicit-int-float-conversion"  // warning: implicit conversion from 'xxx' to 'float' \
                                         // may lose precision
#elif defined(__GNUC__)
// We disable -Wpragmas because GCC doesn't provide an has_warning equivalent and some
// forks/patches may not following the warning/version association.
#  pragma GCC diagnostic ignored \
    "-Wpragmas"  // warning: unknown option after '#pragma GCC diagnostic' kind
#  pragma GCC diagnostic ignored "-Wunused-function"  // warning: 'xxxx' defined but not used
#  pragma GCC diagnostic ignored \
    "-Wint-to-pointer-cast"  // warning: cast to pointer from integer of different size
#  pragma GCC diagnostic ignored \
    "-Wformat"  // warning: format '%p' expects argument of type 'void*', but \
                                                         // argument 6 has type 'AnchorWindow*'
#  pragma GCC diagnostic ignored \
    "-Wdouble-promotion"  // warning: implicit conversion from 'float' to \
                                                         // 'double' when passing argument to function
#  pragma GCC diagnostic ignored \
    "-Wconversion"  // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#  pragma GCC diagnostic ignored \
    "-Wformat-nonliteral"  // warning: format not a string literal, format string not checked
#  pragma GCC diagnostic ignored \
    "-Wstrict-overflow"  // warning: assuming signed overflow does not occur \
                                                        // when assuming that (X - c) > X is always false
#  pragma GCC diagnostic ignored \
    "-Wclass-memaccess"  // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object \
                           // of type 'xxxx' with no trivial copy-assignment; use assignment or \
                           // value-initialization instead
#endif

// Debug options
#define ANCHOR_DEBUG_NAV_SCORING \
  0  // Display navigation scoring preview when hovering items. Display last moving direction \
                                    // matches when holding CTRL
#define ANCHOR_DEBUG_NAV_RECTS 0  // Display the reference navigation rectangle for each window
#define ANCHOR_DEBUG_INI_SETTINGS \
  0  // Save additional comments in .ini file (particularly helps for Docking, but makes saving \
       // slower)

KRAKEN_NAMESPACE_USING

// When using CTRL+TAB (or Gamepad Square+L/R) we delay the visual a little in order to reduce
// visual noise doing a fast switch.
static const float NAV_WINDOWING_HIGHLIGHT_DELAY =
  0.20f;  // Time before the highlight and screen dimming starts fading in
static const float NAV_WINDOWING_LIST_APPEAR_DELAY =
  0.15f;  // Time before the window list starts to appear

// Window resizing from edges (when io.ConfigWindowsResizeFromEdges = true and
// AnchorBackendFlags_HasMouseCursors is set in io.BackendFlags by backend)
static const float WINDOWS_HOVER_PADDING =
  4.0f;  // Extend outside window for hovering/resizing (maxxed with TouchPadding) and inside
         // windows for borders. Affect FindHoveredWindow().
static const float WINDOWS_RESIZE_FROM_EDGES_FEEDBACK_TIMER =
  0.04f;  // Reduce visual noise by only highlighting the border after a certain time.
static const float WINDOWS_MOUSE_WHEEL_SCROLL_LOCK_TIMER =
  2.00f;  // Lock scrolled window (so it doesn't pick child windows that are scrolling through)
          // for a certain time, unless mouse moved.

//-------------------------------------------------------------------------
// [SECTION] FORWARD DECLARATIONS
//-------------------------------------------------------------------------

static void SetCurrentWindow(AnchorWindow *window);
static void FindHoveredWindow();
static AnchorWindow *CreateNewWindow(const char *name, AnchorWindowFlags flags);
static wabi::GfVec2f CalcNextScrollFromScrollTargetAndClamp(AnchorWindow *window);

static void AddDrawListToDrawData(AnchorVector<AnchorDrawList *> *out_list,
                                  AnchorDrawList *draw_list);
static void AddWindowToSortBuffer(AnchorVector<AnchorWindow *> *out_sorted_windows,
                                  AnchorWindow *window);

// Settings
static void WindowSettingsHandler_ClearAll(AnchorContext *, AnchorSettingsHandler *);
static void *WindowSettingsHandler_ReadOpen(AnchorContext *,
                                            AnchorSettingsHandler *,
                                            const char *name);
static void WindowSettingsHandler_ReadLine(AnchorContext *,
                                           AnchorSettingsHandler *,
                                           void *entry,
                                           const char *line);
static void WindowSettingsHandler_ApplyAll(AnchorContext *, AnchorSettingsHandler *);
static void WindowSettingsHandler_WriteAll(AnchorContext *,
                                           AnchorSettingsHandler *,
                                           AnchorTextBuffer *buf);

// Platform Dependents default implementation for IO functions
static const char *GetClipboardTextFn_DefaultImpl(void *user_data);
static void SetClipboardTextFn_DefaultImpl(void *user_data, const char *text);
static void ImeSetInputScreenPosFn_DefaultImpl(int x, int y);

namespace ANCHOR
{
  // Navigation
  static void NavUpdate();
  static void NavUpdateWindowing();
  static void NavUpdateWindowingOverlay();
  static void NavUpdateMoveResult();
  static void NavUpdateInitResult();
  static float NavUpdatePageUpPageDown();
  static inline void NavUpdateAnyRequestFlag();
  static void NavEndFrame();
  static bool NavScoreItem(AnchorNavItemData *result, AnchorBBox cand);
  static void NavApplyItemToResult(AnchorNavItemData *result,
                                   AnchorWindow *window,
                                   ANCHOR_ID id,
                                   const AnchorBBox &nav_bb_rel);
  static void NavProcessItem(AnchorWindow *window, const AnchorBBox &nav_bb, ANCHOR_ID id);
  static wabi::GfVec2f NavCalcPreferredRefPos();
  static void NavSaveLastChildNavWindowIntoParent(AnchorWindow *nav_window);
  static AnchorWindow *NavRestoreLastChildNavWindow(AnchorWindow *window);
  static void NavRestoreLayer(ANCHORNavLayer layer);
  static int FindWindowFocusIndex(AnchorWindow *window);

  // Error Checking
  static void ErrorCheckNewFrameSanityChecks();
  static void ErrorCheckEndFrameSanityChecks();

  // Misc
  static void UpdateSettings();
  static void UpdateMouseInputs();
  static void UpdateMouseWheel();
  static void UpdateTabFocus();
  static void UpdateDebugToolItemPicker();
  static bool UpdateWindowManualResize(AnchorWindow *window,
                                       const wabi::GfVec2f &size_auto_fit,
                                       int *border_held,
                                       int resize_grip_count,
                                       AnchorU32 resize_grip_col[4],
                                       const AnchorBBox &visibility_rect);
  static void RenderWindowOuterBorders(AnchorWindow *window);
  static void RenderWindowDecorations(AnchorWindow *window,
                                      const AnchorBBox &title_bar_rect,
                                      bool title_bar_is_highlight,
                                      int resize_grip_count,
                                      const AnchorU32 resize_grip_col[4],
                                      float resize_grip_draw_size);
  static void RenderWindowTitleBarContents(AnchorWindow *window,
                                           const AnchorBBox &title_bar_rect,
                                           const char *name,
                                           bool *p_open);

  // Viewports
  static void UpdateViewportsNewFrame();

}  // namespace ANCHOR

//-----------------------------------------------------------------------------
// [SECTION] CONTEXT AND MEMORY ALLOCATORS
//-----------------------------------------------------------------------------

// DLL users:
// - Heaps and globals are not shared across DLL boundaries!
// - You will need to call SetCurrentContext() + SetAllocatorFunctions() for each static/DLL
// boundary you are calling from.
// - Same applies for hot-reloading mechanisms that are reliant on reloading DLL (note that many
// hot-reloading mechanisms work without DLL).
// - Using ANCHOR via a shared library is not recommended, because of function call overhead and
// because we don't guarantee backward nor forward ABI compatibility.
// - Confused? In a debugger: add G_CTX to your watch window and notice how its value changes
// depending on your current location (which DLL boundary you are in).

// Current context pointer. Implicitly used by all ANCHOR functions. Always assumed to be != NULL.
// - ANCHOR::CreateContext() will automatically set this pointer if it is NULL.
//   Change to a different context by calling ANCHOR::SetCurrentContext().
// - Important: ANCHOR functions are not thread-safe because of this pointer.
//   If you want thread-safety to allow N threads to access N different contexts:
//   - Change this variable to use thread local storage so each thread can refer to a different
//   context, in your ANCHOR_config.h:
//         struct AnchorContext;
//         extern thread_local AnchorContext* MyANCHORTLS;
//         #define G_CTX MyANCHORTLS
//     And then define MyANCHORTLS in one of your cpp files. Note that thread_local is a C++11
//     keyword, earlier C++ uses compiler-specific keyword.
//   - Future development aims to make this context pointer explicit to all calls. Also read
//   https://github.com/ocornut/ANCHOR/issues/586
//   - If you need a finite number of contexts, you may compile and use multiple instances of the
//   ANCHOR code from a different namespace.
// - DLL users: read comments above.
#ifndef G_CTX
AnchorContext *G_CTX = NULL;
#endif

// Memory Allocator functions. Use SetAllocatorFunctions() to change them.
// - You probably don't want to modify that mid-program, and if you use global/static e.g.
// AnchorVector<> instances you may need to keep them accessible during program destruction.
// - DLL users: read comments above.
#ifndef ANCHOR_DISABLE_DEFAULT_ALLOCATORS
static void *MallocWrapper(size_t size, void *user_data)
{
  TF_UNUSED(user_data);
  return malloc(size);
}
static void FreeWrapper(void *ptr, void *user_data)
{
  TF_UNUSED(user_data);
  free(ptr);
}
#else
static void *MallocWrapper(size_t size, void *user_data)
{
  TF_UNUSED(user_data);
  TF_UNUSED(size);
  ANCHOR_ASSERT(0);
  return NULL;
}
static void FreeWrapper(void *ptr, void *user_data)
{
  TF_UNUSED(user_data);
  TF_UNUSED(ptr);
  ANCHOR_ASSERT(0);
}
#endif
static ANCHORMemAllocFunc GImAllocatorAllocFunc = MallocWrapper;
static ANCHORMemFreeFunc GImAllocatorFreeFunc = FreeWrapper;
static void *GImAllocatorUserData = NULL;

//-----------------------------------------------------------------------------
// [SECTION] USER FACING STRUCTURES (AnchorStyle, AnchorIO)
//-----------------------------------------------------------------------------

AnchorStyle::AnchorStyle()
{
  Alpha = 1.0f;                   // Global alpha applies to everything in ANCHOR
  WindowPadding = wabi::GfVec2f(8, 8);  // Padding within a window
  WindowRounding =
    0.0f;  // Radius of window corners rounding. Set to 0.0f to have rectangular windows.
           // Large values tend to lead to variety of artifacts and are not recommended.
  WindowBorderSize = 1.0f;  // Thickness of border around windows. Generally set to 0.0f or 1.0f.
                            // Other values not well tested.
  WindowMinSize = wabi::GfVec2f(32, 32);         // Minimum window size
  WindowTitleAlign = wabi::GfVec2f(0.0f, 0.5f);  // Alignment for title bar text
  WindowMenuButtonPosition =
    AnchorDir_Left;        // Position of the collapsing/docking button in the title bar
                           // (left/right). Defaults to AnchorDir_Left.
  ChildRounding = 0.0f;    // Radius of child window corners rounding. Set to 0.0f to have
                           // rectangular child windows
  ChildBorderSize = 1.0f;  // Thickness of border around child windows. Generally set to 0.0f
                           // or 1.0f. Other values not well tested.
  PopupRounding = 0.0f;    // Radius of popup window corners rounding. Set to 0.0f to have
                           // rectangular child windows
  PopupBorderSize = 1.0f;  // Thickness of border around popup or tooltip windows. Generally set to
                           // 0.0f or 1.0f. Other values not well tested.
  FramePadding = wabi::GfVec2f(4, 3);  // Padding within a framed rectangle (used by most widgets)
  FrameRounding = 0.0f;    // Radius of frame corners rounding. Set to 0.0f to have rectangular
                           // frames (used by most widgets).
  FrameBorderSize = 0.0f;  // Thickness of border around frames. Generally set to 0.0f or 1.0f.
                           // Other values not well tested.
  ItemSpacing = wabi::GfVec2f(8, 4);       // Horizontal and vertical spacing between widgets/lines
  ItemInnerSpacing = wabi::GfVec2f(4, 4);  // Horizontal and vertical spacing between within elements of
                                     // a composed widget (e.g. a slider and its label)
  CellPadding = wabi::GfVec2f(4, 2);       // Padding within a table cell
  TouchExtraPadding = wabi::GfVec2f(
    0,
    0);  // Expand reactive bounding box for touch-based system where touch position is not
         // accurate enough. Unfortunately we don't sort widgets so priority on overlap will
         // always be given to the first widget. So don't grow this too much!
  IndentSpacing = 21.0f;  // Horizontal spacing when e.g. entering a tree node. Generally ==
                          // (FontSize + FramePadding[0]*2).
  ColumnsMinSpacing =
    6.0f;  // Minimum horizontal spacing between two columns. Preferably > (FramePadding[0] + 1).
  ScrollbarSize = 14.0f;     // Width of the vertical scrollbar, Height of the horizontal scrollbar
  ScrollbarRounding = 9.0f;  // Radius of grab corners rounding for scrollbar
  GrabMinSize = 10.0f;       // Minimum width/height of a grab box for slider/scrollbar
  GrabRounding =
    0.0f;  // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
  LogSliderDeadzone = 4.0f;  // The size in pixels of the dead-zone around zero on logarithmic
                             // sliders that cross zero.
  TabRounding = 4.0f;    // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
  TabBorderSize = 0.0f;  // Thickness of border around tabs.
  TabMinWidthForCloseButton = 0.0f;  // Minimum width for close button to appears on an unselected
                                     // tab when hovered. Set to 0.0f to always show when hovering,
                                     // set to FLT_MAX to never show close button unless selected.
  ColorButtonPosition = AnchorDir_Right;  // Side of the color button in the ColorEdit4 widget
                                          // (left/right). Defaults to AnchorDir_Right.
  ButtonTextAlign = wabi::GfVec2f(0.5f,
                            0.5f);  // Alignment of button text when button is larger than text.
  SelectableTextAlign = wabi::GfVec2f(
    0.0f,
    0.0f);  // Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left
            // aligned). It's generally important to keep this left-aligned if
            // you want to lay multiple items on a same line.
  DisplayWindowPadding = wabi::GfVec2f(
    19,
    19);  // Window position are clamped to be visible within the display area or monitors by
          // at least this amount. Only applies to regular windows.
  DisplaySafeAreaPadding = wabi::GfVec2f(
    3,
    3);  // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area
         // padding. Covers popups/tooltips as well regular windows.
  MouseCursorScale = 1.0f;  // Scale software rendered mouse cursor (when io.MouseDrawCursor is
                            // enabled). May be removed later.
  AntiAliasedLines =
    true;  // Enable anti-aliased lines/borders. Disable if you are really tight on CPU/GPU.
  AntiAliasedLinesUseTex = true;  // Enable anti-aliased lines/borders using textures where
                                  // possible. Require backend to render with bilinear filtering.
  AntiAliasedFill =
    true;  // Enable anti-aliased filled shapes (rounded rectangles, circles, etc.).
  CurveTessellationTol =
    1.25f;  // Tessellation tolerance when using PathBezierCurveTo() without a specific
            // number of segments. Decrease for highly tessellated curves (higher
            // quality, more polygons), increase to reduce quality.
  CircleTessellationMaxError =
    0.30f;  // Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or
            // drawing rounded corner rectangles with no explicit segment count specified.
            // Decrease for higher quality but more geometry.

  // Default theme
  ANCHOR::StyleColorsDark(this);
}

// To scale your entire UI (e.g. if you want your app to use High DPI or generally be DPI aware)
// you may use this helper function. Scaling the fonts is done separately and is up to you.
// Important: This operation is lossy because we round all sizes to integer. If you need to change
// your scale multiples, call this over a freshly initialized AnchorStyle structure rather than
// scaling multiple times.
void AnchorStyle::ScaleAllSizes(float scale_factor)
{
  WindowPadding = AnchorFloor(WindowPadding * scale_factor);
  WindowRounding = AnchorFloor(WindowRounding * scale_factor);
  WindowMinSize = AnchorFloor(WindowMinSize * scale_factor);
  ChildRounding = AnchorFloor(ChildRounding * scale_factor);
  PopupRounding = AnchorFloor(PopupRounding * scale_factor);
  FramePadding = AnchorFloor(FramePadding * scale_factor);
  FrameRounding = AnchorFloor(FrameRounding * scale_factor);
  ItemSpacing = AnchorFloor(ItemSpacing * scale_factor);
  ItemInnerSpacing = AnchorFloor(ItemInnerSpacing * scale_factor);
  CellPadding = AnchorFloor(CellPadding * scale_factor);
  TouchExtraPadding = AnchorFloor(TouchExtraPadding * scale_factor);
  IndentSpacing = AnchorFloor(IndentSpacing * scale_factor);
  ColumnsMinSpacing = AnchorFloor(ColumnsMinSpacing * scale_factor);
  ScrollbarSize = AnchorFloor(ScrollbarSize * scale_factor);
  ScrollbarRounding = AnchorFloor(ScrollbarRounding * scale_factor);
  GrabMinSize = AnchorFloor(GrabMinSize * scale_factor);
  GrabRounding = AnchorFloor(GrabRounding * scale_factor);
  LogSliderDeadzone = AnchorFloor(LogSliderDeadzone * scale_factor);
  TabRounding = AnchorFloor(TabRounding * scale_factor);
  TabMinWidthForCloseButton = (TabMinWidthForCloseButton != FLT_MAX) ?
                                AnchorFloor(TabMinWidthForCloseButton * scale_factor) :
                                FLT_MAX;
  DisplayWindowPadding = AnchorFloor(DisplayWindowPadding * scale_factor);
  DisplaySafeAreaPadding = AnchorFloor(DisplaySafeAreaPadding * scale_factor);
  MouseCursorScale = AnchorFloor(MouseCursorScale * scale_factor);
}

AnchorIO::AnchorIO()
{
  // Most fields are initialized with zero
  memset(this, 0, sizeof(*this));
  ANCHOR_ASSERT(
    ANCHOR_ARRAYSIZE(AnchorIO::MouseDown) == AnchorMouseButton_COUNT &&
    ANCHOR_ARRAYSIZE(AnchorIO::MouseClicked) ==
      AnchorMouseButton_COUNT);  // Our pre-C++11 IM_STATIC_ASSERT() macros triggers warning
                                 // on modern compilers so we don't use it here.

  // Settings
  ConfigFlags = AnchorConfigFlags_None;
  BackendFlags = AnchorBackendFlags_None;
  DisplaySize = wabi::GfVec2f(-1.0f, -1.0f);
  DeltaTime = 1.0f / 60.0f;
  IniSavingRate = 5.0f;
  IniFilename = "ANCHOR.ini";
  LogFilename = "ANCHOR_log.txt";
  MouseDoubleClickTime = 0.30f;
  MouseDoubleClickMaxDist = 6.0f;
  for (int i = 0; i < AnchorKey_COUNT; i++)
    KeyMap[i] = -1;
  KeyRepeatDelay = 0.275f;
  KeyRepeatRate = 0.050f;
  UserData = NULL;

  Fonts = NULL;
  FontGlobalScale = 1.0f;
  FontDefault = NULL;
  FontAllowUserScaling = false;
  DisplayFramebufferScale = wabi::GfVec2f(1.0f, 1.0f);

  // Miscellaneous options
  MouseDrawCursor = false;
#ifdef __APPLE__
  ConfigMacOSXBehaviors =
    true;  // Set Mac OS X style defaults based on __APPLE__ compile time flag
#else
  ConfigMacOSXBehaviors = false;
#endif
  ConfigInputTextCursorBlink = true;
  ConfigWindowsResizeFromEdges = true;
  ConfigWindowsMoveFromTitleBarOnly = false;
  ConfigMemoryCompactTimer = 60.0f;

  // Platform Functions
  BackendPlatformName = BackendRendererName = NULL;
  BackendPlatformUserData = BackendRendererUserData = BackendLanguageUserData = NULL;
  GetClipboardTextFn =
    GetClipboardTextFn_DefaultImpl;  // Platform dependent default implementations
  SetClipboardTextFn = SetClipboardTextFn_DefaultImpl;
  ClipboardUserData = NULL;
  ImeSetInputScreenPosFn = ImeSetInputScreenPosFn_DefaultImpl;
  ImeWindowHandle = NULL;

  // Input (NB: we already have memset zero the entire structure!)
  MousePos = wabi::GfVec2f(-FLT_MAX, -FLT_MAX);
  MousePosPrev = wabi::GfVec2f(-FLT_MAX, -FLT_MAX);
  MouseDragThreshold = 6.0f;
  for (int i = 0; i < ANCHOR_ARRAYSIZE(MouseDownDuration); i++)
    MouseDownDuration[i] = MouseDownDurationPrev[i] = -1.0f;
  for (int i = 0; i < ANCHOR_ARRAYSIZE(KeysDownDuration); i++)
    KeysDownDuration[i] = KeysDownDurationPrev[i] = -1.0f;
  for (int i = 0; i < ANCHOR_ARRAYSIZE(NavInputsDownDuration); i++)
    NavInputsDownDuration[i] = -1.0f;
}

// Pass in translated ASCII characters for text input.
// - with glfw you can get those from the callback set in glfwSetCharCallback()
// - on Windows you can get those using ToAscii+keyboard state, or via the WM_CHAR message
void AnchorIO::AddInputCharacter(unsigned int c)
{
  if (c != 0)
    InputQueueCharacters.push_back(c <= IM_UNICODE_CODEPOINT_MAX ? (AnchorWChar)c :
                                                                   IM_UNICODE_CODEPOINT_INVALID);
}

// UTF16 strings use surrogate pairs to encode codepoints >= 0x10000, so
// we should save the high surrogate.
void AnchorIO::AddInputCharacterUTF16(AnchorWChar16 c)
{
  if (c == 0 && InputQueueSurrogate == 0)
    return;

  if ((c & 0xFC00) == 0xD800)  // High surrogate, must save
  {
    if (InputQueueSurrogate != 0)
      InputQueueCharacters.push_back(IM_UNICODE_CODEPOINT_INVALID);
    InputQueueSurrogate = c;
    return;
  }

  AnchorWChar cp = c;
  if (InputQueueSurrogate != 0) {
    if ((c & 0xFC00) != 0xDC00)  // Invalid low surrogate
    {
      InputQueueCharacters.push_back(IM_UNICODE_CODEPOINT_INVALID);
    } else {
#if IM_UNICODE_CODEPOINT_MAX == 0xFFFF
      cp = IM_UNICODE_CODEPOINT_INVALID;  // Codepoint will not fit in AnchorWChar
#else
      cp = (AnchorWChar)(((InputQueueSurrogate - 0xD800) << 10) + (c - 0xDC00) + 0x10000);
#endif
    }

    InputQueueSurrogate = 0;
  }
  InputQueueCharacters.push_back(cp);
}

void AnchorIO::AddInputCharactersUTF8(const char *utf8_chars)
{
  while (*utf8_chars != 0) {
    unsigned int c = 0;
    utf8_chars += AnchorTextCharFromUtf8(&c, utf8_chars, NULL);
    if (c != 0)
      InputQueueCharacters.push_back((AnchorWChar)c);
  }
}

void AnchorIO::ClearInputCharacters()
{
  InputQueueCharacters.resize(0);
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (Geometry functions)
//-----------------------------------------------------------------------------

wabi::GfVec2f AnchorBezierCubicClosestPoint(const wabi::GfVec2f &p1,
                                            const wabi::GfVec2f &p2,
                                            const wabi::GfVec2f &p3,
                                            const wabi::GfVec2f &p4,
                                            const wabi::GfVec2f &p,
                                      int num_segments)
{
  ANCHOR_ASSERT(num_segments > 0);  // Use AnchorBezierCubicClosestPointCasteljau()
  wabi::GfVec2f p_last = p1;
  wabi::GfVec2f p_closest;
  float p_closest_dist2 = FLT_MAX;
  float t_step = 1.0f / (float)num_segments;
  for (int i_step = 1; i_step <= num_segments; i_step++) {
    wabi::GfVec2f p_current = AnchorBezierCubicCalc(p1, p2, p3, p4, t_step * i_step);
    wabi::GfVec2f p_line = AnchorLineClosestPoint(p_last, p_current, p);
    float dist2 = AnchorLengthSqr(p - p_line);
    if (dist2 < p_closest_dist2) {
      p_closest = p_line;
      p_closest_dist2 = dist2;
    }
    p_last = p_current;
  }
  return p_closest;
}

// Closely mimics PathBezierToCasteljau() in ANCHOR_draw.cpp
static void AnchorBezierCubicClosestPointCasteljauStep(const wabi::GfVec2f &p,
                                                       wabi::GfVec2f &p_closest,
                                                       wabi::GfVec2f &p_last,
                                                       float &p_closest_dist2,
                                                       float x1,
                                                       float y1,
                                                       float x2,
                                                       float y2,
                                                       float x3,
                                                       float y3,
                                                       float x4,
                                                       float y4,
                                                       float tess_tol,
                                                       int level)
{
  float dx = x4 - x1;
  float dy = y4 - y1;
  float d2 = ((x2 - x4) * dy - (y2 - y4) * dx);
  float d3 = ((x3 - x4) * dy - (y3 - y4) * dx);
  d2 = (d2 >= 0) ? d2 : -d2;
  d3 = (d3 >= 0) ? d3 : -d3;
  if ((d2 + d3) * (d2 + d3) < tess_tol * (dx * dx + dy * dy)) {
    wabi::GfVec2f p_current(x4, y4);
    wabi::GfVec2f p_line = AnchorLineClosestPoint(p_last, p_current, p);
    float dist2 = AnchorLengthSqr(p - p_line);
    if (dist2 < p_closest_dist2) {
      p_closest = p_line;
      p_closest_dist2 = dist2;
    }
    p_last = p_current;
  } else if (level < 10) {
    float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
    float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
    float x34 = (x3 + x4) * 0.5f, y34 = (y3 + y4) * 0.5f;
    float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
    float x234 = (x23 + x34) * 0.5f, y234 = (y23 + y34) * 0.5f;
    float x1234 = (x123 + x234) * 0.5f, y1234 = (y123 + y234) * 0.5f;
    AnchorBezierCubicClosestPointCasteljauStep(p,
                                               p_closest,
                                               p_last,
                                               p_closest_dist2,
                                               x1,
                                               y1,
                                               x12,
                                               y12,
                                               x123,
                                               y123,
                                               x1234,
                                               y1234,
                                               tess_tol,
                                               level + 1);
    AnchorBezierCubicClosestPointCasteljauStep(p,
                                               p_closest,
                                               p_last,
                                               p_closest_dist2,
                                               x1234,
                                               y1234,
                                               x234,
                                               y234,
                                               x34,
                                               y34,
                                               x4,
                                               y4,
                                               tess_tol,
                                               level + 1);
  }
}

// tess_tol is generally the same value you would find in ANCHOR::GetStyle().CurveTessellationTol
// Because those ImXXX functions are lower-level than ANCHOR:: we cannot access this value
// automatically.
wabi::GfVec2f AnchorBezierCubicClosestPointCasteljau(const wabi::GfVec2f &p1,
                                               const wabi::GfVec2f &p2,
                                               const wabi::GfVec2f &p3,
                                               const wabi::GfVec2f &p4,
                                               const wabi::GfVec2f &p,
                                               float tess_tol)
{
  ANCHOR_ASSERT(tess_tol > 0.0f);
  wabi::GfVec2f p_last = p1;
  wabi::GfVec2f p_closest;
  float p_closest_dist2 = FLT_MAX;
  AnchorBezierCubicClosestPointCasteljauStep(p,
                                             p_closest,
                                             p_last,
                                             p_closest_dist2,
                                             p1[0],
                                             p1[1],
                                             p2[0],
                                             p2[1],
                                             p3[0],
                                             p3[1],
                                             p4[0],
                                             p4[1],
                                             tess_tol,
                                             0);
  return p_closest;
}

wabi::GfVec2f AnchorLineClosestPoint(const wabi::GfVec2f &a, const wabi::GfVec2f &b, const wabi::GfVec2f &p)
{
  wabi::GfVec2f ap = p - a;
  wabi::GfVec2f ab_dir = b - a;
  float dot = ap[0] * ab_dir[0] + ap[1] * ab_dir[1];
  if (dot < 0.0f)
    return a;
  float ab_len_sqr = ab_dir[0] * ab_dir[0] + ab_dir[1] * ab_dir[1];
  if (dot > ab_len_sqr)
    return b;
  return a + ab_dir * dot / ab_len_sqr;
}

bool AnchorTriangleContainsPoint(const wabi::GfVec2f &a,
                                 const wabi::GfVec2f &b,
                                 const wabi::GfVec2f &c,
                                 const wabi::GfVec2f &p)
{
  bool b1 = ((p[0] - b[0]) * (a[1] - b[1]) - (p[1] - b[1]) * (a[0] - b[0])) < 0.0f;
  bool b2 = ((p[0] - c[0]) * (b[1] - c[1]) - (p[1] - c[1]) * (b[0] - c[0])) < 0.0f;
  bool b3 = ((p[0] - a[0]) * (c[1] - a[1]) - (p[1] - a[1]) * (c[0] - a[0])) < 0.0f;
  return ((b1 == b2) && (b2 == b3));
}

void AnchorTriangleBarycentricCoords(const wabi::GfVec2f &a,
                                     const wabi::GfVec2f &b,
                                     const wabi::GfVec2f &c,
                                     const wabi::GfVec2f &p,
                                     float &out_u,
                                     float &out_v,
                                     float &out_w)
{
  wabi::GfVec2f v0 = b - a;
  wabi::GfVec2f v1 = c - a;
  wabi::GfVec2f v2 = p - a;
  const float denom = v0[0] * v1[1] - v1[0] * v0[1];
  out_v = (v2[0] * v1[1] - v1[0] * v2[1]) / denom;
  out_w = (v0[0] * v2[1] - v2[0] * v0[1]) / denom;
  out_u = 1.0f - out_v - out_w;
}

wabi::GfVec2f AnchorTriangleClosestPoint(const wabi::GfVec2f &a,
                                   const wabi::GfVec2f &b,
                                   const wabi::GfVec2f &c,
                                   const wabi::GfVec2f &p)
{
  wabi::GfVec2f proj_ab = AnchorLineClosestPoint(a, b, p);
  wabi::GfVec2f proj_bc = AnchorLineClosestPoint(b, c, p);
  wabi::GfVec2f proj_ca = AnchorLineClosestPoint(c, a, p);
  float dist2_ab = AnchorLengthSqr(p - proj_ab);
  float dist2_bc = AnchorLengthSqr(p - proj_bc);
  float dist2_ca = AnchorLengthSqr(p - proj_ca);
  float m = AnchorMin(dist2_ab, AnchorMin(dist2_bc, dist2_ca));
  if (m == dist2_ab)
    return proj_ab;
  if (m == dist2_bc)
    return proj_bc;
  return proj_ca;
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (String, Format, Hash functions)
//-----------------------------------------------------------------------------

// Consider using _stricmp/_strnicmp under Windows or strcasecmp/strncasecmp. We don't actually use
// either AnchorStricmp/AnchorStrnicmp in the codebase any more.
int AnchorStricmp(const char *str1, const char *str2)
{
  int d;
  while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) {
    str1++;
    str2++;
  }
  return d;
}

int AnchorStrnicmp(const char *str1, const char *str2, size_t count)
{
  int d = 0;
  while (count > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) {
    str1++;
    str2++;
    count--;
  }
  return d;
}

void AnchorStrncpy(char *dst, const char *src, size_t count)
{
  if (count < 1)
    return;
  if (count > 1)
    strncpy(dst, src, count - 1);
  dst[count - 1] = 0;
}

char *AnchorStrdup(const char *str)
{
  size_t len = strlen(str);
  void *buf = ANCHOR_ALLOC(len + 1);
  return (char *)memcpy(buf, (const void *)str, len + 1);
}

char *AnchorStrdupcpy(char *dst, size_t *p_dst_size, const char *src)
{
  size_t dst_buf_size = p_dst_size ? *p_dst_size : strlen(dst) + 1;
  size_t src_size = strlen(src) + 1;
  if (dst_buf_size < src_size) {
    ANCHOR_FREE(dst);
    dst = (char *)ANCHOR_ALLOC(src_size);
    if (p_dst_size)
      *p_dst_size = src_size;
  }
  return (char *)memcpy(dst, (const void *)src, src_size);
}

const char *AnchorStrchrRange(const char *str, const char *str_end, char c)
{
  const char *p = (const char *)memchr(str, (int)c, str_end - str);
  return p;
}

int AnchorStrlenW(const AnchorWChar *str)
{
  // return (int)wcslen((const wchar_t*)str);  // FIXME-OPT: Could use this when wchar_t are 16-bit
  int n = 0;
  while (*str++)
    n++;
  return n;
}

// Find end-of-line. Return pointer will point to either first \n, either str_end.
const char *AnchorStreolRange(const char *str, const char *str_end)
{
  const char *p = (const char *)memchr(str, '\n', str_end - str);
  return p ? p : str_end;
}

const AnchorWChar *AnchorStrbolW(const AnchorWChar *buf_mid_line,
                                 const AnchorWChar *buf_begin)  // find beginning-of-line
{
  while (buf_mid_line > buf_begin && buf_mid_line[-1] != '\n')
    buf_mid_line--;
  return buf_mid_line;
}

const char *AnchorStristr(const char *haystack,
                          const char *haystack_end,
                          const char *needle,
                          const char *needle_end)
{
  if (!needle_end)
    needle_end = needle + strlen(needle);

  const char un0 = (char)toupper(*needle);
  while ((!haystack_end && *haystack) || (haystack_end && haystack < haystack_end)) {
    if (toupper(*haystack) == un0) {
      const char *b = needle + 1;
      for (const char *a = haystack + 1; b < needle_end; a++, b++)
        if (toupper(*a) != toupper(*b))
          break;
      if (b == needle_end)
        return haystack;
    }
    haystack++;
  }
  return NULL;
}

// Trim str by offsetting contents when there's leading data + writing a \0 at the trailing
// position. We use this in situation where the cost is negligible.
void AnchorTrimBlanks(char *buf)
{
  char *p = buf;
  while (p[0] == ' ' || p[0] == '\t')  // Leading blanks
    p++;
  char *p_start = p;
  while (*p != 0)  // Find end of string
    p++;
  while (p > p_start && (p[-1] == ' ' || p[-1] == '\t'))  // Trailing blanks
    p--;
  if (p_start != buf)  // Copy memory if we had leading blanks
    memmove(buf, p_start, p - p_start);
  buf[p - p_start] = 0;  // Zero terminate
}

const char *AnchorStrSkipBlank(const char *str)
{
  while (str[0] == ' ' || str[0] == '\t')
    str++;
  return str;
}

// A) MSVC version appears to return -1 on overflow, whereas glibc appears to return total count
// (which may be >= buf_size). Ideally we would test for only one of those limits at runtime
// depending on the behavior the vsnprintf(), but trying to deduct it at compile time sounds like a
// pandora can of worm. B) When buf==NULL vsnprintf() will return the output size.
#ifndef ANCHOR_DISABLE_DEFAULT_FORMAT_FUNCTIONS

// We support stb_sprintf which is much faster (see:
// https://github.com/nothings/stb/blob/master/stb_sprintf.h) You may set ANCHOR_USE_STB_SPRINTF to
// use our default wrapper, or set ANCHOR_DISABLE_DEFAULT_FORMAT_FUNCTIONS and setup the wrapper
// yourself. (FIXME-OPT: Some of our high-level operations such as AnchorTextBuffer::appendfv() are
// designed using two-passes worst case, which probably could be improved using the
// stbsp_vsprintfcb() function.)
#  ifdef ANCHOR_USE_STB_SPRINTF
#    define STB_SPRINTF_IMPLEMENTATION
#    include "stb_sprintf.h"
#  endif

#  if defined(_MSC_VER) && !defined(vsnprintf)
#    define vsnprintf _vsnprintf
#  endif

int AnchorFormatString(char *buf, size_t buf_size, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
#  ifdef ANCHOR_USE_STB_SPRINTF
  int w = stbsp_vsnprintf(buf, (int)buf_size, fmt, args);
#  else
  int w = vsnprintf(buf, buf_size, fmt, args);
#  endif
  va_end(args);
  if (buf == NULL)
    return w;
  if (w == -1 || w >= (int)buf_size)
    w = (int)buf_size - 1;
  buf[w] = 0;
  return w;
}

int AnchorFormatStringV(char *buf, size_t buf_size, const char *fmt, va_list args)
{
#  ifdef ANCHOR_USE_STB_SPRINTF
  int w = stbsp_vsnprintf(buf, (int)buf_size, fmt, args);
#  else
  int w = vsnprintf(buf, buf_size, fmt, args);
#  endif
  if (buf == NULL)
    return w;
  if (w == -1 || w >= (int)buf_size)
    w = (int)buf_size - 1;
  buf[w] = 0;
  return w;
}
#endif  // #ifdef ANCHOR_DISABLE_DEFAULT_FORMAT_FUNCTIONS

// CRC32 needs a 1KB lookup table (not cache friendly)
// Although the code to generate the table is simple and shorter than the table itself, using a
// const table allows us to easily:
// - avoid an unnecessary branch/memory tap, - keep the ImHashXXX functions usable by static
// constructors, - make it thread-safe.
static const AnchorU32 GCrc32LookupTable[256] = {
  0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
  0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
  0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
  0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
  0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
  0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
  0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
  0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
  0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
  0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
  0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
  0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
  0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
  0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
  0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
  0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
  0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
  0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
  0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
  0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
  0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
  0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
  0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
  0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
  0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
  0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
  0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
  0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
  0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
  0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
  0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
  0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
};

// Known size hash
// It is ok to call ImHashData on a string with known length but the ### operator won't be
// supported.
// FIXME-OPT: Replace with e.g. FNV1a hash? CRC32 pretty much randomly access 1KB. Need to do
// proper measurements.
ANCHOR_ID ImHashData(const void *data_p, size_t data_size, AnchorU32 seed)
{
  AnchorU32 crc = ~seed;
  const unsigned char *data = (const unsigned char *)data_p;
  const AnchorU32 *crc32_lut = GCrc32LookupTable;
  while (data_size-- != 0)
    crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ *data++];
  return ~crc;
}

// Zero-terminated string hash, with support for ### to reset back to seed value
// We support a syntax of "label###id" where only "###id" is included in the hash, and only "label"
// gets displayed. Because this syntax is rarely used we are optimizing for the common case.
// - If we reach ### in the string we discard the hash so far and reset to the seed.
// - We don't do 'current += 2; continue;' after handling ### to keep the code smaller/faster
// (measured ~10% diff in Debug build)
// FIXME-OPT: Replace with e.g. FNV1a hash? CRC32 pretty much randomly access 1KB. Need to do
// proper measurements.
ANCHOR_ID AnchorHashStr(const char *data_p, size_t data_size, AnchorU32 seed)
{
  seed = ~seed;
  AnchorU32 crc = seed;
  const unsigned char *data = (const unsigned char *)data_p;
  const AnchorU32 *crc32_lut = GCrc32LookupTable;
  if (data_size != 0) {
    while (data_size-- != 0) {
      unsigned char c = *data++;
      if (c == '#' && data_size >= 2 && data[0] == '#' && data[1] == '#')
        crc = seed;
      crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
    }
  } else {
    while (unsigned char c = *data++) {
      if (c == '#' && data[0] == '#' && data[1] == '#')
        crc = seed;
      crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
    }
  }
  return ~crc;
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (File functions)
//-----------------------------------------------------------------------------

// Default file functions
#ifndef ANCHOR_DISABLE_DEFAULT_FILE_FUNCTIONS

ImFileHandle ImFileOpen(const char *filename, const char *mode)
{
#  if defined(_WIN32) && !defined(ANCHOR_DISABLE_WIN32_FUNCTIONS) && !defined(__CYGWIN__) && \
    !defined(__GNUC__)
  // We need a fopen() wrapper because MSVC/Windows fopen doesn't handle UTF-8 filenames.
  // Previously we used AnchorTextCountCharsFromUtf8/AnchorTextStrFromUtf8 here but we now need to
  // support AnchorWChar16 and AnchorWChar32!
  const int filename_wsize = ::MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);
  const int mode_wsize = ::MultiByteToWideChar(CP_UTF8, 0, mode, -1, NULL, 0);
  AnchorVector<AnchorWChar> buf;
  buf.resize(filename_wsize + mode_wsize);
  ::MultiByteToWideChar(CP_UTF8, 0, filename, -1, (wchar_t *)&buf[0], filename_wsize);
  ::MultiByteToWideChar(CP_UTF8, 0, mode, -1, (wchar_t *)&buf[filename_wsize], mode_wsize);
  return ::_wfopen((const wchar_t *)&buf[0], (const wchar_t *)&buf[filename_wsize]);
#  else
  return fopen(filename, mode);
#  endif
}

// We should in theory be using fseeko()/ftello() with off_t and _fseeki64()/_ftelli64() with
// __int64, waiting for the PR that does that in a very portable pre-C++11 zero-warnings way.
bool ImFileClose(ImFileHandle f)
{
  return fclose(f) == 0;
}
AnchorU64 ImFileGetSize(ImFileHandle f)
{
  long off = 0, sz = 0;
  return ((off = ftell(f)) != -1 && !fseek(f, 0, SEEK_END) && (sz = ftell(f)) != -1 &&
          !fseek(f, off, SEEK_SET)) ?
           (AnchorU64)sz :
           (AnchorU64)-1;
}
AnchorU64 ImFileRead(void *data, AnchorU64 sz, AnchorU64 count, ImFileHandle f)
{
  return fread(data, (size_t)sz, (size_t)count, f);
}
AnchorU64 ImFileWrite(const void *data, AnchorU64 sz, AnchorU64 count, ImFileHandle f)
{
  return fwrite(data, (size_t)sz, (size_t)count, f);
}
#endif  // #ifndef ANCHOR_DISABLE_DEFAULT_FILE_FUNCTIONS

// Helper: Load file content into memory
// Memory allocated with ANCHOR_ALLOC(), must be freed by user using ANCHOR_FREE() ==
// ANCHOR::MemFree() This can't really be used with "rt" because fseek size won't match read size.
void *ImFileLoadToMemory(const char *filename,
                         const char *mode,
                         size_t *out_file_size,
                         int padding_bytes)
{
  ANCHOR_ASSERT(filename && mode);
  if (out_file_size)
    *out_file_size = 0;

  ImFileHandle f;
  if ((f = ImFileOpen(filename, mode)) == NULL)
    return NULL;

  size_t file_size = (size_t)ImFileGetSize(f);
  if (file_size == (size_t)-1) {
    ImFileClose(f);
    return NULL;
  }

  void *file_data = ANCHOR_ALLOC(file_size + padding_bytes);
  if (file_data == NULL) {
    ImFileClose(f);
    return NULL;
  }
  if (ImFileRead(file_data, 1, file_size, f) != file_size) {
    ImFileClose(f);
    ANCHOR_FREE(file_data);
    return NULL;
  }
  if (padding_bytes > 0)
    memset((void *)(((char *)file_data) + file_size), 0, (size_t)padding_bytes);

  ImFileClose(f);
  if (out_file_size)
    *out_file_size = file_size;

  return file_data;
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (ImText* functions)
//-----------------------------------------------------------------------------

// Convert UTF-8 to 32-bit character, process single character input.
// A nearly-branchless UTF-8 decoder, based on work of Christopher Wellons
// (https://github.com/skeeto/branchless-utf8). We handle UTF-8 decoding error by skipping forward.
int AnchorTextCharFromUtf8(unsigned int *out_char, const char *in_text, const char *in_text_end)
{
  static const char lengths[32] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                   0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0};
  static const int masks[] = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
  static const uint32_t mins[] = {0x400000, 0, 0x80, 0x800, 0x10000};
  static const int shiftc[] = {0, 18, 12, 6, 0};
  static const int shifte[] = {0, 6, 4, 2, 0};
  int len = lengths[*(const unsigned char *)in_text >> 3];
  int wanted = len + !len;

  if (in_text_end == NULL)
    in_text_end = in_text + wanted;  // Max length, nulls will be taken into account.

  // Copy at most 'len' bytes, stop copying at 0 or past in_text_end. Branch predictor does a good
  // job here, so it is fast even with excessive branching.
  unsigned char s[4];
  s[0] = in_text + 0 < in_text_end ? in_text[0] : 0;
  s[1] = in_text + 1 < in_text_end ? in_text[1] : 0;
  s[2] = in_text + 2 < in_text_end ? in_text[2] : 0;
  s[3] = in_text + 3 < in_text_end ? in_text[3] : 0;

  // Assume a four-byte character and load four bytes. Unused bits are shifted out.
  *out_char = (uint32_t)(s[0] & masks[len]) << 18;
  *out_char |= (uint32_t)(s[1] & 0x3f) << 12;
  *out_char |= (uint32_t)(s[2] & 0x3f) << 6;
  *out_char |= (uint32_t)(s[3] & 0x3f) << 0;
  *out_char >>= shiftc[len];

  // Accumulate the various error conditions.
  int e = 0;
  e = (*out_char < mins[len]) << 6;                  // non-canonical encoding
  e |= ((*out_char >> 11) == 0x1b) << 7;             // surrogate half?
  e |= (*out_char > IM_UNICODE_CODEPOINT_MAX) << 8;  // out of range?
  e |= (s[1] & 0xc0) >> 2;
  e |= (s[2] & 0xc0) >> 4;
  e |= (s[3]) >> 6;
  e ^= 0x2a;  // top two bits of each tail byte correct?
  e >>= shifte[len];

  if (e) {
    // No bytes are consumed when *in_text == 0 || in_text == in_text_end.
    // One byte is consumed in case of invalid first byte of in_text.
    // All available bytes (at most `len` bytes) are consumed on incomplete/invalid second to last
    // bytes. Invalid or incomplete input may consume less bytes than wanted, therefore every byte
    // has to be inspected in s.
    wanted = AnchorMin(wanted, !!s[0] + !!s[1] + !!s[2] + !!s[3]);
    *out_char = IM_UNICODE_CODEPOINT_INVALID;
  }

  return wanted;
}

int AnchorTextStrFromUtf8(AnchorWChar *buf,
                          int buf_size,
                          const char *in_text,
                          const char *in_text_end,
                          const char **in_text_remaining)
{
  AnchorWChar *buf_out = buf;
  AnchorWChar *buf_end = buf + buf_size;
  while (buf_out < buf_end - 1 && (!in_text_end || in_text < in_text_end) && *in_text) {
    unsigned int c;
    in_text += AnchorTextCharFromUtf8(&c, in_text, in_text_end);
    if (c == 0)
      break;
    *buf_out++ = (AnchorWChar)c;
  }
  *buf_out = 0;
  if (in_text_remaining)
    *in_text_remaining = in_text;
  return (int)(buf_out - buf);
}

int AnchorTextCountCharsFromUtf8(const char *in_text, const char *in_text_end)
{
  int char_count = 0;
  while ((!in_text_end || in_text < in_text_end) && *in_text) {
    unsigned int c;
    in_text += AnchorTextCharFromUtf8(&c, in_text, in_text_end);
    if (c == 0)
      break;
    char_count++;
  }
  return char_count;
}

// Based on stb_to_utf8() from github.com/nothings/stb/
static inline int AnchorTextCharToUtf8_inline(char *buf, int buf_size, unsigned int c)
{
  if (c < 0x80) {
    buf[0] = (char)c;
    return 1;
  }
  if (c < 0x800) {
    if (buf_size < 2)
      return 0;
    buf[0] = (char)(0xc0 + (c >> 6));
    buf[1] = (char)(0x80 + (c & 0x3f));
    return 2;
  }
  if (c < 0x10000) {
    if (buf_size < 3)
      return 0;
    buf[0] = (char)(0xe0 + (c >> 12));
    buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
    buf[2] = (char)(0x80 + ((c)&0x3f));
    return 3;
  }
  if (c <= 0x10FFFF) {
    if (buf_size < 4)
      return 0;
    buf[0] = (char)(0xf0 + (c >> 18));
    buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
    buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
    buf[3] = (char)(0x80 + ((c)&0x3f));
    return 4;
  }
  // Invalid code point, the max unicode is 0x10FFFF
  return 0;
}

const char *AnchorTextCharToUtf8(char out_buf[5], unsigned int c)
{
  int count = AnchorTextCharToUtf8_inline(out_buf, 5, c);
  out_buf[count] = 0;
  return out_buf;
}

// Not optimal but we very rarely use this function.
int AnchorTextCountUtf8BytesFromChar(const char *in_text, const char *in_text_end)
{
  unsigned int unused = 0;
  return AnchorTextCharFromUtf8(&unused, in_text, in_text_end);
}

static inline int AnchorTextCountUtf8BytesFromChar(unsigned int c)
{
  if (c < 0x80)
    return 1;
  if (c < 0x800)
    return 2;
  if (c < 0x10000)
    return 3;
  if (c <= 0x10FFFF)
    return 4;
  return 3;
}

int AnchorTextStrToUtf8(char *out_buf,
                        int out_buf_size,
                        const AnchorWChar *in_text,
                        const AnchorWChar *in_text_end)
{
  char *buf_p = out_buf;
  const char *buf_end = out_buf + out_buf_size;
  while (buf_p < buf_end - 1 && (!in_text_end || in_text < in_text_end) && *in_text) {
    unsigned int c = (unsigned int)(*in_text++);
    if (c < 0x80)
      *buf_p++ = (char)c;
    else
      buf_p += AnchorTextCharToUtf8_inline(buf_p, (int)(buf_end - buf_p - 1), c);
  }
  *buf_p = 0;
  return (int)(buf_p - out_buf);
}

int AnchorTextCountUtf8BytesFromStr(const AnchorWChar *in_text, const AnchorWChar *in_text_end)
{
  int bytes_count = 0;
  while ((!in_text_end || in_text < in_text_end) && *in_text) {
    unsigned int c = (unsigned int)(*in_text++);
    if (c < 0x80)
      bytes_count++;
    else
      bytes_count += AnchorTextCountUtf8BytesFromChar(c);
  }
  return bytes_count;
}

//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (Color functions)
// Note: The Convert functions are early design which are not consistent with other API.
//-----------------------------------------------------------------------------

ANCHOR_API AnchorU32 ImAlphaBlendColors(AnchorU32 col_a, AnchorU32 col_b)
{
  float t = ((col_b >> ANCHOR_COL32_A_SHIFT) & 0xFF) / 255.f;
  int r = AnchorLerp((int)(col_a >> ANCHOR_COL32_R_SHIFT) & 0xFF,
                     (int)(col_b >> ANCHOR_COL32_R_SHIFT) & 0xFF,
                     t);
  int g = AnchorLerp((int)(col_a >> ANCHOR_COL32_G_SHIFT) & 0xFF,
                     (int)(col_b >> ANCHOR_COL32_G_SHIFT) & 0xFF,
                     t);
  int b = AnchorLerp((int)(col_a >> ANCHOR_COL32_B_SHIFT) & 0xFF,
                     (int)(col_b >> ANCHOR_COL32_B_SHIFT) & 0xFF,
                     t);
  return ANCHOR_COL32(r, g, b, 0xFF);
}

wabi::GfVec4f ANCHOR::ColorConvertU32ToFloat4(AnchorU32 in)
{
  float s = 1.0f / 255.0f;
  return wabi::GfVec4f(((in >> ANCHOR_COL32_R_SHIFT) & 0xFF) * s,
                 ((in >> ANCHOR_COL32_G_SHIFT) & 0xFF) * s,
                 ((in >> ANCHOR_COL32_B_SHIFT) & 0xFF) * s,
                 ((in >> ANCHOR_COL32_A_SHIFT) & 0xFF) * s);
}

AnchorU32 ANCHOR::ColorConvertFloat4ToU32(const wabi::GfVec4f &in)
{
  AnchorU32 out;
  out = ((AnchorU32)IM_F32_TO_INT8_SAT(in[0])) << ANCHOR_COL32_R_SHIFT;
  out |= ((AnchorU32)IM_F32_TO_INT8_SAT(in[1])) << ANCHOR_COL32_G_SHIFT;
  out |= ((AnchorU32)IM_F32_TO_INT8_SAT(in[2])) << ANCHOR_COL32_B_SHIFT;
  out |= ((AnchorU32)IM_F32_TO_INT8_SAT(in[3])) << ANCHOR_COL32_A_SHIFT;
  return out;
}

// Convert rgb floats ([0-1],[0-1],[0-1]) to hsv floats ([0-1],[0-1],[0-1]), from Foley & van Dam
// p592 Optimized http://lolengine.net/blog/2013/01/13/fast-rgb-to-hsv
void ANCHOR::ColorConvertRGBtoHSV(float r,
                                  float g,
                                  float b,
                                  float &out_h,
                                  float &out_s,
                                  float &out_v)
{
  float K = 0.f;
  if (g < b) {
    AnchorSwap(g, b);
    K = -1.f;
  }
  if (r < g) {
    AnchorSwap(r, g);
    K = -2.f / 6.f - K;
  }

  const float chroma = r - (g < b ? g : b);
  out_h = AnchorFabs(K + (g - b) / (6.f * chroma + 1e-20f));
  out_s = chroma / (r + 1e-20f);
  out_v = r;
}

// Convert hsv floats ([0-1],[0-1],[0-1]) to rgb floats ([0-1],[0-1],[0-1]), from Foley & van Dam
// p593 also http://en.wikipedia.org/wiki/HSL_and_HSV
void ANCHOR::ColorConvertHSVtoRGB(float h,
                                  float s,
                                  float v,
                                  float &out_r,
                                  float &out_g,
                                  float &out_b)
{
  if (s == 0.0f) {
    // gray
    out_r = out_g = out_b = v;
    return;
  }

  h = AnchorFmod(h, 1.0f) / (60.0f / 360.0f);
  int i = (int)h;
  float f = h - (float)i;
  float p = v * (1.0f - s);
  float q = v * (1.0f - s * f);
  float t = v * (1.0f - s * (1.0f - f));

  switch (i) {
    case 0:
      out_r = v;
      out_g = t;
      out_b = p;
      break;
    case 1:
      out_r = q;
      out_g = v;
      out_b = p;
      break;
    case 2:
      out_r = p;
      out_g = v;
      out_b = t;
      break;
    case 3:
      out_r = p;
      out_g = q;
      out_b = v;
      break;
    case 4:
      out_r = t;
      out_g = p;
      out_b = v;
      break;
    case 5:
    default:
      out_r = v;
      out_g = p;
      out_b = q;
      break;
  }
}

//-----------------------------------------------------------------------------
// [SECTION] AnchorStorage
// Helper: Key->value storage
//-----------------------------------------------------------------------------

// std::lower_bound but without the bullshit
static AnchorStorage::AnchorStoragePair *LowerBound(
  AnchorVector<AnchorStorage::AnchorStoragePair> &data,
  ANCHOR_ID key)
{
  AnchorStorage::AnchorStoragePair *first = data.Data;
  AnchorStorage::AnchorStoragePair *last = data.Data + data.Size;
  size_t count = (size_t)(last - first);
  while (count > 0) {
    size_t count2 = count >> 1;
    AnchorStorage::AnchorStoragePair *mid = first + count2;
    if (mid->key < key) {
      first = ++mid;
      count -= count2 + 1;
    } else {
      count = count2;
    }
  }
  return first;
}

// For quicker full rebuild of a storage (instead of an incremental one), you may add all your
// contents and then sort once.
void AnchorStorage::BuildSortByKey()
{
  struct StaticFunc
  {
    static int ANCHOR_CDECL PairCompareByID(const void *lhs, const void *rhs)
    {
      // We can't just do a subtraction because qsort uses signed integers and subtracting our ID
      // doesn't play well with that.
      if (((const AnchorStoragePair *)lhs)->key > ((const AnchorStoragePair *)rhs)->key)
        return +1;
      if (((const AnchorStoragePair *)lhs)->key < ((const AnchorStoragePair *)rhs)->key)
        return -1;
      return 0;
    }
  };
  if (Data.Size > 1)
    ImQsort(Data.Data, (size_t)Data.Size, sizeof(AnchorStoragePair), StaticFunc::PairCompareByID);
}

int AnchorStorage::GetInt(ANCHOR_ID key, int default_val) const
{
  AnchorStoragePair *it = LowerBound(const_cast<AnchorVector<AnchorStoragePair> &>(Data), key);
  if (it == Data.end() || it->key != key)
    return default_val;
  return it->val_i;
}

bool AnchorStorage::GetBool(ANCHOR_ID key, bool default_val) const
{
  return GetInt(key, default_val ? 1 : 0) != 0;
}

float AnchorStorage::GetFloat(ANCHOR_ID key, float default_val) const
{
  AnchorStoragePair *it = LowerBound(const_cast<AnchorVector<AnchorStoragePair> &>(Data), key);
  if (it == Data.end() || it->key != key)
    return default_val;
  return it->val_f;
}

void *AnchorStorage::GetVoidPtr(ANCHOR_ID key) const
{
  AnchorStoragePair *it = LowerBound(const_cast<AnchorVector<AnchorStoragePair> &>(Data), key);
  if (it == Data.end() || it->key != key)
    return NULL;
  return it->val_p;
}

// References are only valid until a new value is added to the storage. Calling a Set***() function
// or a Get***Ref() function invalidates the pointer.
int *AnchorStorage::GetIntRef(ANCHOR_ID key, int default_val)
{
  AnchorStoragePair *it = LowerBound(Data, key);
  if (it == Data.end() || it->key != key)
    it = Data.insert(it, AnchorStoragePair(key, default_val));
  return &it->val_i;
}

bool *AnchorStorage::GetBoolRef(ANCHOR_ID key, bool default_val)
{
  return (bool *)GetIntRef(key, default_val ? 1 : 0);
}

float *AnchorStorage::GetFloatRef(ANCHOR_ID key, float default_val)
{
  AnchorStoragePair *it = LowerBound(Data, key);
  if (it == Data.end() || it->key != key)
    it = Data.insert(it, AnchorStoragePair(key, default_val));
  return &it->val_f;
}

void **AnchorStorage::GetVoidPtrRef(ANCHOR_ID key, void *default_val)
{
  AnchorStoragePair *it = LowerBound(Data, key);
  if (it == Data.end() || it->key != key)
    it = Data.insert(it, AnchorStoragePair(key, default_val));
  return &it->val_p;
}

// FIXME-OPT: Need a way to reuse the result of lower_bound when doing GetInt()/SetInt() - not too
// bad because it only happens on explicit interaction (maximum one a frame)
void AnchorStorage::SetInt(ANCHOR_ID key, int val)
{
  AnchorStoragePair *it = LowerBound(Data, key);
  if (it == Data.end() || it->key != key) {
    Data.insert(it, AnchorStoragePair(key, val));
    return;
  }
  it->val_i = val;
}

void AnchorStorage::SetBool(ANCHOR_ID key, bool val)
{
  SetInt(key, val ? 1 : 0);
}

void AnchorStorage::SetFloat(ANCHOR_ID key, float val)
{
  AnchorStoragePair *it = LowerBound(Data, key);
  if (it == Data.end() || it->key != key) {
    Data.insert(it, AnchorStoragePair(key, val));
    return;
  }
  it->val_f = val;
}

void AnchorStorage::SetVoidPtr(ANCHOR_ID key, void *val)
{
  AnchorStoragePair *it = LowerBound(Data, key);
  if (it == Data.end() || it->key != key) {
    Data.insert(it, AnchorStoragePair(key, val));
    return;
  }
  it->val_p = val;
}

void AnchorStorage::SetAllInt(int v)
{
  for (int i = 0; i < Data.Size; i++)
    Data[i].val_i = v;
}

//-----------------------------------------------------------------------------
// [SECTION] AnchorTextFilter
//-----------------------------------------------------------------------------

// Helper: Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
AnchorTextFilter::AnchorTextFilter(const char *default_filter)
{
  if (default_filter) {
    AnchorStrncpy(InputBuf, default_filter, ANCHOR_ARRAYSIZE(InputBuf));
    Build();
  } else {
    InputBuf[0] = 0;
    CountGrep = 0;
  }
}

bool AnchorTextFilter::Draw(const char *label, float width)
{
  if (width != 0.0f)
    ANCHOR::SetNextItemWidth(width);
  bool value_changed = ANCHOR::InputText(label, InputBuf, ANCHOR_ARRAYSIZE(InputBuf));
  if (value_changed)
    Build();
  return value_changed;
}

void AnchorTextFilter::ANCHORTextRange::split(char separator,
                                              AnchorVector<ANCHORTextRange> *out) const
{
  out->resize(0);
  const char *wb = b;
  const char *we = wb;
  while (we < e) {
    if (*we == separator) {
      out->push_back(ANCHORTextRange(wb, we));
      wb = we + 1;
    }
    we++;
  }
  if (wb != we)
    out->push_back(ANCHORTextRange(wb, we));
}

void AnchorTextFilter::Build()
{
  Filters.resize(0);
  ANCHORTextRange input_range(InputBuf, InputBuf + strlen(InputBuf));
  input_range.split(',', &Filters);

  CountGrep = 0;
  for (int i = 0; i != Filters.Size; i++) {
    ANCHORTextRange &f = Filters[i];
    while (f.b < f.e && AnchorCharIsBlankA(f.b[0]))
      f.b++;
    while (f.e > f.b && AnchorCharIsBlankA(f.e[-1]))
      f.e--;
    if (f.empty())
      continue;
    if (Filters[i].b[0] != '-')
      CountGrep += 1;
  }
}

bool AnchorTextFilter::PassFilter(const char *text, const char *text_end) const
{
  if (Filters.empty())
    return true;

  if (text == NULL)
    text = "";

  for (int i = 0; i != Filters.Size; i++) {
    const ANCHORTextRange &f = Filters[i];
    if (f.empty())
      continue;
    if (f.b[0] == '-') {
      // Subtract
      if (AnchorStristr(text, text_end, f.b + 1, f.e) != NULL)
        return false;
    } else {
      // Grep
      if (AnchorStristr(text, text_end, f.b, f.e) != NULL)
        return true;
    }
  }

  // Implicit * grep
  if (CountGrep == 0)
    return true;

  return false;
}

//-----------------------------------------------------------------------------
// [SECTION] AnchorTextBuffer
//-----------------------------------------------------------------------------

// On some platform vsnprintf() takes va_list by reference and modifies it.
// va_copy is the 'correct' way to copy a va_list but Visual Studio prior to 2013 doesn't have it.
#ifndef va_copy
#  if defined(__GNUC__) || defined(__clang__)
#    define va_copy(dest, src) __builtin_va_copy(dest, src)
#  else
#    define va_copy(dest, src) (dest = src)
#  endif
#endif

char AnchorTextBuffer::EmptyString[1] = {0};

void AnchorTextBuffer::append(const char *str, const char *str_end)
{
  int len = str_end ? (int)(str_end - str) : (int)strlen(str);

  // Add zero-terminator the first time
  const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
  const int needed_sz = write_off + len;
  if (write_off + len >= Buf.Capacity) {
    int new_capacity = Buf.Capacity * 2;
    Buf.reserve(needed_sz > new_capacity ? needed_sz : new_capacity);
  }

  Buf.resize(needed_sz);
  memcpy(&Buf[write_off - 1], str, (size_t)len);
  Buf[write_off - 1 + len] = 0;
}

void AnchorTextBuffer::appendf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  appendfv(fmt, args);
  va_end(args);
}

// Helper: Text buffer for logging/accumulating text
void AnchorTextBuffer::appendfv(const char *fmt, va_list args)
{
  va_list args_copy;
  va_copy(args_copy, args);

  int len = AnchorFormatStringV(
    NULL,
    0,
    fmt,
    args);  // FIXME-OPT: could do a first pass write attempt, likely successful on first pass.
  if (len <= 0) {
    va_end(args_copy);
    return;
  }

  // Add zero-terminator the first time
  const int write_off = (Buf.Size != 0) ? Buf.Size : 1;
  const int needed_sz = write_off + len;
  if (write_off + len >= Buf.Capacity) {
    int new_capacity = Buf.Capacity * 2;
    Buf.reserve(needed_sz > new_capacity ? needed_sz : new_capacity);
  }

  Buf.resize(needed_sz);
  AnchorFormatStringV(&Buf[write_off - 1], (size_t)len + 1, fmt, args_copy);
  va_end(args_copy);
}

//-----------------------------------------------------------------------------
// [SECTION] AnchorListClipper
// This is currently not as flexible/powerful as it should be and really confusing/spaghetti,
// mostly because we changed the API mid-way through development and support two ways to using the
// clipper, needs some rework (see TODO)
//-----------------------------------------------------------------------------

// FIXME-TABLE: This prevents us from using AnchorListClipper _inside_ a table cell.
// The problem we have is that without a Begin/End scheme for rows using the clipper is ambiguous.
static bool GetSkipItemForListClipping()
{
  AnchorContext &g = *G_CTX;
  return (g.CurrentTable ? g.CurrentTable->HostSkipItems : g.CurrentWindow->SkipItems);
}

// Helper to calculate coarse clipping of large list of evenly sized items.
// NB: Prefer using the AnchorListClipper higher-level helper if you can! Read comments and
// instructions there on how those use this sort of pattern. NB: 'items_count' is only used to
// clamp the result, if you don't know your count you can use INT_MAX
void ANCHOR::CalcListClipping(int items_count,
                              float items_height,
                              int *out_items_display_start,
                              int *out_items_display_end)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  if (g.LogEnabled) {
    // If logging is active, do not perform any clipping
    *out_items_display_start = 0;
    *out_items_display_end = items_count;
    return;
  }
  if (GetSkipItemForListClipping()) {
    *out_items_display_start = *out_items_display_end = 0;
    return;
  }

  // We create the union of the ClipRect and the NavScoringRect which at worst should be 1 page
  // away from ClipRect
  AnchorBBox unclipped_rect = window->ClipRect;
  if (g.NavMoveRequest)
    unclipped_rect.Add(g.NavScoringRect);
  if (g.NavJustMovedToId && window->NavLastIds[0] == g.NavJustMovedToId)
    unclipped_rect.Add(AnchorBBox(window->Pos + window->NavRectRel[0].Min,
                                  window->Pos + window->NavRectRel[0].Max));

  const wabi::GfVec2f pos = window->DC.CursorPos;
  int start = (int)((unclipped_rect.Min[1] - pos[1]) / items_height);
  int end = (int)((unclipped_rect.Max[1] - pos[1]) / items_height);

  // When performing a navigation request, ensure we have one item extra in the direction we are
  // moving to
  if (g.NavMoveRequest && g.NavMoveClipDir == AnchorDir_Up)
    start--;
  if (g.NavMoveRequest && g.NavMoveClipDir == AnchorDir_Down)
    end++;

  start = AnchorClamp(start, 0, items_count);
  end = AnchorClamp(end + 1, start, items_count);
  *out_items_display_start = start;
  *out_items_display_end = end;
}

static void SetCursorPosYAndSetupForPrevLine(float pos_y, float line_height)
{
  // Set cursor position and a few other things so that SetScrollHereY() and Columns() can work
  // when seeking cursor.
  // FIXME: It is problematic that we have to do that here, because custom/equivalent end-user code
  // would stumble on the same issue. The clipper should probably have a 4th step to display the
  // last item in a regular manner.
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  float off_y = pos_y - window->DC.CursorPos[1];
  window->DC.CursorPos[1] = pos_y;
  window->DC.CursorMaxPos[1] = AnchorMax(window->DC.CursorMaxPos[1], pos_y);
  window->DC.CursorPosPrevLine[1] =
    window->DC.CursorPos[1] -
    line_height;  // Setting those fields so that SetScrollHereY() can
                  // properly function after the end of our clipper usage.
  window->DC.PrevLineSize[1] =
    (line_height -
     g.Style.ItemSpacing[1]);  // If we end up needing more accurate data (to e.g. use
                               // SameLine) we may as well make the clipper have a fourth step
                               // to let user process and display the last item in their list.
  if (AnchorOldColumns *columns = window->DC.CurrentColumns)
    columns->LineMinY =
      window->DC.CursorPos[1];  // Setting this so that cell Y position are set properly
  if (AnchorTable *table = g.CurrentTable) {
    if (table->IsInsideRow)
      ANCHOR::TableEndRow(table);
    table->RowPosY2 = window->DC.CursorPos[1];
    const int row_increase = (int)((off_y / line_height) + 0.5f);
    // table->CurrentRow += row_increase; // Can't do without fixing TableEndRow()
    table->RowBgColorCounter += row_increase;
  }
}

AnchorListClipper::AnchorListClipper()
{
  memset(this, 0, sizeof(*this));
  ItemsCount = -1;
}

AnchorListClipper::~AnchorListClipper()
{
  ANCHOR_ASSERT(ItemsCount == -1 && "Forgot to call End(), or to Step() until false?");
}

// Use case A: Begin() called from constructor with items_height<0, then called again from Step()
// in StepNo 1 Use case B: Begin() called from constructor with items_height>0
// FIXME-LEGACY: Ideally we should remove the Begin/End functions but they are part of the legacy
// API we still support. This is why some of the code in Step() calling Begin() and reassign some
// fields, spaghetti style.
void AnchorListClipper::Begin(int items_count, float items_height)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  if (AnchorTable *table = g.CurrentTable)
    if (table->IsInsideRow)
      ANCHOR::TableEndRow(table);

  StartPosY = window->DC.CursorPos[1];
  ItemsHeight = items_height;
  ItemsCount = items_count;
  ItemsFrozen = 0;
  StepNo = 0;
  DisplayStart = -1;
  DisplayEnd = 0;
}

void AnchorListClipper::End()
{
  if (ItemsCount < 0)  // Already ended
    return;

  // In theory here we should assert that ANCHOR::GetCursorPosY() == StartPosY + DisplayEnd *
  // ItemsHeight, but it feels saner to just seek at the end and not assert/crash the user.
  if (ItemsCount < INT_MAX && DisplayStart >= 0)
    SetCursorPosYAndSetupForPrevLine(StartPosY + (ItemsCount - ItemsFrozen) * ItemsHeight,
                                     ItemsHeight);
  ItemsCount = -1;
  StepNo = 3;
}

bool AnchorListClipper::Step()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  AnchorTable *table = g.CurrentTable;
  if (table && table->IsInsideRow)
    ANCHOR::TableEndRow(table);

  // No items
  if (ItemsCount == 0 || GetSkipItemForListClipping()) {
    End();
    return false;
  }

  // Step 0: Let you process the first element (regardless of it being visible or not, so we can
  // measure the element height)
  if (StepNo == 0) {
    // While we are in frozen row state, keep displaying items one by one, unclipped
    // FIXME: Could be stored as a table-agnostic state.
    if (table != NULL && !table->IsUnfrozenRows) {
      DisplayStart = ItemsFrozen;
      DisplayEnd = ItemsFrozen + 1;
      ItemsFrozen++;
      return true;
    }

    StartPosY = window->DC.CursorPos[1];
    if (ItemsHeight <= 0.0f) {
      // Submit the first item so we can measure its height (generally it is 0..1)
      DisplayStart = ItemsFrozen;
      DisplayEnd = ItemsFrozen + 1;
      StepNo = 1;
      return true;
    }

    // Already has item height (given by user in Begin): skip to calculating step
    DisplayStart = DisplayEnd;
    StepNo = 2;
  }

  // Step 1: the clipper infer height from first element
  if (StepNo == 1) {
    ANCHOR_ASSERT(ItemsHeight <= 0.0f);
    if (table) {
      const float pos_y1 = table->RowPosY1;  // Using this instead of StartPosY to handle clipper
                                             // straddling the frozen row
      const float pos_y2 =
        table->RowPosY2;  // Using this instead of CursorPos[1] to take account of tallest cell.
      ItemsHeight = pos_y2 - pos_y1;
      window->DC.CursorPos[1] = pos_y2;
    } else {
      ItemsHeight = window->DC.CursorPos[1] - StartPosY;
    }
    ANCHOR_ASSERT(
      ItemsHeight > 0.0f &&
      "Unable to calculate item height! First item hasn't moved the cursor vertically!");
    StepNo = 2;
  }

  // Reached end of list
  if (DisplayEnd >= ItemsCount) {
    End();
    return false;
  }

  // Step 2: calculate the actual range of elements to display, and position the cursor before the
  // first element
  if (StepNo == 2) {
    ANCHOR_ASSERT(ItemsHeight > 0.0f);

    int already_submitted = DisplayEnd;
    ANCHOR::CalcListClipping(ItemsCount - already_submitted,
                             ItemsHeight,
                             &DisplayStart,
                             &DisplayEnd);
    DisplayStart += already_submitted;
    DisplayEnd += already_submitted;

    // Seek cursor
    if (DisplayStart > already_submitted)
      SetCursorPosYAndSetupForPrevLine(StartPosY + (DisplayStart - ItemsFrozen) * ItemsHeight,
                                       ItemsHeight);

    StepNo = 3;
    return true;
  }

  // Step 3: the clipper validate that we have reached the expected Y position (corresponding to
  // element DisplayEnd), Advance the cursor to the end of the list and then returns 'false' to end
  // the loop.
  if (StepNo == 3) {
    // Seek cursor
    if (ItemsCount < INT_MAX)
      SetCursorPosYAndSetupForPrevLine(StartPosY + (ItemsCount - ItemsFrozen) * ItemsHeight,
                                       ItemsHeight);  // advance cursor
    ItemsCount = -1;
    return false;
  }

  ANCHOR_ASSERT(0);
  return false;
}

//-----------------------------------------------------------------------------
// [SECTION] STYLING
//-----------------------------------------------------------------------------

AnchorStyle &ANCHOR::GetStyle()
{
  ANCHOR_ASSERT(G_CTX != NULL &&
                "No current context. Did you call ANCHOR::CreateContext() and "
                "ANCHOR::SetCurrentContext() ?");
  return G_CTX->Style;
}

AnchorU32 ANCHOR::GetColorU32(AnchorCol idx, float alpha_mul)
{
  AnchorStyle &style = G_CTX->Style;
  wabi::GfVec4f c = style.Colors[idx];
  c[3] *= style.Alpha * alpha_mul;
  return ColorConvertFloat4ToU32(c);
}

AnchorU32 ANCHOR::GetColorU32(const wabi::GfVec4f &col)
{
  AnchorStyle &style = G_CTX->Style;
  wabi::GfVec4f c = col;
  c[3] *= style.Alpha;
  return ColorConvertFloat4ToU32(c);
}

const wabi::GfVec4f &ANCHOR::GetStyleColorVec4(AnchorCol idx)
{
  AnchorStyle &style = G_CTX->Style;
  return style.Colors[idx];
}

AnchorU32 ANCHOR::GetColorU32(AnchorU32 col)
{
  AnchorStyle &style = G_CTX->Style;
  if (style.Alpha >= 1.0f)
    return col;
  AnchorU32 a = (col & ANCHOR_COL32_A_MASK) >> ANCHOR_COL32_A_SHIFT;
  a = (AnchorU32)(a * style.Alpha);  // We don't need to clamp 0..255 because Style.Alpha is in
                                     // 0..1 range.
  return (col & ~ANCHOR_COL32_A_MASK) | (a << ANCHOR_COL32_A_SHIFT);
}

// FIXME: This may incur a round-trip (if the end user got their data from a float4) but eventually
// we aim to store the in-flight colors as AnchorU32
void ANCHOR::PushStyleColor(AnchorCol idx, AnchorU32 col)
{
  AnchorContext &g = *G_CTX;
  AnchorColorMod backup;
  backup.Col = idx;
  backup.BackupValue = g.Style.Colors[idx];
  g.ColorStack.push_back(backup);
  g.Style.Colors[idx] = ColorConvertU32ToFloat4(col);
}

void ANCHOR::PushStyleColor(AnchorCol idx, const wabi::GfVec4f &col)
{
  AnchorContext &g = *G_CTX;
  AnchorColorMod backup;
  backup.Col = idx;
  backup.BackupValue = g.Style.Colors[idx];
  g.ColorStack.push_back(backup);
  g.Style.Colors[idx] = col;
}

void ANCHOR::PopStyleColor(int count)
{
  AnchorContext &g = *G_CTX;
  while (count > 0) {
    AnchorColorMod &backup = g.ColorStack.back();
    g.Style.Colors[backup.Col] = backup.BackupValue;
    g.ColorStack.pop_back();
    count--;
  }
}

struct AnchorStyleVarInfo
{
  AnchorDataType Type;
  AnchorU32 Count;
  AnchorU32 Offset;
  void *GetVarPtr(AnchorStyle *style) const
  {
    return (void *)((unsigned char *)style + Offset);
  }
};

static const AnchorStyleVarInfo GStyleVarInfo[] = {
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, Alpha)           }, // AnchorStyleVar_Alpha
  {AnchorDataType_Float,
   2, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, WindowPadding)   }, // AnchorStyleVar_WindowPadding
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, WindowRounding)  }, // AnchorStyleVar_WindowRounding
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, WindowBorderSize)}, // AnchorStyleVar_WindowBorderSize
  {AnchorDataType_Float,
   2, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, WindowMinSize)   }, // AnchorStyleVar_WindowMinSize
  {AnchorDataType_Float,
   2, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, WindowTitleAlign)}, // AnchorStyleVar_WindowTitleAlign
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, ChildRounding)   }, // AnchorStyleVar_ChildRounding
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, ChildBorderSize) }, // AnchorStyleVar_ChildBorderSize
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, PopupRounding)   }, // AnchorStyleVar_PopupRounding
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, PopupBorderSize) }, // AnchorStyleVar_PopupBorderSize
  {AnchorDataType_Float,
   2, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, FramePadding)    }, // AnchorStyleVar_FramePadding
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, FrameRounding)   }, // AnchorStyleVar_FrameRounding
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, FrameBorderSize) }, // AnchorStyleVar_FrameBorderSize
  {AnchorDataType_Float,
   2, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, ItemSpacing)     }, // AnchorStyleVar_ItemSpacing
  {AnchorDataType_Float,
   2, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, ItemInnerSpacing)}, // AnchorStyleVar_ItemInnerSpacing
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, IndentSpacing)   }, // AnchorStyleVar_IndentSpacing
  {AnchorDataType_Float,
   2, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, CellPadding)     }, // AnchorStyleVar_CellPadding
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, ScrollbarSize)   }, // AnchorStyleVar_ScrollbarSize
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle,
   ScrollbarRounding)                                          }, // AnchorStyleVar_ScrollbarRounding
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, GrabMinSize)     }, // AnchorStyleVar_GrabMinSize
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, GrabRounding)    }, // AnchorStyleVar_GrabRounding
  {AnchorDataType_Float,
   1, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, TabRounding)     }, // AnchorStyleVar_TabRounding
  {AnchorDataType_Float,
   2, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle, ButtonTextAlign) }, // AnchorStyleVar_ButtonTextAlign
  {AnchorDataType_Float,
   2, (AnchorU32)ANCHOR_OFFSETOF(AnchorStyle,
   SelectableTextAlign)                                        }, // AnchorStyleVar_SelectableTextAlign
};

static const AnchorStyleVarInfo *GetStyleVarInfo(AnchorStyleVar idx)
{
  ANCHOR_ASSERT(idx >= 0 && idx < AnchorStyleVar_COUNT);
  ANCHOR_ASSERT(ANCHOR_ARRAYSIZE(GStyleVarInfo) == AnchorStyleVar_COUNT);
  return &GStyleVarInfo[idx];
}

void ANCHOR::PushStyleVar(AnchorStyleVar idx, float val)
{
  const AnchorStyleVarInfo *var_info = GetStyleVarInfo(idx);
  if (var_info->Type == AnchorDataType_Float && var_info->Count == 1) {
    AnchorContext &g = *G_CTX;
    float *pvar = (float *)var_info->GetVarPtr(&g.Style);
    g.StyleVarStack.push_back(AnchorStyleMod(idx, *pvar));
    *pvar = val;
    return;
  }
  ANCHOR_ASSERT(0 && "Called PushStyleVar() float variant but variable is not a float!");
}

void ANCHOR::PushStyleVar(AnchorStyleVar idx, const wabi::GfVec2f &val)
{
  const AnchorStyleVarInfo *var_info = GetStyleVarInfo(idx);
  if (var_info->Type == AnchorDataType_Float && var_info->Count == 2) {
    AnchorContext &g = *G_CTX;
    wabi::GfVec2f *pvar = (wabi::GfVec2f *)var_info->GetVarPtr(&g.Style);
    g.StyleVarStack.push_back(AnchorStyleMod(idx, *pvar));
    *pvar = val;
    return;
  }
  ANCHOR_ASSERT(0 && "Called PushStyleVar() wabi::GfVec2f variant but variable is not a wabi::GfVec2f!");
}

void ANCHOR::PopStyleVar(int count)
{
  AnchorContext &g = *G_CTX;
  while (count > 0) {
    // We avoid a generic memcpy(data, &backup.Backup.., GDataTypeSize[info->Type] * info->Count),
    // the overhead in Debug is not worth it.
    AnchorStyleMod &backup = g.StyleVarStack.back();
    const AnchorStyleVarInfo *info = GetStyleVarInfo(backup.VarIdx);
    void *data = info->GetVarPtr(&g.Style);
    if (info->Type == AnchorDataType_Float && info->Count == 1) {
      ((float *)data)[0] = backup.BackupFloat[0];
    } else if (info->Type == AnchorDataType_Float && info->Count == 2) {
      ((float *)data)[0] = backup.BackupFloat[0];
      ((float *)data)[1] = backup.BackupFloat[1];
    }
    g.StyleVarStack.pop_back();
    count--;
  }
}

const char *ANCHOR::GetStyleColorName(AnchorCol idx)
{
  // Create switch-case from enum with regexp: AnchorCol_{.*}, --> case AnchorCol_\1: return
  // "\1";
  switch (idx) {
    case AnchorCol_Text:
      return "Text";
    case AnchorCol_TextDisabled:
      return "TextDisabled";
    case AnchorCol_WindowBg:
      return "WindowBg";
    case AnchorCol_ChildBg:
      return "ChildBg";
    case AnchorCol_PopupBg:
      return "PopupBg";
    case AnchorCol_Border:
      return "Border";
    case AnchorCol_BorderShadow:
      return "BorderShadow";
    case AnchorCol_FrameBg:
      return "FrameBg";
    case AnchorCol_FrameBgHovered:
      return "FrameBgHovered";
    case AnchorCol_FrameBgActive:
      return "FrameBgActive";
    case AnchorCol_TitleBg:
      return "TitleBg";
    case AnchorCol_TitleBgActive:
      return "TitleBgActive";
    case AnchorCol_TitleBgCollapsed:
      return "TitleBgCollapsed";
    case AnchorCol_MenuBarBg:
      return "MenuBarBg";
    case AnchorCol_ScrollbarBg:
      return "ScrollbarBg";
    case AnchorCol_ScrollbarGrab:
      return "ScrollbarGrab";
    case AnchorCol_ScrollbarGrabHovered:
      return "ScrollbarGrabHovered";
    case AnchorCol_ScrollbarGrabActive:
      return "ScrollbarGrabActive";
    case AnchorCol_CheckMark:
      return "CheckMark";
    case AnchorCol_SliderGrab:
      return "SliderGrab";
    case AnchorCol_SliderGrabActive:
      return "SliderGrabActive";
    case AnchorCol_Button:
      return "Button";
    case AnchorCol_ButtonHovered:
      return "ButtonHovered";
    case AnchorCol_ButtonActive:
      return "ButtonActive";
    case AnchorCol_Header:
      return "Header";
    case AnchorCol_HeaderHovered:
      return "HeaderHovered";
    case AnchorCol_HeaderActive:
      return "HeaderActive";
    case AnchorCol_Separator:
      return "Separator";
    case AnchorCol_SeparatorHovered:
      return "SeparatorHovered";
    case AnchorCol_SeparatorActive:
      return "SeparatorActive";
    case AnchorCol_ResizeGrip:
      return "ResizeGrip";
    case AnchorCol_ResizeGripHovered:
      return "ResizeGripHovered";
    case AnchorCol_ResizeGripActive:
      return "ResizeGripActive";
    case AnchorCol_Tab:
      return "Tab";
    case AnchorCol_TabHovered:
      return "TabHovered";
    case AnchorCol_TabActive:
      return "TabActive";
    case AnchorCol_TabUnfocused:
      return "TabUnfocused";
    case AnchorCol_TabUnfocusedActive:
      return "TabUnfocusedActive";
    case AnchorCol_PlotLines:
      return "PlotLines";
    case AnchorCol_PlotLinesHovered:
      return "PlotLinesHovered";
    case AnchorCol_PlotHistogram:
      return "PlotHistogram";
    case AnchorCol_PlotHistogramHovered:
      return "PlotHistogramHovered";
    case AnchorCol_TableHeaderBg:
      return "TableHeaderBg";
    case AnchorCol_TableBorderStrong:
      return "TableBorderStrong";
    case AnchorCol_TableBorderLight:
      return "TableBorderLight";
    case AnchorCol_TableRowBg:
      return "TableRowBg";
    case AnchorCol_TableRowBgAlt:
      return "TableRowBgAlt";
    case AnchorCol_TextSelectedBg:
      return "TextSelectedBg";
    case AnchorCol_DragDropTarget:
      return "DragDropTarget";
    case AnchorCol_NavHighlight:
      return "NavHighlight";
    case AnchorCol_NavWindowingHighlight:
      return "NavWindowingHighlight";
    case AnchorCol_NavWindowingDimBg:
      return "NavWindowingDimBg";
    case AnchorCol_ModalWindowDimBg:
      return "ModalWindowDimBg";
  }
  ANCHOR_ASSERT(0);
  return "Unknown";
}

//-----------------------------------------------------------------------------
// [SECTION] RENDER HELPERS
// Some of those (internal) functions are currently quite a legacy mess - their signature and
// behavior will change, we need a nicer separation between low-level functions and high-level
// functions relying on the ANCHOR context. Also see ANCHOR_draw.cpp for some more which have been
// reworked to not rely on ANCHOR:: context.
//-----------------------------------------------------------------------------

const char *ANCHOR::FindRenderedTextEnd(const char *text, const char *text_end)
{
  const char *text_display_end = text;
  if (!text_end)
    text_end = (const char *)-1;

  while (text_display_end < text_end && *text_display_end != '\0' &&
         (text_display_end[0] != '#' || text_display_end[1] != '#'))
    text_display_end++;
  return text_display_end;
}

// Internal ANCHOR functions to render text
// RenderText***() functions calls AnchorDrawList::AddText() calls AnchorBitmapFont::RenderText()
void ANCHOR::RenderText(wabi::GfVec2f pos,
                        const char *text,
                        const char *text_end,
                        bool hide_text_after_hash)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  // Hide anything after a '##' string
  const char *text_display_end;
  if (hide_text_after_hash) {
    text_display_end = FindRenderedTextEnd(text, text_end);
  } else {
    if (!text_end)
      text_end = text + strlen(text);  // FIXME-OPT
    text_display_end = text_end;
  }

  if (text != text_display_end) {
    window->DrawList
      ->AddText(g.Font, g.FontSize, pos, GetColorU32(AnchorCol_Text), text, text_display_end);
    if (g.LogEnabled)
      LogRenderedText(&pos, text, text_display_end);
  }
}

void ANCHOR::RenderTextWrapped(wabi::GfVec2f pos,
                               const char *text,
                               const char *text_end,
                               float wrap_width)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  if (!text_end)
    text_end = text + strlen(text);  // FIXME-OPT

  if (text != text_end) {
    window->DrawList
      ->AddText(g.Font, g.FontSize, pos, GetColorU32(AnchorCol_Text), text, text_end, wrap_width);
    if (g.LogEnabled)
      LogRenderedText(&pos, text, text_end);
  }
}

// Default clip_rect uses (pos_min,pos_max)
// Handle clipping on CPU immediately (vs typically let the GPU clip the triangles that are
// overlapping the clipping rectangle edges)
void ANCHOR::RenderTextClippedEx(AnchorDrawList *draw_list,
                                 const wabi::GfVec2f &pos_min,
                                 const wabi::GfVec2f &pos_max,
                                 const char *text,
                                 const char *text_display_end,
                                 const wabi::GfVec2f *text_size_if_known,
                                 const wabi::GfVec2f &align,
                                 const AnchorBBox *clip_rect)
{
  // Perform CPU side clipping for single clipped element to avoid using scissor state
  wabi::GfVec2f pos = pos_min;
  const wabi::GfVec2f text_size = text_size_if_known ? *text_size_if_known :
                                                 CalcTextSize(text, text_display_end, false, 0.0f);

  const wabi::GfVec2f *clip_min = clip_rect ? &clip_rect->Min : &pos_min;
  const wabi::GfVec2f *clip_max = clip_rect ? &clip_rect->Max : &pos_max;
  bool need_clipping = (pos[0] + text_size[0] >= clip_max->GetArray()[0]) ||
                       (pos[1] + text_size[1] >= clip_max->GetArray()[1]);
  if (clip_rect)  // If we had no explicit clipping rectangle then pos==clip_min
    need_clipping |= (pos[0] < clip_min->GetArray()[0]) || (pos[1] < clip_min->GetArray()[1]);

  // Align whole block. We should defer that to the better rendering function when we'll have
  // support for individual line alignment.
  if (align[0] > 0.0f)
    pos[0] = AnchorMax(pos[0], pos[0] + (pos_max[0] - pos[0] - text_size[0]) * align[0]);
  if (align[1] > 0.0f)
    pos[1] = AnchorMax(pos[1], pos[1] + (pos_max[1] - pos[1] - text_size[1]) * align[1]);

  // Render
  if (need_clipping) {
    wabi::GfVec4f fine_clip_rect(clip_min->GetArray()[0],
                           clip_min->GetArray()[1],
                           clip_max->GetArray()[0],
                           clip_max->GetArray()[1]);
    draw_list->AddText(NULL,
                       0.0f,
                       pos,
                       GetColorU32(AnchorCol_Text),
                       text,
                       text_display_end,
                       0.0f,
                       &fine_clip_rect);
  } else {
    draw_list
      ->AddText(NULL, 0.0f, pos, GetColorU32(AnchorCol_Text), text, text_display_end, 0.0f, NULL);
  }
}

void ANCHOR::RenderTextClipped(const wabi::GfVec2f &pos_min,
                               const wabi::GfVec2f &pos_max,
                               const char *text,
                               const char *text_end,
                               const wabi::GfVec2f *text_size_if_known,
                               const wabi::GfVec2f &align,
                               const AnchorBBox *clip_rect)
{
  // Hide anything after a '##' string
  const char *text_display_end = FindRenderedTextEnd(text, text_end);
  const int text_len = (int)(text_display_end - text);
  if (text_len == 0)
    return;

  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  RenderTextClippedEx(window->DrawList,
                      pos_min,
                      pos_max,
                      text,
                      text_display_end,
                      text_size_if_known,
                      align,
                      clip_rect);
  if (g.LogEnabled)
    LogRenderedText(&pos_min, text, text_display_end);
}

// Another overly complex function until we reorganize everything into a nice all-in-one helper.
// This is made more complex because we have dissociated the layout rectangle (pos_min..pos_max)
// which define _where_ the ellipsis is, from actual clipping of text and limit of the ellipsis
// display. This is because in the context of tabs we selectively hide part of the text when the
// Close Button appears, but we don't want the ellipsis to move.
void ANCHOR::RenderTextEllipsis(AnchorDrawList *draw_list,
                                const wabi::GfVec2f &pos_min,
                                const wabi::GfVec2f &pos_max,
                                float clip_max_x,
                                float ellipsis_max_x,
                                const char *text,
                                const char *text_end_full,
                                const wabi::GfVec2f *text_size_if_known)
{
  AnchorContext &g = *G_CTX;
  if (text_end_full == NULL)
    text_end_full = FindRenderedTextEnd(text);
  const wabi::GfVec2f text_size = text_size_if_known ? *text_size_if_known :
                                                 CalcTextSize(text, text_end_full, false, 0.0f);

  // draw_list->AddLine(wabi::GfVec2f(pos_max[0], pos_min[1] - 4), wabi::GfVec2f(pos_max[0], pos_max[1] + 4),
  // ANCHOR_COL32(0, 0, 255, 255)); draw_list->AddLine(wabi::GfVec2f(ellipsis_max_x, pos_min[1]-2),
  // wabi::GfVec2f(ellipsis_max_x, pos_max[1]+2), ANCHOR_COL32(0, 255, 0, 255));
  // draw_list->AddLine(wabi::GfVec2f(clip_max_x, pos_min[1]), wabi::GfVec2f(clip_max_x, pos_max[1]),
  // ANCHOR_COL32(255, 0, 0, 255));
  // FIXME: We could technically remove (last_glyph->AdvanceX - last_glyph->X1) from text_size[0]
  // here and save a few pixels.
  if (text_size[0] > pos_max[0] - pos_min[0]) {
    // Hello wo...
    // |       |   |
    // min   max   ellipsis_max
    //          <-> this is generally some padding value

    const AnchorFont *font = draw_list->_Data->Font;
    const float font_size = draw_list->_Data->FontSize;
    const char *text_end_ellipsis = NULL;

    AnchorWChar ellipsis_char = font->EllipsisChar;
    int ellipsis_char_count = 1;
    if (ellipsis_char == (AnchorWChar)-1) {
      ellipsis_char = (AnchorWChar)'.';
      ellipsis_char_count = 3;
    }
    const AnchorFontGlyph *glyph = font->FindGlyph(ellipsis_char);

    float ellipsis_glyph_width = glyph->X1;  // Width of the glyph with no padding on either side
    float ellipsis_total_width = ellipsis_glyph_width;  // Full width of entire ellipsis

    if (ellipsis_char_count > 1) {
      // Full ellipsis size without free spacing after it.
      const float spacing_between_dots = 1.0f * (draw_list->_Data->FontSize / font->FontSize);
      ellipsis_glyph_width = glyph->X1 - glyph->X0 + spacing_between_dots;
      ellipsis_total_width = ellipsis_glyph_width * (float)ellipsis_char_count -
                             spacing_between_dots;
    }

    // We can now claim the space between pos_max[0] and ellipsis_max[0]
    const float text_avail_width = AnchorMax(
      (AnchorMax(pos_max[0], ellipsis_max_x) - ellipsis_total_width) - pos_min[0],
      1.0f);
    float text_size_clipped_x = font->CalcTextSizeA(font_size,
                                                    text_avail_width,
                                                    0.0f,
                                                    text,
                                                    text_end_full,
                                                    &text_end_ellipsis)[0];
    if (text == text_end_ellipsis && text_end_ellipsis < text_end_full) {
      // Always display at least 1 character if there's no room for character + ellipsis
      text_end_ellipsis = text + AnchorTextCountUtf8BytesFromChar(text, text_end_full);
      text_size_clipped_x =
        font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text, text_end_ellipsis)[0];
    }
    while (text_end_ellipsis > text && AnchorCharIsBlankA(text_end_ellipsis[-1])) {
      // Trim trailing space before ellipsis (FIXME: Supporting non-ascii blanks would be nice, for
      // this we need a function to backtrack in UTF-8 text)
      text_end_ellipsis--;
      text_size_clipped_x -= font->CalcTextSizeA(font_size,
                                                 FLT_MAX,
                                                 0.0f,
                                                 text_end_ellipsis,
                                                 text_end_ellipsis +
                                                   1)[0];  // Ascii blanks are always 1 byte
    }

    // Render text, render ellipsis
    RenderTextClippedEx(draw_list,
                        pos_min,
                        wabi::GfVec2f(clip_max_x, pos_max[1]),
                        text,
                        text_end_ellipsis,
                        &text_size,
                        wabi::GfVec2f(0.0f, 0.0f));
    float ellipsis_x = pos_min[0] + text_size_clipped_x;
    if (ellipsis_x + ellipsis_total_width <= ellipsis_max_x)
      for (int i = 0; i < ellipsis_char_count; i++) {
        font->RenderChar(draw_list,
                         font_size,
                         wabi::GfVec2f(ellipsis_x, pos_min[1]),
                         GetColorU32(AnchorCol_Text),
                         ellipsis_char);
        ellipsis_x += ellipsis_glyph_width;
      }
  } else {
    RenderTextClippedEx(draw_list,
                        pos_min,
                        wabi::GfVec2f(clip_max_x, pos_max[1]),
                        text,
                        text_end_full,
                        &text_size,
                        wabi::GfVec2f(0.0f, 0.0f));
  }

  if (g.LogEnabled)
    LogRenderedText(&pos_min, text, text_end_full);
}

// Render a rectangle shaped with optional rounding and borders
void ANCHOR::RenderFrame(wabi::GfVec2f p_min,
                         wabi::GfVec2f p_max,
                         AnchorU32 fill_col,
                         bool border,
                         float rounding)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding);
  const float border_size = g.Style.FrameBorderSize;
  if (border && border_size > 0.0f) {
    window->DrawList->AddRect(p_min + wabi::GfVec2f(1, 1),
                              p_max + wabi::GfVec2f(1, 1),
                              GetColorU32(AnchorCol_BorderShadow),
                              rounding,
                              0,
                              border_size);
    window->DrawList
      ->AddRect(p_min, p_max, GetColorU32(AnchorCol_Border), rounding, 0, border_size);
  }
}

void ANCHOR::RenderFrameBorder(wabi::GfVec2f p_min, wabi::GfVec2f p_max, float rounding)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  const float border_size = g.Style.FrameBorderSize;
  if (border_size > 0.0f) {
    window->DrawList->AddRect(p_min + wabi::GfVec2f(1, 1),
                              p_max + wabi::GfVec2f(1, 1),
                              GetColorU32(AnchorCol_BorderShadow),
                              rounding,
                              0,
                              border_size);
    window->DrawList
      ->AddRect(p_min, p_max, GetColorU32(AnchorCol_Border), rounding, 0, border_size);
  }
}

void ANCHOR::RenderNavHighlight(const AnchorBBox &bb, ANCHOR_ID id, AnchorNavHighlightFlags flags)
{
  AnchorContext &g = *G_CTX;
  if (id != g.NavId)
    return;
  if (g.NavDisableHighlight && !(flags & AnchorNavHighlightFlags_AlwaysDraw))
    return;
  AnchorWindow *window = g.CurrentWindow;
  if (window->DC.NavHideHighlightOneFrame)
    return;

  float rounding = (flags & AnchorNavHighlightFlags_NoRounding) ? 0.0f : g.Style.FrameRounding;
  AnchorBBox display_rect = bb;
  display_rect.ClipWith(window->ClipRect);
  if (flags & AnchorNavHighlightFlags_TypeDefault) {
    const float THICKNESS = 2.0f;
    const float DISTANCE = 3.0f + THICKNESS * 0.5f;
    display_rect.Expand(wabi::GfVec2f(DISTANCE, DISTANCE));
    bool fully_visible = window->ClipRect.Contains(display_rect);
    if (!fully_visible)
      window->DrawList->PushClipRect(display_rect.Min, display_rect.Max);
    window->DrawList->AddRect(display_rect.Min + wabi::GfVec2f(THICKNESS * 0.5f, THICKNESS * 0.5f),
                              display_rect.Max - wabi::GfVec2f(THICKNESS * 0.5f, THICKNESS * 0.5f),
                              GetColorU32(AnchorCol_NavHighlight),
                              rounding,
                              0,
                              THICKNESS);
    if (!fully_visible)
      window->DrawList->PopClipRect();
  }
  if (flags & AnchorNavHighlightFlags_TypeThin) {
    window->DrawList->AddRect(display_rect.Min,
                              display_rect.Max,
                              GetColorU32(AnchorCol_NavHighlight),
                              rounding,
                              0,
                              1.0f);
  }
}

//-----------------------------------------------------------------------------
// [SECTION] MAIN CODE (most of the code! lots of stuff, needs tidying up!)
//-----------------------------------------------------------------------------

// AnchorWindow is mostly a dumb struct. It merely has a constructor and a few helper methods
AnchorWindow::AnchorWindow(AnchorContext *context, const char *name) : DrawListInst(NULL)
{
  memset(this, 0, sizeof(*this));
  Name = AnchorStrdup(name);
  NameBufLen = (int)strlen(name) + 1;
  ID = AnchorHashStr(name);
  IDStack.push_back(ID);
  MoveId = GetID("#MOVE");
  ScrollTarget = wabi::GfVec2f(FLT_MAX, FLT_MAX);
  ScrollTargetCenterRatio = wabi::GfVec2f(0.5f, 0.5f);
  AutoFitFramesX = AutoFitFramesY = -1;
  AutoPosLastDirection = AnchorDir_None;
  SetWindowPosAllowFlags = SetWindowSizeAllowFlags = SetWindowCollapsedAllowFlags =
    AnchorCond_Always | AnchorCond_Once | AnchorCond_FirstUseEver | AnchorCond_Appearing;
  SetWindowPosVal = SetWindowPosPivot = wabi::GfVec2f(FLT_MAX, FLT_MAX);
  LastFrameActive = -1;
  LastTimeActive = -1.0f;
  FontWindowScale = 1.0f;
  SettingsOffset = -1;
  DrawList = &DrawListInst;
  DrawList->_Data = &context->DrawListSharedData;
  DrawList->_OwnerName = Name;
}

AnchorWindow::~AnchorWindow()
{
  ANCHOR_ASSERT(DrawList == &DrawListInst);
  ANCHOR_DELETE(Name);
  for (int i = 0; i != ColumnsStorage.Size; i++)
    ColumnsStorage[i].~AnchorOldColumns();
}

ANCHOR_ID AnchorWindow::GetID(const char *str, const char *str_end)
{
  ANCHOR_ID seed = IDStack.back();
  ANCHOR_ID id = AnchorHashStr(str, str_end ? (str_end - str) : 0, seed);
  ANCHOR::KeepAliveID(id);
#ifdef ANCHOR_ENABLE_TEST_ENGINE
  AnchorContext &g = *G_CTX;
  ANCHOR_TEST_ENGINE_ID_INFO2(id, AnchorDataType_String, str, str_end);
#endif
  return id;
}

ANCHOR_ID AnchorWindow::GetID(const void *ptr)
{
  ANCHOR_ID seed = IDStack.back();
  ANCHOR_ID id = ImHashData(&ptr, sizeof(void *), seed);
  ANCHOR::KeepAliveID(id);
#ifdef ANCHOR_ENABLE_TEST_ENGINE
  AnchorContext &g = *G_CTX;
  ANCHOR_TEST_ENGINE_ID_INFO(id, AnchorDataType_Pointer, ptr);
#endif
  return id;
}

ANCHOR_ID AnchorWindow::GetID(int n)
{
  ANCHOR_ID seed = IDStack.back();
  ANCHOR_ID id = ImHashData(&n, sizeof(n), seed);
  ANCHOR::KeepAliveID(id);
#ifdef ANCHOR_ENABLE_TEST_ENGINE
  AnchorContext &g = *G_CTX;
  ANCHOR_TEST_ENGINE_ID_INFO(id, AnchorDataType_S32, (intptr_t)n);
#endif
  return id;
}

ANCHOR_ID AnchorWindow::GetIDNoKeepAlive(const char *str, const char *str_end)
{
  ANCHOR_ID seed = IDStack.back();
  ANCHOR_ID id = AnchorHashStr(str, str_end ? (str_end - str) : 0, seed);
#ifdef ANCHOR_ENABLE_TEST_ENGINE
  AnchorContext &g = *G_CTX;
  ANCHOR_TEST_ENGINE_ID_INFO2(id, AnchorDataType_String, str, str_end);
#endif
  return id;
}

ANCHOR_ID AnchorWindow::GetIDNoKeepAlive(const void *ptr)
{
  ANCHOR_ID seed = IDStack.back();
  ANCHOR_ID id = ImHashData(&ptr, sizeof(void *), seed);
#ifdef ANCHOR_ENABLE_TEST_ENGINE
  AnchorContext &g = *G_CTX;
  ANCHOR_TEST_ENGINE_ID_INFO(id, AnchorDataType_Pointer, ptr);
#endif
  return id;
}

ANCHOR_ID AnchorWindow::GetIDNoKeepAlive(int n)
{
  ANCHOR_ID seed = IDStack.back();
  ANCHOR_ID id = ImHashData(&n, sizeof(n), seed);
#ifdef ANCHOR_ENABLE_TEST_ENGINE
  AnchorContext &g = *G_CTX;
  ANCHOR_TEST_ENGINE_ID_INFO(id, AnchorDataType_S32, (intptr_t)n);
#endif
  return id;
}

// This is only used in rare/specific situations to manufacture an ID out of nowhere.
ANCHOR_ID AnchorWindow::GetIDFromRectangle(const AnchorBBox &r_abs)
{
  ANCHOR_ID seed = IDStack.back();
  const int r_rel[4] = {(int)(r_abs.Min[0] - Pos[0]),
                        (int)(r_abs.Min[1] - Pos[1]),
                        (int)(r_abs.Max[0] - Pos[0]),
                        (int)(r_abs.Max[1] - Pos[1])};
  ANCHOR_ID id = ImHashData(&r_rel, sizeof(r_rel), seed);
  ANCHOR::KeepAliveID(id);
  return id;
}

static void SetCurrentWindow(AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;
  g.CurrentWindow = window;
  g.CurrentTable = window && window->DC.CurrentTableIdx != -1 ?
                     g.Tables.GetByIndex(window->DC.CurrentTableIdx) :
                     NULL;
  if (window)
    g.FontSize = g.DrawListSharedData.FontSize = window->CalcFontSize();
}

void ANCHOR::GcCompactTransientMiscBuffers()
{
  AnchorContext &g = *G_CTX;
  g.ItemFlagsStack.clear();
  g.GroupStack.clear();
  TableGcCompactSettings();
}

// Free up/compact internal window buffers, we can use this when a window becomes unused.
// Not freed:
// - AnchorWindow, AnchorWindowSettings, Name, StateStorage, ColumnsStorage (may hold useful
// data) This should have no noticeable visual effect. When the window reappear however, expect new
// allocation/buffer growth/copy cost.
void ANCHOR::GcCompactTransientWindowBuffers(AnchorWindow *window)
{
  window->MemoryCompacted = true;
  window->MemoryDrawListIdxCapacity = window->DrawList->IdxBuffer.Capacity;
  window->MemoryDrawListVtxCapacity = window->DrawList->VtxBuffer.Capacity;
  window->IDStack.clear();
  window->DrawList->_ClearFreeMemory();
  window->DC.ChildWindows.clear();
  window->DC.ItemWidthStack.clear();
  window->DC.TextWrapPosStack.clear();
}

void ANCHOR::GcAwakeTransientWindowBuffers(AnchorWindow *window)
{
  // We stored capacity of the AnchorDrawList buffer to reduce growth-caused allocation/copy when
  // awakening. The other buffers tends to amortize much faster.
  window->MemoryCompacted = false;
  window->DrawList->IdxBuffer.reserve(window->MemoryDrawListIdxCapacity);
  window->DrawList->VtxBuffer.reserve(window->MemoryDrawListVtxCapacity);
  window->MemoryDrawListIdxCapacity = window->MemoryDrawListVtxCapacity = 0;
}

void ANCHOR::SetActiveID(ANCHOR_ID id, AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;
  g.ActiveIdIsJustActivated = (g.ActiveId != id);
  if (g.ActiveIdIsJustActivated) {
    g.ActiveIdTimer = 0.0f;
    g.ActiveIdHasBeenPressedBefore = false;
    g.ActiveIdHasBeenEditedBefore = false;
    g.ActiveIdMouseButton = -1;
    if (id != 0) {
      g.LastActiveId = id;
      g.LastActiveIdTimer = 0.0f;
    }
  }
  g.ActiveId = id;
  g.ActiveIdAllowOverlap = false;
  g.ActiveIdNoClearOnFocusLoss = false;
  g.ActiveIdWindow = window;
  g.ActiveIdHasBeenEditedThisFrame = false;
  if (id) {
    g.ActiveIdIsAlive = id;
    g.ActiveIdSource = (g.NavActivateId == id || g.NavInputId == id || g.NavJustTabbedId == id ||
                        g.NavJustMovedToId == id) ?
                         ANCHORInputSource_Nav :
                         ANCHORInputSource_Mouse;
  }

  // Clear declaration of inputs claimed by the widget
  // (Please note that this is WIP and not all keys/inputs are thoroughly declared by all widgets
  // yet)
  g.ActiveIdUsingMouseWheel = false;
  g.ActiveIdUsingNavDirMask = 0x00;
  g.ActiveIdUsingNavInputMask = 0x00;
  g.ActiveIdUsingKeyInputMask = 0x00;
}

void ANCHOR::ClearActiveID()
{
  SetActiveID(0, NULL);  // g.ActiveId = 0;
}

void ANCHOR::SetHoveredID(ANCHOR_ID id)
{
  AnchorContext &g = *G_CTX;
  g.HoveredId = id;
  g.HoveredIdAllowOverlap = false;
  g.HoveredIdUsingMouseWheel = false;
  if (id != 0 && g.HoveredIdPreviousFrame != id)
    g.HoveredIdTimer = g.HoveredIdNotActiveTimer = 0.0f;
}

ANCHOR_ID ANCHOR::GetHoveredID()
{
  AnchorContext &g = *G_CTX;
  return g.HoveredId ? g.HoveredId : g.HoveredIdPreviousFrame;
}

void ANCHOR::KeepAliveID(ANCHOR_ID id)
{
  AnchorContext &g = *G_CTX;
  if (g.ActiveId == id)
    g.ActiveIdIsAlive = id;
  if (g.ActiveIdPreviousFrame == id)
    g.ActiveIdPreviousFrameIsAlive = true;
}

void ANCHOR::MarkItemEdited(ANCHOR_ID id)
{
  // This marking is solely to be able to provide info for IsItemDeactivatedAfterEdit().
  // ActiveId might have been released by the time we call this (as in the typical press/release
  // button behavior) but still need need to fill the data.
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(g.ActiveId == id || g.ActiveId == 0 || g.DragDropActive);
  TF_UNUSED(id);  // Avoid unused variable warnings when asserts are compiled out.
  // ANCHOR_ASSERT(g.CurrentWindow->DC.LastItemId == id);
  g.ActiveIdHasBeenEditedThisFrame = true;
  g.ActiveIdHasBeenEditedBefore = true;
  g.CurrentWindow->DC.LastItemStatusFlags |= AnchorItemStatusFlags_Edited;
}

static inline bool IsWindowContentHoverable(AnchorWindow *window, AnchorHoveredFlags flags)
{
  // An active popup disable hovering on other windows (apart from its own children)
  // FIXME-OPT: This could be cached/stored within the window.
  AnchorContext &g = *G_CTX;
  if (g.NavWindow)
    if (AnchorWindow *focused_root_window = g.NavWindow->RootWindow)
      if (focused_root_window->WasActive && focused_root_window != window->RootWindow) {
        // For the purpose of those flags we differentiate "standard popup" from "modal popup"
        // NB: The order of those two tests is important because Modal windows are also Popups.
        if (focused_root_window->Flags & AnchorWindowFlags_Modal)
          return false;
        if ((focused_root_window->Flags & AnchorWindowFlags_Popup) &&
            !(flags & AnchorHoveredFlags_AllowWhenBlockedByPopup))
          return false;
      }
  return true;
}

// This is roughly matching the behavior of internal-facing ItemHoverable()
// - we allow hovering to be true when ActiveId==window->MoveID, so that clicking on
// non-interactive items such as a Text() item still returns true with IsItemHovered()
// - this should work even for non-interactive items that have no ID, so we cannot use LastItemId
bool ANCHOR::IsItemHovered(AnchorHoveredFlags flags)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  if (g.NavDisableMouseHover && !g.NavDisableHighlight)
    return IsItemFocused();

  // Test for bounding box overlap, as updated as ItemAdd()
  AnchorItemStatusFlags status_flags = window->DC.LastItemStatusFlags;
  if (!(status_flags & AnchorItemStatusFlags_HoveredRect))
    return false;
  ANCHOR_ASSERT((flags & (AnchorHoveredFlags_RootWindow | AnchorHoveredFlags_ChildWindows)) ==
                0);  // Flags not supported by this function

  // Test if we are hovering the right window (our window could be behind another window)
  // [2021/03/02] Reworked / reverted the revert, finally. Note we want e.g.
  // BeginGroup/ItemAdd/EndGroup to work as well. (#3851) [2017/10/16] Reverted commit 344d48be3
  // and testing RootWindow instead. I believe it is correct to NOT test for RootWindow but this
  // leaves us unable to use IsItemHovered() after EndChild() itself. Until a solution is found I
  // believe reverting to the test from 2017/09/27 is safe since this was the test that has been
  // running for a long while.
  if (g.HoveredWindow != window && (status_flags & AnchorItemStatusFlags_HoveredWindow) == 0)
    if ((flags & AnchorHoveredFlags_AllowWhenOverlapped) == 0)
      return false;

  // Test if another item is active (e.g. being dragged)
  if ((flags & AnchorHoveredFlags_AllowWhenBlockedByActiveItem) == 0)
    if (g.ActiveId != 0 && g.ActiveId != window->DC.LastItemId && !g.ActiveIdAllowOverlap &&
        g.ActiveId != window->MoveId)
      return false;

  // Test if interactions on this window are blocked by an active popup or modal.
  // The AnchorHoveredFlags_AllowWhenBlockedByPopup flag will be tested here.
  if (!IsWindowContentHoverable(window, flags))
    return false;

  // Test if the item is disabled
  if ((g.CurrentItemFlags & AnchorItemFlags_Disabled) &&
      !(flags & AnchorHoveredFlags_AllowWhenDisabled))
    return false;

  // Special handling for calling after Begin() which represent the title bar or tab.
  // When the window is collapsed (SkipItems==true) that last item will never be overwritten so we
  // need to detect the case.
  if (window->DC.LastItemId == window->MoveId && window->WriteAccessed)
    return false;
  return true;
}

// Internal facing ItemHoverable() used when submitting widgets. Differs slightly from
// IsItemHovered().
bool ANCHOR::ItemHoverable(const AnchorBBox &bb, ANCHOR_ID id)
{
  AnchorContext &g = *G_CTX;
  if (g.HoveredId != 0 && g.HoveredId != id && !g.HoveredIdAllowOverlap)
    return false;

  AnchorWindow *window = g.CurrentWindow;
  if (g.HoveredWindow != window)
    return false;
  if (g.ActiveId != 0 && g.ActiveId != id && !g.ActiveIdAllowOverlap)
    return false;
  if (!IsMouseHoveringRect(bb.Min, bb.Max))
    return false;
  if (g.NavDisableMouseHover)
    return false;
  if (!IsWindowContentHoverable(window, AnchorHoveredFlags_None) ||
      (g.CurrentItemFlags & AnchorItemFlags_Disabled)) {
    g.HoveredIdDisabled = true;
    return false;
  }

  // We exceptionally allow this function to be called with id==0 to allow using it for easy
  // high-level hover test in widgets code. We could also decide to split this function is two.
  if (id != 0) {
    SetHoveredID(id);

    // [DEBUG] Item Picker tool!
    // We perform the check here because SetHoveredID() is not frequently called (1~ time a frame),
    // making the cost of this tool near-zero. We can get slightly better call-stack and support
    // picking non-hovered items if we perform the test in ItemAdd(), but that would incur a small
    // runtime cost. #define ANCHOR_DEBUG_TOOL_ITEM_PICKER_EX in ANCHOR_config.h if you want this
    // check to also be performed in ItemAdd().
    if (g.DebugItemPickerActive && g.HoveredIdPreviousFrame == id)
      GetForegroundDrawList()->AddRect(bb.Min, bb.Max, ANCHOR_COL32(255, 255, 0, 255));
    if (g.DebugItemPickerBreakId == id)
      IM_DEBUG_BREAK();
  }

  return true;
}

bool ANCHOR::IsClippedEx(const AnchorBBox &bb, ANCHOR_ID id, bool clip_even_when_logged)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  if (!bb.Overlaps(window->ClipRect))
    if (id == 0 || (id != g.ActiveId && id != g.NavId))
      if (clip_even_when_logged || !g.LogEnabled)
        return true;
  return false;
}

// This is also inlined in ItemAdd()
// Note: if AnchorItemStatusFlags_HasDisplayRect is set, user needs to set
// window->DC.LastItemDisplayRect!
void ANCHOR::SetLastItemData(AnchorWindow *window,
                             ANCHOR_ID item_id,
                             AnchorItemStatusFlags item_flags,
                             const AnchorBBox &item_rect)
{
  window->DC.LastItemId = item_id;
  window->DC.LastItemStatusFlags = item_flags;
  window->DC.LastItemRect = item_rect;
}

// Process TAB/Shift+TAB. Be mindful that this function may _clear_ the ActiveID when tabbing out.
void ANCHOR::ItemFocusable(AnchorWindow *window, ANCHOR_ID id)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(id != 0 && id == window->DC.LastItemId);

  // Increment counters
  // FIXME: AnchorItemFlags_Disabled should disable more.
  const bool is_tab_stop = (g.CurrentItemFlags &
                            (AnchorItemFlags_NoTabStop | AnchorItemFlags_Disabled)) == 0;
  window->DC.FocusCounterRegular++;
  if (is_tab_stop) {
    window->DC.FocusCounterTabStop++;
    if (g.NavId == id)
      g.NavIdTabCounter = window->DC.FocusCounterTabStop;
  }

  // Process TAB/Shift-TAB to tab *OUT* of the currently focused item.
  // (Note that we can always TAB out of a widget that doesn't allow tabbing in)
  if (g.ActiveId == id && g.TabFocusPressed && !IsActiveIdUsingKey(AnchorKey_Tab) &&
      g.TabFocusRequestNextWindow == NULL) {
    g.TabFocusRequestNextWindow = window;
    g.TabFocusRequestNextCounterTabStop =
      window->DC.FocusCounterTabStop + (g.IO.KeyShift ?
                                          (is_tab_stop ? -1 : 0) :
                                          +1);  // Modulo on index will be applied at the end of
                                                // frame once we've got the total counter of items.
  }

  // Handle focus requests
  if (g.TabFocusRequestCurrWindow == window) {
    if (window->DC.FocusCounterRegular == g.TabFocusRequestCurrCounterRegular) {
      window->DC.LastItemStatusFlags |= AnchorItemStatusFlags_FocusedByCode;
      return;
    }
    if (is_tab_stop && window->DC.FocusCounterTabStop == g.TabFocusRequestCurrCounterTabStop) {
      g.NavJustTabbedId = id;
      window->DC.LastItemStatusFlags |= AnchorItemStatusFlags_FocusedByTabbing;
      return;
    }

    // If another item is about to be focused, we clear our own active id
    if (g.ActiveId == id)
      ClearActiveID();
  }
}

float ANCHOR::CalcWrapWidthForPos(const wabi::GfVec2f &pos, float wrap_pos_x)
{
  if (wrap_pos_x < 0.0f)
    return 0.0f;

  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  if (wrap_pos_x == 0.0f) {
    // We could decide to setup a default wrapping max point for auto-resizing windows,
    // or have auto-wrap (with unspecified wrapping pos) behave as a ContentSize extending
    // function?
    // if (window->Hidden && (window->Flags & AnchorWindowFlags_AlwaysAutoResize))
    //    wrap_pos_x = AnchorMax(window->WorkRect.Min[0] + g.FontSize * 10.0f,
    //    window->WorkRect.Max[0]);
    // else
    wrap_pos_x = window->WorkRect.Max[0];
  } else if (wrap_pos_x > 0.0f) {
    wrap_pos_x += window->Pos[0] -
                  window->Scroll[0];  // wrap_pos_x is provided is window local space
  }

  return AnchorMax(wrap_pos_x - pos[0], 1.0f);
}

// ANCHOR_ALLOC() == ANCHOR::MemAlloc()
void *ANCHOR::MemAlloc(size_t size)
{
  if (AnchorContext *ctx = G_CTX)
    ctx->IO.MetricsActiveAllocations++;
  return (*GImAllocatorAllocFunc)(size, GImAllocatorUserData);
}

// ANCHOR_FREE() == ANCHOR::MemFree()
void ANCHOR::MemFree(void *ptr)
{
  if (ptr)
    if (AnchorContext *ctx = G_CTX)
      ctx->IO.MetricsActiveAllocations--;
  return (*GImAllocatorFreeFunc)(ptr, GImAllocatorUserData);
}

const char *ANCHOR::GetClipboardText()
{
  AnchorContext &g = *G_CTX;
  return g.IO.GetClipboardTextFn ? g.IO.GetClipboardTextFn(g.IO.ClipboardUserData) : "";
}

void ANCHOR::SetClipboardText(const char *text)
{
  AnchorContext &g = *G_CTX;
  if (g.IO.SetClipboardTextFn)
    g.IO.SetClipboardTextFn(g.IO.ClipboardUserData, text);
}

const char *ANCHOR::GetVersion()
{
  return ANCHOR_VERSION;
}

// Internal state access - if you want to share ANCHOR state between modules (e.g. DLL) or allocate
// it yourself Note that we still point to some static data and members (such as GFontAtlas), so
// the state instance you end up using will point to the static data within its module
AnchorContext *ANCHOR::GetCurrentContext()
{
  return G_CTX;
}

void ANCHOR::SetCurrentContext(AnchorContext *ctx)
{
  G_CTX = ctx;
}

bool ANCHOR::ProcessEvents(AnchorSystemHandle systemhandle, bool waitForEvent)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;

  return system->processEvents(waitForEvent);
}

void ANCHOR::DispatchEvents(AnchorSystemHandle systemhandle)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;

  system->dispatchEvents();
}

AnchorU64 ANCHOR::GetMilliSeconds(AnchorSystemHandle systemhandle)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;

  return system->getMilliSeconds();
}

AnchorU8 ANCHOR::GetNumDisplays(AnchorSystemHandle systemhandle)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;

  return system->getNumDisplays();
}

eAnchorStatus ANCHOR::DestroySystem(AnchorSystemHandle systemhandle)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;

  return system->destroySystem();
}

AnchorSystemWindowHandle ANCHOR::CreateSystemWindow(AnchorSystemHandle systemhandle,
                                                    AnchorSystemWindowHandle parent_windowhandle,
                                                    const char *title,
                                                    const char *icon,
                                                    AnchorS32 left,
                                                    AnchorS32 top,
                                                    AnchorU32 width,
                                                    AnchorU32 height,
                                                    eAnchorWindowState state,
                                                    bool is_dialog,
                                                    eAnchorDrawingContextType type,
                                                    int vkSettings)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;

  return (AnchorSystemWindowHandle)system->createWindow(
    title,
    icon,
    left,
    top,
    width,
    height,
    state,
    type,
    0,
    false,
    is_dialog,
    (AnchorISystemWindow *)parent_windowhandle);
}

void ANCHOR::SetTitle(AnchorSystemWindowHandle windowhandle, const char *title)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  window->setTitle(title);
}

eAnchorStatus ANCHOR::SwapChain(AnchorSystemWindowHandle windowhandle)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;
  return window->swapBuffers();
}

eAnchorStatus ANCHOR::ActivateWindowDrawingContext(AnchorSystemWindowHandle windowhandle)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  return window->activateDrawingContext();
}

eAnchorStatus ANCHOR::AddEventConsumer(AnchorSystemHandle systemhandle,
                                       AnchorEventConsumerHandle consumerhandle)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;

  return system->addEventConsumer((ANCHOR_CallbackEventConsumer *)consumerhandle);
}

eAnchorEventType ANCHOR::GetEventType(AnchorEventHandle eventhandle)
{
  AnchorIEvent *event = (AnchorIEvent *)eventhandle;

  return event->getType();
}

AnchorSystemWindowHandle ANCHOR::GetEventWindow(AnchorEventHandle eventhandle)
{
  AnchorIEvent *event = (AnchorIEvent *)eventhandle;

  return (AnchorSystemWindowHandle)event->getWindow();
}

AnchorEventDataPtr ANCHOR::GetEventData(AnchorEventHandle eventhandle)
{
  AnchorIEvent *event = (AnchorIEvent *)eventhandle;

  return event->getData();
}

eAnchorStatus ANCHOR::GetModifierKeyState(AnchorSystemHandle systemhandle,
                                          eAnchorModifierKeyMask mask,
                                          int *isDown)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;
  eAnchorStatus result;
  bool isdown = false;

  result = system->getModifierKeyState(mask, isdown);
  *isDown = (int)isdown;

  return result;
}

void ANCHOR::ScreenToClient(AnchorSystemWindowHandle windowhandle,
                            AnchorS32 inX,
                            AnchorS32 inY,
                            AnchorS32 *outX,
                            AnchorS32 *outY)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  window->screenToClient(inX, inY, *outX, *outY);
}

eAnchorStatus ANCHOR::GetCursorPosition(AnchorSystemHandle systemhandle,
                                        AnchorS32 *x,
                                        AnchorS32 *y)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;
  return system->getCursorPosition(*x, *y);
}


int ANCHOR::ValidWindow(AnchorSystemHandle systemhandle, AnchorSystemWindowHandle windowhandle)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  return (int)system->validWindow(window);
}

ANCHOR_UserPtr ANCHOR::GetWindowUserData(AnchorSystemWindowHandle windowhandle)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  return window->getUserData();
}

void ANCHOR::SetWindowUserData(AnchorSystemWindowHandle windowhandle, ANCHOR_UserPtr userdata)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  window->setUserData(userdata);
}

AnchorU16 ANCHOR::GetDPIHint(AnchorSystemWindowHandle windowhandle)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;
  return window->getDPIHint();
}

int ANCHOR::ToggleConsole(int action)
{
  AnchorISystem *system = AnchorISystem::getSystem();
  return system->toggleConsole(action);
}

int ANCHOR::UseNativePixels(void)
{
  AnchorISystem *system = AnchorISystem::getSystem();
  return system->useNativePixel();
}

void ANCHOR::UseWindowFocus(int use_focus)
{
  AnchorISystem *system = AnchorISystem::getSystem();
  return system->useWindowFocus(use_focus);
}

float ANCHOR::GetNativePixelSize(AnchorSystemWindowHandle windowhandle)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;
  if (window)
    return window->getNativePixelSize();
  return 1.0f;
}

void ANCHOR::ClientToScreen(AnchorSystemWindowHandle windowhandle,
                            AnchorS32 inX,
                            AnchorS32 inY,
                            AnchorS32 *outX,
                            AnchorS32 *outY)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  window->clientToScreen(inX, inY, *outX, *outY);
}

void ANCHOR::GetMainDisplayDimensions(AnchorSystemHandle systemhandle,
                                      AnchorU32 *width,
                                      AnchorU32 *height)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;

  system->getMainDisplayDimensions(*width, *height);
}

eAnchorWindowState ANCHOR::GetWindowState(AnchorSystemWindowHandle windowhandle)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  return window->getState();
}

eAnchorStatus ANCHOR::SetWindowState(AnchorSystemWindowHandle windowhandle,
                                     eAnchorWindowState state)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  return window->setState(state);
}

eAnchorStatus ANCHOR::SetWindowOrder(AnchorSystemWindowHandle windowhandle,
                                     eAnchorWindowOrder order)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  return window->setOrder(order);
}

int ANCHOR::IsDialogWindow(AnchorSystemWindowHandle windowhandle)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  return (int)window->isDialog();
}

char *ANCHOR::GetTitle(AnchorSystemWindowHandle windowhandle)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;
  std::string title = window->getTitle();

  char *ctitle = (char *)malloc(title.size() + 1);

  if (ctitle == NULL) {
    return NULL;
  }

  strcpy(ctitle, title.c_str());

  return ctitle;
}

eAnchorStatus ANCHOR::SetClientSize(AnchorSystemWindowHandle windowhandle,
                                    AnchorU32 width,
                                    AnchorU32 height)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;

  return window->setClientSize(width, height);
}

AnchorRectangleHandle ANCHOR::GetClientBounds(AnchorSystemWindowHandle windowhandle)
{
  AnchorISystemWindow *window = (AnchorISystemWindow *)windowhandle;
  AnchorRect *rectangle = NULL;

  rectangle = new AnchorRect();
  window->getClientBounds(*rectangle);

  return (AnchorRectangleHandle)rectangle;
}

AnchorS32 ANCHOR::GetWidthRectangle(AnchorRectangleHandle rectanglehandle)
{
  return ((AnchorRect *)rectanglehandle)->getWidth();
}

AnchorS32 ANCHOR::GetHeightRectangle(AnchorRectangleHandle rectanglehandle)
{
  return ((AnchorRect *)rectanglehandle)->getHeight();
}

void ANCHOR::GetRectangle(AnchorRectangleHandle rectanglehandle,
                          AnchorS32 *l,
                          AnchorS32 *t,
                          AnchorS32 *r,
                          AnchorS32 *b)
{
  AnchorRect *rect = (AnchorRect *)rectanglehandle;

  *l = rect->m_l;
  *t = rect->m_t;
  *r = rect->m_r;
  *b = rect->m_b;
}

void ANCHOR::DisposeRectangle(AnchorRectangleHandle rectanglehandle)
{
  delete (AnchorRect *)rectanglehandle;
}

void ANCHOR::GetAllDisplayDimensions(AnchorSystemHandle systemhandle,
                                     AnchorU32 *width,
                                     AnchorU32 *height)
{
  AnchorISystem *system = (AnchorISystem *)systemhandle;

  system->getAllDisplayDimensions(*width, *height);
}

void ANCHOR::SetAllocatorFunctions(ANCHORMemAllocFunc alloc_func,
                                   ANCHORMemFreeFunc free_func,
                                   void *user_data)
{
  GImAllocatorAllocFunc = alloc_func;
  GImAllocatorFreeFunc = free_func;
  GImAllocatorUserData = user_data;
}

// This is provided to facilitate copying allocators from one static/DLL boundary to another (e.g.
// retrieve default allocator of your executable address space)
void ANCHOR::GetAllocatorFunctions(ANCHORMemAllocFunc *p_alloc_func,
                                   ANCHORMemFreeFunc *p_free_func,
                                   void **p_user_data)
{
  *p_alloc_func = GImAllocatorAllocFunc;
  *p_free_func = GImAllocatorFreeFunc;
  *p_user_data = GImAllocatorUserData;
}

AnchorContext *ANCHOR::CreateContext(AnchorFontAtlas *shared_font_atlas)
{
  AnchorContext *ctx = ANCHOR_NEW(AnchorContext)(shared_font_atlas);
  if (G_CTX == NULL)
    SetCurrentContext(ctx);
  Initialize(ctx);
  return ctx;
}

void ANCHOR::DestroyContext(AnchorContext *ctx)
{
  if (ctx == NULL)
    ctx = G_CTX;
  Shutdown(ctx);
  if (G_CTX == ctx)
    SetCurrentContext(NULL);
  ANCHOR_DELETE(ctx);
}

// No specific ordering/dependency support, will see as needed
ANCHOR_ID ANCHOR::AddContextHook(AnchorContext *ctx, const AnchorContextHook *hook)
{
  AnchorContext &g = *ctx;
  ANCHOR_ASSERT(hook->Callback != NULL && hook->HookId == 0 &&
                hook->Type != AnchorContextHookType_PendingRemoval_);
  g.Hooks.push_back(*hook);
  g.Hooks.back().HookId = ++g.HookIdNext;
  return g.HookIdNext;
}

// Deferred removal, avoiding issue with changing vector while iterating it
void ANCHOR::RemoveContextHook(AnchorContext *ctx, ANCHOR_ID hook_id)
{
  AnchorContext &g = *ctx;
  ANCHOR_ASSERT(hook_id != 0);
  for (int n = 0; n < g.Hooks.Size; n++)
    if (g.Hooks[n].HookId == hook_id)
      g.Hooks[n].Type = AnchorContextHookType_PendingRemoval_;
}

// Call context hooks (used by e.g. test engine)
// We assume a small number of hooks so all stored in same array
void ANCHOR::CallContextHooks(AnchorContext *ctx, AnchorContextHookType hook_type)
{
  AnchorContext &g = *ctx;
  for (int n = 0; n < g.Hooks.Size; n++)
    if (g.Hooks[n].Type == hook_type)
      g.Hooks[n].Callback(&g, &g.Hooks[n]);
}

wabi::HdDriver &ANCHOR::GetPixarDriver()
{
  return G_CTX->HydraDriver;
}

kraken::UsdImagingGLEngineSharedPtr ANCHOR::GetEngineGL()
{
  return G_CTX->GLEngine;
}

AnchorIO &ANCHOR::GetIO()
{
  ANCHOR_ASSERT(G_CTX != NULL &&
                "No current context. Did you call ANCHOR::CreateContext() and "
                "ANCHOR::SetCurrentContext() ?");
  return G_CTX->IO;
}

// Pass this to your backend rendering function! Valid after Render() and until the next call to
// NewFrame()
AnchorDrawData *ANCHOR::GetDrawData()
{
  AnchorContext &g = *G_CTX;
  AnchorViewportP *viewport = g.Viewports[0];
  return viewport->DrawDataP.Valid ? &viewport->DrawDataP : NULL;
}

double ANCHOR::GetTime()
{
  return G_CTX->Time;
}

int ANCHOR::GetFrameCount()
{
  return G_CTX->FrameCount;
}

static AnchorDrawList *GetViewportDrawList(AnchorViewportP *viewport,
                                           size_t drawlist_no,
                                           const char *drawlist_name)
{
  // Create the draw list on demand, because they are not frequently used for all viewports
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(drawlist_no < ANCHOR_ARRAYSIZE(viewport->DrawLists));
  AnchorDrawList *draw_list = viewport->DrawLists[drawlist_no];
  if (draw_list == NULL) {
    draw_list = ANCHOR_NEW(AnchorDrawList)(&g.DrawListSharedData);
    draw_list->_OwnerName = drawlist_name;
    viewport->DrawLists[drawlist_no] = draw_list;
  }

  // Our AnchorDrawList system requires that there is always a command
  if (viewport->DrawListsLastFrame[drawlist_no] != g.FrameCount) {
    draw_list->_ResetForNewFrame();
    draw_list->PushTextureID(g.IO.Fonts->TexID);
    draw_list->PushClipRect(viewport->Pos, viewport->Pos + viewport->Size, false);
    viewport->DrawListsLastFrame[drawlist_no] = g.FrameCount;
  }
  return draw_list;
}

AnchorDrawList *ANCHOR::GetBackgroundDrawList(AnchorViewport *viewport)
{
  return GetViewportDrawList((AnchorViewportP *)viewport, 0, "##Background");
}

AnchorDrawList *ANCHOR::GetBackgroundDrawList()
{
  AnchorContext &g = *G_CTX;
  return GetBackgroundDrawList(g.Viewports[0]);
}

AnchorDrawList *ANCHOR::GetForegroundDrawList(AnchorViewport *viewport)
{
  return GetViewportDrawList((AnchorViewportP *)viewport, 1, "##Foreground");
}

AnchorDrawList *ANCHOR::GetForegroundDrawList()
{
  AnchorContext &g = *G_CTX;
  return GetForegroundDrawList(g.Viewports[0]);
}

AnchorDrawListSharedData *ANCHOR::GetDrawListSharedData()
{
  return &G_CTX->DrawListSharedData;
}

void ANCHOR::StartMouseMovingWindow(AnchorWindow *window)
{
  // Set ActiveId even if the _NoMove flag is set. Without it, dragging away from a window with
  // _NoMove would activate hover on other windows. We _also_ call this when clicking in a window
  // empty space when io.ConfigWindowsMoveFromTitleBarOnly is set, but clear g.MovingWindow
  // afterward. This is because we want ActiveId to be set even when the window is not permitted to
  // move.
  AnchorContext &g = *G_CTX;
  FocusWindow(window);
  SetActiveID(window->MoveId, window);
  g.NavDisableHighlight = true;
  g.ActiveIdNoClearOnFocusLoss = true;
  g.ActiveIdClickOffset = g.IO.MousePos - window->RootWindow->Pos;

  bool can_move_window = true;
  if ((window->Flags & AnchorWindowFlags_NoMove) ||
      (window->RootWindow->Flags & AnchorWindowFlags_NoMove))
    can_move_window = false;
  if (can_move_window)
    g.MovingWindow = window;
}

// Handle mouse moving window
// Note: moving window with the navigation keys (Square + d-pad / CTRL+TAB + Arrows) are processed
// in NavUpdateWindowing()
// FIXME: We don't have strong guarantee that g.MovingWindow stay synched with g.ActiveId ==
// g.MovingWindow->MoveId. This is currently enforced by the fact that BeginDragDropSource() is
// setting all g.ActiveIdUsingXXXX flags to inhibit navigation inputs, but if we should more
// thoroughly test cases where g.ActiveId or g.MovingWindow gets changed and not the other.
void ANCHOR::UpdateMouseMovingWindowNewFrame()
{
  AnchorContext &g = *G_CTX;
  if (g.MovingWindow != NULL) {
    // We actually want to move the root window. g.MovingWindow == window we clicked on (could be a
    // child window). We track it to preserve Focus and so that generally ActiveIdWindow ==
    // MovingWindow and ActiveId == MovingWindow->MoveId for consistency.
    KeepAliveID(g.ActiveId);
    ANCHOR_ASSERT(g.MovingWindow && g.MovingWindow->RootWindow);
    AnchorWindow *moving_window = g.MovingWindow->RootWindow;
    if (g.IO.MouseDown[0] && IsMousePosValid(&g.IO.MousePos)) {
      wabi::GfVec2f pos = g.IO.MousePos - g.ActiveIdClickOffset;
      if (moving_window->Pos[0] != pos[0] || moving_window->Pos[1] != pos[1]) {
        MarkIniSettingsDirty(moving_window);
        SetWindowPos(moving_window, pos, AnchorCond_Always);
      }
      FocusWindow(g.MovingWindow);
    } else {
      ClearActiveID();
      g.MovingWindow = NULL;
    }
  } else {
    // When clicking/dragging from a window that has the _NoMove flag, we still set the ActiveId in
    // order to prevent hovering others.
    if (g.ActiveIdWindow && g.ActiveIdWindow->MoveId == g.ActiveId) {
      KeepAliveID(g.ActiveId);
      if (!g.IO.MouseDown[0])
        ClearActiveID();
    }
  }
}

// Initiate moving window when clicking on empty space or title bar.
// Handle left-click and right-click focus.
void ANCHOR::UpdateMouseMovingWindowEndFrame()
{
  AnchorContext &g = *G_CTX;
  if (g.ActiveId != 0 || g.HoveredId != 0)
    return;

  // Unless we just made a window/popup appear
  if (g.NavWindow && g.NavWindow->Appearing)
    return;

  // Click on empty space to focus window and start moving
  // (after we're done with all our widgets)
  if (g.IO.MouseClicked[0]) {
    // Handle the edge case of a popup being closed while clicking in its empty space.
    // If we try to focus it, FocusWindow() > ClosePopupsOverWindow() will accidentally close any
    // parent popups because they are not linked together any more.
    AnchorWindow *root_window = g.HoveredWindow ? g.HoveredWindow->RootWindow : NULL;
    const bool is_closed_popup = root_window && (root_window->Flags & AnchorWindowFlags_Popup) &&
                                 !IsPopupOpen(root_window->PopupId,
                                              AnchorPopupFlags_AnyPopupLevel);

    if (root_window != NULL && !is_closed_popup) {
      StartMouseMovingWindow(g.HoveredWindow);  //-V595

      // Cancel moving if clicked outside of title bar
      if (g.IO.ConfigWindowsMoveFromTitleBarOnly &&
          !(root_window->Flags & AnchorWindowFlags_NoTitleBar))
        if (!root_window->TitleBarRect().Contains(g.IO.MouseClickedPos[0]))
          g.MovingWindow = NULL;

      // Cancel moving if clicked over an item which was disabled or inhibited by popups (note that
      // we know HoveredId == 0 already)
      if (g.HoveredIdDisabled)
        g.MovingWindow = NULL;
    } else if (root_window == NULL && g.NavWindow != NULL && GetTopMostPopupModal() == NULL) {
      // Clicking on void disable focus
      FocusWindow(NULL);
    }
  }

  // With right mouse button we close popups without changing focus based on where the mouse is
  // aimed Instead, focus will be restored to the window under the bottom-most closed popup. (The
  // left mouse button path calls FocusWindow on the hovered window, which will lead
  // NewFrame->ClosePopupsOverWindow to trigger)
  if (g.IO.MouseClicked[1]) {
    // Find the top-most window between HoveredWindow and the top-most Modal Window.
    // This is where we can trim the popup stack.
    AnchorWindow *modal = GetTopMostPopupModal();
    bool hovered_window_above_modal = g.HoveredWindow && IsWindowAbove(g.HoveredWindow, modal);
    ClosePopupsOverWindow(hovered_window_above_modal ? g.HoveredWindow : modal, true);
  }
}

static bool IsWindowActiveAndVisible(AnchorWindow *window)
{
  return (window->Active) && (!window->Hidden);
}

static void ANCHOR::UpdateMouseInputs()
{
  AnchorContext &g = *G_CTX;

  // Round mouse position to avoid spreading non-rounded position (e.g. UpdateManualResize doesn't
  // support them well)
  if (IsMousePosValid(&g.IO.MousePos))
    g.IO.MousePos = g.LastValidMousePos = AnchorFloor(g.IO.MousePos);

  // If mouse just appeared or disappeared (usually denoted by -FLT_MAX components) we cancel out
  // movement in MouseDelta
  if (IsMousePosValid(&g.IO.MousePos) && IsMousePosValid(&g.IO.MousePosPrev))
    g.IO.MouseDelta = g.IO.MousePos - g.IO.MousePosPrev;
  else
    g.IO.MouseDelta = wabi::GfVec2f(0.0f, 0.0f);
  if (g.IO.MouseDelta[0] != 0.0f || g.IO.MouseDelta[1] != 0.0f)
    g.NavDisableMouseHover = false;

  g.IO.MousePosPrev = g.IO.MousePos;
  for (int i = 0; i < ANCHOR_ARRAYSIZE(g.IO.MouseDown); i++) {
    g.IO.MouseClicked[i] = g.IO.MouseDown[i] && g.IO.MouseDownDuration[i] < 0.0f;
    g.IO.MouseReleased[i] = !g.IO.MouseDown[i] && g.IO.MouseDownDuration[i] >= 0.0f;
    g.IO.MouseDownDurationPrev[i] = g.IO.MouseDownDuration[i];
    g.IO.MouseDownDuration[i] = g.IO.MouseDown[i] ?
                                  (g.IO.MouseDownDuration[i] < 0.0f ?
                                     0.0f :
                                     g.IO.MouseDownDuration[i] + g.IO.DeltaTime) :
                                  -1.0f;
    g.IO.MouseDoubleClicked[i] = false;
    if (g.IO.MouseClicked[i]) {
      if ((float)(g.Time - g.IO.MouseClickedTime[i]) < g.IO.MouseDoubleClickTime) {
        wabi::GfVec2f delta_from_click_pos = IsMousePosValid(&g.IO.MousePos) ?
                                         (g.IO.MousePos - g.IO.MouseClickedPos[i]) :
                                         wabi::GfVec2f(0.0f, 0.0f);
        if (AnchorLengthSqr(delta_from_click_pos) <
            g.IO.MouseDoubleClickMaxDist * g.IO.MouseDoubleClickMaxDist)
          g.IO.MouseDoubleClicked[i] = true;
        g.IO.MouseClickedTime[i] =
          -g.IO.MouseDoubleClickTime *
          2.0f;  // Mark as "old enough" so the third click isn't turned into a double-click
      } else {
        g.IO.MouseClickedTime[i] = g.Time;
      }
      g.IO.MouseClickedPos[i] = g.IO.MousePos;
      g.IO.MouseDownWasDoubleClick[i] = g.IO.MouseDoubleClicked[i];
      g.IO.MouseDragMaxDistanceAbs[i] = wabi::GfVec2f(0.0f, 0.0f);
      g.IO.MouseDragMaxDistanceSqr[i] = 0.0f;
    } else if (g.IO.MouseDown[i]) {
      // Maintain the maximum distance we reaching from the initial click position, which is used
      // with dragging threshold
      wabi::GfVec2f delta_from_click_pos = IsMousePosValid(&g.IO.MousePos) ?
                                       (g.IO.MousePos - g.IO.MouseClickedPos[i]) :
                                       wabi::GfVec2f(0.0f, 0.0f);
      g.IO.MouseDragMaxDistanceSqr[i] = AnchorMax(g.IO.MouseDragMaxDistanceSqr[i],
                                                  AnchorLengthSqr(delta_from_click_pos));
      g.IO.MouseDragMaxDistanceAbs[i][0] = AnchorMax(
        g.IO.MouseDragMaxDistanceAbs[i][0],
        delta_from_click_pos[0] < 0.0f ? -delta_from_click_pos[0] : delta_from_click_pos[0]);
      g.IO.MouseDragMaxDistanceAbs[i][1] = AnchorMax(
        g.IO.MouseDragMaxDistanceAbs[i][1],
        delta_from_click_pos[1] < 0.0f ? -delta_from_click_pos[1] : delta_from_click_pos[1]);
    }
    if (!g.IO.MouseDown[i] && !g.IO.MouseReleased[i])
      g.IO.MouseDownWasDoubleClick[i] = false;
    if (g.IO.MouseClicked[i])  // Clicking any mouse button reactivate mouse hovering which may
                               // have been deactivated by gamepad/keyboard navigation
      g.NavDisableMouseHover = false;
  }
}

static void StartLockWheelingWindow(AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;
  if (g.WheelingWindow == window)
    return;
  g.WheelingWindow = window;
  g.WheelingWindowRefMousePos = g.IO.MousePos;
  g.WheelingWindowTimer = WINDOWS_MOUSE_WHEEL_SCROLL_LOCK_TIMER;
}

void ANCHOR::UpdateMouseWheel()
{
  AnchorContext &g = *G_CTX;

  // Reset the locked window if we move the mouse or after the timer elapses
  if (g.WheelingWindow != NULL) {
    g.WheelingWindowTimer -= g.IO.DeltaTime;
    if (IsMousePosValid() && AnchorLengthSqr(g.IO.MousePos - g.WheelingWindowRefMousePos) >
                               g.IO.MouseDragThreshold * g.IO.MouseDragThreshold)
      g.WheelingWindowTimer = 0.0f;
    if (g.WheelingWindowTimer <= 0.0f) {
      g.WheelingWindow = NULL;
      g.WheelingWindowTimer = 0.0f;
    }
  }

  if (g.IO.MouseWheel == 0.0f && g.IO.MouseWheelH == 0.0f)
    return;

  if ((g.ActiveId != 0 && g.ActiveIdUsingMouseWheel) ||
      (g.HoveredIdPreviousFrame != 0 && g.HoveredIdPreviousFrameUsingMouseWheel))
    return;

  AnchorWindow *window = g.WheelingWindow ? g.WheelingWindow : g.HoveredWindow;
  if (!window || window->Collapsed)
    return;

  // Zoom / Scale window
  // FIXME-OBSOLETE: This is an old feature, it still works but pretty much nobody is using it and
  // may be best redesigned.
  if (g.IO.MouseWheel != 0.0f && g.IO.KeyCtrl && g.IO.FontAllowUserScaling) {
    StartLockWheelingWindow(window);
    const float new_font_scale = AnchorClamp(window->FontWindowScale + g.IO.MouseWheel * 0.10f,
                                             0.50f,
                                             2.50f);
    const float scale = new_font_scale / window->FontWindowScale;
    window->FontWindowScale = new_font_scale;
    if (window == window->RootWindow) {
      /** todo::check_math */
      const wabi::GfVec2f offset = wabi::GfVec2f(
        window->Size[0] * (1.0f - scale) * (g.IO.MousePos[0] - window->Pos[0]) / window->Size[0],
        window->Size[1] * (1.0f - scale) * (g.IO.MousePos[1] - window->Pos[1]) / window->Size[1]);
      SetWindowPos(window, window->Pos + offset, 0);
      window->Size = AnchorFloor(window->Size * scale);
      window->SizeFull = AnchorFloor(window->SizeFull * scale);
    }
    return;
  }

  // Mouse wheel scrolling
  // If a child window has the AnchorWindowFlags_NoScrollWithMouse flag, we give a chance to
  // scroll its parent
  if (g.IO.KeyCtrl)
    return;

  // As a standard behavior holding SHIFT while using Vertical Mouse Wheel triggers Horizontal
  // scroll instead (we avoid doing it on OSX as it the OS input layer handles this already)
  const bool swap_axis = g.IO.KeyShift && !g.IO.ConfigMacOSXBehaviors;
  const float wheel_y = swap_axis ? 0.0f : g.IO.MouseWheel;
  const float wheel_x = swap_axis ? g.IO.MouseWheel : g.IO.MouseWheelH;

  // Vertical Mouse Wheel scrolling
  if (wheel_y != 0.0f) {
    StartLockWheelingWindow(window);
    while (
      (window->Flags & AnchorWindowFlags_ChildWindow) &&
      ((window->ScrollMax[1] == 0.0f) || ((window->Flags & AnchorWindowFlags_NoScrollWithMouse) &&
                                          !(window->Flags & AnchorWindowFlags_NoMouseInputs))))
      window = window->ParentWindow;
    if (!(window->Flags & AnchorWindowFlags_NoScrollWithMouse) &&
        !(window->Flags & AnchorWindowFlags_NoMouseInputs)) {
      float max_step = window->InnerRect.GetHeight() * 0.67f;
      float scroll_step = AnchorFloor(AnchorMin(5 * window->CalcFontSize(), max_step));
      SetScrollY(window, window->Scroll[1] - wheel_y * scroll_step);
    }
  }

  // Horizontal Mouse Wheel scrolling, or Vertical Mouse Wheel w/ Shift held
  if (wheel_x != 0.0f) {
    StartLockWheelingWindow(window);
    while (
      (window->Flags & AnchorWindowFlags_ChildWindow) &&
      ((window->ScrollMax[0] == 0.0f) || ((window->Flags & AnchorWindowFlags_NoScrollWithMouse) &&
                                          !(window->Flags & AnchorWindowFlags_NoMouseInputs))))
      window = window->ParentWindow;
    if (!(window->Flags & AnchorWindowFlags_NoScrollWithMouse) &&
        !(window->Flags & AnchorWindowFlags_NoMouseInputs)) {
      float max_step = window->InnerRect.GetWidth() * 0.67f;
      float scroll_step = AnchorFloor(AnchorMin(2 * window->CalcFontSize(), max_step));
      SetScrollX(window, window->Scroll[0] - wheel_x * scroll_step);
    }
  }
}

void ANCHOR::UpdateTabFocus()
{
  AnchorContext &g = *G_CTX;

  // Pressing TAB activate widget focus
  g.TabFocusPressed = (g.NavWindow && g.NavWindow->Active &&
                       !(g.NavWindow->Flags & AnchorWindowFlags_NoNavInputs) && !g.IO.KeyCtrl &&
                       IsKeyPressedMap(AnchorKey_Tab));
  if (g.ActiveId == 0 && g.TabFocusPressed) {
    // - This path is only taken when no widget are active/tabbed-into yet.
    //   Subsequent tabbing will be processed by FocusableItemRegister()
    // - Note that SetKeyboardFocusHere() sets the Next fields mid-frame. To be consistent we also
    //   manipulate the Next fields here even though they will be turned into Curr fields below.
    g.TabFocusRequestNextWindow = g.NavWindow;
    g.TabFocusRequestNextCounterRegular = INT_MAX;
    if (g.NavId != 0 && g.NavIdTabCounter != INT_MAX)
      g.TabFocusRequestNextCounterTabStop = g.NavIdTabCounter + (g.IO.KeyShift ? -1 : 0);
    else
      g.TabFocusRequestNextCounterTabStop = g.IO.KeyShift ? -1 : 0;
  }

  // Turn queued focus request into current one
  g.TabFocusRequestCurrWindow = NULL;
  g.TabFocusRequestCurrCounterRegular = g.TabFocusRequestCurrCounterTabStop = INT_MAX;
  if (g.TabFocusRequestNextWindow != NULL) {
    AnchorWindow *window = g.TabFocusRequestNextWindow;
    g.TabFocusRequestCurrWindow = window;
    if (g.TabFocusRequestNextCounterRegular != INT_MAX && window->DC.FocusCounterRegular != -1)
      g.TabFocusRequestCurrCounterRegular = AnchorModPositive(g.TabFocusRequestNextCounterRegular,
                                                              window->DC.FocusCounterRegular + 1);
    if (g.TabFocusRequestNextCounterTabStop != INT_MAX && window->DC.FocusCounterTabStop != -1)
      g.TabFocusRequestCurrCounterTabStop = AnchorModPositive(g.TabFocusRequestNextCounterTabStop,
                                                              window->DC.FocusCounterTabStop + 1);
    g.TabFocusRequestNextWindow = NULL;
    g.TabFocusRequestNextCounterRegular = g.TabFocusRequestNextCounterTabStop = INT_MAX;
  }

  g.NavIdTabCounter = INT_MAX;
}

// The reason this is exposed in ANCHOR_internal.h is: on touch-based system that don't have
// hovering, we want to dispatch inputs to the right target (ANCHOR vs ANCHOR+app)
void ANCHOR::UpdateHoveredWindowAndCaptureFlags()
{
  AnchorContext &g = *G_CTX;
  g.WindowsHoverPadding = AnchorMax(g.Style.TouchExtraPadding,
                                    wabi::GfVec2f(WINDOWS_HOVER_PADDING, WINDOWS_HOVER_PADDING));

  // Find the window hovered by mouse:
  // - Child windows can extend beyond the limit of their parent so we need to derive
  // HoveredRootWindow from HoveredWindow.
  // - When moving a window we can skip the search, which also conveniently bypasses the fact that
  // window->WindowRectClipped is lagging as this point of the frame.
  // - We also support the moved window toggling the NoInputs flag after moving has started in
  // order to be able to detect windows below it, which is useful for e.g. docking mechanisms.
  bool clear_hovered_windows = false;
  FindHoveredWindow();

  // Modal windows prevents mouse from hovering behind them.
  AnchorWindow *modal_window = GetTopMostPopupModal();
  if (modal_window && g.HoveredWindow &&
      !IsWindowChildOf(g.HoveredWindow->RootWindow, modal_window))
    clear_hovered_windows = true;

  // Disabled mouse?
  if (g.IO.ConfigFlags & AnchorConfigFlags_NoMouse)
    clear_hovered_windows = true;

  // We track click ownership. When clicked outside of a window the click is owned by the
  // application and won't report hovering nor request capture even while dragging over our windows
  // afterward.
  int mouse_earliest_button_down = -1;
  bool mouse_any_down = false;
  for (int i = 0; i < ANCHOR_ARRAYSIZE(g.IO.MouseDown); i++) {
    if (g.IO.MouseClicked[i])
      g.IO.MouseDownOwned[i] = (g.HoveredWindow != NULL) || (g.OpenPopupStack.Size > 0);
    mouse_any_down |= g.IO.MouseDown[i];
    if (g.IO.MouseDown[i])
      if (mouse_earliest_button_down == -1 ||
          g.IO.MouseClickedTime[i] < g.IO.MouseClickedTime[mouse_earliest_button_down])
        mouse_earliest_button_down = i;
  }
  const bool mouse_avail_to_ANCHOR = (mouse_earliest_button_down == -1) ||
                                     g.IO.MouseDownOwned[mouse_earliest_button_down];

  // If mouse was first clicked outside of ANCHOR bounds we also cancel out hovering.
  // FIXME: For patterns of drag and drop across OS windows, we may need to rework/remove this test
  // (first committed 311c0ca9 on 2015/02)
  const bool mouse_dragging_extern_payload = g.DragDropActive &&
                                             (g.DragDropSourceFlags &
                                              AnchorDragDropFlags_SourceExtern) != 0;
  if (!mouse_avail_to_ANCHOR && !mouse_dragging_extern_payload)
    clear_hovered_windows = true;

  if (clear_hovered_windows)
    g.HoveredWindow = g.HoveredWindowUnderMovingWindow = NULL;

  // Update io.WantCaptureMouse for the user application (true = dispatch mouse info to ANCHOR,
  // false = dispatch mouse info to ANCHOR + app)
  if (g.WantCaptureMouseNextFrame != -1)
    g.IO.WantCaptureMouse = (g.WantCaptureMouseNextFrame != 0);
  else
    g.IO.WantCaptureMouse = (mouse_avail_to_ANCHOR &&
                             (g.HoveredWindow != NULL || mouse_any_down)) ||
                            (g.OpenPopupStack.Size > 0);

  // Update io.WantCaptureKeyboard for the user application (true = dispatch keyboard info to
  // ANCHOR, false = dispatch keyboard info to ANCHOR + app)
  if (g.WantCaptureKeyboardNextFrame != -1)
    g.IO.WantCaptureKeyboard = (g.WantCaptureKeyboardNextFrame != 0);
  else
    g.IO.WantCaptureKeyboard = (g.ActiveId != 0) || (modal_window != NULL);
  if (g.IO.NavActive && (g.IO.ConfigFlags & AnchorConfigFlags_NavEnableKeyboard) &&
      !(g.IO.ConfigFlags & AnchorConfigFlags_NavNoCaptureKeyboard))
    g.IO.WantCaptureKeyboard = true;

  // Update io.WantTextInput flag, this is to allow systems without a keyboard (e.g. mobile,
  // hand-held) to show a software keyboard if possible
  g.IO.WantTextInput = (g.WantTextInputNextFrame != -1) ? (g.WantTextInputNextFrame != 0) : false;
}

AnchorKeyModFlags ANCHOR::GetMergedKeyModFlags()
{
  AnchorContext &g = *G_CTX;
  AnchorKeyModFlags key_mod_flags = AnchorKeyModFlags_None;
  if (g.IO.KeyCtrl) {
    key_mod_flags |= AnchorKeyModFlags_Ctrl;
  }
  if (g.IO.KeyShift) {
    key_mod_flags |= AnchorKeyModFlags_Shift;
  }
  if (g.IO.KeyAlt) {
    key_mod_flags |= AnchorKeyModFlags_Alt;
  }
  if (g.IO.KeySuper) {
    key_mod_flags |= AnchorKeyModFlags_Super;
  }
  return key_mod_flags;
}

void ANCHOR::NewFrame()
{
  ANCHOR_ASSERT(G_CTX != NULL &&
                "No current context. Did you call ANCHOR::CreateContext() and "
                "ANCHOR::SetCurrentContext() ?");
  AnchorContext &g = *G_CTX;

  // Remove pending delete hooks before frame start.
  // This deferred removal avoid issues of removal while iterating the hook vector
  for (int n = g.Hooks.Size - 1; n >= 0; n--)
    if (g.Hooks[n].Type == AnchorContextHookType_PendingRemoval_)
      g.Hooks.erase(&g.Hooks[n]);

  CallContextHooks(&g, AnchorContextHookType_NewFramePre);

  // Check and assert for various common IO and Configuration mistakes
  ErrorCheckNewFrameSanityChecks();

  // Load settings on first frame, save settings when modified (after a delay)
  UpdateSettings();

  g.Time += g.IO.DeltaTime;
  g.WithinFrameScope = true;
  g.FrameCount += 1;
  g.TooltipOverrideCount = 0;
  g.WindowsActiveCount = 0;
  g.MenusIdSubmittedThisFrame.resize(0);

  // Calculate frame-rate for the user, as a purely luxurious feature
  g.FramerateSecPerFrameAccum += g.IO.DeltaTime -
                                 g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx];
  g.FramerateSecPerFrame[g.FramerateSecPerFrameIdx] = g.IO.DeltaTime;
  g.FramerateSecPerFrameIdx = (g.FramerateSecPerFrameIdx + 1) %
                              ANCHOR_ARRAYSIZE(g.FramerateSecPerFrame);
  g.FramerateSecPerFrameCount = AnchorMin(g.FramerateSecPerFrameCount + 1,
                                          ANCHOR_ARRAYSIZE(g.FramerateSecPerFrame));
  g.IO.Framerate = (g.FramerateSecPerFrameAccum > 0.0f) ?
                     (1.0f / (g.FramerateSecPerFrameAccum / (float)g.FramerateSecPerFrameCount)) :
                     FLT_MAX;

  UpdateViewportsNewFrame();

  // Setup current font and draw list shared data
  g.IO.Fonts->Locked = true;
  SetCurrentFont(GetDefaultFont());
  ANCHOR_ASSERT(g.Font->IsLoaded());
  AnchorBBox virtual_space(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);
  for (int n = 0; n < g.Viewports.Size; n++)
    virtual_space.Add(g.Viewports[n]->GetMainRect());
  g.DrawListSharedData.ClipRectFullscreen = virtual_space.ToVec4();
  g.DrawListSharedData.CurveTessellationTol = g.Style.CurveTessellationTol;
  g.DrawListSharedData.SetCircleTessellationMaxError(g.Style.CircleTessellationMaxError);
  g.DrawListSharedData.InitialFlags = AnchorDrawListFlags_None;
  if (g.Style.AntiAliasedLines)
    g.DrawListSharedData.InitialFlags |= AnchorDrawListFlags_AntiAliasedLines;
  if (g.Style.AntiAliasedLinesUseTex &&
      !(g.Font->ContainerAtlas->Flags & AnchorFontAtlasFlags_NoBakedLines))
    g.DrawListSharedData.InitialFlags |= AnchorDrawListFlags_AntiAliasedLinesUseTex;
  if (g.Style.AntiAliasedFill)
    g.DrawListSharedData.InitialFlags |= AnchorDrawListFlags_AntiAliasedFill;
  if (g.IO.BackendFlags & AnchorBackendFlags_RendererHasVtxOffset)
    g.DrawListSharedData.InitialFlags |= AnchorDrawListFlags_AllowVtxOffset;

  // Mark rendering data as invalid to prevent user who may have a handle on it to use it.
  for (int n = 0; n < g.Viewports.Size; n++) {
    AnchorViewportP *viewport = g.Viewports[n];
    viewport->DrawDataP.Clear();
  }

  // Drag and drop keep the source ID alive so even if the source disappear our state is consistent
  if (g.DragDropActive && g.DragDropPayload.SourceId == g.ActiveId)
    KeepAliveID(g.DragDropPayload.SourceId);

  // Update HoveredId data
  if (!g.HoveredIdPreviousFrame)
    g.HoveredIdTimer = 0.0f;
  if (!g.HoveredIdPreviousFrame || (g.HoveredId && g.ActiveId == g.HoveredId))
    g.HoveredIdNotActiveTimer = 0.0f;
  if (g.HoveredId)
    g.HoveredIdTimer += g.IO.DeltaTime;
  if (g.HoveredId && g.ActiveId != g.HoveredId)
    g.HoveredIdNotActiveTimer += g.IO.DeltaTime;
  g.HoveredIdPreviousFrame = g.HoveredId;
  g.HoveredIdPreviousFrameUsingMouseWheel = g.HoveredIdUsingMouseWheel;
  g.HoveredId = 0;
  g.HoveredIdAllowOverlap = false;
  g.HoveredIdUsingMouseWheel = false;
  g.HoveredIdDisabled = false;

  // Update ActiveId data (clear reference to active widget if the widget isn't alive anymore)
  if (g.ActiveIdIsAlive != g.ActiveId && g.ActiveIdPreviousFrame == g.ActiveId && g.ActiveId != 0)
    ClearActiveID();
  if (g.ActiveId)
    g.ActiveIdTimer += g.IO.DeltaTime;
  g.LastActiveIdTimer += g.IO.DeltaTime;
  g.ActiveIdPreviousFrame = g.ActiveId;
  g.ActiveIdPreviousFrameWindow = g.ActiveIdWindow;
  g.ActiveIdPreviousFrameHasBeenEditedBefore = g.ActiveIdHasBeenEditedBefore;
  g.ActiveIdIsAlive = 0;
  g.ActiveIdHasBeenEditedThisFrame = false;
  g.ActiveIdPreviousFrameIsAlive = false;
  g.ActiveIdIsJustActivated = false;
  if (g.TempInputId != 0 && g.ActiveId != g.TempInputId)
    g.TempInputId = 0;
  if (g.ActiveId == 0) {
    g.ActiveIdUsingNavDirMask = 0x00;
    g.ActiveIdUsingNavInputMask = 0x00;
    g.ActiveIdUsingKeyInputMask = 0x00;
  }

  // Drag and drop
  g.DragDropAcceptIdPrev = g.DragDropAcceptIdCurr;
  g.DragDropAcceptIdCurr = 0;
  g.DragDropAcceptIdCurrRectSurface = FLT_MAX;
  g.DragDropWithinSource = false;
  g.DragDropWithinTarget = false;
  g.DragDropHoldJustPressedId = 0;

  // Update keyboard input state
  // Synchronize io.KeyMods with individual modifiers io.KeyXXX bools
  g.IO.KeyMods = GetMergedKeyModFlags();
  memcpy(g.IO.KeysDownDurationPrev, g.IO.KeysDownDuration, sizeof(g.IO.KeysDownDuration));
  for (int i = 0; i < ANCHOR_ARRAYSIZE(g.IO.KeysDown); i++)
    g.IO.KeysDownDuration[i] = g.IO.KeysDown[i] ? (g.IO.KeysDownDuration[i] < 0.0f ?
                                                     0.0f :
                                                     g.IO.KeysDownDuration[i] + g.IO.DeltaTime) :
                                                  -1.0f;

  // Update gamepad/keyboard navigation
  NavUpdate();

  // Update mouse input state
  UpdateMouseInputs();

  // Find hovered window
  // (needs to be before UpdateMouseMovingWindowNewFrame so we fill
  // g.HoveredWindowUnderMovingWindow on the mouse release frame)
  UpdateHoveredWindowAndCaptureFlags();

  // Handle user moving window with mouse (at the beginning of the frame to avoid input lag or
  // sheering)
  UpdateMouseMovingWindowNewFrame();

  // Background darkening/whitening
  if (GetTopMostPopupModal() != NULL ||
      (g.NavWindowingTarget != NULL && g.NavWindowingHighlightAlpha > 0.0f))
    g.DimBgRatio = AnchorMin(g.DimBgRatio + g.IO.DeltaTime * 6.0f, 1.0f);
  else
    g.DimBgRatio = AnchorMax(g.DimBgRatio - g.IO.DeltaTime * 10.0f, 0.0f);

  g.MouseCursor = ANCHOR_StandardCursorDefault;
  g.WantCaptureMouseNextFrame = g.WantCaptureKeyboardNextFrame = g.WantTextInputNextFrame = -1;
  g.PlatformImePos = wabi::GfVec2f(
    1.0f,
    1.0f);  // OS Input Method Editor showing on top-left of our window by default

  // Mouse wheel scrolling, scale
  UpdateMouseWheel();

  // Update legacy TAB focus
  UpdateTabFocus();

  // Mark all windows as not visible and compact unused memory.
  ANCHOR_ASSERT(g.WindowsFocusOrder.Size <= g.Windows.Size);
  const float memory_compact_start_time = (g.GcCompactAll ||
                                           g.IO.ConfigMemoryCompactTimer < 0.0f) ?
                                            FLT_MAX :
                                            (float)g.Time - g.IO.ConfigMemoryCompactTimer;
  for (int i = 0; i != g.Windows.Size; i++) {
    AnchorWindow *window = g.Windows[i];
    window->WasActive = window->Active;
    window->BeginCount = 0;
    window->Active = false;
    window->WriteAccessed = false;

    // Garbage collect transient buffers of recently unused windows
    if (!window->WasActive && !window->MemoryCompacted &&
        window->LastTimeActive < memory_compact_start_time)
      GcCompactTransientWindowBuffers(window);
  }

  // Garbage collect transient buffers of recently unused tables
  for (int i = 0; i < g.TablesLastTimeActive.Size; i++)
    if (g.TablesLastTimeActive[i] >= 0.0f && g.TablesLastTimeActive[i] < memory_compact_start_time)
      TableGcCompactTransientBuffers(g.Tables.GetByIndex(i));
  for (int i = 0; i < g.TablesTempDataStack.Size; i++)
    if (g.TablesTempDataStack[i].LastTimeActive >= 0.0f &&
        g.TablesTempDataStack[i].LastTimeActive < memory_compact_start_time)
      TableGcCompactTransientBuffers(&g.TablesTempDataStack[i]);
  if (g.GcCompactAll)
    GcCompactTransientMiscBuffers();
  g.GcCompactAll = false;

  // Closing the focused window restore focus to the first active root window in descending z-order
  if (g.NavWindow && !g.NavWindow->WasActive)
    FocusTopMostWindowUnderOne(NULL, NULL);

  // No window should be open at the beginning of the frame.
  // But in order to allow the user to call NewFrame() multiple times without calling Render(), we
  // are doing an explicit clear.
  g.CurrentWindowStack.resize(0);
  g.BeginPopupStack.resize(0);
  g.ItemFlagsStack.resize(0);
  g.ItemFlagsStack.push_back(AnchorItemFlags_None);
  g.GroupStack.resize(0);
  ClosePopupsOverWindow(g.NavWindow, false);

  // [DEBUG] Item picker tool - start with DebugStartItemPicker() - useful to visually select an
  // item and break into its call-stack.
  UpdateDebugToolItemPicker();

  // Create implicit/fallback window - which we will only render it if the user has added something
  // to it. We don't use "Debug" to avoid colliding with user trying to create a "Debug" window
  // with custom flags. This fallback is particularly important as it avoid ANCHOR:: calls from
  // crashing.
  g.WithinFrameScopeWithImplicitWindow = true;
  SetNextWindowSize(wabi::GfVec2f(400, 400), AnchorCond_FirstUseEver);
  Begin("Debug##Default");
  ANCHOR_ASSERT(g.CurrentWindow->IsFallbackWindow == true);

  CallContextHooks(&g, AnchorContextHookType_NewFramePost);
}

// [DEBUG] Item picker tool - start with DebugStartItemPicker() - useful to visually select an item
// and break into its call-stack.
void ANCHOR::UpdateDebugToolItemPicker()
{
  AnchorContext &g = *G_CTX;
  g.DebugItemPickerBreakId = 0;
  if (g.DebugItemPickerActive) {
    const ANCHOR_ID hovered_id = g.HoveredIdPreviousFrame;
    SetMouseCursor(ANCHOR_StandardCursorMove);
    if (IsKeyPressedMap(AnchorKey_Escape))
      g.DebugItemPickerActive = false;
    if (IsMouseClicked(0) && hovered_id) {
      g.DebugItemPickerBreakId = hovered_id;
      g.DebugItemPickerActive = false;
    }
    SetNextWindowBgAlpha(0.60f);
    BeginTooltip();
    Text("HoveredId: 0x%08X", hovered_id);
    Text("Press ESC to abort picking.");
    TextColored(GetStyleColorVec4(hovered_id ? AnchorCol_Text : AnchorCol_TextDisabled),
                "Click to break in debugger!");
    EndTooltip();
  }
}

void ANCHOR::Initialize(AnchorContext *context)
{
  AnchorContext &g = *context;
  ANCHOR_ASSERT(!g.Initialized && !g.SettingsLoaded);

  // Add .ini handle for AnchorWindow type
  {
    AnchorSettingsHandler ini_handler;
    ini_handler.TypeName = "Window";
    ini_handler.TypeHash = AnchorHashStr("Window");
    ini_handler.ClearAllFn = WindowSettingsHandler_ClearAll;
    ini_handler.ReadOpenFn = WindowSettingsHandler_ReadOpen;
    ini_handler.ReadLineFn = WindowSettingsHandler_ReadLine;
    ini_handler.ApplyAllFn = WindowSettingsHandler_ApplyAll;
    ini_handler.WriteAllFn = WindowSettingsHandler_WriteAll;
    g.SettingsHandlers.push_back(ini_handler);
  }

  // Add .ini handle for AnchorTable type
  TableSettingsInstallHandler(context);

  // Create default viewport
  AnchorViewportP *viewport = ANCHOR_NEW(AnchorViewportP)();
  g.Viewports.push_back(viewport);

#ifdef ANCHOR_HAS_DOCK
#endif  // #ifdef ANCHOR_HAS_DOCK

  g.Initialized = true;
}

// This function is merely here to free heap allocations.
void ANCHOR::Shutdown(AnchorContext *context)
{
  // The fonts atlas can be used prior to calling NewFrame(), so we clear it even if g.Initialized
  // is FALSE (which would happen if we never called NewFrame)
  AnchorContext &g = *context;
  if (g.IO.Fonts && g.FontAtlasOwnedByContext) {
    g.IO.Fonts->Locked = false;
    ANCHOR_DELETE(g.IO.Fonts);
  }
  g.IO.Fonts = NULL;

  // Cleanup of other data are conditional on actually having initialized ANCHOR.
  if (!g.Initialized)
    return;

  // Save settings (unless we haven't attempted to load them: CreateContext/DestroyContext without
  // a call to NewFrame shouldn't save an empty file)
  if (g.SettingsLoaded && g.IO.IniFilename != NULL) {
    AnchorContext *backup_context = G_CTX;
    SetCurrentContext(&g);
    SaveIniSettingsToDisk(g.IO.IniFilename);
    SetCurrentContext(backup_context);
  }

  CallContextHooks(&g, AnchorContextHookType_Shutdown);

  // Clear everything else
  for (int i = 0; i < g.Windows.Size; i++)
    ANCHOR_DELETE(g.Windows[i]);
  g.Windows.clear();
  g.WindowsFocusOrder.clear();
  g.WindowsTempSortBuffer.clear();
  g.CurrentWindow = NULL;
  g.CurrentWindowStack.clear();
  g.WindowsById.Clear();
  g.NavWindow = NULL;
  g.HoveredWindow = g.HoveredWindowUnderMovingWindow = NULL;
  g.ActiveIdWindow = g.ActiveIdPreviousFrameWindow = NULL;
  g.MovingWindow = NULL;
  g.ColorStack.clear();
  g.StyleVarStack.clear();
  g.FontStack.clear();
  g.OpenPopupStack.clear();
  g.BeginPopupStack.clear();

  for (int i = 0; i < g.Viewports.Size; i++)
    ANCHOR_DELETE(g.Viewports[i]);
  g.Viewports.clear();

  g.TabBars.Clear();
  g.CurrentTabBarStack.clear();
  g.ShrinkWidthBuffer.clear();

  g.Tables.Clear();
  for (int i = 0; i < g.TablesTempDataStack.Size; i++)
    g.TablesTempDataStack[i].~AnchorTableTempData();
  g.TablesTempDataStack.clear();
  g.DrawChannelsTempMergeBuffer.clear();

  g.ClipboardHandlerData.clear();
  g.MenusIdSubmittedThisFrame.clear();
  g.InputTextState.ClearFreeMemory();

  g.SettingsWindows.clear();
  g.SettingsHandlers.clear();

  if (g.LogFile) {
#ifndef ANCHOR_DISABLE_TTY_FUNCTIONS
    if (g.LogFile != stdout)
#endif
      ImFileClose(g.LogFile);
    g.LogFile = NULL;
  }
  g.LogBuffer.clear();

  g.Initialized = false;
}

// FIXME: Add a more explicit sort order in the window structure.
static int ANCHOR_CDECL ChildWindowComparer(const void *lhs, const void *rhs)
{
  const AnchorWindow *const a = *(const AnchorWindow *const *)lhs;
  const AnchorWindow *const b = *(const AnchorWindow *const *)rhs;
  if (int d = (a->Flags & AnchorWindowFlags_Popup) - (b->Flags & AnchorWindowFlags_Popup))
    return d;
  if (int d = (a->Flags & AnchorWindowFlags_Tooltip) - (b->Flags & AnchorWindowFlags_Tooltip))
    return d;
  return (a->BeginOrderWithinParent - b->BeginOrderWithinParent);
}

static void AddWindowToSortBuffer(AnchorVector<AnchorWindow *> *out_sorted_windows,
                                  AnchorWindow *window)
{
  out_sorted_windows->push_back(window);
  if (window->Active) {
    int count = window->DC.ChildWindows.Size;
    if (count > 1)
      ImQsort(window->DC.ChildWindows.Data,
              (size_t)count,
              sizeof(AnchorWindow *),
              ChildWindowComparer);
    for (int i = 0; i < count; i++) {
      AnchorWindow *child = window->DC.ChildWindows[i];
      if (child->Active)
        AddWindowToSortBuffer(out_sorted_windows, child);
    }
  }
}

static void AddDrawListToDrawData(AnchorVector<AnchorDrawList *> *out_list,
                                  AnchorDrawList *draw_list)
{
  // Remove trailing command if unused.
  // Technically we could return directly instead of popping, but this make things looks neat in
  // Metrics/Debugger window as well.
  draw_list->_PopUnusedDrawCmd();
  if (draw_list->CmdBuffer.Size == 0)
    return;

  // Draw list sanity check. Detect mismatch between PrimReserve() calls and incrementing
  // _VtxCurrentIdx, _VtxWritePtr etc. May trigger for you if you are using PrimXXX functions
  // incorrectly.
  ANCHOR_ASSERT(draw_list->VtxBuffer.Size == 0 ||
                draw_list->_VtxWritePtr == draw_list->VtxBuffer.Data + draw_list->VtxBuffer.Size);
  ANCHOR_ASSERT(draw_list->IdxBuffer.Size == 0 ||
                draw_list->_IdxWritePtr == draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size);
  if (!(draw_list->Flags & AnchorDrawListFlags_AllowVtxOffset))
    ANCHOR_ASSERT((int)draw_list->_VtxCurrentIdx == draw_list->VtxBuffer.Size);

  // Check that draw_list doesn't use more vertices than indexable (default AnchorDrawIdx =
  // unsigned short = 2 bytes = 64K vertices per AnchorDrawList = per window) If this assert
  // triggers because you are drawing lots of stuff manually:
  // - First, make sure you are coarse clipping yourself and not trying to draw many things outside
  // visible bounds.
  //   Be mindful that the AnchorDrawList API doesn't filter vertices. Use the Metrics/Debugger
  //   window to inspect draw list contents.
  // - If you want large meshes with more than 64K vertices, you can either:
  //   (A) Handle the AnchorDrawCmd::VtxOffset value in your renderer backend, and set
  //   'io.BackendFlags
  //   |= AnchorBackendFlags_RendererHasVtxOffset'.
  //       Most example backends already support this from 1.71. Pre-1.71 backends won't.
  //       Some graphics API such as GL ES 1/2 don't have a way to offset the starting vertex so it
  //       is not supported for them.
  //   (B) Or handle 32-bit indices in your renderer backend, and uncomment '#define AnchorDrawIdx
  //   unsigned int' line in ANCHOR_config.h.
  //       Most example backends already support this. For example, the OpenGL example code detect
  //       index size at compile-time:
  //         glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(AnchorDrawIdx) == 2 ?
  //         GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
  //       Your own engine or render API may use different parameters or function calls to specify
  //       index sizes. 2 and 4 bytes indices are generally supported by most graphics API.
  // - If for some reason neither of those solutions works for you, a workaround is to call
  // BeginChild()/EndChild() before reaching
  //   the 64K limit to split your draw commands in multiple draw lists.
  if (sizeof(AnchorDrawIdx) == 2)
    ANCHOR_ASSERT(draw_list->_VtxCurrentIdx < (1 << 16) &&
                  "Too many vertices in AnchorDrawList using 16-bit indices. Read comment above");

  out_list->push_back(draw_list);
}

static void AddWindowToDrawData(AnchorWindow *window, int layer)
{
  AnchorContext &g = *G_CTX;
  AnchorViewportP *viewport = g.Viewports[0];
  g.IO.MetricsRenderWindows++;
  AddDrawListToDrawData(&viewport->DrawDataBuilder.Layers[layer], window->DrawList);
  for (int i = 0; i < window->DC.ChildWindows.Size; i++) {
    AnchorWindow *child = window->DC.ChildWindows[i];
    if (IsWindowActiveAndVisible(child))  // Clipped children may have been marked not active
      AddWindowToDrawData(child, layer);
  }
}

// Layer is locked for the root window, however child windows may use a different viewport (e.g.
// extruding menu)
static void AddRootWindowToDrawData(AnchorWindow *window)
{
  int layer = (window->Flags & AnchorWindowFlags_Tooltip) ? 1 : 0;
  AddWindowToDrawData(window, layer);
}

void AnchorDrawDataBuilder::FlattenIntoSingleLayer()
{
  int n = Layers[0].Size;
  int size = n;
  for (int i = 1; i < ANCHOR_ARRAYSIZE(Layers); i++)
    size += Layers[i].Size;
  Layers[0].resize(size);
  for (int layer_n = 1; layer_n < ANCHOR_ARRAYSIZE(Layers); layer_n++) {
    AnchorVector<AnchorDrawList *> &layer = Layers[layer_n];
    if (layer.empty())
      continue;
    memcpy(&Layers[0][n], &layer[0], layer.Size * sizeof(AnchorDrawList *));
    n += layer.Size;
    layer.resize(0);
  }
}

static void SetupViewportDrawData(AnchorViewportP *viewport,
                                  AnchorVector<AnchorDrawList *> *draw_lists)
{
  AnchorIO &io = ANCHOR::GetIO();
  AnchorDrawData *draw_data = &viewport->DrawDataP;
  draw_data->Valid = true;
  draw_data->CmdLists = (draw_lists->Size > 0) ? draw_lists->Data : NULL;
  draw_data->CmdListsCount = draw_lists->Size;
  draw_data->TotalVtxCount = draw_data->TotalIdxCount = 0;
  draw_data->DisplayPos = viewport->Pos;
  draw_data->DisplaySize = viewport->Size;
  draw_data->FramebufferScale = io.DisplayFramebufferScale;
  for (int n = 0; n < draw_lists->Size; n++) {
    draw_data->TotalVtxCount += draw_lists->Data[n]->VtxBuffer.Size;
    draw_data->TotalIdxCount += draw_lists->Data[n]->IdxBuffer.Size;
  }
}

// Push a clipping rectangle for both ANCHOR logic (hit-testing etc.) and low-level AnchorDrawList
// rendering.
// - When using this function it is sane to ensure that float are perfectly rounded to integer
// values,
//   so that e.g. (int)(max[0]-min[0]) in user's render produce correct result.
// - If the code here changes, may need to update code of functions like NextColumn() and
// PushColumnClipRect():
//   some frequently called functions which to modify both channels and clipping simultaneously
//   tend to use the more specialized SetWindowClipRectBeforeSetChannel() to avoid extraneous
//   updates of underlying AnchorDrawCmds.
void ANCHOR::PushClipRect(const wabi::GfVec2f &clip_rect_min,
                          const wabi::GfVec2f &clip_rect_max,
                          bool intersect_with_current_clip_rect)
{
  AnchorWindow *window = GetCurrentWindow();
  window->DrawList->PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
  window->ClipRect = window->DrawList->_ClipRectStack.back();
}

void ANCHOR::PopClipRect()
{
  AnchorWindow *window = GetCurrentWindow();
  window->DrawList->PopClipRect();
  window->ClipRect = window->DrawList->_ClipRectStack.back();
}

// This is normally called by Render(). You may want to call it directly if you want to avoid
// calling Render() but the gain will be very minimal.
void ANCHOR::EndFrame()
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(g.Initialized);

  // Don't process EndFrame() multiple times.
  if (g.FrameCountEnded == g.FrameCount)
    return;
  ANCHOR_ASSERT(g.WithinFrameScope && "Forgot to call ANCHOR::NewFrame()?");

  CallContextHooks(&g, AnchorContextHookType_EndFramePre);

  ErrorCheckEndFrameSanityChecks();

  // Notify OS when our Input Method Editor cursor has moved (e.g. CJK inputs using Microsoft IME)
  if (g.IO.ImeSetInputScreenPosFn &&
      (g.PlatformImeLastPos[0] == FLT_MAX ||
       AnchorLengthSqr(g.PlatformImeLastPos - g.PlatformImePos) > 0.0001f)) {
    g.IO.ImeSetInputScreenPosFn((int)g.PlatformImePos[0], (int)g.PlatformImePos[1]);
    g.PlatformImeLastPos = g.PlatformImePos;
  }

  // Hide implicit/fallback "Debug" window if it hasn't been used
  g.WithinFrameScopeWithImplicitWindow = false;
  if (g.CurrentWindow && !g.CurrentWindow->WriteAccessed)
    g.CurrentWindow->Active = false;
  End();

  // Update navigation: CTRL+Tab, wrap-around requests
  NavEndFrame();

  // Drag and Drop: Elapse payload (if delivered, or if source stops being submitted)
  if (g.DragDropActive) {
    bool is_delivered = g.DragDropPayload.Delivery;
    bool is_elapsed = (g.DragDropPayload.DataFrameCount + 1 < g.FrameCount) &&
                      ((g.DragDropSourceFlags & AnchorDragDropFlags_SourceAutoExpirePayload) ||
                       !IsMouseDown(g.DragDropMouseButton));
    if (is_delivered || is_elapsed)
      ClearDragDrop();
  }

  // Drag and Drop: Fallback for source tooltip. This is not ideal but better than nothing.
  if (g.DragDropActive && g.DragDropSourceFrameCount < g.FrameCount &&
      !(g.DragDropSourceFlags & AnchorDragDropFlags_SourceNoPreviewTooltip)) {
    g.DragDropWithinSource = true;
    SetTooltip("...");
    g.DragDropWithinSource = false;
  }

  // End frame
  g.WithinFrameScope = false;
  g.FrameCountEnded = g.FrameCount;

  // Initiate moving window + handle left-click and right-click focus
  UpdateMouseMovingWindowEndFrame();

  // Sort the window list so that all child windows are after their parent
  // We cannot do that on FocusWindow() because children may not exist yet
  g.WindowsTempSortBuffer.resize(0);
  g.WindowsTempSortBuffer.reserve(g.Windows.Size);
  for (int i = 0; i != g.Windows.Size; i++) {
    AnchorWindow *window = g.Windows[i];
    if (window->Active &&
        (window->Flags &
         AnchorWindowFlags_ChildWindow))  // if a child is active its parent will add it
      continue;
    AddWindowToSortBuffer(&g.WindowsTempSortBuffer, window);
  }

  // This usually assert if there is a mismatch between the AnchorWindowFlags_ChildWindow /
  // ParentWindow values and DC.ChildWindows[] in parents, aka we've done something wrong.
  ANCHOR_ASSERT(g.Windows.Size == g.WindowsTempSortBuffer.Size);
  g.Windows.swap(g.WindowsTempSortBuffer);
  g.IO.MetricsActiveWindows = g.WindowsActiveCount;

  // Unlock font atlas
  g.IO.Fonts->Locked = false;

  // Clear Input data for next frame
  g.IO.MouseWheel = g.IO.MouseWheelH = 0.0f;
  g.IO.InputQueueCharacters.resize(0);
  memset(g.IO.NavInputs, 0, sizeof(g.IO.NavInputs));

  CallContextHooks(&g, AnchorContextHookType_EndFramePost);
}

void ANCHOR::Render()
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(g.Initialized);

  if (g.FrameCountEnded != g.FrameCount)
    EndFrame();
  g.FrameCountRendered = g.FrameCount;
  g.IO.MetricsRenderWindows = 0;

  CallContextHooks(&g, AnchorContextHookType_RenderPre);

  // Add background AnchorDrawList (for each active viewport)
  for (int n = 0; n != g.Viewports.Size; n++) {
    AnchorViewportP *viewport = g.Viewports[n];
    viewport->DrawDataBuilder.Clear();
    if (viewport->DrawLists[0] != NULL)
      AddDrawListToDrawData(&viewport->DrawDataBuilder.Layers[0], GetBackgroundDrawList(viewport));
  }

  // Add AnchorDrawList to render
  AnchorWindow *windows_to_render_top_most[2];
  windows_to_render_top_most[0] = (g.NavWindowingTarget &&
                                   !(g.NavWindowingTarget->Flags &
                                     AnchorWindowFlags_NoBringToFrontOnFocus)) ?
                                    g.NavWindowingTarget->RootWindow :
                                    NULL;
  windows_to_render_top_most[1] = (g.NavWindowingTarget ? g.NavWindowingListWindow : NULL);
  for (int n = 0; n != g.Windows.Size; n++) {
    AnchorWindow *window = g.Windows[n];
    ANCHOR_MSVC_WARNING_SUPPRESS(6011);  // Static Analysis false positive "warning C6011:
                                         // Dereferencing NULL pointer 'window'"
    if (IsWindowActiveAndVisible(window) && (window->Flags & AnchorWindowFlags_ChildWindow) == 0 &&
        window != windows_to_render_top_most[0] && window != windows_to_render_top_most[1])
      AddRootWindowToDrawData(window);
  }
  for (int n = 0; n < ANCHOR_ARRAYSIZE(windows_to_render_top_most); n++)
    if (windows_to_render_top_most[n] &&
        IsWindowActiveAndVisible(
          windows_to_render_top_most[n]))  // NavWindowingTarget is always temporarily
                                           // displayed as the top-most window
      AddRootWindowToDrawData(windows_to_render_top_most[n]);

  // Setup AnchorDrawData structures for end-user
  g.IO.MetricsRenderVertices = g.IO.MetricsRenderIndices = 0;
  for (int n = 0; n < g.Viewports.Size; n++) {
    AnchorViewportP *viewport = g.Viewports[n];
    viewport->DrawDataBuilder.FlattenIntoSingleLayer();

    // Draw software mouse cursor if requested by io.MouseDrawCursor flag
    if (g.IO.MouseDrawCursor)
      RenderMouseCursor(GetForegroundDrawList(viewport),
                        g.IO.MousePos,
                        g.Style.MouseCursorScale,
                        g.MouseCursor,
                        ANCHOR_COL32_WHITE,
                        ANCHOR_COL32_BLACK,
                        ANCHOR_COL32(0, 0, 0, 48));

    // Add foreground AnchorDrawList (for each active viewport)
    if (viewport->DrawLists[1] != NULL)
      AddDrawListToDrawData(&viewport->DrawDataBuilder.Layers[0], GetForegroundDrawList(viewport));

    SetupViewportDrawData(viewport, &viewport->DrawDataBuilder.Layers[0]);
    AnchorDrawData *draw_data = &viewport->DrawDataP;
    g.IO.MetricsRenderVertices += draw_data->TotalVtxCount;
    g.IO.MetricsRenderIndices += draw_data->TotalIdxCount;
  }

  CallContextHooks(&g, AnchorContextHookType_RenderPost);
}

// Calculate text size. Text can be multi-line. Optionally ignore text after a ## marker.
// CalcTextSize("") should return wabi::GfVec2f(0.0f, g.FontSize)
wabi::GfVec2f ANCHOR::CalcTextSize(const char *text,
                             const char *text_end,
                             bool hide_text_after_double_hash,
                             float wrap_width)
{
  AnchorContext &g = *G_CTX;

  const char *text_display_end;
  if (hide_text_after_double_hash)
    text_display_end = FindRenderedTextEnd(text, text_end);  // Hide anything after a '##' string
  else
    text_display_end = text_end;

  AnchorFont *font = g.Font;
  const float font_size = g.FontSize;
  if (text == text_display_end)
    return wabi::GfVec2f(0.0f, font_size);
  wabi::GfVec2f text_size =
    font->CalcTextSizeA(font_size, FLT_MAX, wrap_width, text, text_display_end, NULL);

  // Round
  // FIXME: This has been here since Dec 2015 (7b0bf230) but down the line we want this out.
  // FIXME: Investigate using ceilf or e.g.
  // - https://git.musl-libc.org/cgit/musl/tree/src/math/ceilf.c
  // - https://embarkstudios.github.io/rust-gpu/api/src/libm/math/ceilf.rs.html
  text_size[0] = ANCHOR_FLOOR(text_size[0] + 0.99999f);

  return text_size;
}

// Find window given position, search front-to-back
// FIXME: Note that we have an inconsequential lag here: OuterRectClipped is updated in Begin(), so
// windows moved programmatically with SetWindowPos() and not SetNextWindowPos() will have that
// rectangle lagging by a frame at the time FindHoveredWindow() is called, aka before the next
// Begin(). Moving window isn't affected.
static void FindHoveredWindow()
{
  AnchorContext &g = *G_CTX;

  AnchorWindow *hovered_window = NULL;
  AnchorWindow *hovered_window_ignoring_moving_window = NULL;
  if (g.MovingWindow && !(g.MovingWindow->Flags & AnchorWindowFlags_NoMouseInputs))
    hovered_window = g.MovingWindow;

  wabi::GfVec2f padding_regular = g.Style.TouchExtraPadding;
  wabi::GfVec2f padding_for_resize = g.IO.ConfigWindowsResizeFromEdges ? g.WindowsHoverPadding :
                                                                   padding_regular;
  for (int i = g.Windows.Size - 1; i >= 0; i--) {
    AnchorWindow *window = g.Windows[i];
    ANCHOR_MSVC_WARNING_SUPPRESS(28182);  // [Static Analyzer] Dereferencing NULL pointer.
    if (!window->Active || window->Hidden)
      continue;
    if (window->Flags & AnchorWindowFlags_NoMouseInputs)
      continue;

    // Using the clipped AABB, a child window will typically be clipped by its parent (not always)
    AnchorBBox bb(window->OuterRectClipped);
    if (window->Flags & (AnchorWindowFlags_ChildWindow | AnchorWindowFlags_NoResize |
                         AnchorWindowFlags_AlwaysAutoResize))
      bb.Expand(padding_regular);
    else
      bb.Expand(padding_for_resize);
    if (!bb.Contains(g.IO.MousePos))
      continue;

    // Support for one rectangular hole in any given window
    // FIXME: Consider generalizing hit-testing override (with more generic data, callback, etc.)
    // (#1512)
    if (window->HitTestHoleSize[0] != 0) {
      wabi::GfVec2f hole_pos(window->Pos[0] + (float)window->HitTestHoleOffset[0],
                       window->Pos[1] + (float)window->HitTestHoleOffset[1]);
      wabi::GfVec2f hole_size((float)window->HitTestHoleSize[0], (float)window->HitTestHoleSize[1]);
      if (AnchorBBox(hole_pos, hole_pos + hole_size).Contains(g.IO.MousePos))
        continue;
    }

    if (hovered_window == NULL)
      hovered_window = window;
    ANCHOR_MSVC_WARNING_SUPPRESS(28182);  // [Static Analyzer] Dereferencing NULL pointer.
    if (hovered_window_ignoring_moving_window == NULL &&
        (!g.MovingWindow || window->RootWindow != g.MovingWindow->RootWindow))
      hovered_window_ignoring_moving_window = window;
    if (hovered_window && hovered_window_ignoring_moving_window)
      break;
  }

  g.HoveredWindow = hovered_window;
  g.HoveredWindowUnderMovingWindow = hovered_window_ignoring_moving_window;
}

// Test if mouse cursor is hovering given rectangle
// NB- Rectangle is clipped by our current clip setting
// NB- Expand the rectangle to be generous on imprecise inputs systems (g.Style.TouchExtraPadding)
bool ANCHOR::IsMouseHoveringRect(const wabi::GfVec2f &r_min, const wabi::GfVec2f &r_max, bool clip)
{
  AnchorContext &g = *G_CTX;

  // Clip
  AnchorBBox rect_clipped(r_min, r_max);
  if (clip)
    rect_clipped.ClipWith(g.CurrentWindow->ClipRect);

  // Expand for touch input
  const AnchorBBox rect_for_touch(rect_clipped.Min - g.Style.TouchExtraPadding,
                                  rect_clipped.Max + g.Style.TouchExtraPadding);
  if (!rect_for_touch.Contains(g.IO.MousePos))
    return false;
  return true;
}

int ANCHOR::GetKeyIndex(AnchorKey ANCHOR_key)
{
  ANCHOR_ASSERT(ANCHOR_key >= 0 && ANCHOR_key < AnchorKey_COUNT);
  AnchorContext &g = *G_CTX;
  return g.IO.KeyMap[ANCHOR_key];
}

// Note that ANCHOR doesn't know the semantic of each entry of io.KeysDown[]!
// Use your own indices/enums according to how your backend/engine stored them into io.KeysDown[]!
bool ANCHOR::IsKeyDown(int user_key_index)
{
  if (user_key_index < 0)
    return false;
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(user_key_index >= 0 && user_key_index < ANCHOR_ARRAYSIZE(g.IO.KeysDown));
  return g.IO.KeysDown[user_key_index];
}

// t0 = previous time (e.g.: g.Time - g.IO.DeltaTime)
// t1 = current time (e.g.: g.Time)
// An event is triggered at:
//  t = 0.0f     t = repeat_delay,    t = repeat_delay + repeat_rate*N
int ANCHOR::CalcTypematicRepeatAmount(float t0, float t1, float repeat_delay, float repeat_rate)
{
  if (t1 == 0.0f)
    return 1;
  if (t0 >= t1)
    return 0;
  if (repeat_rate <= 0.0f)
    return (t0 < repeat_delay) && (t1 >= repeat_delay);
  const int count_t0 = (t0 < repeat_delay) ? -1 : (int)((t0 - repeat_delay) / repeat_rate);
  const int count_t1 = (t1 < repeat_delay) ? -1 : (int)((t1 - repeat_delay) / repeat_rate);
  const int count = count_t1 - count_t0;
  return count;
}

int ANCHOR::GetKeyPressedAmount(int key_index, float repeat_delay, float repeat_rate)
{
  AnchorContext &g = *G_CTX;
  if (key_index < 0)
    return 0;
  ANCHOR_ASSERT(key_index >= 0 && key_index < ANCHOR_ARRAYSIZE(g.IO.KeysDown));
  const float t = g.IO.KeysDownDuration[key_index];
  return CalcTypematicRepeatAmount(t - g.IO.DeltaTime, t, repeat_delay, repeat_rate);
}

bool ANCHOR::IsKeyPressed(int user_key_index, bool repeat)
{
  AnchorContext &g = *G_CTX;
  if (user_key_index < 0)
    return false;
  ANCHOR_ASSERT(user_key_index >= 0 && user_key_index < ANCHOR_ARRAYSIZE(g.IO.KeysDown));
  const float t = g.IO.KeysDownDuration[user_key_index];
  if (t == 0.0f)
    return true;
  if (repeat && t > g.IO.KeyRepeatDelay)
    return GetKeyPressedAmount(user_key_index, g.IO.KeyRepeatDelay, g.IO.KeyRepeatRate) > 0;
  return false;
}

bool ANCHOR::IsKeyReleased(int user_key_index)
{
  AnchorContext &g = *G_CTX;
  if (user_key_index < 0)
    return false;
  ANCHOR_ASSERT(user_key_index >= 0 && user_key_index < ANCHOR_ARRAYSIZE(g.IO.KeysDown));
  return g.IO.KeysDownDurationPrev[user_key_index] >= 0.0f && !g.IO.KeysDown[user_key_index];
}

bool ANCHOR::IsMouseDown(AnchorMouseButton button)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(button >= 0 && button < ANCHOR_ARRAYSIZE(g.IO.MouseDown));
  return g.IO.MouseDown[button];
}

bool ANCHOR::IsMouseClicked(AnchorMouseButton button, bool repeat)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(button >= 0 && button < ANCHOR_ARRAYSIZE(g.IO.MouseDown));
  const float t = g.IO.MouseDownDuration[button];
  if (t == 0.0f)
    return true;

  if (repeat && t > g.IO.KeyRepeatDelay) {
    // FIXME: 2019/05/03: Our old repeat code was wrong here and led to doubling the repeat rate,
    // which made it an ok rate for repeat on mouse hold.
    int amount = CalcTypematicRepeatAmount(t - g.IO.DeltaTime,
                                           t,
                                           g.IO.KeyRepeatDelay,
                                           g.IO.KeyRepeatRate * 0.50f);
    if (amount > 0)
      return true;
  }
  return false;
}

bool ANCHOR::IsMouseReleased(AnchorMouseButton button)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(button >= 0 && button < ANCHOR_ARRAYSIZE(g.IO.MouseDown));
  return g.IO.MouseReleased[button];
}

bool ANCHOR::IsMouseDoubleClicked(AnchorMouseButton button)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(button >= 0 && button < ANCHOR_ARRAYSIZE(g.IO.MouseDown));
  return g.IO.MouseDoubleClicked[button];
}

// Return if a mouse click/drag went past the given threshold. Valid to call during the
// MouseReleased frame. [Internal] This doesn't test if the button is pressed
bool ANCHOR::IsMouseDragPastThreshold(AnchorMouseButton button, float lock_threshold)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(button >= 0 && button < ANCHOR_ARRAYSIZE(g.IO.MouseDown));
  if (lock_threshold < 0.0f)
    lock_threshold = g.IO.MouseDragThreshold;
  return g.IO.MouseDragMaxDistanceSqr[button] >= lock_threshold * lock_threshold;
}

bool ANCHOR::IsMouseDragging(AnchorMouseButton button, float lock_threshold)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(button >= 0 && button < ANCHOR_ARRAYSIZE(g.IO.MouseDown));
  if (!g.IO.MouseDown[button])
    return false;
  return IsMouseDragPastThreshold(button, lock_threshold);
}

wabi::GfVec2f ANCHOR::GetMousePos()
{
  AnchorContext &g = *G_CTX;
  return g.IO.MousePos;
}

// NB: prefer to call right after BeginPopup(). At the time Selectable/MenuItem is activated, the
// popup is already closed!
wabi::GfVec2f ANCHOR::GetMousePosOnOpeningCurrentPopup()
{
  AnchorContext &g = *G_CTX;
  if (g.BeginPopupStack.Size > 0)
    return g.OpenPopupStack[g.BeginPopupStack.Size - 1].OpenMousePos;
  return g.IO.MousePos;
}

// We typically use wabi::GfVec2f(-FLT_MAX,-FLT_MAX) to denote an invalid mouse position.
bool ANCHOR::IsMousePosValid(const wabi::GfVec2f *mouse_pos)
{
  // The assert is only to silence a false-positive in XCode Static Analysis.
  // Because G_CTX is not dereferenced in every code path, the static analyzer assume that it may
  // be NULL (which it doesn't for other functions).
  ANCHOR_ASSERT(G_CTX != NULL);
  const float MOUSE_INVALID = -256000.0f;
  wabi::GfVec2f p = mouse_pos ? *mouse_pos : G_CTX->IO.MousePos;
  return p[0] >= MOUSE_INVALID && p[1] >= MOUSE_INVALID;
}

bool ANCHOR::IsAnyMouseDown()
{
  AnchorContext &g = *G_CTX;
  for (int n = 0; n < ANCHOR_ARRAYSIZE(g.IO.MouseDown); n++)
    if (g.IO.MouseDown[n])
      return true;
  return false;
}

// Return the delta from the initial clicking position while the mouse button is clicked or was
// just released. This is locked and return 0.0f until the mouse moves past a distance threshold at
// least once. NB: This is only valid if IsMousePosValid(). backends in theory should always keep
// mouse position valid when dragging even outside the client window.
wabi::GfVec2f ANCHOR::GetMouseDragDelta(AnchorMouseButton button, float lock_threshold)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(button >= 0 && button < ANCHOR_ARRAYSIZE(g.IO.MouseDown));
  if (lock_threshold < 0.0f)
    lock_threshold = g.IO.MouseDragThreshold;
  if (g.IO.MouseDown[button] || g.IO.MouseReleased[button])
    if (g.IO.MouseDragMaxDistanceSqr[button] >= lock_threshold * lock_threshold)
      if (IsMousePosValid(&g.IO.MousePos) && IsMousePosValid(&g.IO.MouseClickedPos[button]))
        return g.IO.MousePos - g.IO.MouseClickedPos[button];
  return wabi::GfVec2f(0.0f, 0.0f);
}

void ANCHOR::ResetMouseDragDelta(AnchorMouseButton button)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(button >= 0 && button < ANCHOR_ARRAYSIZE(g.IO.MouseDown));
  // NB: We don't need to reset g.IO.MouseDragMaxDistanceSqr
  g.IO.MouseClickedPos[button] = g.IO.MousePos;
}

AnchorMouseCursor ANCHOR::GetMouseCursor()
{
  return G_CTX->MouseCursor;
}

void ANCHOR::SetMouseCursor(AnchorMouseCursor cursor_type)
{
  G_CTX->MouseCursor = cursor_type;
}

void ANCHOR::CaptureKeyboardFromApp(bool capture)
{
  G_CTX->WantCaptureKeyboardNextFrame = capture ? 1 : 0;
}

void ANCHOR::CaptureMouseFromApp(bool capture)
{
  G_CTX->WantCaptureMouseNextFrame = capture ? 1 : 0;
}

bool ANCHOR::IsItemActive()
{
  AnchorContext &g = *G_CTX;
  if (g.ActiveId) {
    AnchorWindow *window = g.CurrentWindow;
    return g.ActiveId == window->DC.LastItemId;
  }
  return false;
}

bool ANCHOR::IsItemActivated()
{
  AnchorContext &g = *G_CTX;
  if (g.ActiveId) {
    AnchorWindow *window = g.CurrentWindow;
    if (g.ActiveId == window->DC.LastItemId && g.ActiveIdPreviousFrame != window->DC.LastItemId)
      return true;
  }
  return false;
}

bool ANCHOR::IsItemDeactivated()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  if (window->DC.LastItemStatusFlags & AnchorItemStatusFlags_HasDeactivated)
    return (window->DC.LastItemStatusFlags & AnchorItemStatusFlags_Deactivated) != 0;
  return (g.ActiveIdPreviousFrame == window->DC.LastItemId && g.ActiveIdPreviousFrame != 0 &&
          g.ActiveId != window->DC.LastItemId);
}

bool ANCHOR::IsItemDeactivatedAfterEdit()
{
  AnchorContext &g = *G_CTX;
  return IsItemDeactivated() && (g.ActiveIdPreviousFrameHasBeenEditedBefore ||
                                 (g.ActiveId == 0 && g.ActiveIdHasBeenEditedBefore));
}

// == GetItemID() == GetFocusID()
bool ANCHOR::IsItemFocused()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  if (g.NavId != window->DC.LastItemId || g.NavId == 0)
    return false;
  return true;
}

// Important: this can be useful but it is NOT equivalent to the behavior of e.g.Button()!
// Most widgets have specific reactions based on mouse-up/down state, mouse position etc.
bool ANCHOR::IsItemClicked(AnchorMouseButton mouse_button)
{
  return IsMouseClicked(mouse_button) && IsItemHovered(AnchorHoveredFlags_None);
}

bool ANCHOR::IsItemToggledOpen()
{
  AnchorContext &g = *G_CTX;
  return (g.CurrentWindow->DC.LastItemStatusFlags & AnchorItemStatusFlags_ToggledOpen) ? true :
                                                                                         false;
}

bool ANCHOR::IsItemToggledSelection()
{
  AnchorContext &g = *G_CTX;
  return (g.CurrentWindow->DC.LastItemStatusFlags & AnchorItemStatusFlags_ToggledSelection) ?
           true :
           false;
}

bool ANCHOR::IsAnyItemHovered()
{
  AnchorContext &g = *G_CTX;
  return g.HoveredId != 0 || g.HoveredIdPreviousFrame != 0;
}

bool ANCHOR::IsAnyItemActive()
{
  AnchorContext &g = *G_CTX;
  return g.ActiveId != 0;
}

bool ANCHOR::IsAnyItemFocused()
{
  AnchorContext &g = *G_CTX;
  return g.NavId != 0 && !g.NavDisableHighlight;
}

bool ANCHOR::IsItemVisible()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->ClipRect.Overlaps(window->DC.LastItemRect);
}

bool ANCHOR::IsItemEdited()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return (window->DC.LastItemStatusFlags & AnchorItemStatusFlags_Edited) != 0;
}

// Allow last item to be overlapped by a subsequent item. Both may be activated during the same
// frame before the later one takes priority.
// FIXME: Although this is exposed, its interaction and ideal idiom with using
// AnchorButtonFlags_AllowItemOverlap flag are extremely confusing, need rework.
void ANCHOR::SetItemAllowOverlap()
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ID id = g.CurrentWindow->DC.LastItemId;
  if (g.HoveredId == id)
    g.HoveredIdAllowOverlap = true;
  if (g.ActiveId == id)
    g.ActiveIdAllowOverlap = true;
}

void ANCHOR::SetItemUsingMouseWheel()
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ID id = g.CurrentWindow->DC.LastItemId;
  if (g.HoveredId == id)
    g.HoveredIdUsingMouseWheel = true;
  if (g.ActiveId == id)
    g.ActiveIdUsingMouseWheel = true;
}

wabi::GfVec2f ANCHOR::GetItemRectMin()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->DC.LastItemRect.Min;
}

wabi::GfVec2f ANCHOR::GetItemRectMax()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->DC.LastItemRect.Max;
}

wabi::GfVec2f ANCHOR::GetItemRectSize()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->DC.LastItemRect.GetSize();
}

bool ANCHOR::BeginChildEx(const char *name,
                          ANCHOR_ID id,
                          const wabi::GfVec2f &size_arg,
                          bool border,
                          AnchorWindowFlags flags)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *parent_window = g.CurrentWindow;

  flags |= AnchorWindowFlags_NoTitleBar | AnchorWindowFlags_NoResize |
           AnchorWindowFlags_NoSavedSettings | AnchorWindowFlags_ChildWindow;
  flags |= (parent_window->Flags & AnchorWindowFlags_NoMove);  // Inherit the NoMove flag

  // Size
  const wabi::GfVec2f content_avail = GetContentRegionAvail();
  wabi::GfVec2f size = AnchorFloor(size_arg);
  const int auto_fit_axises = ((size[0] == 0.0f) ? (1 << ANCHOR_Axis_X) : 0x00) |
                              ((size[1] == 0.0f) ? (1 << ANCHOR_Axis_Y) : 0x00);
  if (size[0] <= 0.0f)
    size[0] = AnchorMax(content_avail[0] + size[0],
                        4.0f);  // Arbitrary minimum child size (0.0f causing too much issues)
  if (size[1] <= 0.0f)
    size[1] = AnchorMax(content_avail[1] + size[1], 4.0f);
  SetNextWindowSize(size);

  // Build up name. If you need to append to a same child from multiple location in the ID stack,
  // use BeginChild(ANCHOR_ID id) with a stable value.
  if (name)
    AnchorFormatString(g.TempBuffer,
                       ANCHOR_ARRAYSIZE(g.TempBuffer),
                       "%s/%s_%08X",
                       parent_window->Name,
                       name,
                       id);
  else
    AnchorFormatString(g.TempBuffer,
                       ANCHOR_ARRAYSIZE(g.TempBuffer),
                       "%s/%08X",
                       parent_window->Name,
                       id);

  const float backup_border_size = g.Style.ChildBorderSize;
  if (!border)
    g.Style.ChildBorderSize = 0.0f;
  bool ret = Begin(g.TempBuffer, NULL, flags);
  g.Style.ChildBorderSize = backup_border_size;

  AnchorWindow *child_window = g.CurrentWindow;
  child_window->ChildId = id;
  child_window->AutoFitChildAxises = (AnchorS8)auto_fit_axises;

  // Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
  // While this is not really documented/defined, it seems that the expected thing to do.
  if (child_window->BeginCount == 1)
    parent_window->DC.CursorPos = child_window->Pos;

  // Process navigation-in immediately so NavInit can run on first frame
  if (g.NavActivateId == id && !(flags & AnchorWindowFlags_NavFlattened) &&
      (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavHasScroll)) {
    FocusWindow(child_window);
    NavInitWindow(child_window, false);
    SetActiveID(id + 1, child_window);  // Steal ActiveId with another arbitrary id so that
                                        // key-press won't activate child item
    g.ActiveIdSource = ANCHORInputSource_Nav;
  }
  return ret;
}

bool ANCHOR::BeginChild(const char *str_id,
                        const wabi::GfVec2f &size_arg,
                        bool border,
                        AnchorWindowFlags extra_flags)
{
  AnchorWindow *window = GetCurrentWindow();
  return BeginChildEx(str_id, window->GetID(str_id), size_arg, border, extra_flags);
}

bool ANCHOR::BeginChild(ANCHOR_ID id,
                        const wabi::GfVec2f &size_arg,
                        bool border,
                        AnchorWindowFlags extra_flags)
{
  ANCHOR_ASSERT(id != 0);
  return BeginChildEx(NULL, id, size_arg, border, extra_flags);
}

void ANCHOR::EndChild()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  ANCHOR_ASSERT(g.WithinEndChild == false);
  ANCHOR_ASSERT(window->Flags &
                AnchorWindowFlags_ChildWindow);  // Mismatched BeginChild()/EndChild() calls

  g.WithinEndChild = true;
  if (window->BeginCount > 1) {
    End();
  } else {
    wabi::GfVec2f sz = window->Size;
    if (window->AutoFitChildAxises &
        (1 << ANCHOR_Axis_X))  // Arbitrary minimum zero-ish child size of 4.0f
                               // causes less trouble than a 0.0f
      sz[0] = AnchorMax(4.0f, sz[0]);
    if (window->AutoFitChildAxises & (1 << ANCHOR_Axis_Y))
      sz[1] = AnchorMax(4.0f, sz[1]);
    End();

    AnchorWindow *parent_window = g.CurrentWindow;
    AnchorBBox bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
    ItemSize(sz);
    if ((window->DC.NavLayersActiveMask != 0 || window->DC.NavHasScroll) &&
        !(window->Flags & AnchorWindowFlags_NavFlattened)) {
      ItemAdd(bb, window->ChildId);
      RenderNavHighlight(bb, window->ChildId);

      // When browsing a window that has no activable items (scroll only) we keep a highlight on
      // the child
      if (window->DC.NavLayersActiveMask == 0 && window == g.NavWindow)
        RenderNavHighlight(AnchorBBox(bb.Min - wabi::GfVec2f(2, 2), bb.Max + wabi::GfVec2f(2, 2)),
                           g.NavId,
                           AnchorNavHighlightFlags_TypeThin);
    } else {
      // Not navigable into
      ItemAdd(bb, 0);
    }
    if (g.HoveredWindow == window)
      parent_window->DC.LastItemStatusFlags |= AnchorItemStatusFlags_HoveredWindow;
  }
  g.WithinEndChild = false;
  g.LogLinePosY = -FLT_MAX;  // To enforce a carriage return
}

// Helper to create a child window / scrolling region that looks like a normal widget frame.
bool ANCHOR::BeginChildFrame(ANCHOR_ID id, const wabi::GfVec2f &size, AnchorWindowFlags extra_flags)
{
  AnchorContext &g = *G_CTX;
  const AnchorStyle &style = g.Style;
  PushStyleColor(AnchorCol_ChildBg, style.Colors[AnchorCol_FrameBg]);
  PushStyleVar(AnchorStyleVar_ChildRounding, style.FrameRounding);
  PushStyleVar(AnchorStyleVar_ChildBorderSize, style.FrameBorderSize);
  PushStyleVar(AnchorStyleVar_WindowPadding, style.FramePadding);
  bool ret = BeginChild(id,
                        size,
                        true,
                        AnchorWindowFlags_NoMove | AnchorWindowFlags_AlwaysUseWindowPadding |
                          extra_flags);
  PopStyleVar(3);
  PopStyleColor();
  return ret;
}

void ANCHOR::EndChildFrame()
{
  EndChild();
}

static void SetWindowConditionAllowFlags(AnchorWindow *window, AnchorCond flags, bool enabled)
{
  window->SetWindowPosAllowFlags = enabled ? (window->SetWindowPosAllowFlags | flags) :
                                             (window->SetWindowPosAllowFlags & ~flags);
  window->SetWindowSizeAllowFlags = enabled ? (window->SetWindowSizeAllowFlags | flags) :
                                              (window->SetWindowSizeAllowFlags & ~flags);
  window->SetWindowCollapsedAllowFlags = enabled ? (window->SetWindowCollapsedAllowFlags | flags) :
                                                   (window->SetWindowCollapsedAllowFlags & ~flags);
}

AnchorWindow *ANCHOR::FindWindowByID(ANCHOR_ID id)
{
  AnchorContext &g = *G_CTX;
  return (AnchorWindow *)g.WindowsById.GetVoidPtr(id);
}

AnchorWindow *ANCHOR::FindWindowByName(const char *name)
{
  ANCHOR_ID id = AnchorHashStr(name);
  return FindWindowByID(id);
}

static void ApplyWindowSettings(AnchorWindow *window, AnchorWindowSettings *settings)
{
  window->Pos = AnchorFloor(wabi::GfVec2f(settings->Pos[0], settings->Pos[1]));
  if (settings->Size[0] > 0 && settings->Size[1] > 0)
    window->Size = window->SizeFull = AnchorFloor(wabi::GfVec2f(settings->Size[0], settings->Size[1]));
  window->Collapsed = settings->Collapsed;
}

static AnchorWindow *CreateNewWindow(const char *name, AnchorWindowFlags flags)
{
  AnchorContext &g = *G_CTX;
  // ANCHOR_DEBUG_LOG("CreateNewWindow '%s', flags = 0x%08X\n", name, flags);

  // Create window the first time
  AnchorWindow *window = ANCHOR_NEW(AnchorWindow)(&g, name);
  window->Flags = flags;
  g.WindowsById.SetVoidPtr(window->ID, window);

  // Default/arbitrary window position. Use SetNextWindowPos() with the appropriate condition flag
  // to change the initial position of a window.
  const AnchorViewport *main_viewport = ANCHOR::GetMainViewport();
  window->Pos = main_viewport->Pos + wabi::GfVec2f(60, 60);

  // User can disable loading and saving of settings. Tooltip and child windows also don't store
  // settings.
  if (!(flags & AnchorWindowFlags_NoSavedSettings))
    if (AnchorWindowSettings *settings = ANCHOR::FindWindowSettings(window->ID)) {
      // Retrieve settings from .ini file
      window->SettingsOffset = g.SettingsWindows.offset_from_ptr(settings);
      SetWindowConditionAllowFlags(window, AnchorCond_FirstUseEver, false);
      ApplyWindowSettings(window, settings);
    }
  window->DC.CursorStartPos = window->DC.CursorMaxPos =
    window->Pos;  // So first call to CalcContentSize() doesn't return crazy values

  if ((flags & AnchorWindowFlags_AlwaysAutoResize) != 0) {
    window->AutoFitFramesX = window->AutoFitFramesY = 2;
    window->AutoFitOnlyGrows = false;
  } else {
    if (window->Size[0] <= 0.0f)
      window->AutoFitFramesX = 2;
    if (window->Size[1] <= 0.0f)
      window->AutoFitFramesY = 2;
    window->AutoFitOnlyGrows = (window->AutoFitFramesX > 0) || (window->AutoFitFramesY > 0);
  }

  if (!(flags & AnchorWindowFlags_ChildWindow)) {
    g.WindowsFocusOrder.push_back(window);
    window->FocusOrder = (short)(g.WindowsFocusOrder.Size - 1);
  }

  if (flags & AnchorWindowFlags_NoBringToFrontOnFocus)
    g.Windows.push_front(window);  // Quite slow but rare and only once
  else
    g.Windows.push_back(window);
  return window;
}

static wabi::GfVec2f CalcWindowSizeAfterConstraint(AnchorWindow *window, const wabi::GfVec2f &size_desired)
{
  AnchorContext &g = *G_CTX;
  wabi::GfVec2f new_size = size_desired;
  if (g.NextWindowData.Flags & AnchorNextWindowDataFlags_HasSizeConstraint) {
    // Using -1,-1 on either X/Y axis to preserve the current size.
    AnchorBBox cr = g.NextWindowData.SizeConstraintRect;
    new_size[0] = (cr.Min[0] >= 0 && cr.Max[0] >= 0) ?
                    AnchorClamp(new_size[0], cr.Min[0], cr.Max[0]) :
                    window->SizeFull[0];
    new_size[1] = (cr.Min[1] >= 0 && cr.Max[1] >= 0) ?
                    AnchorClamp(new_size[1], cr.Min[1], cr.Max[1]) :
                    window->SizeFull[1];
    if (g.NextWindowData.SizeCallback) {
      AnchorSizeCallbackData data;
      data.UserData = g.NextWindowData.SizeCallbackUserData;
      data.Pos = window->Pos;
      data.CurrentSize = window->SizeFull;
      data.DesiredSize = new_size;
      g.NextWindowData.SizeCallback(&data);
      new_size = data.DesiredSize;
    }
    new_size[0] = ANCHOR_FLOOR(new_size[0]);
    new_size[1] = ANCHOR_FLOOR(new_size[1]);
  }

  // Minimum size
  if (!(window->Flags & (AnchorWindowFlags_ChildWindow | AnchorWindowFlags_AlwaysAutoResize))) {
    AnchorWindow *window_for_height = window;
    const float decoration_up_height = window_for_height->TitleBarHeight() +
                                       window_for_height->MenuBarHeight();
    new_size = AnchorMax(new_size, g.Style.WindowMinSize);
    new_size[1] = AnchorMax(
      new_size[1],
      decoration_up_height +
        AnchorMax(0.0f,
                  g.Style.WindowRounding - 1.0f));  // Reduce artifacts with very small windows
  }
  return new_size;
}

static void CalcWindowContentSizes(AnchorWindow *window,
                                   wabi::GfVec2f *content_size_current,
                                   wabi::GfVec2f *content_size_ideal)
{
  bool preserve_old_content_sizes = false;
  if (window->Collapsed && window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
    preserve_old_content_sizes = true;
  else if (window->Hidden && window->HiddenFramesCannotSkipItems == 0 &&
           window->HiddenFramesCanSkipItems > 0)
    preserve_old_content_sizes = true;
  if (preserve_old_content_sizes) {
    *content_size_current = window->ContentSize;
    *content_size_ideal = window->ContentSizeIdeal;
    return;
  }

  content_size_current->data()[0] = (window->ContentSizeExplicit[0] != 0.0f) ?
                                      window->ContentSizeExplicit[0] :
                                      ANCHOR_FLOOR(window->DC.CursorMaxPos[0] -
                                                   window->DC.CursorStartPos[0]);
  content_size_current->data()[1] = (window->ContentSizeExplicit[1] != 0.0f) ?
                                      window->ContentSizeExplicit[1] :
                                      ANCHOR_FLOOR(window->DC.CursorMaxPos[1] -
                                                   window->DC.CursorStartPos[1]);
  content_size_ideal->data()[0] = (window->ContentSizeExplicit[0] != 0.0f) ?
                                    window->ContentSizeExplicit[0] :
                                    ANCHOR_FLOOR(AnchorMax(window->DC.CursorMaxPos[0],
                                                           window->DC.IdealMaxPos[0]) -
                                                 window->DC.CursorStartPos[0]);
  content_size_ideal->data()[1] = (window->ContentSizeExplicit[1] != 0.0f) ?
                                    window->ContentSizeExplicit[1] :
                                    ANCHOR_FLOOR(AnchorMax(window->DC.CursorMaxPos[1],
                                                           window->DC.IdealMaxPos[1]) -
                                                 window->DC.CursorStartPos[1]);
}

static wabi::GfVec2f CalcWindowAutoFitSize(AnchorWindow *window, const wabi::GfVec2f &size_contents)
{
  AnchorContext &g = *G_CTX;
  AnchorStyle &style = g.Style;
  const float decoration_up_height = window->TitleBarHeight() + window->MenuBarHeight();
  wabi::GfVec2f size_pad = window->WindowPadding * 2.0f;
  wabi::GfVec2f size_desired = size_contents + size_pad + wabi::GfVec2f(0.0f, decoration_up_height);
  if (window->Flags & AnchorWindowFlags_Tooltip) {
    // Tooltip always resize
    return size_desired;
  } else {
    // Maximum window size is determined by the viewport size or monitor size
    const bool is_popup = (window->Flags & AnchorWindowFlags_Popup) != 0;
    const bool is_menu = (window->Flags & AnchorWindowFlags_ChildMenu) != 0;
    wabi::GfVec2f size_min = style.WindowMinSize;
    if (is_popup || is_menu)  // Popups and menus bypass style.WindowMinSize by default, but we
                              // give then a non-zero minimum size to facilitate understanding
                              // problematic cases (e.g. empty popups)
      size_min = AnchorMin(size_min, wabi::GfVec2f(4.0f, 4.0f));

    // FIXME-VIEWPORT-WORKAREA: May want to use GetWorkSize() instead of Size depending on the type
    // of windows?
    wabi::GfVec2f avail_size = ANCHOR::GetMainViewport()->Size;
    wabi::GfVec2f size_auto_fit = AnchorClamp(
      size_desired,
      size_min,
      AnchorMax(size_min, avail_size - style.DisplaySafeAreaPadding * 2.0f));

    // When the window cannot fit all contents (either because of constraints, either because
    // screen is too small), we are growing the size on the other axis to compensate for expected
    // scrollbar. FIXME: Might turn bigger than ViewportSize-WindowPadding.
    wabi::GfVec2f size_auto_fit_after_constraint = CalcWindowSizeAfterConstraint(window, size_auto_fit);
    bool will_have_scrollbar_x = (size_auto_fit_after_constraint[0] - size_pad[0] - 0.0f <
                                    size_contents[0] &&
                                  !(window->Flags & AnchorWindowFlags_NoScrollbar) &&
                                  (window->Flags & AnchorWindowFlags_HorizontalScrollbar)) ||
                                 (window->Flags & AnchorWindowFlags_AlwaysHorizontalScrollbar);
    bool will_have_scrollbar_y = (size_auto_fit_after_constraint[1] - size_pad[1] -
                                      decoration_up_height <
                                    size_contents[1] &&
                                  !(window->Flags & AnchorWindowFlags_NoScrollbar)) ||
                                 (window->Flags & AnchorWindowFlags_AlwaysVerticalScrollbar);
    if (will_have_scrollbar_x)
      size_auto_fit[1] += style.ScrollbarSize;
    if (will_have_scrollbar_y)
      size_auto_fit[0] += style.ScrollbarSize;
    return size_auto_fit;
  }
}

wabi::GfVec2f ANCHOR::CalcWindowNextAutoFitSize(AnchorWindow *window)
{
  wabi::GfVec2f size_contents_current;
  wabi::GfVec2f size_contents_ideal;
  CalcWindowContentSizes(window, &size_contents_current, &size_contents_ideal);
  wabi::GfVec2f size_auto_fit = CalcWindowAutoFitSize(window, size_contents_ideal);
  wabi::GfVec2f size_final = CalcWindowSizeAfterConstraint(window, size_auto_fit);
  return size_final;
}

static AnchorCol GetWindowBgColorIdxFromFlags(AnchorWindowFlags flags)
{
  if (flags & (AnchorWindowFlags_Tooltip | AnchorWindowFlags_Popup))
    return AnchorCol_PopupBg;
  if (flags & AnchorWindowFlags_ChildWindow)
    return AnchorCol_ChildBg;
  return AnchorCol_WindowBg;
}

static void CalcResizePosSizeFromAnyCorner(AnchorWindow *window,
                                           const wabi::GfVec2f &corner_target,
                                           const wabi::GfVec2f &corner_norm,
                                           wabi::GfVec2f *out_pos,
                                           wabi::GfVec2f *out_size)
{
  wabi::GfVec2f pos_min = AnchorLerp(corner_target,
                               window->Pos,
                               corner_norm);  // Expected window upper-left
  wabi::GfVec2f pos_max = AnchorLerp(window->Pos + window->Size,
                               corner_target,
                               corner_norm);  // Expected window lower-right
  wabi::GfVec2f size_expected = pos_max - pos_min;
  wabi::GfVec2f size_constrained = CalcWindowSizeAfterConstraint(window, size_expected);
  *out_pos = pos_min;
  if (corner_norm[0] == 0.0f)
    out_pos->data()[0] -= (size_constrained[0] - size_expected[0]);
  if (corner_norm[1] == 0.0f)
    out_pos->data()[1] -= (size_constrained[1] - size_expected[1]);
  *out_size = size_constrained;
}

// Data for resizing from corner
struct ANCHORResizeGripDef
{
  wabi::GfVec2f CornerPosN;
  wabi::GfVec2f InnerDir;
  int AngleMin12, AngleMax12;
};
static const ANCHORResizeGripDef resize_grip_def[4] = {
  {wabi::GfVec2f(1, 1), wabi::GfVec2f(-1, -1), 0, 3 }, // Lower-right
  {wabi::GfVec2f(0, 1), wabi::GfVec2f(+1, -1), 3, 6 }, // Lower-left
  {wabi::GfVec2f(0, 0), wabi::GfVec2f(+1, +1), 6, 9 }, // Upper-left (Unused)
  {wabi::GfVec2f(1, 0), wabi::GfVec2f(-1, +1), 9, 12}  // Upper-right (Unused)
};

// Data for resizing from borders
struct ANCHORResizeBorderDef
{
  wabi::GfVec2f InnerDir;
  wabi::GfVec2f SegmentN1, SegmentN2;
  float OuterAngle;
};
static const ANCHORResizeBorderDef resize_border_def[4] = {
  {wabi::GfVec2f(+1, 0),  wabi::GfVec2f(0, 1), wabi::GfVec2f(0, 0), IM_PI * 1.00f}, // Left
  {wabi::GfVec2f(-1, 0),  wabi::GfVec2f(1, 0), wabi::GfVec2f(1, 1), IM_PI * 0.00f}, // Right
  {wabi::GfVec2f(0,  +1), wabi::GfVec2f(0, 0), wabi::GfVec2f(1, 0), IM_PI * 1.50f}, // Up
  {wabi::GfVec2f(0,  -1), wabi::GfVec2f(1, 1), wabi::GfVec2f(0, 1), IM_PI * 0.50f}  // Down
};

static AnchorBBox GetResizeBorderRect(AnchorWindow *window,
                                      int border_n,
                                      float perp_padding,
                                      float thickness)
{
  AnchorBBox rect = window->Rect();
  if (thickness == 0.0f)
    rect.Max -= wabi::GfVec2f(1, 1);
  if (border_n == AnchorDir_Left) {
    return AnchorBBox(rect.Min[0] - thickness,
                      rect.Min[1] + perp_padding,
                      rect.Min[0] + thickness,
                      rect.Max[1] - perp_padding);
  }
  if (border_n == AnchorDir_Right) {
    return AnchorBBox(rect.Max[0] - thickness,
                      rect.Min[1] + perp_padding,
                      rect.Max[0] + thickness,
                      rect.Max[1] - perp_padding);
  }
  if (border_n == AnchorDir_Up) {
    return AnchorBBox(rect.Min[0] + perp_padding,
                      rect.Min[1] - thickness,
                      rect.Max[0] - perp_padding,
                      rect.Min[1] + thickness);
  }
  if (border_n == AnchorDir_Down) {
    return AnchorBBox(rect.Min[0] + perp_padding,
                      rect.Max[1] - thickness,
                      rect.Max[0] - perp_padding,
                      rect.Max[1] + thickness);
  }
  ANCHOR_ASSERT(0);
  return AnchorBBox();
}

// 0..3: corners (Lower-right, Lower-left, Unused, Unused)
ANCHOR_ID ANCHOR::GetWindowResizeCornerID(AnchorWindow *window, int n)
{
  ANCHOR_ASSERT(n >= 0 && n < 4);
  ANCHOR_ID id = window->ID;
  id = AnchorHashStr("#RESIZE", 0, id);
  id = ImHashData(&n, sizeof(int), id);
  return id;
}

// Borders (Left, Right, Up, Down)
ANCHOR_ID ANCHOR::GetWindowResizeBorderID(AnchorWindow *window, AnchorDir dir)
{
  ANCHOR_ASSERT(dir >= 0 && dir < 4);
  int n = (int)dir + 4;
  ANCHOR_ID id = window->ID;
  id = AnchorHashStr("#RESIZE", 0, id);
  id = ImHashData(&n, sizeof(int), id);
  return id;
}

// Handle resize for: Resize Grips, Borders, Gamepad
// Return true when using auto-fit (double click on resize grip)
static bool ANCHOR::UpdateWindowManualResize(AnchorWindow *window,
                                             const wabi::GfVec2f &size_auto_fit,
                                             int *border_held,
                                             int resize_grip_count,
                                             AnchorU32 resize_grip_col[4],
                                             const AnchorBBox &visibility_rect)
{
  AnchorContext &g = *G_CTX;
  AnchorWindowFlags flags = window->Flags;

  if ((flags & AnchorWindowFlags_NoResize) || (flags & AnchorWindowFlags_AlwaysAutoResize) ||
      window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
    return false;
  if (window->WasActive == false)  // Early out to avoid running this code for e.g. an hidden
                                   // implicit/fallback Debug window.
    return false;

  bool ret_auto_fit = false;
  const int resize_border_count = g.IO.ConfigWindowsResizeFromEdges ? 4 : 0;
  const float grip_draw_size = ANCHOR_FLOOR(
    AnchorMax(g.FontSize * 1.35f, window->WindowRounding + 1.0f + g.FontSize * 0.2f));
  const float grip_hover_inner_size = ANCHOR_FLOOR(grip_draw_size * 0.75f);
  const float grip_hover_outer_size = g.IO.ConfigWindowsResizeFromEdges ? WINDOWS_HOVER_PADDING :
                                                                          0.0f;

  wabi::GfVec2f pos_target(FLT_MAX, FLT_MAX);
  wabi::GfVec2f size_target(FLT_MAX, FLT_MAX);

  // Resize grips and borders are on layer 1
  window->DC.NavLayerCurrent = ANCHORNavLayer_Menu;

  // Manual resize grips
  PushID("#RESIZE");
  for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++) {
    const ANCHORResizeGripDef &def = resize_grip_def[resize_grip_n];
    const wabi::GfVec2f corner = AnchorLerp(window->Pos, window->Pos + window->Size, def.CornerPosN);

    // Using the FlattenChilds button flag we make the resize button accessible even if we are
    // hovering over a child window
    bool hovered, held;
    AnchorBBox resize_rect(corner - def.InnerDir * grip_hover_outer_size,
                           corner + def.InnerDir * grip_hover_inner_size);
    if (resize_rect.Min[0] > resize_rect.Max[0])
      AnchorSwap(resize_rect.Min[0], resize_rect.Max[0]);
    if (resize_rect.Min[1] > resize_rect.Max[1])
      AnchorSwap(resize_rect.Min[1], resize_rect.Max[1]);
    ANCHOR_ID resize_grip_id = window->GetID(resize_grip_n);  // == GetWindowResizeCornerID()
    ButtonBehavior(resize_rect,
                   resize_grip_id,
                   &hovered,
                   &held,
                   AnchorButtonFlags_FlattenChildren | AnchorButtonFlags_NoNavFocus);
    // GetForegroundDrawList(window)->AddRect(resize_rect.Min, resize_rect.Max, ANCHOR_COL32(255,
    // 255, 0, 255));
    if (hovered || held)
      g.MouseCursor = (resize_grip_n & 1) ? ANCHOR_StandardCursorBottomLeftCorner :
                                            ANCHOR_StandardCursorBottomRightCorner;

    if (held && g.IO.MouseDoubleClicked[0] && resize_grip_n == 0) {
      // Manual auto-fit when double-clicking
      size_target = CalcWindowSizeAfterConstraint(window, size_auto_fit);
      ret_auto_fit = true;
      ClearActiveID();
    } else if (held) {
      // Resize from any of the four corners
      // We don't use an incremental MouseDelta but rather compute an absolute target size based on
      // mouse position
      wabi::GfVec2f clamp_min = wabi::GfVec2f(def.CornerPosN[0] == 1.0f ? visibility_rect.Min[0] : -FLT_MAX,
                                  def.CornerPosN[1] == 1.0f ? visibility_rect.Min[1] : -FLT_MAX);
      wabi::GfVec2f clamp_max = wabi::GfVec2f(def.CornerPosN[0] == 0.0f ? visibility_rect.Max[0] : +FLT_MAX,
                                  def.CornerPosN[1] == 0.0f ? visibility_rect.Max[1] : +FLT_MAX);
      wabi::GfVec2f corner_target =
        g.IO.MousePos - g.ActiveIdClickOffset +
        AnchorLerp(def.InnerDir * grip_hover_outer_size,
                   def.InnerDir * -grip_hover_inner_size,
                   def.CornerPosN);  // Corner of the window corresponding to our corner grip
      corner_target = AnchorClamp(corner_target, clamp_min, clamp_max);
      CalcResizePosSizeFromAnyCorner(window,
                                     corner_target,
                                     def.CornerPosN,
                                     &pos_target,
                                     &size_target);
    }

    // Only lower-left grip is visible before hovering/activating
    if (resize_grip_n == 0 || held || hovered)
      resize_grip_col[resize_grip_n] = GetColorU32(held    ? AnchorCol_ResizeGripActive :
                                                   hovered ? AnchorCol_ResizeGripHovered :
                                                             AnchorCol_ResizeGrip);
  }
  for (int border_n = 0; border_n < resize_border_count; border_n++) {
    const ANCHORResizeBorderDef &def = resize_border_def[border_n];
    const ANCHOR_Axis axis = (border_n == AnchorDir_Left || border_n == AnchorDir_Right) ?
                               ANCHOR_Axis_X :
                               ANCHOR_Axis_Y;

    bool hovered, held;
    AnchorBBox border_rect = GetResizeBorderRect(window,
                                                 border_n,
                                                 grip_hover_inner_size,
                                                 WINDOWS_HOVER_PADDING);
    ANCHOR_ID border_id = window->GetID(border_n + 4);  // == GetWindowResizeBorderID()
    ButtonBehavior(border_rect, border_id, &hovered, &held, AnchorButtonFlags_FlattenChildren);
    // GetForegroundDrawLists(window)->AddRect(border_rect.Min, border_rect.Max, ANCHOR_COL32(255,
    // 255, 0, 255));
    if ((hovered && g.HoveredIdTimer > WINDOWS_RESIZE_FROM_EDGES_FEEDBACK_TIMER) || held) {
      g.MouseCursor = (axis == ANCHOR_Axis_X) ? ANCHOR_StandardCursorEWScroll :
                                                ANCHOR_StandardCursorNSScroll;
      if (held)
        *border_held = border_n;
    }
    if (held) {
      wabi::GfVec2f clamp_min(border_n == AnchorDir_Right ? visibility_rect.Min[0] : -FLT_MAX,
                        border_n == AnchorDir_Down ? visibility_rect.Min[1] : -FLT_MAX);
      wabi::GfVec2f clamp_max(border_n == AnchorDir_Left ? visibility_rect.Max[0] : +FLT_MAX,
                        border_n == AnchorDir_Up ? visibility_rect.Max[1] : +FLT_MAX);
      wabi::GfVec2f border_target = window->Pos;
      border_target[axis] = g.IO.MousePos[axis] - g.ActiveIdClickOffset[axis] +
                            WINDOWS_HOVER_PADDING;
      border_target = AnchorClamp(border_target, clamp_min, clamp_max);
      CalcResizePosSizeFromAnyCorner(window,
                                     border_target,
                                     AnchorMin(def.SegmentN1, def.SegmentN2),
                                     &pos_target,
                                     &size_target);
    }
  }
  PopID();

  // Restore nav layer
  window->DC.NavLayerCurrent = ANCHORNavLayer_Main;

  // Navigation resize (keyboard/gamepad)
  if (g.NavWindowingTarget && g.NavWindowingTarget->RootWindow == window) {
    wabi::GfVec2f nav_resize_delta;
    if (g.NavInputSource == ANCHORInputSource_Keyboard && g.IO.KeyShift)
      nav_resize_delta = GetNavInputAmount2d(AnchorNavDirSourceFlags_Keyboard,
                                             ANCHOR_InputReadMode_Down);
    if (g.NavInputSource == ANCHORInputSource_Gamepad)
      nav_resize_delta = GetNavInputAmount2d(AnchorNavDirSourceFlags_PadDPad,
                                             ANCHOR_InputReadMode_Down);
    if (nav_resize_delta[0] != 0.0f || nav_resize_delta[1] != 0.0f) {
      const float NAV_RESIZE_SPEED = 600.0f;
      nav_resize_delta *= AnchorFloor(
        NAV_RESIZE_SPEED * g.IO.DeltaTime *
        AnchorMin(g.IO.DisplayFramebufferScale[0], g.IO.DisplayFramebufferScale[1]));
      nav_resize_delta = AnchorMax(nav_resize_delta,
                                   visibility_rect.Min - window->Pos - window->Size);
      g.NavWindowingToggleLayer = false;
      g.NavDisableMouseHover = true;
      resize_grip_col[0] = GetColorU32(AnchorCol_ResizeGripActive);
      // FIXME-NAV: Should store and accumulate into a separate size buffer to handle sizing
      // constraints properly, right now a constraint will make us stuck.
      size_target = CalcWindowSizeAfterConstraint(window, window->SizeFull + nav_resize_delta);
    }
  }

  // Apply back modified position/size to window
  if (size_target[0] != FLT_MAX) {
    window->SizeFull = size_target;
    MarkIniSettingsDirty(window);
  }
  if (pos_target[0] != FLT_MAX) {
    window->Pos = AnchorFloor(pos_target);
    MarkIniSettingsDirty(window);
  }

  window->Size = window->SizeFull;
  return ret_auto_fit;
}

static inline void ClampWindowRect(AnchorWindow *window, const AnchorBBox &visibility_rect)
{
  AnchorContext &g = *G_CTX;
  wabi::GfVec2f size_for_clamping = window->Size;
  if (g.IO.ConfigWindowsMoveFromTitleBarOnly && !(window->Flags & AnchorWindowFlags_NoTitleBar))
    size_for_clamping[1] = window->TitleBarHeight();
  window->Pos = AnchorClamp(window->Pos,
                            visibility_rect.Min - size_for_clamping,
                            visibility_rect.Max);
}

static void ANCHOR::RenderWindowOuterBorders(AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;
  float rounding = window->WindowRounding;
  float border_size = window->WindowBorderSize;
  if (border_size > 0.0f && !(window->Flags & AnchorWindowFlags_NoBackground))
    window->DrawList->AddRect(window->Pos,
                              window->Pos + window->Size,
                              GetColorU32(AnchorCol_Border),
                              rounding,
                              0,
                              border_size);

  int border_held = window->ResizeBorderHeld;
  if (border_held != -1) {
    const ANCHORResizeBorderDef &def = resize_border_def[border_held];
    AnchorBBox border_r = GetResizeBorderRect(window, border_held, rounding, 0.0f);
    window->DrawList->PathArcTo(AnchorLerp(border_r.Min, border_r.Max, def.SegmentN1) +
                                  wabi::GfVec2f(0.5f, 0.5f) + def.InnerDir * rounding,
                                rounding,
                                def.OuterAngle - IM_PI * 0.25f,
                                def.OuterAngle);
    window->DrawList->PathArcTo(AnchorLerp(border_r.Min, border_r.Max, def.SegmentN2) +
                                  wabi::GfVec2f(0.5f, 0.5f) + def.InnerDir * rounding,
                                rounding,
                                def.OuterAngle,
                                def.OuterAngle + IM_PI * 0.25f);
    window->DrawList->PathStroke(GetColorU32(AnchorCol_SeparatorActive),
                                 0,
                                 AnchorMax(2.0f, border_size));  // Thicker than usual
  }
  if (g.Style.FrameBorderSize > 0 && !(window->Flags & AnchorWindowFlags_NoTitleBar)) {
    float y = window->Pos[1] + window->TitleBarHeight() - 1;
    window->DrawList->AddLine(wabi::GfVec2f(window->Pos[0] + border_size, y),
                              wabi::GfVec2f(window->Pos[0] + window->Size[0] - border_size, y),
                              GetColorU32(AnchorCol_Border),
                              g.Style.FrameBorderSize);
  }
}

// Draw background and borders
// Draw and handle scrollbars
void ANCHOR::RenderWindowDecorations(AnchorWindow *window,
                                     const AnchorBBox &title_bar_rect,
                                     bool title_bar_is_highlight,
                                     int resize_grip_count,
                                     const AnchorU32 resize_grip_col[4],
                                     float resize_grip_draw_size)
{
  AnchorContext &g = *G_CTX;
  AnchorStyle &style = g.Style;
  AnchorWindowFlags flags = window->Flags;

  // Ensure that ScrollBar doesn't read last frame's SkipItems
  ANCHOR_ASSERT(window->BeginCount == 0);
  window->SkipItems = false;

  // Draw window + handle manual resize
  // As we highlight the title bar when want_focus is set, multiple reappearing windows will have
  // have their title bar highlighted on their reappearing frame.
  const float window_rounding = window->WindowRounding;
  const float window_border_size = window->WindowBorderSize;
  if (window->Collapsed) {
    // Title bar only
    float backup_border_size = style.FrameBorderSize;
    g.Style.FrameBorderSize = window->WindowBorderSize;
    AnchorU32 title_bar_col = GetColorU32((title_bar_is_highlight && !g.NavDisableHighlight) ?
                                            AnchorCol_TitleBgActive :
                                            AnchorCol_TitleBgCollapsed);
    RenderFrame(title_bar_rect.Min, title_bar_rect.Max, title_bar_col, true, window_rounding);
    g.Style.FrameBorderSize = backup_border_size;
  } else {
    // Window background
    if (!(flags & AnchorWindowFlags_NoBackground)) {
      AnchorU32 bg_col = GetColorU32(GetWindowBgColorIdxFromFlags(flags));
      bool override_alpha = false;
      float alpha = 1.0f;
      if (g.NextWindowData.Flags & AnchorNextWindowDataFlags_HasBgAlpha) {
        alpha = g.NextWindowData.BgAlphaVal;
        override_alpha = true;
      }
      if (override_alpha)
        bg_col = (bg_col & ~ANCHOR_COL32_A_MASK) |
                 (IM_F32_TO_INT8_SAT(alpha) << ANCHOR_COL32_A_SHIFT);
      window->DrawList->AddRectFilled(
        window->Pos + wabi::GfVec2f(0, window->TitleBarHeight()),
        window->Pos + window->Size,
        bg_col,
        window_rounding,
        (flags & AnchorWindowFlags_NoTitleBar) ? 0 : AnchorDrawFlags_RoundCornersBottom);
    }

    // Title bar
    if (!(flags & AnchorWindowFlags_NoTitleBar)) {
      AnchorU32 title_bar_col = GetColorU32(title_bar_is_highlight ? AnchorCol_TitleBgActive :
                                                                     AnchorCol_TitleBg);
      window->DrawList->AddRectFilled(title_bar_rect.Min,
                                      title_bar_rect.Max,
                                      title_bar_col,
                                      window_rounding,
                                      AnchorDrawFlags_RoundCornersTop);
    }

    // Menu bar
    if (flags & AnchorWindowFlags_MenuBar) {
      AnchorBBox menu_bar_rect = window->MenuBarRect();
      menu_bar_rect.ClipWith(
        window->Rect());  // Soft clipping, in particular child window don't have minimum size
                          // covering the menu bar so this is useful for them.
      window->DrawList->AddRectFilled(menu_bar_rect.Min + wabi::GfVec2f(window_border_size, 0),
                                      menu_bar_rect.Max - wabi::GfVec2f(window_border_size, 0),
                                      GetColorU32(AnchorCol_MenuBarBg),
                                      (flags & AnchorWindowFlags_NoTitleBar) ? window_rounding :
                                                                               0.0f,
                                      AnchorDrawFlags_RoundCornersTop);
      if (style.FrameBorderSize > 0.0f && menu_bar_rect.Max[1] < window->Pos[1] + window->Size[1])
        window->DrawList->AddLine(menu_bar_rect.GetBL(),
                                  menu_bar_rect.GetBR(),
                                  GetColorU32(AnchorCol_Border),
                                  style.FrameBorderSize);
    }

    // Scrollbars
    if (window->ScrollbarX)
      Scrollbar(ANCHOR_Axis_X);
    if (window->ScrollbarY)
      Scrollbar(ANCHOR_Axis_Y);

    // Render resize grips (after their input handling so we don't have a frame of latency)
    if (!(flags & AnchorWindowFlags_NoResize)) {
      for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++) {
        const ANCHORResizeGripDef &grip = resize_grip_def[resize_grip_n];
        const wabi::GfVec2f corner = AnchorLerp(window->Pos,
                                          window->Pos + window->Size,
                                          grip.CornerPosN);
        /** todo::check_math */
        window->DrawList->PathLineTo(wabi::GfVec2f(
          corner[0] + grip.InnerDir[0] * ((resize_grip_n & 1) ?
                                            wabi::GfVec2f(window_border_size, resize_grip_draw_size)[0] :
                                            wabi::GfVec2f(resize_grip_draw_size, window_border_size)[0]),
          corner[1] +
            grip.InnerDir[1] * ((resize_grip_n & 1) ?
                                  wabi::GfVec2f(window_border_size, resize_grip_draw_size)[1] :
                                  wabi::GfVec2f(resize_grip_draw_size, window_border_size)[1])));
        window->DrawList->PathLineTo(wabi::GfVec2f(
          corner[0] + grip.InnerDir[0] * ((resize_grip_n & 1) ?
                                            wabi::GfVec2f(resize_grip_draw_size, window_border_size)[0] :
                                            wabi::GfVec2f(window_border_size, resize_grip_draw_size)[0]),
          corner[1] +
            grip.InnerDir[1] * ((resize_grip_n & 1) ?
                                  wabi::GfVec2f(resize_grip_draw_size, window_border_size)[1] :
                                  wabi::GfVec2f(window_border_size, resize_grip_draw_size)[1])));
        window->DrawList->PathArcToFast(
          wabi::GfVec2f(corner[0] + grip.InnerDir[0] * (window_rounding + window_border_size),
                  corner[1] + grip.InnerDir[1] * (window_rounding + window_border_size)),
          window_rounding,
          grip.AngleMin12,
          grip.AngleMax12);
        window->DrawList->PathFillConvex(resize_grip_col[resize_grip_n]);
      }
    }

    // Borders
    RenderWindowOuterBorders(window);
  }
}

// Render title text, collapse button, close button
void ANCHOR::RenderWindowTitleBarContents(AnchorWindow *window,
                                          const AnchorBBox &title_bar_rect,
                                          const char *name,
                                          bool *p_open)
{
  AnchorContext &g = *G_CTX;
  AnchorStyle &style = g.Style;
  AnchorWindowFlags flags = window->Flags;

  const bool has_close_button = (p_open != NULL);
  const bool has_collapse_button = !(flags & AnchorWindowFlags_NoCollapse) &&
                                   (style.WindowMenuButtonPosition != AnchorDir_None);

  // Close & Collapse button are on the Menu NavLayer and don't default focus (unless there's
  // nothing else on that layer)
  const AnchorItemFlags item_flags_backup = g.CurrentItemFlags;
  g.CurrentItemFlags |= AnchorItemFlags_NoNavDefaultFocus;
  window->DC.NavLayerCurrent = ANCHORNavLayer_Menu;

  // Layout buttons
  // FIXME: Would be nice to generalize the subtleties expressed here into reusable code.
  float pad_l = style.FramePadding[0];
  float pad_r = style.FramePadding[0];
  float button_sz = g.FontSize;
  wabi::GfVec2f close_button_pos;
  wabi::GfVec2f collapse_button_pos;
  if (has_close_button) {
    pad_r += button_sz;
    close_button_pos = wabi::GfVec2f(title_bar_rect.Max[0] - pad_r - style.FramePadding[0],
                               title_bar_rect.Min[1]);
  }
  if (has_collapse_button && style.WindowMenuButtonPosition == AnchorDir_Right) {
    pad_r += button_sz;
    collapse_button_pos = wabi::GfVec2f(title_bar_rect.Max[0] - pad_r - style.FramePadding[0],
                                  title_bar_rect.Min[1]);
  }
  if (has_collapse_button && style.WindowMenuButtonPosition == AnchorDir_Left) {
    collapse_button_pos = wabi::GfVec2f(title_bar_rect.Min[0] + pad_l - style.FramePadding[0],
                                  title_bar_rect.Min[1]);
    pad_l += button_sz;
  }

  // Collapse button (submitting first so it gets priority when choosing a navigation init
  // fallback)
  if (has_collapse_button)
    if (CollapseButton(window->GetID("#COLLAPSE"), collapse_button_pos))
      window->WantCollapseToggle =
        true;  // Defer actual collapsing to next frame as we are too far in the Begin() function

  // Close button
  if (has_close_button)
    if (CloseButton(window->GetID("#CLOSE"), close_button_pos))
      *p_open = false;

  window->DC.NavLayerCurrent = ANCHORNavLayer_Main;
  g.CurrentItemFlags = item_flags_backup;

  // Title bar text (with: horizontal alignment, avoiding collapse/close button, optional "unsaved
  // document" marker)
  // FIXME: Refactor text alignment facilities along with RenderText helpers, this is WAY too much
  // messy code..
  const char *UNSAVED_DOCUMENT_MARKER = "*";
  const float marker_size_x = (flags & AnchorWindowFlags_UnsavedDocument) ?
                                CalcTextSize(UNSAVED_DOCUMENT_MARKER, NULL, false)[0] :
                                0.0f;
  const wabi::GfVec2f text_size = CalcTextSize(name, NULL, true) + wabi::GfVec2f(marker_size_x, 0.0f);

  // As a nice touch we try to ensure that centered title text doesn't get affected by visibility
  // of Close/Collapse button, while uncentered title text will still reach edges correctly.
  if (pad_l > style.FramePadding[0])
    pad_l += g.Style.ItemInnerSpacing[0];
  if (pad_r > style.FramePadding[0])
    pad_r += g.Style.ItemInnerSpacing[0];
  if (style.WindowTitleAlign[0] > 0.0f && style.WindowTitleAlign[0] < 1.0f) {
    float centerness = AnchorSaturate(1.0f - AnchorFabs(style.WindowTitleAlign[0] - 0.5f) *
                                               2.0f);  // 0.0f on either edges, 1.0f on center
    float pad_extend = AnchorMin(AnchorMax(pad_l, pad_r),
                                 title_bar_rect.GetWidth() - pad_l - pad_r - text_size[0]);
    pad_l = AnchorMax(pad_l, pad_extend * centerness);
    pad_r = AnchorMax(pad_r, pad_extend * centerness);
  }

  AnchorBBox layout_r(title_bar_rect.Min[0] + pad_l,
                      title_bar_rect.Min[1],
                      title_bar_rect.Max[0] - pad_r,
                      title_bar_rect.Max[1]);
  AnchorBBox clip_r(
    layout_r.Min[0],
    layout_r.Min[1],
    AnchorMin(layout_r.Max[0] + g.Style.ItemInnerSpacing[0], title_bar_rect.Max[0]),
    layout_r.Max[1]);
  // if (g.IO.KeyShift) window->DrawList->AddRect(layout_r.Min, layout_r.Max, ANCHOR_COL32(255,
  // 128, 0, 255)); // [DEBUG] if (g.IO.KeyCtrl) window->DrawList->AddRect(clip_r.Min, clip_r.Max,
  // ANCHOR_COL32(255, 128, 0, 255)); // [DEBUG]
  RenderTextClipped(layout_r.Min,
                    layout_r.Max,
                    name,
                    NULL,
                    &text_size,
                    style.WindowTitleAlign,
                    &clip_r);
  if (flags & AnchorWindowFlags_UnsavedDocument) {
    wabi::GfVec2f marker_pos = wabi::GfVec2f(AnchorMax(layout_r.Min[0],
                                           layout_r.Min[0] + (layout_r.GetWidth() - text_size[0]) *
                                                               style.WindowTitleAlign[0]) +
                                   text_size[0],
                                 layout_r.Min[1]) +
                         wabi::GfVec2f(2 - marker_size_x, 0.0f);
    wabi::GfVec2f off = wabi::GfVec2f(0.0f, ANCHOR_FLOOR(-g.FontSize * 0.25f));
    RenderTextClipped(marker_pos + off,
                      layout_r.Max + off,
                      UNSAVED_DOCUMENT_MARKER,
                      NULL,
                      NULL,
                      wabi::GfVec2f(0, style.WindowTitleAlign[1]),
                      &clip_r);
  }
}

void ANCHOR::UpdateWindowParentAndRootLinks(AnchorWindow *window,
                                            AnchorWindowFlags flags,
                                            AnchorWindow *parent_window)
{
  window->ParentWindow = parent_window;
  window->RootWindow = window->RootWindowForTitleBarHighlight = window->RootWindowForNav = window;
  if (parent_window && (flags & AnchorWindowFlags_ChildWindow) &&
      !(flags & AnchorWindowFlags_Tooltip))
    window->RootWindow = parent_window->RootWindow;
  if (parent_window && !(flags & AnchorWindowFlags_Modal) &&
      (flags & (AnchorWindowFlags_ChildWindow | AnchorWindowFlags_Popup)))
    window->RootWindowForTitleBarHighlight = parent_window->RootWindowForTitleBarHighlight;
  while (window->RootWindowForNav->Flags & AnchorWindowFlags_NavFlattened) {
    ANCHOR_ASSERT(window->RootWindowForNav->ParentWindow != NULL);
    window->RootWindowForNav = window->RootWindowForNav->ParentWindow;
  }
}

// Push a new ANCHOR window to add widgets to.
// - A default window called "Debug" is automatically stacked at the beginning of every frame so
// you can use widgets without explicitly calling a Begin/End pair.
// - Begin/End can be called multiple times during the frame with the same window name to append
// content.
// - The window name is used as a unique identifier to preserve window information across frames
// (and save rudimentary information to the .ini file).
//   You can use the "##" or "###" markers to use the same label with different id, or same id with
//   different label. See documentation at the top of this file.
// - Return false when window is collapsed, so you can early out in your code. You always need to
// call ANCHOR::End() even if false is returned.
// - Passing 'bool* p_open' displays a Close button on the upper-right corner of the window, the
// pointed value will be set to false when the button is pressed.
bool ANCHOR::Begin(const char *name, bool *p_open, AnchorWindowFlags flags)
{
  AnchorContext &g = *G_CTX;
  const AnchorStyle &style = g.Style;
  ANCHOR_ASSERT(name != NULL && name[0] != '\0');  // Window name required
  ANCHOR_ASSERT(g.WithinFrameScope);               // Forgot to call ANCHOR::NewFrame()
  ANCHOR_ASSERT(g.FrameCountEnded !=
                g.FrameCount);  // Called ANCHOR::Render() or ANCHOR::EndFrame() and
                                // haven't called ANCHOR::NewFrame() again yet

  // Find or create
  AnchorWindow *window = FindWindowByName(name);
  const bool window_just_created = (window == NULL);
  if (window_just_created)
    window = CreateNewWindow(name, flags);

  // Automatically disable manual moving/resizing when NoInputs is set
  if ((flags & AnchorWindowFlags_NoInputs) == AnchorWindowFlags_NoInputs)
    flags |= AnchorWindowFlags_NoMove | AnchorWindowFlags_NoResize;

  if (flags & AnchorWindowFlags_NavFlattened)
    ANCHOR_ASSERT(flags & AnchorWindowFlags_ChildWindow);

  const int current_frame = g.FrameCount;
  const bool first_begin_of_the_frame = (window->LastFrameActive != current_frame);
  window->IsFallbackWindow = (g.CurrentWindowStack.Size == 0 &&
                              g.WithinFrameScopeWithImplicitWindow);

  // Update the Appearing flag
  bool window_just_activated_by_user = (window->LastFrameActive <
                                        current_frame -
                                          1);  // Not using !WasActive because the implicit
                                               // "Debug" window would always toggle off->on
  if (flags & AnchorWindowFlags_Popup) {
    AnchorPopupData &popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
    window_just_activated_by_user |=
      (window->PopupId !=
       popup_ref.PopupId);  // We recycle popups so treat window as activated if popup id changed
    window_just_activated_by_user |= (window != popup_ref.Window);
  }
  window->Appearing = window_just_activated_by_user;
  if (window->Appearing)
    SetWindowConditionAllowFlags(window, AnchorCond_Appearing, true);

  // Update Flags, LastFrameActive, BeginOrderXXX fields
  if (first_begin_of_the_frame) {
    window->Flags = (AnchorWindowFlags)flags;
    window->LastFrameActive = current_frame;
    window->LastTimeActive = (float)g.Time;
    window->BeginOrderWithinParent = 0;
    window->BeginOrderWithinContext = (short)(g.WindowsActiveCount++);
  } else {
    flags = window->Flags;
  }

  // Parent window is latched only on the first call to Begin() of the frame, so further
  // append-calls can be done from a different window stack
  AnchorWindow *parent_window_in_stack = g.CurrentWindowStack.empty() ?
                                           NULL :
                                           g.CurrentWindowStack.back();
  AnchorWindow *parent_window = first_begin_of_the_frame ?
                                  ((flags &
                                    (AnchorWindowFlags_ChildWindow | AnchorWindowFlags_Popup)) ?
                                     parent_window_in_stack :
                                     NULL) :
                                  window->ParentWindow;
  ANCHOR_ASSERT(parent_window != NULL || !(flags & AnchorWindowFlags_ChildWindow));

  // We allow window memory to be compacted so recreate the base stack when needed.
  if (window->IDStack.Size == 0)
    window->IDStack.push_back(window->ID);

  // Add to stack
  // We intentionally set g.CurrentWindow to NULL to prevent usage until when the viewport is set,
  // then will call SetCurrentWindow()
  g.CurrentWindowStack.push_back(window);
  g.CurrentWindow = window;
  window->DC.StackSizesOnBegin.SetToCurrentState();
  g.CurrentWindow = NULL;

  if (flags & AnchorWindowFlags_Popup) {
    AnchorPopupData &popup_ref = g.OpenPopupStack[g.BeginPopupStack.Size];
    popup_ref.Window = window;
    g.BeginPopupStack.push_back(popup_ref);
    window->PopupId = popup_ref.PopupId;
  }

  // Update ->RootWindow and others pointers (before any possible call to FocusWindow)
  if (first_begin_of_the_frame)
    UpdateWindowParentAndRootLinks(window, flags, parent_window);

  // Process SetNextWindow***() calls
  // (FIXME: Consider splitting the HasXXX flags into X/Y components
  bool window_pos_set_by_api = false;
  bool window_size_x_set_by_api = false, window_size_y_set_by_api = false;
  if (g.NextWindowData.Flags & AnchorNextWindowDataFlags_HasPos) {
    window_pos_set_by_api = (window->SetWindowPosAllowFlags & g.NextWindowData.PosCond) != 0;
    if (window_pos_set_by_api && AnchorLengthSqr(g.NextWindowData.PosPivotVal) > 0.00001f) {
      // May be processed on the next frame if this is our first frame and we are measuring size
      // FIXME: Look into removing the branch so everything can go through this same code path for
      // consistency.
      window->SetWindowPosVal = g.NextWindowData.PosVal;
      window->SetWindowPosPivot = g.NextWindowData.PosPivotVal;
      window->SetWindowPosAllowFlags &= ~(AnchorCond_Once | AnchorCond_FirstUseEver |
                                          AnchorCond_Appearing);
    } else {
      SetWindowPos(window, g.NextWindowData.PosVal, g.NextWindowData.PosCond);
    }
  }
  if (g.NextWindowData.Flags & AnchorNextWindowDataFlags_HasSize) {
    window_size_x_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) !=
                                 0 &&
                               (g.NextWindowData.SizeVal[0] > 0.0f);
    window_size_y_set_by_api = (window->SetWindowSizeAllowFlags & g.NextWindowData.SizeCond) !=
                                 0 &&
                               (g.NextWindowData.SizeVal[1] > 0.0f);
    SetWindowSize(window, g.NextWindowData.SizeVal, g.NextWindowData.SizeCond);
  }
  if (g.NextWindowData.Flags & AnchorNextWindowDataFlags_HasScroll) {
    if (g.NextWindowData.ScrollVal[0] >= 0.0f) {
      window->ScrollTarget[0] = g.NextWindowData.ScrollVal[0];
      window->ScrollTargetCenterRatio[0] = 0.0f;
    }
    if (g.NextWindowData.ScrollVal[1] >= 0.0f) {
      window->ScrollTarget[1] = g.NextWindowData.ScrollVal[1];
      window->ScrollTargetCenterRatio[1] = 0.0f;
    }
  }
  if (g.NextWindowData.Flags & AnchorNextWindowDataFlags_HasContentSize)
    window->ContentSizeExplicit = g.NextWindowData.ContentSizeVal;
  else if (first_begin_of_the_frame)
    window->ContentSizeExplicit = wabi::GfVec2f(0.0f, 0.0f);
  if (g.NextWindowData.Flags & AnchorNextWindowDataFlags_HasCollapsed)
    SetWindowCollapsed(window, g.NextWindowData.CollapsedVal, g.NextWindowData.CollapsedCond);
  if (g.NextWindowData.Flags & AnchorNextWindowDataFlags_HasFocus)
    FocusWindow(window);
  if (window->Appearing)
    SetWindowConditionAllowFlags(window, AnchorCond_Appearing, false);

  // When reusing window again multiple times a frame, just append content (don't need to setup
  // again)
  if (first_begin_of_the_frame) {
    // Initialize
    const bool window_is_child_tooltip =
      (flags & AnchorWindowFlags_ChildWindow) &&
      (flags & AnchorWindowFlags_Tooltip);  // FIXME-WIP: Undocumented behavior of Child+Tooltip
                                            // for pinned tooltip (#1345)
    window->Active = true;
    window->HasCloseButton = (p_open != NULL);
    window->ClipRect = wabi::GfVec4f(-FLT_MAX, -FLT_MAX, +FLT_MAX, +FLT_MAX);
    window->IDStack.resize(1);
    window->DrawList->_ResetForNewFrame();
    window->DC.CurrentTableIdx = -1;

    // Restore buffer capacity when woken from a compacted state, to avoid
    if (window->MemoryCompacted)
      GcAwakeTransientWindowBuffers(window);

    // Update stored window name when it changes (which can _only_ happen with the "###" operator,
    // so the ID would stay unchanged). The title bar always display the 'name' parameter, so we
    // only update the string storage if it needs to be visible to the end-user elsewhere.
    bool window_title_visible_elsewhere = false;
    if (g.NavWindowingListWindow != NULL && (window->Flags & AnchorWindowFlags_NoNavFocus) ==
                                              0)  // Window titles visible when using CTRL+TAB
      window_title_visible_elsewhere = true;
    if (window_title_visible_elsewhere && !window_just_created &&
        strcmp(name, window->Name) != 0) {
      size_t buf_len = (size_t)window->NameBufLen;
      window->Name = AnchorStrdupcpy(window->Name, &buf_len, name);
      window->NameBufLen = (int)buf_len;
    }

    // UPDATE CONTENTS SIZE, UPDATE HIDDEN STATUS

    // Update contents size from last frame for auto-fitting (or use explicit size)
    const bool window_just_appearing_after_hidden_for_resize =
      (window->HiddenFramesCannotSkipItems > 0);
    CalcWindowContentSizes(window, &window->ContentSize, &window->ContentSizeIdeal);
    if (window->HiddenFramesCanSkipItems > 0)
      window->HiddenFramesCanSkipItems--;
    if (window->HiddenFramesCannotSkipItems > 0)
      window->HiddenFramesCannotSkipItems--;
    if (window->HiddenFramesForRenderOnly > 0)
      window->HiddenFramesForRenderOnly--;

    // Hide new windows for one frame until they calculate their size
    if (window_just_created && (!window_size_x_set_by_api || !window_size_y_set_by_api))
      window->HiddenFramesCannotSkipItems = 1;

    // Hide popup/tooltip window when re-opening while we measure size (because we recycle the
    // windows) We reset Size/ContentSize for reappearing popups/tooltips early in this function,
    // so further code won't be tempted to use the old size.
    if (window_just_activated_by_user &&
        (flags & (AnchorWindowFlags_Popup | AnchorWindowFlags_Tooltip)) != 0) {
      window->HiddenFramesCannotSkipItems = 1;
      if (flags & AnchorWindowFlags_AlwaysAutoResize) {
        if (!window_size_x_set_by_api)
          window->Size[0] = window->SizeFull[0] = 0.f;
        if (!window_size_y_set_by_api)
          window->Size[1] = window->SizeFull[1] = 0.f;
        window->ContentSize = window->ContentSizeIdeal = wabi::GfVec2f(0.f, 0.f);
      }
    }

    // SELECT VIEWPORT
    // FIXME-VIEWPORT: In the docking/viewport branch, this is the point where we select the
    // current viewport (which may affect the style)
    SetCurrentWindow(window);

    // LOCK BORDER SIZE AND PADDING FOR THE FRAME (so that altering them doesn't cause
    // inconsistencies)

    if (flags & AnchorWindowFlags_ChildWindow)
      window->WindowBorderSize = style.ChildBorderSize;
    else
      window->WindowBorderSize = ((flags &
                                   (AnchorWindowFlags_Popup | AnchorWindowFlags_Tooltip)) &&
                                  !(flags & AnchorWindowFlags_Modal)) ?
                                   style.PopupBorderSize :
                                   style.WindowBorderSize;
    window->WindowPadding = style.WindowPadding;
    if ((flags & AnchorWindowFlags_ChildWindow) &&
        !(flags & (AnchorWindowFlags_AlwaysUseWindowPadding | AnchorWindowFlags_Popup)) &&
        window->WindowBorderSize == 0.0f)
      window->WindowPadding = wabi::GfVec2f(
        0.0f,
        (flags & AnchorWindowFlags_MenuBar) ? style.WindowPadding[1] : 0.0f);

    // Lock menu offset so size calculation can use it as menu-bar windows need a minimum size.
    window->DC.MenuBarOffset[0] = AnchorMax(
      AnchorMax(window->WindowPadding[0], style.ItemSpacing[0]),
      g.NextWindowData.MenuBarOffsetMinVal[0]);
    window->DC.MenuBarOffset[1] = g.NextWindowData.MenuBarOffsetMinVal[1];

    // Collapse window by double-clicking on title bar
    // At this point we don't have a clipping rectangle setup yet, so we can use the title bar area
    // for hit detection and drawing
    if (!(flags & AnchorWindowFlags_NoTitleBar) && !(flags & AnchorWindowFlags_NoCollapse)) {
      // We don't use a regular button+id to test for double-click on title bar (mostly due to
      // legacy reason, could be fixed), so verify that we don't have items over the title bar.
      AnchorBBox title_bar_rect = window->TitleBarRect();
      if (g.HoveredWindow == window && g.HoveredId == 0 && g.HoveredIdPreviousFrame == 0 &&
          IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max) &&
          g.IO.MouseDoubleClicked[0])
        window->WantCollapseToggle = true;
      if (window->WantCollapseToggle) {
        window->Collapsed = !window->Collapsed;
        MarkIniSettingsDirty(window);
      }
    } else {
      window->Collapsed = false;
    }
    window->WantCollapseToggle = false;

    // SIZE

    // Calculate auto-fit size, handle automatic resize
    const wabi::GfVec2f size_auto_fit = CalcWindowAutoFitSize(window, window->ContentSizeIdeal);
    bool use_current_size_for_scrollbar_x = window_just_created;
    bool use_current_size_for_scrollbar_y = window_just_created;
    if ((flags & AnchorWindowFlags_AlwaysAutoResize) && !window->Collapsed) {
      // Using SetNextWindowSize() overrides AnchorWindowFlags_AlwaysAutoResize, so it can be used
      // on tooltips/popups, etc.
      if (!window_size_x_set_by_api) {
        window->SizeFull[0] = size_auto_fit[0];
        use_current_size_for_scrollbar_x = true;
      }
      if (!window_size_y_set_by_api) {
        window->SizeFull[1] = size_auto_fit[1];
        use_current_size_for_scrollbar_y = true;
      }
    } else if (window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0) {
      // Auto-fit may only grow window during the first few frames
      // We still process initial auto-fit on collapsed windows to get a window width, but
      // otherwise don't honor AnchorWindowFlags_AlwaysAutoResize when collapsed.
      if (!window_size_x_set_by_api && window->AutoFitFramesX > 0) {
        window->SizeFull[0] = window->AutoFitOnlyGrows ?
                                AnchorMax(window->SizeFull[0], size_auto_fit[0]) :
                                size_auto_fit[0];
        use_current_size_for_scrollbar_x = true;
      }
      if (!window_size_y_set_by_api && window->AutoFitFramesY > 0) {
        window->SizeFull[1] = window->AutoFitOnlyGrows ?
                                AnchorMax(window->SizeFull[1], size_auto_fit[1]) :
                                size_auto_fit[1];
        use_current_size_for_scrollbar_y = true;
      }
      if (!window->Collapsed)
        MarkIniSettingsDirty(window);
    }

    // Apply minimum/maximum window size constraints and final size
    window->SizeFull = CalcWindowSizeAfterConstraint(window, window->SizeFull);
    window->Size = window->Collapsed && !(flags & AnchorWindowFlags_ChildWindow) ?
                     window->TitleBarRect().GetSize() :
                     window->SizeFull;

    // Decoration size
    const float decoration_up_height = window->TitleBarHeight() + window->MenuBarHeight();

    // POSITION

    // Popup latch its initial position, will position itself when it appears next frame
    if (window_just_activated_by_user) {
      window->AutoPosLastDirection = AnchorDir_None;
      if ((flags & AnchorWindowFlags_Popup) != 0 && !(flags & AnchorWindowFlags_Modal) &&
          !window_pos_set_by_api)  // FIXME: BeginPopup() could use SetNextWindowPos()
        window->Pos = g.BeginPopupStack.back().OpenPopupPos;
    }

    // Position child window
    if (flags & AnchorWindowFlags_ChildWindow) {
      ANCHOR_ASSERT(parent_window && parent_window->Active);
      window->BeginOrderWithinParent = (short)parent_window->DC.ChildWindows.Size;
      parent_window->DC.ChildWindows.push_back(window);
      if (!(flags & AnchorWindowFlags_Popup) && !window_pos_set_by_api && !window_is_child_tooltip)
        window->Pos = parent_window->DC.CursorPos;
    }

    const bool window_pos_with_pivot = (window->SetWindowPosVal[0] != FLT_MAX &&
                                        window->HiddenFramesCannotSkipItems == 0);
    if (window_pos_with_pivot)
      /** todo::check_math */
      // Position given a pivot (e.g. for centering)
      SetWindowPos(
        window,
        wabi::GfVec2f(window->SetWindowPosVal[0] - window->Size[0] * window->SetWindowPosPivot[0],
                window->SetWindowPosVal[1] - window->Size[1] * window->SetWindowPosPivot[1]));
    else if ((flags & AnchorWindowFlags_ChildMenu) != 0)
      window->Pos = FindBestWindowPosForPopup(window);
    else if ((flags & AnchorWindowFlags_Popup) != 0 && !window_pos_set_by_api &&
             window_just_appearing_after_hidden_for_resize)
      window->Pos = FindBestWindowPosForPopup(window);
    else if ((flags & AnchorWindowFlags_Tooltip) != 0 && !window_pos_set_by_api &&
             !window_is_child_tooltip)
      window->Pos = FindBestWindowPosForPopup(window);

    // Calculate the range of allowed position for that window (to be movable and visible past safe
    // area padding) When clamping to stay visible, we will enforce that window->Pos stays inside
    // of visibility_rect.
    AnchorViewportP *viewport = (AnchorViewportP *)(void *)GetMainViewport();
    AnchorBBox viewport_rect(viewport->GetMainRect());
    AnchorBBox viewport_work_rect(viewport->GetWorkRect());
    wabi::GfVec2f visibility_padding = AnchorMax(style.DisplayWindowPadding,
                                           style.DisplaySafeAreaPadding);
    AnchorBBox visibility_rect(viewport_work_rect.Min + visibility_padding,
                               viewport_work_rect.Max - visibility_padding);

    // Clamp position/size so window stays visible within its viewport or monitor
    // Ignore zero-sized display explicitly to avoid losing positions if a window manager reports
    // zero-sized window when initializing or minimizing.
    if (!window_pos_set_by_api && !(flags & AnchorWindowFlags_ChildWindow) &&
        window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0)
      if (viewport_rect.GetWidth() > 0.0f && viewport_rect.GetHeight() > 0.0f)
        ClampWindowRect(window, visibility_rect);
    window->Pos = AnchorFloor(window->Pos);

    // Lock window rounding for the frame (so that altering them doesn't cause inconsistencies)
    // Large values tend to lead to variety of artifacts and are not recommended.
    window->WindowRounding = (flags & AnchorWindowFlags_ChildWindow) ? style.ChildRounding :
                             ((flags & AnchorWindowFlags_Popup) &&
                              !(flags & AnchorWindowFlags_Modal)) ?
                                                                       style.PopupRounding :
                                                                       style.WindowRounding;

    // For windows with title bar or menu bar, we clamp to FrameHeight(FontSize + FramePadding[1]
    // * 2.0f) to completely hide artifacts.
    // if ((window->Flags & AnchorWindowFlags_MenuBar) || !(window->Flags &
    // AnchorWindowFlags_NoTitleBar))
    //    window->WindowRounding = AnchorMin(window->WindowRounding, g.FontSize +
    //    style.FramePadding[1]
    //    * 2.0f);

    // Apply window focus (new and reactivated windows are moved to front)
    bool want_focus = false;
    if (window_just_activated_by_user && !(flags & AnchorWindowFlags_NoFocusOnAppearing)) {
      if (flags & AnchorWindowFlags_Popup)
        want_focus = true;
      else if ((flags & (AnchorWindowFlags_ChildWindow | AnchorWindowFlags_Tooltip)) == 0)
        want_focus = true;
    }

    // Handle manual resize: Resize Grips, Borders, Gamepad
    int border_held = -1;
    AnchorU32 resize_grip_col[4] = {};
    const int resize_grip_count =
      g.IO.ConfigWindowsResizeFromEdges ?
        2 :
        1;  // Allow resize from lower-left if we have the mouse cursor feedback for it.
    const float resize_grip_draw_size = ANCHOR_FLOOR(
      AnchorMax(g.FontSize * 1.10f, window->WindowRounding + 1.0f + g.FontSize * 0.2f));
    if (!window->Collapsed)
      if (UpdateWindowManualResize(window,
                                   size_auto_fit,
                                   &border_held,
                                   resize_grip_count,
                                   &resize_grip_col[0],
                                   visibility_rect))
        use_current_size_for_scrollbar_x = use_current_size_for_scrollbar_y = true;
    window->ResizeBorderHeld = (signed char)border_held;

    // SCROLLBAR VISIBILITY

    // Update scrollbar visibility (based on the Size that was effective during last frame or the
    // auto-resized Size).
    if (!window->Collapsed) {
      // When reading the current size we need to read it after size constraints have been applied.
      // When we use InnerRect here we are intentionally reading last frame size, same for
      // ScrollbarSizes values before we set them again.
      wabi::GfVec2f avail_size_from_current_frame = wabi::GfVec2f(window->SizeFull[0],
                                                      window->SizeFull[1] - decoration_up_height);
      wabi::GfVec2f avail_size_from_last_frame = window->InnerRect.GetSize() + window->ScrollbarSizes;
      wabi::GfVec2f needed_size_from_last_frame = window_just_created ?
                                              wabi::GfVec2f(0, 0) :
                                              window->ContentSize + window->WindowPadding * 2.0f;
      float size_x_for_scrollbars = use_current_size_for_scrollbar_x ?
                                      avail_size_from_current_frame[0] :
                                      avail_size_from_last_frame[0];
      float size_y_for_scrollbars = use_current_size_for_scrollbar_y ?
                                      avail_size_from_current_frame[1] :
                                      avail_size_from_last_frame[1];
      // bool scrollbar_y_from_last_frame = window->ScrollbarY; // FIXME: May want to use that in
      // the ScrollbarX expression? How many pros vs cons?
      window->ScrollbarY = (flags & AnchorWindowFlags_AlwaysVerticalScrollbar) ||
                           ((needed_size_from_last_frame[1] > size_y_for_scrollbars) &&
                            !(flags & AnchorWindowFlags_NoScrollbar));
      window->ScrollbarX = (flags & AnchorWindowFlags_AlwaysHorizontalScrollbar) ||
                           ((needed_size_from_last_frame[0] >
                             size_x_for_scrollbars -
                               (window->ScrollbarY ? style.ScrollbarSize : 0.0f)) &&
                            !(flags & AnchorWindowFlags_NoScrollbar) &&
                            (flags & AnchorWindowFlags_HorizontalScrollbar));
      if (window->ScrollbarX && !window->ScrollbarY)
        window->ScrollbarY = (needed_size_from_last_frame[1] > size_y_for_scrollbars) &&
                             !(flags & AnchorWindowFlags_NoScrollbar);
      window->ScrollbarSizes = wabi::GfVec2f(window->ScrollbarY ? style.ScrollbarSize : 0.0f,
                                       window->ScrollbarX ? style.ScrollbarSize : 0.0f);
    }

    // UPDATE RECTANGLES (1- THOSE NOT AFFECTED BY SCROLLING)
    // Update various regions. Variables they depends on should be set above in this function.
    // We set this up after processing the resize grip so that our rectangles doesn't lag by a
    // frame.

    // Outer rectangle
    // Not affected by window border size. Used by:
    // - FindHoveredWindow() (w/ extra padding when border resize is enabled)
    // - Begin() initial clipping rect for drawing window background and borders.
    // - Begin() clipping whole child
    const AnchorBBox host_rect = ((flags & AnchorWindowFlags_ChildWindow) &&
                                  !(flags & AnchorWindowFlags_Popup) && !window_is_child_tooltip) ?
                                   parent_window->ClipRect :
                                   viewport_rect;
    const AnchorBBox outer_rect = window->Rect();
    const AnchorBBox title_bar_rect = window->TitleBarRect();
    window->OuterRectClipped = outer_rect;
    window->OuterRectClipped.ClipWith(host_rect);

    // Inner rectangle
    // Not affected by window border size. Used by:
    // - InnerClipRect
    // - ScrollToBringRectIntoView()
    // - NavUpdatePageUpPageDown()
    // - Scrollbar()
    window->InnerRect.Min[0] = window->Pos[0];
    window->InnerRect.Min[1] = window->Pos[1] + decoration_up_height;
    window->InnerRect.Max[0] = window->Pos[0] + window->Size[0] - window->ScrollbarSizes[0];
    window->InnerRect.Max[1] = window->Pos[1] + window->Size[1] - window->ScrollbarSizes[1];

    // Inner clipping rectangle.
    // Will extend a little bit outside the normal work region.
    // This is to allow e.g. Selectable or CollapsingHeader or some separators to cover that space.
    // Force round operator last to ensure that e.g. (int)(max[0]-min[0]) in user's render code
    // produce correct result. Note that if our window is collapsed we will end up with an inverted
    // (~null) clipping rectangle which is the correct behavior. Affected by window/frame border
    // size. Used by:
    // - Begin() initial clip rect
    float top_border_size = (((flags & AnchorWindowFlags_MenuBar) ||
                              !(flags & AnchorWindowFlags_NoTitleBar)) ?
                               style.FrameBorderSize :
                               window->WindowBorderSize);
    window->InnerClipRect.Min[0] = AnchorFloor(
      0.5f + window->InnerRect.Min[0] +
      AnchorMax(AnchorFloor(window->WindowPadding[0] * 0.5f), window->WindowBorderSize));
    window->InnerClipRect.Min[1] = AnchorFloor(0.5f + window->InnerRect.Min[1] + top_border_size);
    window->InnerClipRect.Max[0] = AnchorFloor(
      0.5f + window->InnerRect.Max[0] -
      AnchorMax(AnchorFloor(window->WindowPadding[0] * 0.5f), window->WindowBorderSize));
    window->InnerClipRect.Max[1] = AnchorFloor(0.5f + window->InnerRect.Max[1] -
                                               window->WindowBorderSize);
    window->InnerClipRect.ClipWithFull(host_rect);

    // Default item width. Make it proportional to window size if window manually resizes
    if (window->Size[0] > 0.0f && !(flags & AnchorWindowFlags_Tooltip) &&
        !(flags & AnchorWindowFlags_AlwaysAutoResize))
      window->ItemWidthDefault = AnchorFloor(window->Size[0] * 0.65f);
    else
      window->ItemWidthDefault = AnchorFloor(g.FontSize * 16.0f);

    // SCROLLING

    // Lock down maximum scrolling
    // The value of ScrollMax are ahead from ScrollbarX/ScrollbarY which is intentionally using
    // InnerRect from previous rect in order to accommodate for right/bottom aligned items without
    // creating a scrollbar.
    window->ScrollMax[0] = AnchorMax(0.0f,
                                     window->ContentSize[0] + window->WindowPadding[0] * 2.0f -
                                       window->InnerRect.GetWidth());
    window->ScrollMax[1] = AnchorMax(0.0f,
                                     window->ContentSize[1] + window->WindowPadding[1] * 2.0f -
                                       window->InnerRect.GetHeight());

    // Apply scrolling
    window->Scroll = CalcNextScrollFromScrollTargetAndClamp(window);
    window->ScrollTarget = wabi::GfVec2f(FLT_MAX, FLT_MAX);

    // DRAWING

    // Setup draw list and outer clipping rectangle
    ANCHOR_ASSERT(window->DrawList->CmdBuffer.Size == 1 &&
                  window->DrawList->CmdBuffer[0].ElemCount == 0);
    window->DrawList->PushTextureID(g.Font->ContainerAtlas->TexID);
    PushClipRect(host_rect.Min, host_rect.Max, false);

    // Draw modal window background (darkens what is behind them, all viewports)
    const bool dim_bg_for_modal = (flags & AnchorWindowFlags_Modal) &&
                                  window == GetTopMostPopupModal() &&
                                  window->HiddenFramesCannotSkipItems <= 0;
    const bool dim_bg_for_window_list = g.NavWindowingTargetAnim &&
                                        (window == g.NavWindowingTargetAnim->RootWindow);
    if (dim_bg_for_modal || dim_bg_for_window_list) {
      const AnchorU32 dim_bg_col = GetColorU32(dim_bg_for_modal ? AnchorCol_ModalWindowDimBg :
                                                                  AnchorCol_NavWindowingDimBg,
                                               g.DimBgRatio);
      window->DrawList->AddRectFilled(viewport_rect.Min, viewport_rect.Max, dim_bg_col);
    }

    // Draw navigation selection/windowing rectangle background
    if (dim_bg_for_window_list && window == g.NavWindowingTargetAnim) {
      AnchorBBox bb = window->Rect();
      bb.Expand(g.FontSize);
      if (!bb.Contains(
            viewport_rect))  // Avoid drawing if the window covers all the viewport anyway
        window->DrawList->AddRectFilled(
          bb.Min,
          bb.Max,
          GetColorU32(AnchorCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha * 0.25f),
          g.Style.WindowRounding);
    }

    // Since 1.71, child window can render their decoration (bg color, border, scrollbars, etc.)
    // within their parent to save a draw call. When using overlapping child windows, this will
    // break the assumption that child z-order is mapped to submission order. We disable this when
    // the parent window has zero vertices, which is a common pattern leading to laying out
    // multiple overlapping child. We also disabled this when we have dimming overlay behind this
    // specific one child.
    // FIXME: More code may rely on explicit sorting of overlapping child window and would need to
    // disable this somehow. Please get in contact if you are affected.
    {
      bool render_decorations_in_parent = false;
      if ((flags & AnchorWindowFlags_ChildWindow) && !(flags & AnchorWindowFlags_Popup) &&
          !window_is_child_tooltip)
        if (window->DrawList->CmdBuffer.back().ElemCount == 0 &&
            parent_window->DrawList->VtxBuffer.Size > 0)
          render_decorations_in_parent = true;
      if (render_decorations_in_parent)
        window->DrawList = parent_window->DrawList;

      // Handle title bar, scrollbar, resize grips and resize borders
      const AnchorWindow *window_to_highlight = g.NavWindowingTarget ? g.NavWindowingTarget :
                                                                       g.NavWindow;
      const bool title_bar_is_highlight = want_focus ||
                                          (window_to_highlight &&
                                           window->RootWindowForTitleBarHighlight ==
                                             window_to_highlight->RootWindowForTitleBarHighlight);
      RenderWindowDecorations(window,
                              title_bar_rect,
                              title_bar_is_highlight,
                              resize_grip_count,
                              resize_grip_col,
                              resize_grip_draw_size);

      if (render_decorations_in_parent)
        window->DrawList = &window->DrawListInst;
    }

    // Draw navigation selection/windowing rectangle border
    if (g.NavWindowingTargetAnim == window) {
      float rounding = AnchorMax(window->WindowRounding, g.Style.WindowRounding);
      AnchorBBox bb = window->Rect();
      bb.Expand(g.FontSize);
      if (bb.Contains(
            viewport_rect))  // If a window fits the entire viewport, adjust its highlight inward
      {
        bb.Expand(-g.FontSize - 1.0f);
        rounding = window->WindowRounding;
      }
      window->DrawList->AddRect(
        bb.Min,
        bb.Max,
        GetColorU32(AnchorCol_NavWindowingHighlight, g.NavWindowingHighlightAlpha),
        rounding,
        0,
        3.0f);
    }

    // UPDATE RECTANGLES (2- THOSE AFFECTED BY SCROLLING)

    // Work rectangle.
    // Affected by window padding and border size. Used by:
    // - Columns() for right-most edge
    // - TreeNode(), CollapsingHeader() for right-most edge
    // - BeginTabBar() for right-most edge
    const bool allow_scrollbar_x = !(flags & AnchorWindowFlags_NoScrollbar) &&
                                   (flags & AnchorWindowFlags_HorizontalScrollbar);
    const bool allow_scrollbar_y = !(flags & AnchorWindowFlags_NoScrollbar);
    const float work_rect_size_x = (window->ContentSizeExplicit[0] != 0.0f ?
                                      window->ContentSizeExplicit[0] :
                                      AnchorMax(allow_scrollbar_x ? window->ContentSize[0] : 0.0f,
                                                window->Size[0] - window->WindowPadding[0] * 2.0f -
                                                  window->ScrollbarSizes[0]));
    const float work_rect_size_y = (window->ContentSizeExplicit[1] != 0.0f ?
                                      window->ContentSizeExplicit[1] :
                                      AnchorMax(allow_scrollbar_y ? window->ContentSize[1] : 0.0f,
                                                window->Size[1] - window->WindowPadding[1] * 2.0f -
                                                  decoration_up_height -
                                                  window->ScrollbarSizes[1]));
    window->WorkRect.Min[0] = AnchorFloor(
      window->InnerRect.Min[0] - window->Scroll[0] +
      AnchorMax(window->WindowPadding[0], window->WindowBorderSize));
    window->WorkRect.Min[1] = AnchorFloor(
      window->InnerRect.Min[1] - window->Scroll[1] +
      AnchorMax(window->WindowPadding[1], window->WindowBorderSize));
    window->WorkRect.Max[0] = window->WorkRect.Min[0] + work_rect_size_x;
    window->WorkRect.Max[1] = window->WorkRect.Min[1] + work_rect_size_y;
    window->ParentWorkRect = window->WorkRect;

    // [LEGACY] Content Region
    // FIXME-OBSOLETE: window->ContentRegionRect.Max is currently very misleading / partly faulty,
    // but some BeginChild() patterns relies on it. Used by:
    // - Mouse wheel scrolling + many other things
    window->ContentRegionRect.Min[0] = window->Pos[0] - window->Scroll[0] +
                                       window->WindowPadding[0];
    window->ContentRegionRect.Min[1] = window->Pos[1] - window->Scroll[1] +
                                       window->WindowPadding[1] + decoration_up_height;
    window->ContentRegionRect.Max[0] = window->ContentRegionRect.Min[0] +
                                       (window->ContentSizeExplicit[0] != 0.0f ?
                                          window->ContentSizeExplicit[0] :
                                          (window->Size[0] - window->WindowPadding[0] * 2.0f -
                                           window->ScrollbarSizes[0]));
    window->ContentRegionRect.Max[1] = window->ContentRegionRect.Min[1] +
                                       (window->ContentSizeExplicit[1] != 0.0f ?
                                          window->ContentSizeExplicit[1] :
                                          (window->Size[1] - window->WindowPadding[1] * 2.0f -
                                           decoration_up_height - window->ScrollbarSizes[1]));

    // Setup drawing context
    // (NB: That term "drawing context / DC" lost its meaning a long time ago. Initially was meant
    // to hold transient data only. Nowadays difference between window-> and window->DC-> is
    // dubious.)
    window->DC.Indent.x = 0.0f + window->WindowPadding[0] - window->Scroll[0];
    window->DC.GroupOffset.x = 0.0f;
    window->DC.ColumnsOffset.x = 0.0f;
    window->DC.CursorStartPos = window->Pos +
                                wabi::GfVec2f(window->DC.Indent.x + window->DC.ColumnsOffset.x,
                                        decoration_up_height + window->WindowPadding[1] -
                                          window->Scroll[1]);
    window->DC.CursorPos = window->DC.CursorStartPos;
    window->DC.CursorPosPrevLine = window->DC.CursorPos;
    window->DC.CursorMaxPos = window->DC.CursorStartPos;
    window->DC.IdealMaxPos = window->DC.CursorStartPos;
    window->DC.CurrLineSize = window->DC.PrevLineSize = wabi::GfVec2f(0.0f, 0.0f);
    window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset = 0.0f;

    window->DC.NavLayerCurrent = ANCHORNavLayer_Main;
    window->DC.NavLayersActiveMask = window->DC.NavLayersActiveMaskNext;
    window->DC.NavLayersActiveMaskNext = 0x00;
    window->DC.NavHideHighlightOneFrame = false;
    window->DC.NavHasScroll = (window->ScrollMax[1] > 0.0f);

    window->DC.MenuBarAppending = false;
    window->DC.MenuColumns.Update(3, style.ItemSpacing[0], window_just_activated_by_user);
    window->DC.TreeDepth = 0;
    window->DC.TreeJumpToParentOnPopMask = 0x00;
    window->DC.ChildWindows.resize(0);
    window->DC.StateStorage = &window->StateStorage;
    window->DC.CurrentColumns = NULL;
    window->DC.LayoutType = AnchorLayoutType_Vertical;
    window->DC.ParentLayoutType = parent_window ? parent_window->DC.LayoutType :
                                                  AnchorLayoutType_Vertical;
    window->DC.FocusCounterRegular = window->DC.FocusCounterTabStop = -1;

    window->DC.ItemWidth = window->ItemWidthDefault;
    window->DC.TextWrapPos = -1.0f;  // disabled
    window->DC.ItemWidthStack.resize(0);
    window->DC.TextWrapPosStack.resize(0);

    if (window->AutoFitFramesX > 0)
      window->AutoFitFramesX--;
    if (window->AutoFitFramesY > 0)
      window->AutoFitFramesY--;

    // Apply focus (we need to call FocusWindow() AFTER setting DC.CursorStartPos so our initial
    // navigation reference rectangle can start around there)
    if (want_focus) {
      FocusWindow(window);
      NavInitWindow(window, false);  // <-- this is in the way for us to be able to defer and sort
                                     // reappearing FocusWindow() calls
    }

    // Title bar
    if (!(flags & AnchorWindowFlags_NoTitleBar))
      RenderWindowTitleBarContents(window,
                                   AnchorBBox(title_bar_rect.Min[0] + window->WindowBorderSize,
                                              title_bar_rect.Min[1],
                                              title_bar_rect.Max[0] - window->WindowBorderSize,
                                              title_bar_rect.Max[1]),
                                   name,
                                   p_open);

    // Clear hit test shape every frame
    window->HitTestHoleSize[0] = window->HitTestHoleSize[1] = 0;

    // Pressing CTRL+C while holding on a window copy its content to the clipboard
    // This works but 1. doesn't handle multiple Begin/End pairs, 2. recursing into another
    // Begin/End pair - so we need to work that out and add better logging scope. Maybe we can
    // support CTRL+C on every element?
    /*
    //if (g.NavWindow == window && g.ActiveId == 0)
    if (g.ActiveId == window->MoveId)
        if (g.IO.KeyCtrl && IsKeyPressedMap(AnchorKey_C))
            LogToClipboard();
    */

    // We fill last item data based on Title Bar/Tab, in order for IsItemHovered() and
    // IsItemActive() to be usable after Begin(). This is useful to allow creating context menus on
    // title bar only, etc.
    SetLastItemData(window,
                    window->MoveId,
                    IsMouseHoveringRect(title_bar_rect.Min, title_bar_rect.Max, false) ?
                      AnchorItemStatusFlags_HoveredRect :
                      0,
                    title_bar_rect);

#ifdef ANCHOR_ENABLE_TEST_ENGINE
    if (!(window->Flags & AnchorWindowFlags_NoTitleBar))
      ANCHOR_TEST_ENGINE_ITEM_ADD(window->DC.LastItemRect, window->DC.LastItemId);
#endif
  } else {
    // Append
    SetCurrentWindow(window);
  }

  // Pull/inherit current state
  g.CurrentItemFlags = g.ItemFlagsStack.back();  // Inherit from shared stack
  window->DC.NavFocusScopeIdCurrent = (flags & AnchorWindowFlags_ChildWindow) ?
                                        parent_window->DC.NavFocusScopeIdCurrent :
                                        0;  // Inherit from parent only // -V595

  PushClipRect(window->InnerClipRect.Min, window->InnerClipRect.Max, true);

  // Clear 'accessed' flag last thing (After PushClipRect which will set the flag. We want the flag
  // to stay false when the default "Debug" window is unused)
  window->WriteAccessed = false;
  window->BeginCount++;
  g.NextWindowData.ClearFlags();

  // Update visibility
  if (first_begin_of_the_frame) {
    if (flags & AnchorWindowFlags_ChildWindow) {
      // Child window can be out of sight and have "negative" clip windows.
      // Mark them as collapsed so commands are skipped earlier (we can't manually collapse them
      // because they have no title bar).
      ANCHOR_ASSERT((flags & AnchorWindowFlags_NoTitleBar) != 0);
      if (!(flags & AnchorWindowFlags_AlwaysAutoResize) && window->AutoFitFramesX <= 0 &&
          window->AutoFitFramesY <= 0)  // FIXME: Doesn't make sense for ChildWindow??
        if (!g.LogEnabled)
          if (window->OuterRectClipped.Min[0] >= window->OuterRectClipped.Max[0] ||
              window->OuterRectClipped.Min[1] >= window->OuterRectClipped.Max[1])
            window->HiddenFramesCanSkipItems = 1;

      // Hide along with parent or if parent is collapsed
      if (parent_window &&
          (parent_window->Collapsed || parent_window->HiddenFramesCanSkipItems > 0))
        window->HiddenFramesCanSkipItems = 1;
      if (parent_window &&
          (parent_window->Collapsed || parent_window->HiddenFramesCannotSkipItems > 0))
        window->HiddenFramesCannotSkipItems = 1;
    }

    // Don't render if style alpha is 0.0 at the time of Begin(). This is arbitrary and
    // inconsistent but has been there for a long while (may remove at some point)
    if (style.Alpha <= 0.0f)
      window->HiddenFramesCanSkipItems = 1;

    // Update the Hidden flag
    window->Hidden = (window->HiddenFramesCanSkipItems > 0) ||
                     (window->HiddenFramesCannotSkipItems > 0) ||
                     (window->HiddenFramesForRenderOnly > 0);

    // Disable inputs for requested number of frames
    if (window->DisableInputsFrames > 0) {
      window->DisableInputsFrames--;
      window->Flags |= AnchorWindowFlags_NoInputs;
    }

    // Update the SkipItems flag, used to early out of all items functions (no layout required)
    bool skip_items = false;
    if (window->Collapsed || !window->Active || window->Hidden)
      if (window->AutoFitFramesX <= 0 && window->AutoFitFramesY <= 0 &&
          window->HiddenFramesCannotSkipItems <= 0)
        skip_items = true;
    window->SkipItems = skip_items;
  }

  return !window->SkipItems;
}

void ANCHOR::End()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  // Error checking: verify that user hasn't called End() too many times!
  if (g.CurrentWindowStack.Size <= 1 && g.WithinFrameScopeWithImplicitWindow) {
    ANCHOR_ASSERT_USER_ERROR(g.CurrentWindowStack.Size > 1, "Calling End() too many times!");
    return;
  }
  ANCHOR_ASSERT(g.CurrentWindowStack.Size > 0);

  // Error checking: verify that user doesn't directly call End() on a child window.
  if (window->Flags & AnchorWindowFlags_ChildWindow)
    ANCHOR_ASSERT_USER_ERROR(g.WithinEndChild, "Must call EndChild() and not End()!");

  // Close anything that is open
  if (window->DC.CurrentColumns)
    EndColumns();
  PopClipRect();  // Inner window clip rectangle

  // Stop logging
  if (!(window->Flags &
        AnchorWindowFlags_ChildWindow))  // FIXME: add more options for scope of logging
    LogFinish();

  // Pop from window stack
  g.CurrentWindowStack.pop_back();
  if (window->Flags & AnchorWindowFlags_Popup)
    g.BeginPopupStack.pop_back();
  window->DC.StackSizesOnBegin.CompareWithCurrentState();
  SetCurrentWindow(g.CurrentWindowStack.empty() ? NULL : g.CurrentWindowStack.back());
}

void ANCHOR::BringWindowToFocusFront(AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(window == window->RootWindow);

  const int cur_order = window->FocusOrder;
  ANCHOR_ASSERT(g.WindowsFocusOrder[cur_order] == window);
  if (g.WindowsFocusOrder.back() == window)
    return;

  const int new_order = g.WindowsFocusOrder.Size - 1;
  for (int n = cur_order; n < new_order; n++) {
    g.WindowsFocusOrder[n] = g.WindowsFocusOrder[n + 1];
    g.WindowsFocusOrder[n]->FocusOrder--;
    ANCHOR_ASSERT(g.WindowsFocusOrder[n]->FocusOrder == n);
  }
  g.WindowsFocusOrder[new_order] = window;
  window->FocusOrder = (short)new_order;
}

void ANCHOR::BringWindowToDisplayFront(AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *current_front_window = g.Windows.back();
  if (current_front_window == window ||
      current_front_window->RootWindow == window)  // Cheap early out (could be better)
    return;
  for (int i = g.Windows.Size - 2; i >= 0; i--)  // We can ignore the top-most window
    if (g.Windows[i] == window) {
      memmove(&g.Windows[i],
              &g.Windows[i + 1],
              (size_t)(g.Windows.Size - i - 1) * sizeof(AnchorWindow *));
      g.Windows[g.Windows.Size - 1] = window;
      break;
    }
}

void ANCHOR::BringWindowToDisplayBack(AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;
  if (g.Windows[0] == window)
    return;
  for (int i = 0; i < g.Windows.Size; i++)
    if (g.Windows[i] == window) {
      memmove(&g.Windows[1], &g.Windows[0], (size_t)i * sizeof(AnchorWindow *));
      g.Windows[0] = window;
      break;
    }
}

// Moving window to front of display and set focus (which happens to be back of our sorted list)
void ANCHOR::FocusWindow(AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;

  if (g.NavWindow != window) {
    g.NavWindow = window;
    if (window && g.NavDisableMouseHover)
      g.NavMousePosDirty = true;
    g.NavId = window ? window->NavLastIds[0] : 0;  // Restore NavId
    g.NavFocusScopeId = 0;
    g.NavIdIsAlive = false;
    g.NavLayer = ANCHORNavLayer_Main;
    g.NavInitRequest = g.NavMoveRequest = false;
    NavUpdateAnyRequestFlag();
    // ANCHOR_DEBUG_LOG("FocusWindow(\"%s\")\n", window ? window->Name : NULL);
  }

  // Close popups if any
  ClosePopupsOverWindow(window, false);

  // Move the root window to the top of the pile
  ANCHOR_ASSERT(window == NULL || window->RootWindow != NULL);
  AnchorWindow *focus_front_window =
    window ? window->RootWindow :
             NULL;  // NB: In docking branch this is window->RootWindowDockStop
  AnchorWindow *display_front_window = window ? window->RootWindow : NULL;

  // Steal active widgets. Some of the cases it triggers includes:
  // - Focus a window while an InputText in another window is active, if focus happens before the
  // old InputText can run.
  // - When using Nav to activate menu items (due to timing of activating on press->new window
  // appears->losing ActiveId)
  if (g.ActiveId != 0 && g.ActiveIdWindow && g.ActiveIdWindow->RootWindow != focus_front_window)
    if (!g.ActiveIdNoClearOnFocusLoss)
      ClearActiveID();

  // Passing NULL allow to disable keyboard focus
  if (!window)
    return;

  // Bring to front
  BringWindowToFocusFront(focus_front_window);
  if (((window->Flags | display_front_window->Flags) & AnchorWindowFlags_NoBringToFrontOnFocus) ==
      0)
    BringWindowToDisplayFront(display_front_window);
}

void ANCHOR::FocusTopMostWindowUnderOne(AnchorWindow *under_this_window,
                                        AnchorWindow *ignore_window)
{
  AnchorContext &g = *G_CTX;

  const int start_idx = ((under_this_window != NULL) ? FindWindowFocusIndex(under_this_window) :
                                                       g.WindowsFocusOrder.Size) -
                        1;
  for (int i = start_idx; i >= 0; i--) {
    // We may later decide to test for different NoXXXInputs based on the active navigation input
    // (mouse vs nav) but that may feel more confusing to the user.
    AnchorWindow *window = g.WindowsFocusOrder[i];
    ANCHOR_ASSERT(window == window->RootWindow);
    if (window != ignore_window && window->WasActive)
      if ((window->Flags & (AnchorWindowFlags_NoMouseInputs | AnchorWindowFlags_NoNavInputs)) !=
          (AnchorWindowFlags_NoMouseInputs | AnchorWindowFlags_NoNavInputs)) {
        AnchorWindow *focus_window = NavRestoreLastChildNavWindow(window);
        FocusWindow(focus_window);
        return;
      }
  }
  FocusWindow(NULL);
}

// Important: this alone doesn't alter current AnchorDrawList state. This is called by
// PushFont/PopFont only.
void ANCHOR::SetCurrentFont(AnchorFont *font)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(font && font->IsLoaded());  // Font Atlas not created. Did you call
                                            // io.Fonts->GetTexDataAsRGBA32 / GetTexDataAsAlpha8 ?
  ANCHOR_ASSERT(font->Scale > 0.0f);
  g.Font = font;
  g.FontBaseSize = AnchorMax(1.0f, g.IO.FontGlobalScale * g.Font->FontSize * g.Font->Scale);
  g.FontSize = g.CurrentWindow ? g.CurrentWindow->CalcFontSize() : 0.0f;

  AnchorFontAtlas *atlas = g.Font->ContainerAtlas;
  g.DrawListSharedData.TexUvWhitePixel = atlas->TexUvWhitePixel;
  g.DrawListSharedData.TexUvLines = atlas->TexUvLines;
  g.DrawListSharedData.Font = g.Font;
  g.DrawListSharedData.FontSize = g.FontSize;
}

void ANCHOR::PushFont(AnchorFont *font)
{
  AnchorContext &g = *G_CTX;
  if (!font)
    font = GetDefaultFont();
  SetCurrentFont(font);
  g.FontStack.push_back(font);
  g.CurrentWindow->DrawList->PushTextureID(font->ContainerAtlas->TexID);
}

void ANCHOR::PopFont()
{
  AnchorContext &g = *G_CTX;
  g.CurrentWindow->DrawList->PopTextureID();
  g.FontStack.pop_back();
  SetCurrentFont(g.FontStack.empty() ? GetDefaultFont() : g.FontStack.back());
}

void ANCHOR::PushItemFlag(AnchorItemFlags option, bool enabled)
{
  AnchorContext &g = *G_CTX;
  AnchorItemFlags item_flags = g.CurrentItemFlags;
  ANCHOR_ASSERT(item_flags == g.ItemFlagsStack.back());
  if (enabled)
    item_flags |= option;
  else
    item_flags &= ~option;
  g.CurrentItemFlags = item_flags;
  g.ItemFlagsStack.push_back(item_flags);
}

void ANCHOR::PopItemFlag()
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(
    g.ItemFlagsStack.Size >
    1);  // Too many calls to PopItemFlag() - we always leave a 0 at the bottom of the stack.
  g.ItemFlagsStack.pop_back();
  g.CurrentItemFlags = g.ItemFlagsStack.back();
}

// FIXME: Look into renaming this once we have settled the new Focus/Activation/TabStop system.
void ANCHOR::PushAllowKeyboardFocus(bool allow_keyboard_focus)
{
  PushItemFlag(AnchorItemFlags_NoTabStop, !allow_keyboard_focus);
}

void ANCHOR::PopAllowKeyboardFocus()
{
  PopItemFlag();
}

void ANCHOR::PushButtonRepeat(bool repeat)
{
  PushItemFlag(AnchorItemFlags_ButtonRepeat, repeat);
}

void ANCHOR::PopButtonRepeat()
{
  PopItemFlag();
}

void ANCHOR::PushTextWrapPos(float wrap_pos_x)
{
  AnchorWindow *window = GetCurrentWindow();
  window->DC.TextWrapPosStack.push_back(window->DC.TextWrapPos);
  window->DC.TextWrapPos = wrap_pos_x;
}

void ANCHOR::PopTextWrapPos()
{
  AnchorWindow *window = GetCurrentWindow();
  window->DC.TextWrapPos = window->DC.TextWrapPosStack.back();
  window->DC.TextWrapPosStack.pop_back();
}

bool ANCHOR::IsWindowChildOf(AnchorWindow *window, AnchorWindow *potential_parent)
{
  if (window->RootWindow == potential_parent)
    return true;
  while (window != NULL) {
    if (window == potential_parent)
      return true;
    window = window->ParentWindow;
  }
  return false;
}

bool ANCHOR::IsWindowAbove(AnchorWindow *potential_above, AnchorWindow *potential_below)
{
  AnchorContext &g = *G_CTX;
  for (int i = g.Windows.Size - 1; i >= 0; i--) {
    AnchorWindow *candidate_window = g.Windows[i];
    if (candidate_window == potential_above)
      return true;
    if (candidate_window == potential_below)
      return false;
  }
  return false;
}

bool ANCHOR::IsWindowHovered(AnchorHoveredFlags flags)
{
  ANCHOR_ASSERT((flags & AnchorHoveredFlags_AllowWhenOverlapped) ==
                0);  // Flags not supported by this function
  AnchorContext &g = *G_CTX;
  if (g.HoveredWindow == NULL)
    return false;

  if ((flags & AnchorHoveredFlags_AnyWindow) == 0) {
    AnchorWindow *window = g.CurrentWindow;
    switch (flags & (AnchorHoveredFlags_RootWindow | AnchorHoveredFlags_ChildWindows)) {
      case AnchorHoveredFlags_RootWindow | AnchorHoveredFlags_ChildWindows:
        if (g.HoveredWindow->RootWindow != window->RootWindow)
          return false;
        break;
      case AnchorHoveredFlags_RootWindow:
        if (g.HoveredWindow != window->RootWindow)
          return false;
        break;
      case AnchorHoveredFlags_ChildWindows:
        if (!IsWindowChildOf(g.HoveredWindow, window))
          return false;
        break;
      default:
        if (g.HoveredWindow != window)
          return false;
        break;
    }
  }

  if (!IsWindowContentHoverable(g.HoveredWindow, flags))
    return false;
  if (!(flags & AnchorHoveredFlags_AllowWhenBlockedByActiveItem))
    if (g.ActiveId != 0 && !g.ActiveIdAllowOverlap && g.ActiveId != g.HoveredWindow->MoveId)
      return false;
  return true;
}

bool ANCHOR::IsWindowFocused(AnchorFocusedFlags flags)
{
  AnchorContext &g = *G_CTX;

  if (flags & AnchorFocusedFlags_AnyWindow)
    return g.NavWindow != NULL;

  ANCHOR_ASSERT(g.CurrentWindow);  // Not inside a Begin()/End()
  switch (flags & (AnchorFocusedFlags_RootWindow | AnchorFocusedFlags_ChildWindows)) {
    case AnchorFocusedFlags_RootWindow | AnchorFocusedFlags_ChildWindows:
      return g.NavWindow && g.NavWindow->RootWindow == g.CurrentWindow->RootWindow;
    case AnchorFocusedFlags_RootWindow:
      return g.NavWindow == g.CurrentWindow->RootWindow;
    case AnchorFocusedFlags_ChildWindows:
      return g.NavWindow && IsWindowChildOf(g.NavWindow, g.CurrentWindow);
    default:
      return g.NavWindow == g.CurrentWindow;
  }
}

// Can we focus this window with CTRL+TAB (or PadMenu + PadFocusPrev/PadFocusNext)
// Note that NoNavFocus makes the window not reachable with CTRL+TAB but it can still be focused
// with mouse or programmatically. If you want a window to never be focused, you may use the e.g.
// NoInputs flag.
bool ANCHOR::IsWindowNavFocusable(AnchorWindow *window)
{
  return window->WasActive && window == window->RootWindow &&
         !(window->Flags & AnchorWindowFlags_NoNavFocus);
}

float ANCHOR::GetWindowWidth()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->Size[0];
}

float ANCHOR::GetWindowHeight()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->Size[1];
}

wabi::GfVec2f ANCHOR::GetWindowPos()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  return window->Pos;
}

void ANCHOR::SetWindowPos(AnchorWindow *window, const wabi::GfVec2f &pos, AnchorCond cond)
{
  // Test condition (NB: bit 0 is always true) and clear flags for next time
  if (cond && (window->SetWindowPosAllowFlags & cond) == 0)
    return;

  ANCHOR_ASSERT(
    cond == 0 ||
    ImIsPowerOfTwo(
      cond));  // Make sure the user doesn't attempt to combine multiple condition flags.
  window->SetWindowPosAllowFlags &= ~(AnchorCond_Once | AnchorCond_FirstUseEver |
                                      AnchorCond_Appearing);
  window->SetWindowPosVal = wabi::GfVec2f(FLT_MAX, FLT_MAX);

  // Set
  const wabi::GfVec2f old_pos = window->Pos;
  window->Pos = AnchorFloor(pos);
  wabi::GfVec2f offset = window->Pos - old_pos;
  window->DC.CursorPos +=
    offset;  // As we happen to move the window while it is being appended to (which is
             // a bad idea - will smear) let's at least offset the cursor
  window->DC.CursorMaxPos +=
    offset;  // And more importantly we need to offset CursorMaxPos/CursorStartPos
             // this so ContentSize calculation doesn't get affected.
  window->DC.IdealMaxPos += offset;
  window->DC.CursorStartPos += offset;
}

void ANCHOR::SetWindowPos(const wabi::GfVec2f &pos, AnchorCond cond)
{
  AnchorWindow *window = GetCurrentWindowRead();
  SetWindowPos(window, pos, cond);
}

void ANCHOR::SetWindowPos(const char *name, const wabi::GfVec2f &pos, AnchorCond cond)
{
  if (AnchorWindow *window = FindWindowByName(name))
    SetWindowPos(window, pos, cond);
}

wabi::GfVec2f ANCHOR::GetWindowSize()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->Size;
}

void ANCHOR::SetWindowSize(AnchorWindow *window, const wabi::GfVec2f &size, AnchorCond cond)
{
  // Test condition (NB: bit 0 is always true) and clear flags for next time
  if (cond && (window->SetWindowSizeAllowFlags & cond) == 0)
    return;

  ANCHOR_ASSERT(
    cond == 0 ||
    ImIsPowerOfTwo(
      cond));  // Make sure the user doesn't attempt to combine multiple condition flags.
  window->SetWindowSizeAllowFlags &= ~(AnchorCond_Once | AnchorCond_FirstUseEver |
                                       AnchorCond_Appearing);

  // Set
  if (size[0] > 0.0f) {
    window->AutoFitFramesX = 0;
    window->SizeFull[0] = ANCHOR_FLOOR(size[0]);
  } else {
    window->AutoFitFramesX = 2;
    window->AutoFitOnlyGrows = false;
  }
  if (size[1] > 0.0f) {
    window->AutoFitFramesY = 0;
    window->SizeFull[1] = ANCHOR_FLOOR(size[1]);
  } else {
    window->AutoFitFramesY = 2;
    window->AutoFitOnlyGrows = false;
  }
}

void ANCHOR::SetWindowSize(const wabi::GfVec2f &size, AnchorCond cond)
{
  SetWindowSize(G_CTX->CurrentWindow, size, cond);
}

void ANCHOR::SetWindowSize(const char *name, const wabi::GfVec2f &size, AnchorCond cond)
{
  if (AnchorWindow *window = FindWindowByName(name))
    SetWindowSize(window, size, cond);
}

void ANCHOR::SetWindowCollapsed(AnchorWindow *window, bool collapsed, AnchorCond cond)
{
  // Test condition (NB: bit 0 is always true) and clear flags for next time
  if (cond && (window->SetWindowCollapsedAllowFlags & cond) == 0)
    return;
  window->SetWindowCollapsedAllowFlags &= ~(AnchorCond_Once | AnchorCond_FirstUseEver |
                                            AnchorCond_Appearing);

  // Set
  window->Collapsed = collapsed;
}

void ANCHOR::SetWindowHitTestHole(AnchorWindow *window, const wabi::GfVec2f &pos, const wabi::GfVec2f &size)
{
  ANCHOR_ASSERT(window->HitTestHoleSize[0] ==
                0);  // We don't support multiple holes/hit test filters
  window->HitTestHoleSize = wabi::GfVec2h(size);
  window->HitTestHoleOffset = wabi::GfVec2h(pos - window->Pos);
}

void ANCHOR::SetWindowCollapsed(bool collapsed, AnchorCond cond)
{
  SetWindowCollapsed(G_CTX->CurrentWindow, collapsed, cond);
}

bool ANCHOR::IsWindowCollapsed()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->Collapsed;
}

bool ANCHOR::IsWindowAppearing()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->Appearing;
}

void ANCHOR::SetWindowCollapsed(const char *name, bool collapsed, AnchorCond cond)
{
  if (AnchorWindow *window = FindWindowByName(name))
    SetWindowCollapsed(window, collapsed, cond);
}

void ANCHOR::SetWindowFocus()
{
  FocusWindow(G_CTX->CurrentWindow);
}

void ANCHOR::SetWindowFocus(const char *name)
{
  if (name) {
    if (AnchorWindow *window = FindWindowByName(name))
      FocusWindow(window);
  } else {
    FocusWindow(NULL);
  }
}

void ANCHOR::SetNextWindowPos(const wabi::GfVec2f &pos, AnchorCond cond, const wabi::GfVec2f &pivot)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(
    cond == 0 ||
    ImIsPowerOfTwo(
      cond));  // Make sure the user doesn't attempt to combine multiple condition flags.
  g.NextWindowData.Flags |= AnchorNextWindowDataFlags_HasPos;
  g.NextWindowData.PosVal = pos;
  g.NextWindowData.PosPivotVal = pivot;
  g.NextWindowData.PosCond = cond ? cond : AnchorCond_Always;
}

void ANCHOR::SetNextWindowSize(const wabi::GfVec2f &size, AnchorCond cond)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(
    cond == 0 ||
    ImIsPowerOfTwo(
      cond));  // Make sure the user doesn't attempt to combine multiple condition flags.
  g.NextWindowData.Flags |= AnchorNextWindowDataFlags_HasSize;
  g.NextWindowData.SizeVal = size;
  g.NextWindowData.SizeCond = cond ? cond : AnchorCond_Always;
}

void ANCHOR::SetNextWindowSizeConstraints(const wabi::GfVec2f &size_min,
                                          const wabi::GfVec2f &size_max,
                                          ANCHORSizeCallback custom_callback,
                                          void *custom_callback_user_data)
{
  AnchorContext &g = *G_CTX;
  g.NextWindowData.Flags |= AnchorNextWindowDataFlags_HasSizeConstraint;
  g.NextWindowData.SizeConstraintRect = AnchorBBox(size_min, size_max);
  g.NextWindowData.SizeCallback = custom_callback;
  g.NextWindowData.SizeCallbackUserData = custom_callback_user_data;
}

// Content size = inner scrollable rectangle, padded with WindowPadding.
// SetNextWindowContentSize(wabi::GfVec2f(100,100) + AnchorWindowFlags_AlwaysAutoResize will always
// allow submitting a 100x100 item.
void ANCHOR::SetNextWindowContentSize(const wabi::GfVec2f &size)
{
  AnchorContext &g = *G_CTX;
  g.NextWindowData.Flags |= AnchorNextWindowDataFlags_HasContentSize;
  g.NextWindowData.ContentSizeVal = AnchorFloor(size);
}

void ANCHOR::SetNextWindowScroll(const wabi::GfVec2f &scroll)
{
  AnchorContext &g = *G_CTX;
  g.NextWindowData.Flags |= AnchorNextWindowDataFlags_HasScroll;
  g.NextWindowData.ScrollVal = scroll;
}

void ANCHOR::SetNextWindowCollapsed(bool collapsed, AnchorCond cond)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(
    cond == 0 ||
    ImIsPowerOfTwo(
      cond));  // Make sure the user doesn't attempt to combine multiple condition flags.
  g.NextWindowData.Flags |= AnchorNextWindowDataFlags_HasCollapsed;
  g.NextWindowData.CollapsedVal = collapsed;
  g.NextWindowData.CollapsedCond = cond ? cond : AnchorCond_Always;
}

void ANCHOR::SetNextWindowFocus()
{
  AnchorContext &g = *G_CTX;
  g.NextWindowData.Flags |= AnchorNextWindowDataFlags_HasFocus;
}

void ANCHOR::SetNextWindowBgAlpha(float alpha)
{
  AnchorContext &g = *G_CTX;
  g.NextWindowData.Flags |= AnchorNextWindowDataFlags_HasBgAlpha;
  g.NextWindowData.BgAlphaVal = alpha;
}

AnchorDrawList *ANCHOR::GetWindowDrawList()
{
  AnchorWindow *window = GetCurrentWindow();
  return window->DrawList;
}

AnchorFont *ANCHOR::GetFont()
{
  return G_CTX->Font;
}

float ANCHOR::GetFontSize()
{
  return G_CTX->FontSize;
}

wabi::GfVec2f ANCHOR::GetFontTexUvWhitePixel()
{
  return G_CTX->DrawListSharedData.TexUvWhitePixel;
}

void ANCHOR::SetWindowFontScale(float scale)
{
  ANCHOR_ASSERT(scale > 0.0f);
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = GetCurrentWindow();
  window->FontWindowScale = scale;
  g.FontSize = g.DrawListSharedData.FontSize = window->CalcFontSize();
}

void ANCHOR::ActivateItem(ANCHOR_ID id)
{
  AnchorContext &g = *G_CTX;
  g.NavNextActivateId = id;
}

void ANCHOR::PushFocusScope(ANCHOR_ID id)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  g.FocusScopeStack.push_back(window->DC.NavFocusScopeIdCurrent);
  window->DC.NavFocusScopeIdCurrent = id;
}

void ANCHOR::PopFocusScope()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  ANCHOR_ASSERT(g.FocusScopeStack.Size > 0);  // Too many PopFocusScope() ?
  window->DC.NavFocusScopeIdCurrent = g.FocusScopeStack.back();
  g.FocusScopeStack.pop_back();
}

void ANCHOR::SetKeyboardFocusHere(int offset)
{
  ANCHOR_ASSERT(offset >= -1);  // -1 is allowed but not below
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  g.TabFocusRequestNextWindow = window;
  g.TabFocusRequestNextCounterRegular = window->DC.FocusCounterRegular + 1 + offset;
  g.TabFocusRequestNextCounterTabStop = INT_MAX;
}

void ANCHOR::SetItemDefaultFocus()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  if (!window->Appearing)
    return;
  if (g.NavWindow == window->RootWindowForNav && (g.NavInitRequest || g.NavInitResultId != 0) &&
      g.NavLayer == window->DC.NavLayerCurrent) {
    g.NavInitRequest = false;
    g.NavInitResultId = window->DC.LastItemId;
    g.NavInitResultRectRel = AnchorBBox(window->DC.LastItemRect.Min - window->Pos,
                                        window->DC.LastItemRect.Max - window->Pos);
    NavUpdateAnyRequestFlag();
    if (!IsItemVisible())
      SetScrollHereY();
  }
}

void ANCHOR::SetStateStorage(AnchorStorage *tree)
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  window->DC.StateStorage = tree ? tree : &window->StateStorage;
}

AnchorStorage *ANCHOR::GetStateStorage()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->DC.StateStorage;
}

void ANCHOR::PushID(const char *str_id)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  ANCHOR_ID id = window->GetIDNoKeepAlive(str_id);
  window->IDStack.push_back(id);
}

void ANCHOR::PushID(const char *str_id_begin, const char *str_id_end)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  ANCHOR_ID id = window->GetIDNoKeepAlive(str_id_begin, str_id_end);
  window->IDStack.push_back(id);
}

void ANCHOR::PushID(const void *ptr_id)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  ANCHOR_ID id = window->GetIDNoKeepAlive(ptr_id);
  window->IDStack.push_back(id);
}

void ANCHOR::PushID(int int_id)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  ANCHOR_ID id = window->GetIDNoKeepAlive(int_id);
  window->IDStack.push_back(id);
}

// Push a given id value ignoring the ID stack as a seed.
void ANCHOR::PushOverrideID(ANCHOR_ID id)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  window->IDStack.push_back(id);
}

// Helper to avoid a common series of PushOverrideID -> GetID() -> PopID() call
// (note that when using this pattern, TestEngine's "Stack Tool" will tend to not display the
// intermediate stack level.
//  for that to work we would need to do PushOverrideID() -> ItemAdd() -> PopID() which would alter
//  widget code a little more)
ANCHOR_ID ANCHOR::GetIDWithSeed(const char *str, const char *str_end, ANCHOR_ID seed)
{
  ANCHOR_ID id = AnchorHashStr(str, str_end ? (str_end - str) : 0, seed);
  ANCHOR::KeepAliveID(id);
#ifdef ANCHOR_ENABLE_TEST_ENGINE
  AnchorContext &g = *G_CTX;
  ANCHOR_TEST_ENGINE_ID_INFO2(id, AnchorDataType_String, str, str_end);
#endif
  return id;
}

void ANCHOR::PopID()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  ANCHOR_ASSERT(window->IDStack.Size >
                1);  // Too many PopID(), or could be popping in a wrong/different window?
  window->IDStack.pop_back();
}

ANCHOR_ID ANCHOR::GetID(const char *str_id)
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->GetID(str_id);
}

ANCHOR_ID ANCHOR::GetID(const char *str_id_begin, const char *str_id_end)
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->GetID(str_id_begin, str_id_end);
}

ANCHOR_ID ANCHOR::GetID(const void *ptr_id)
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->GetID(ptr_id);
}

bool ANCHOR::IsRectVisible(const wabi::GfVec2f &size)
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->ClipRect.Overlaps(AnchorBBox(window->DC.CursorPos, window->DC.CursorPos + size));
}

bool ANCHOR::IsRectVisible(const wabi::GfVec2f &rect_min, const wabi::GfVec2f &rect_max)
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->ClipRect.Overlaps(AnchorBBox(rect_min, rect_max));
}

//-----------------------------------------------------------------------------
// [SECTION] ERROR CHECKING
//-----------------------------------------------------------------------------

// Helper function to verify ABI compatibility between caller code and compiled version of ANCHOR.
// Verify that the type sizes are matching between the calling file's compilation unit and
// ANCHOR.cpp's compilation unit If the user has inconsistent compilation settings, ANCHOR
// configuration #define, packing pragma, etc. your user code may see different structures than
// what ANCHOR.cpp sees, which is problematic. We usually require settings to be in ANCHOR_config.h
// to make sure that they are accessible to all compilation units involved with ANCHOR.
bool ANCHOR::DebugCheckVersionAndDataLayout(const char *version,
                                            size_t sz_io,
                                            size_t sz_style,
                                            size_t sz_vec2,
                                            size_t sz_vec4,
                                            size_t sz_vert,
                                            size_t sz_idx)
{
  bool error = false;
  if (strcmp(version, ANCHOR_VERSION) != 0) {
    error = true;
    ANCHOR_ASSERT(strcmp(version, ANCHOR_VERSION) == 0 && "Mismatched version string!");
  }
  if (sz_io != sizeof(AnchorIO)) {
    error = true;
    ANCHOR_ASSERT(sz_io == sizeof(AnchorIO) && "Mismatched struct layout!");
  }
  if (sz_style != sizeof(AnchorStyle)) {
    error = true;
    ANCHOR_ASSERT(sz_style == sizeof(AnchorStyle) && "Mismatched struct layout!");
  }
  if (sz_vec2 != sizeof(wabi::GfVec2f)) {
    error = true;
    ANCHOR_ASSERT(sz_vec2 == sizeof(wabi::GfVec2f) && "Mismatched struct layout!");
  }
  if (sz_vec4 != sizeof(wabi::GfVec4f)) {
    error = true;
    ANCHOR_ASSERT(sz_vec4 == sizeof(wabi::GfVec4f) && "Mismatched struct layout!");
  }
  if (sz_vert != sizeof(AnchorDrawVert)) {
    error = true;
    ANCHOR_ASSERT(sz_vert == sizeof(AnchorDrawVert) && "Mismatched struct layout!");
  }
  if (sz_idx != sizeof(AnchorDrawIdx)) {
    error = true;
    ANCHOR_ASSERT(sz_idx == sizeof(AnchorDrawIdx) && "Mismatched struct layout!");
  }
  return !error;
}

static void ANCHOR::ErrorCheckNewFrameSanityChecks()
{
  AnchorContext &g = *G_CTX;

  // Check user ANCHOR_ASSERT macro
  // (IF YOU GET A WARNING OR COMPILE ERROR HERE: it means your assert macro is incorrectly
  // defined!
  //  If your macro uses multiple statements, it NEEDS to be surrounded by a 'do { ... } while (0)'
  //  block. This is a common C/C++ idiom to allow multiple statements macros to be used in control
  //  flow blocks.)
  // #define ANCHOR_ASSERT(EXPR)   if (SomeCode(EXPR)) SomeMoreCode();                    // Wrong!
  // #define ANCHOR_ASSERT(EXPR)   do { if (SomeCode(EXPR)) SomeMoreCode(); } while (0)   //
  // Correct!
  if (true)
    ANCHOR_ASSERT(1);
  else
    ANCHOR_ASSERT(0);

  // Check user data
  // (We pass an error message in the assert expression to make it visible to programmers who are
  // not using a debugger, as most assert handlers display their argument)
  ANCHOR_ASSERT(g.Initialized);
  ANCHOR_ASSERT((g.IO.DeltaTime > 0.0f || g.FrameCount == 0) && "Need a positive DeltaTime!");
  ANCHOR_ASSERT((g.FrameCount == 0 || g.FrameCountEnded == g.FrameCount) &&
                "Forgot to call Render() or EndFrame() at the end of the previous frame?");
  ANCHOR_ASSERT(g.IO.DisplaySize[0] >= 0.0f && g.IO.DisplaySize[1] >= 0.0f &&
                "Invalid DisplaySize value!");
  ANCHOR_ASSERT(
    g.IO.Fonts->Fonts.Size > 0 &&
    "Font Atlas not built. Did you call io.Fonts->GetTexDataAsRGBA32() / GetTexDataAsAlpha8()?");
  ANCHOR_ASSERT(
    g.IO.Fonts->Fonts[0]->IsLoaded() &&
    "Font Atlas not built. Did you call io.Fonts->GetTexDataAsRGBA32() / GetTexDataAsAlpha8()?");
  ANCHOR_ASSERT(g.Style.CurveTessellationTol > 0.0f && "Invalid style setting!");
  ANCHOR_ASSERT(g.Style.CircleTessellationMaxError > 0.0f && "Invalid style setting!");
  ANCHOR_ASSERT(
    g.Style.Alpha >= 0.0f && g.Style.Alpha <= 1.0f &&
    "Invalid style setting!");  // Allows us to avoid a few clamps in color computations
  ANCHOR_ASSERT(g.Style.WindowMinSize[0] >= 1.0f && g.Style.WindowMinSize[1] >= 1.0f &&
                "Invalid style setting.");
  ANCHOR_ASSERT(g.Style.WindowMenuButtonPosition == AnchorDir_None ||
                g.Style.WindowMenuButtonPosition == AnchorDir_Left ||
                g.Style.WindowMenuButtonPosition == AnchorDir_Right);
  for (int n = 0; n < AnchorKey_COUNT; n++)
    ANCHOR_ASSERT(
      g.IO.KeyMap[n] >= -1 && g.IO.KeyMap[n] < ANCHOR_ARRAYSIZE(g.IO.KeysDown) &&
      "io.KeyMap[] contains an out of bound value (need to be 0..512, or -1 for unmapped key)");

  // Check: required key mapping (we intentionally do NOT check all keys to not pressure user into
  // setting up everything, but Space is required and was only added in 1.60 WIP)
  if (g.IO.ConfigFlags & AnchorConfigFlags_NavEnableKeyboard)
    ANCHOR_ASSERT(g.IO.KeyMap[AnchorKey_Space] != -1 &&
                  "AnchorKey_Space is not mapped, required for keyboard navigation.");

  // Check: the io.ConfigWindowsResizeFromEdges option requires backend to honor mouse cursor
  // changes and set the AnchorBackendFlags_HasMouseCursors flag accordingly.
  if (g.IO.ConfigWindowsResizeFromEdges &&
      !(g.IO.BackendFlags & AnchorBackendFlags_HasMouseCursors))
    g.IO.ConfigWindowsResizeFromEdges = false;
}

static void ANCHOR::ErrorCheckEndFrameSanityChecks()
{
  AnchorContext &g = *G_CTX;

  // Verify that io.KeyXXX fields haven't been tampered with. Key mods should not be modified
  // between NewFrame() and EndFrame() One possible reason leading to this assert is that your
  // backends update inputs _AFTER_ NewFrame(). It is known that when some modal native windows
  // called mid-frame takes focus away, some backends such as GLFW will send key release events
  // mid-frame. This would normally trigger this assertion and lead to sheared inputs. We silently
  // accommodate for this case by ignoring/ the case where all io.KeyXXX modifiers were released
  // (aka key_mod_flags == 0), while still correctly asserting on mid-frame key press events.
  const AnchorKeyModFlags key_mod_flags = GetMergedKeyModFlags();
  ANCHOR_ASSERT((key_mod_flags == 0 || g.IO.KeyMods == key_mod_flags) &&
                "Mismatching io.KeyCtrl/io.KeyShift/io.KeyAlt/io.KeySuper vs io.KeyMods");
  TF_UNUSED(key_mod_flags);

  // Recover from errors
  // ErrorCheckEndFrameRecover();

  // Report when there is a mismatch of Begin/BeginChild vs End/EndChild calls. Important: Remember
  // that the Begin/BeginChild API requires you to always call End/EndChild even if
  // Begin/BeginChild returns false! (this is unfortunately inconsistent with most other Begin*
  // API).
  if (g.CurrentWindowStack.Size != 1) {
    if (g.CurrentWindowStack.Size > 1) {
      ANCHOR_ASSERT_USER_ERROR(g.CurrentWindowStack.Size == 1,
                               "Mismatched Begin/BeginChild vs End/EndChild calls: did you forget "
                               "to call End/EndChild?");
      while (g.CurrentWindowStack.Size > 1)
        End();
    } else {
      ANCHOR_ASSERT_USER_ERROR(g.CurrentWindowStack.Size == 1,
                               "Mismatched Begin/BeginChild vs End/EndChild calls: did you call "
                               "End/EndChild too much?");
    }
  }

  ANCHOR_ASSERT_USER_ERROR(g.GroupStack.Size == 0, "Missing EndGroup call!");
}

// Experimental recovery from incorrect usage of BeginXXX/EndXXX/PushXXX/PopXXX calls.
// Must be called during or before EndFrame().
// This is generally flawed as we are not necessarily End/Popping things in the right order.
// FIXME: Can't recover from inside BeginTabItem/EndTabItem yet.
// FIXME: Can't recover from interleaved BeginTabBar/Begin
void ANCHOR::ErrorCheckEndFrameRecover(AnchorErrorLogCallback log_callback, void *user_data)
{
  // PVS-Studio V1044 is "Loop break conditions do not depend on the number of iterations"
  AnchorContext &g = *G_CTX;
  while (g.CurrentWindowStack.Size > 0) {
    while (g.CurrentTable && (g.CurrentTable->OuterWindow == g.CurrentWindow ||
                              g.CurrentTable->InnerWindow == g.CurrentWindow)) {
      if (log_callback)
        log_callback(user_data,
                     "Recovered from missing EndTable() in '%s'",
                     g.CurrentTable->OuterWindow->Name);
      EndTable();
    }
    AnchorWindow *window = g.CurrentWindow;
    ANCHOR_ASSERT(window != NULL);
    while (g.CurrentTabBar != NULL)  //-V1044
    {
      if (log_callback)
        log_callback(user_data, "Recovered from missing EndTabBar() in '%s'", window->Name);
      EndTabBar();
    }
    while (window->DC.TreeDepth > 0) {
      if (log_callback)
        log_callback(user_data, "Recovered from missing TreePop() in '%s'", window->Name);
      TreePop();
    }
    while (g.GroupStack.Size > window->DC.StackSizesOnBegin.SizeOfGroupStack) {
      if (log_callback)
        log_callback(user_data, "Recovered from missing EndGroup() in '%s'", window->Name);
      EndGroup();
    }
    while (window->IDStack.Size > 1) {
      if (log_callback)
        log_callback(user_data, "Recovered from missing PopID() in '%s'", window->Name);
      PopID();
    }
    while (g.ColorStack.Size > window->DC.StackSizesOnBegin.SizeOfColorStack) {
      if (log_callback)
        log_callback(user_data,
                     "Recovered from missing PopStyleColor() in '%s' for AnchorCol_%s",
                     window->Name,
                     GetStyleColorName(g.ColorStack.back().Col));
      PopStyleColor();
    }
    while (g.StyleVarStack.Size > window->DC.StackSizesOnBegin.SizeOfStyleVarStack) {
      if (log_callback)
        log_callback(user_data, "Recovered from missing PopStyleVar() in '%s'", window->Name);
      PopStyleVar();
    }
    while (g.FocusScopeStack.Size > window->DC.StackSizesOnBegin.SizeOfFocusScopeStack) {
      if (log_callback)
        log_callback(user_data, "Recovered from missing PopFocusScope() in '%s'", window->Name);
      PopFocusScope();
    }
    if (g.CurrentWindowStack.Size == 1) {
      ANCHOR_ASSERT(g.CurrentWindow->IsFallbackWindow);
      break;
    }
    ANCHOR_ASSERT(window == g.CurrentWindow);
    if (window->Flags & AnchorWindowFlags_ChildWindow) {
      if (log_callback)
        log_callback(user_data, "Recovered from missing EndChild() for '%s'", window->Name);
      EndChild();
    } else {
      if (log_callback)
        log_callback(user_data, "Recovered from missing End() for '%s'", window->Name);
      End();
    }
  }
}

// Save current stack sizes for later compare
void AnchorStackSizes::SetToCurrentState()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  SizeOfIDStack = (short)window->IDStack.Size;
  SizeOfColorStack = (short)g.ColorStack.Size;
  SizeOfStyleVarStack = (short)g.StyleVarStack.Size;
  SizeOfFontStack = (short)g.FontStack.Size;
  SizeOfFocusScopeStack = (short)g.FocusScopeStack.Size;
  SizeOfGroupStack = (short)g.GroupStack.Size;
  SizeOfBeginPopupStack = (short)g.BeginPopupStack.Size;
}

// Compare to detect usage errors
void AnchorStackSizes::CompareWithCurrentState()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  TF_UNUSED(window);

  // Window stacks
  // NOT checking: DC.ItemWidth, DC.TextWrapPos (per window) to allow user to conveniently push
  // once and not pop (they are cleared on Begin)
  ANCHOR_ASSERT(SizeOfIDStack == window->IDStack.Size &&
                "PushID/PopID or TreeNode/TreePop Mismatch!");

  // Global stacks
  // For color, style and font stacks there is an incentive to use Push/Begin/Pop/.../End patterns,
  // so we relax our checks a little to allow them.
  ANCHOR_ASSERT(SizeOfGroupStack == g.GroupStack.Size && "BeginGroup/EndGroup Mismatch!");
  ANCHOR_ASSERT(SizeOfBeginPopupStack == g.BeginPopupStack.Size &&
                "BeginPopup/EndPopup or BeginMenu/EndMenu Mismatch!");
  ANCHOR_ASSERT(SizeOfColorStack >= g.ColorStack.Size && "PushStyleColor/PopStyleColor Mismatch!");
  ANCHOR_ASSERT(SizeOfStyleVarStack >= g.StyleVarStack.Size &&
                "PushStyleVar/PopStyleVar Mismatch!");
  ANCHOR_ASSERT(SizeOfFontStack >= g.FontStack.Size && "PushFont/PopFont Mismatch!");
  ANCHOR_ASSERT(SizeOfFocusScopeStack == g.FocusScopeStack.Size &&
                "PushFocusScope/PopFocusScope Mismatch!");
}

//-----------------------------------------------------------------------------
// [SECTION] LAYOUT
//-----------------------------------------------------------------------------
// - ItemSize()
// - ItemAdd()
// - SameLine()
// - GetCursorScreenPos()
// - SetCursorScreenPos()
// - GetCursorPos(), GetCursorPosX(), GetCursorPosY()
// - SetCursorPos(), SetCursorPosX(), SetCursorPosY()
// - GetCursorStartPos()
// - Indent()
// - Unindent()
// - SetNextItemWidth()
// - PushItemWidth()
// - PushMultiItemsWidths()
// - PopItemWidth()
// - CalcItemWidth()
// - CalcItemSize()
// - GetTextLineHeight()
// - GetTextLineHeightWithSpacing()
// - GetFrameHeight()
// - GetFrameHeightWithSpacing()
// - GetContentRegionMax()
// - GetContentRegionMaxAbs() [Internal]
// - GetContentRegionAvail(),
// - GetWindowContentRegionMin(), GetWindowContentRegionMax()
// - GetWindowContentRegionWidth()
// - BeginGroup()
// - EndGroup()
// Also see in ANCHOR_widgets: tab bars, columns.
//-----------------------------------------------------------------------------

// Advance cursor given item size for layout.
// Register minimum needed size so it can extend the bounding box used for auto-fit calculation.
// See comments in ItemAdd() about how/why the size provided to ItemSize() vs ItemAdd() may often
// different.
void ANCHOR::ItemSize(const wabi::GfVec2f &size, float text_baseline_y)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  if (window->SkipItems)
    return;

  // We increase the height in this function to accommodate for baseline offset.
  // In theory we should be offsetting the starting position (window->DC.CursorPos), that will be
  // the topic of a larger refactor, but since ItemSize() is not yet an API that moves the cursor
  // (to handle e.g. wrapping) enlarging the height has the same effect.
  const float offset_to_match_baseline_y =
    (text_baseline_y >= 0) ? AnchorMax(0.0f, window->DC.CurrLineTextBaseOffset - text_baseline_y) :
                             0.0f;
  const float line_height = AnchorMax(window->DC.CurrLineSize[1],
                                      size[1] + offset_to_match_baseline_y);

  // Always align ourselves on pixel boundaries
  // if (g.IO.KeyAlt) window->DrawList->AddRect(window->DC.CursorPos, window->DC.CursorPos +
  // wabi::GfVec2f(size[0], line_height), ANCHOR_COL32(255,0,0,200)); // [DEBUG]
  window->DC.CursorPosPrevLine[0] = window->DC.CursorPos[0] + size[0];
  window->DC.CursorPosPrevLine[1] = window->DC.CursorPos[1];
  window->DC.CursorPos[0] = ANCHOR_FLOOR(window->Pos[0] + window->DC.Indent.x +
                                         window->DC.ColumnsOffset.x);  // Next line
  window->DC.CursorPos[1] = ANCHOR_FLOOR(window->DC.CursorPos[1] + line_height +
                                         g.Style.ItemSpacing[1]);  // Next line
  window->DC.CursorMaxPos[0] = AnchorMax(window->DC.CursorMaxPos[0],
                                         window->DC.CursorPosPrevLine[0]);
  window->DC.CursorMaxPos[1] = AnchorMax(window->DC.CursorMaxPos[1],
                                         window->DC.CursorPos[1] - g.Style.ItemSpacing[1]);
  // if (g.IO.KeyAlt) window->DrawList->AddCircle(window->DC.CursorMaxPos, 3.0f,
  // ANCHOR_COL32(255,0,0,255), 4); // [DEBUG]

  window->DC.PrevLineSize[1] = line_height;
  window->DC.CurrLineSize[1] = 0.0f;
  window->DC.PrevLineTextBaseOffset = AnchorMax(window->DC.CurrLineTextBaseOffset,
                                                text_baseline_y);
  window->DC.CurrLineTextBaseOffset = 0.0f;

  // Horizontal layout mode
  if (window->DC.LayoutType == AnchorLayoutType_Horizontal)
    SameLine();
}

void ANCHOR::ItemSize(const AnchorBBox &bb, float text_baseline_y)
{
  ItemSize(bb.GetSize(), text_baseline_y);
}

// Declare item bounding box for clipping and interaction.
// Note that the size can be different than the one provided to ItemSize(). Typically, widgets that
// spread over available surface declare their minimum size requirement to ItemSize() and provide a
// larger region to ItemAdd() which is used drawing/interaction.
bool ANCHOR::ItemAdd(const AnchorBBox &bb,
                     ANCHOR_ID id,
                     const AnchorBBox *nav_bb_arg,
                     AnchorItemAddFlags flags)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  if (id != 0) {
    // Navigation processing runs prior to clipping early-out
    //  (a) So that NavInitRequest can be honored, for newly opened windows to select a default
    //  widget (b) So that we can scroll up/down past clipped items. This adds a small O(N) cost to
    //  regular navigation requests
    //      unfortunately, but it is still limited to one window. It may not scale very well for
    //      windows with ten of thousands of item, but at least NavMoveRequest is only set on user
    //      interaction, aka maximum once a frame. We could early out with "if (is_clipped &&
    //      !g.NavInitRequest) return false;" but when we wouldn't be able to reach unclipped
    //      widgets. This would work if user had explicit scrolling control (e.g. mapped on a
    //      stick).
    // We intentionally don't check if g.NavWindow != NULL because g.NavAnyRequest should only be
    // set when it is non null. If we crash on a NULL g.NavWindow we need to fix the bug elsewhere.
    window->DC.NavLayersActiveMaskNext |= (1 << window->DC.NavLayerCurrent);
    if (g.NavId == id || g.NavAnyRequest)
      if (g.NavWindow->RootWindowForNav == window->RootWindowForNav)
        if (window == g.NavWindow ||
            ((window->Flags | g.NavWindow->Flags) & AnchorWindowFlags_NavFlattened))
          NavProcessItem(window, nav_bb_arg ? *nav_bb_arg : bb, id);

          // [DEBUG] Item Picker tool, when enabling the "extended" version we perform the check in
          // ItemAdd()
#ifdef ANCHOR_DEBUG_TOOL_ITEM_PICKER_EX
    if (id == g.DebugItemPickerBreakId) {
      IM_DEBUG_BREAK();
      g.DebugItemPickerBreakId = 0;
    }
#endif
  }

  // Equivalent to calling SetLastItemData()
  window->DC.LastItemId = id;
  window->DC.LastItemRect = bb;
  window->DC.LastItemStatusFlags = AnchorItemStatusFlags_None;
  g.NextItemData.Flags = AnchorNextItemDataFlags_None;

#ifdef ANCHOR_ENABLE_TEST_ENGINE
  if (id != 0)
    ANCHOR_TEST_ENGINE_ITEM_ADD(nav_bb_arg ? *nav_bb_arg : bb, id);
#endif

  // Clipping test
  const bool is_clipped = IsClippedEx(bb, id, false);
  if (is_clipped)
    return false;
  // if (g.IO.KeyAlt) window->DrawList->AddRect(bb.Min, bb.Max, ANCHOR_COL32(255,255,0,120)); //
  // [DEBUG]

  // Tab stop handling (previously was using internal ItemFocusable() api)
  // FIXME-NAV: We would now want to move this above the clipping test, but this would require
  // being able to scroll and currently this would mean an extra frame. (#4079, #343)
  if (flags & AnchorItemAddFlags_Focusable)
    ItemFocusable(window, id);

  // We need to calculate this now to take account of the current clipping rectangle (as items like
  // Selectable may change them)
  if (IsMouseHoveringRect(bb.Min, bb.Max))
    window->DC.LastItemStatusFlags |= AnchorItemStatusFlags_HoveredRect;
  return true;
}

// Gets back to previous line and continue with horizontal layout
//      offset_from_start_x == 0 : follow right after previous item
//      offset_from_start_x != 0 : align to specified x position (relative to window/group left)
//      spacing_w < 0            : use default spacing if pos_x == 0, no spacing if pos_x != 0
//      spacing_w >= 0           : enforce spacing amount
void ANCHOR::SameLine(float offset_from_start_x, float spacing_w)
{
  AnchorWindow *window = GetCurrentWindow();
  if (window->SkipItems)
    return;

  AnchorContext &g = *G_CTX;
  if (offset_from_start_x != 0.0f) {
    if (spacing_w < 0.0f)
      spacing_w = 0.0f;
    window->DC.CursorPos[0] = window->Pos[0] - window->Scroll[0] + offset_from_start_x +
                              spacing_w + window->DC.GroupOffset.x + window->DC.ColumnsOffset.x;
    window->DC.CursorPos[1] = window->DC.CursorPosPrevLine[1];
  } else {
    if (spacing_w < 0.0f)
      spacing_w = g.Style.ItemSpacing[0];
    window->DC.CursorPos[0] = window->DC.CursorPosPrevLine[0] + spacing_w;
    window->DC.CursorPos[1] = window->DC.CursorPosPrevLine[1];
  }
  window->DC.CurrLineSize = window->DC.PrevLineSize;
  window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
}

wabi::GfVec2f ANCHOR::GetCursorScreenPos()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->DC.CursorPos;
}

void ANCHOR::SetCursorScreenPos(const wabi::GfVec2f &pos)
{
  AnchorWindow *window = GetCurrentWindow();
  window->DC.CursorPos = pos;
  window->DC.CursorMaxPos = AnchorMax(window->DC.CursorMaxPos, window->DC.CursorPos);
}

// User generally sees positions in window coordinates. Internally we store CursorPos in absolute
// screen coordinates because it is more convenient. Conversion happens as we pass the value to
// user, but it makes our naming convention confusing because GetCursorPos() == (DC.CursorPos -
// window.Pos). May want to rename 'DC.CursorPos'.
wabi::GfVec2f ANCHOR::GetCursorPos()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->DC.CursorPos - window->Pos + window->Scroll;
}

float ANCHOR::GetCursorPosX()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->DC.CursorPos[0] - window->Pos[0] + window->Scroll[0];
}

float ANCHOR::GetCursorPosY()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->DC.CursorPos[1] - window->Pos[1] + window->Scroll[1];
}

void ANCHOR::SetCursorPos(const wabi::GfVec2f &local_pos)
{
  AnchorWindow *window = GetCurrentWindow();
  window->DC.CursorPos = window->Pos - window->Scroll + local_pos;
  window->DC.CursorMaxPos = AnchorMax(window->DC.CursorMaxPos, window->DC.CursorPos);
}

void ANCHOR::SetCursorPosX(float x)
{
  AnchorWindow *window = GetCurrentWindow();
  window->DC.CursorPos[0] = window->Pos[0] - window->Scroll[0] + x;
  window->DC.CursorMaxPos[0] = AnchorMax(window->DC.CursorMaxPos[0], window->DC.CursorPos[0]);
}

void ANCHOR::SetCursorPosY(float y)
{
  AnchorWindow *window = GetCurrentWindow();
  window->DC.CursorPos[1] = window->Pos[1] - window->Scroll[1] + y;
  window->DC.CursorMaxPos[1] = AnchorMax(window->DC.CursorMaxPos[1], window->DC.CursorPos[1]);
}

wabi::GfVec2f ANCHOR::GetCursorStartPos()
{
  AnchorWindow *window = GetCurrentWindowRead();
  return window->DC.CursorStartPos - window->Pos;
}

void ANCHOR::Indent(float indent_w)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = GetCurrentWindow();
  window->DC.Indent.x += (indent_w != 0.0f) ? indent_w : g.Style.IndentSpacing;
  window->DC.CursorPos[0] = window->Pos[0] + window->DC.Indent.x + window->DC.ColumnsOffset.x;
}

void ANCHOR::Unindent(float indent_w)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = GetCurrentWindow();
  window->DC.Indent.x -= (indent_w != 0.0f) ? indent_w : g.Style.IndentSpacing;
  window->DC.CursorPos[0] = window->Pos[0] + window->DC.Indent.x + window->DC.ColumnsOffset.x;
}

// Affect large frame+labels widgets only.
void ANCHOR::SetNextItemWidth(float item_width)
{
  AnchorContext &g = *G_CTX;
  g.NextItemData.Flags |= AnchorNextItemDataFlags_HasWidth;
  g.NextItemData.Width = item_width;
}

// FIXME: Remove the == 0.0f behavior?
void ANCHOR::PushItemWidth(float item_width)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  window->DC.ItemWidthStack.push_back(window->DC.ItemWidth);  // Backup current width
  window->DC.ItemWidth = (item_width == 0.0f ? window->ItemWidthDefault : item_width);
  g.NextItemData.Flags &= ~AnchorNextItemDataFlags_HasWidth;
}

void ANCHOR::PushMultiItemsWidths(int components, float w_full)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  const AnchorStyle &style = g.Style;
  const float w_item_one = AnchorMax(
    1.0f,
    ANCHOR_FLOOR((w_full - (style.ItemInnerSpacing[0]) * (components - 1)) / (float)components));
  const float w_item_last = AnchorMax(
    1.0f,
    ANCHOR_FLOOR(w_full - (w_item_one + style.ItemInnerSpacing[0]) * (components - 1)));
  window->DC.ItemWidthStack.push_back(window->DC.ItemWidth);  // Backup current width
  window->DC.ItemWidthStack.push_back(w_item_last);
  for (int i = 0; i < components - 2; i++)
    window->DC.ItemWidthStack.push_back(w_item_one);
  window->DC.ItemWidth = (components == 1) ? w_item_last : w_item_one;
  g.NextItemData.Flags &= ~AnchorNextItemDataFlags_HasWidth;
}

void ANCHOR::PopItemWidth()
{
  AnchorWindow *window = GetCurrentWindow();
  window->DC.ItemWidth = window->DC.ItemWidthStack.back();
  window->DC.ItemWidthStack.pop_back();
}

// Calculate default item width given value passed to PushItemWidth() or SetNextItemWidth().
// The SetNextItemWidth() data is generally cleared/consumed by ItemAdd() or
// NextItemData.ClearFlags()
float ANCHOR::CalcItemWidth()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  float w;
  if (g.NextItemData.Flags & AnchorNextItemDataFlags_HasWidth)
    w = g.NextItemData.Width;
  else
    w = window->DC.ItemWidth;
  if (w < 0.0f) {
    float region_max_x = GetContentRegionMaxAbs()[0];
    w = AnchorMax(1.0f, region_max_x - window->DC.CursorPos[0] + w);
  }
  w = ANCHOR_FLOOR(w);
  return w;
}

// [Internal] Calculate full item size given user provided 'size' parameter and default
// width/height. Default width is often == CalcItemWidth(). Those two functions CalcItemWidth vs
// CalcItemSize are awkwardly named because they are not fully symmetrical. Note that only
// CalcItemWidth() is publicly exposed. The 4.0f here may be changed to match CalcItemWidth()
// and/or BeginChild() (right now we have a mismatch which is harmless but undesirable)
wabi::GfVec2f ANCHOR::CalcItemSize(wabi::GfVec2f size, float default_w, float default_h)
{
  AnchorWindow *window = G_CTX->CurrentWindow;

  wabi::GfVec2f region_max;
  if (size[0] < 0.0f || size[1] < 0.0f)
    region_max = GetContentRegionMaxAbs();

  if (size[0] == 0.0f)
    size[0] = default_w;
  else if (size[0] < 0.0f)
    size[0] = AnchorMax(4.0f, region_max[0] - window->DC.CursorPos[0] + size[0]);

  if (size[1] == 0.0f)
    size[1] = default_h;
  else if (size[1] < 0.0f)
    size[1] = AnchorMax(4.0f, region_max[1] - window->DC.CursorPos[1] + size[1]);

  return size;
}

float ANCHOR::GetTextLineHeight()
{
  AnchorContext &g = *G_CTX;
  return g.FontSize;
}

float ANCHOR::GetTextLineHeightWithSpacing()
{
  AnchorContext &g = *G_CTX;
  return g.FontSize + g.Style.ItemSpacing[1];
}

float ANCHOR::GetFrameHeight()
{
  AnchorContext &g = *G_CTX;
  return g.FontSize + g.Style.FramePadding[1] * 2.0f;
}

float ANCHOR::GetFrameHeightWithSpacing()
{
  AnchorContext &g = *G_CTX;
  return g.FontSize + g.Style.FramePadding[1] * 2.0f + g.Style.ItemSpacing[1];
}

// FIXME: All the Contents Region function are messy or misleading. WE WILL AIM TO OBSOLETE ALL OF
// THEM WITH A NEW "WORK RECT" API. Thanks for your patience!

// FIXME: This is in window space (not screen space!).
wabi::GfVec2f ANCHOR::GetContentRegionMax()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  wabi::GfVec2f mx = window->ContentRegionRect.Max - window->Pos;
  if (window->DC.CurrentColumns || g.CurrentTable)
    mx[0] = window->WorkRect.Max[0] - window->Pos[0];
  return mx;
}

// [Internal] Absolute coordinate. Saner. This is not exposed until we finishing refactoring work
// rect features.
wabi::GfVec2f ANCHOR::GetContentRegionMaxAbs()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  wabi::GfVec2f mx = window->ContentRegionRect.Max;
  if (window->DC.CurrentColumns || g.CurrentTable)
    mx[0] = window->WorkRect.Max[0];
  return mx;
}

wabi::GfVec2f ANCHOR::GetContentRegionAvail()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return GetContentRegionMaxAbs() - window->DC.CursorPos;
}

// In window space (not screen space!)
wabi::GfVec2f ANCHOR::GetWindowContentRegionMin()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->ContentRegionRect.Min - window->Pos;
}

wabi::GfVec2f ANCHOR::GetWindowContentRegionMax()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->ContentRegionRect.Max - window->Pos;
}

float ANCHOR::GetWindowContentRegionWidth()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->ContentRegionRect.GetWidth();
}

// Lock horizontal starting position + capture group bounding box into one "item" (so you can use
// IsItemHovered() or layout primitives such as SameLine() on whole group, etc.) Groups are
// currently a mishmash of functionalities which should perhaps be clarified and separated.
void ANCHOR::BeginGroup()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  g.GroupStack.resize(g.GroupStack.Size + 1);
  AnchorGroupData &group_data = g.GroupStack.back();
  group_data.WindowID = window->ID;
  group_data.BackupCursorPos = window->DC.CursorPos;
  group_data.BackupCursorMaxPos = window->DC.CursorMaxPos;
  group_data.BackupIndent = window->DC.Indent;
  group_data.BackupGroupOffset = window->DC.GroupOffset;
  group_data.BackupCurrLineSize = window->DC.CurrLineSize;
  group_data.BackupCurrLineTextBaseOffset = window->DC.CurrLineTextBaseOffset;
  group_data.BackupActiveIdIsAlive = g.ActiveIdIsAlive;
  group_data.BackupHoveredIdIsAlive = g.HoveredId != 0;
  group_data.BackupActiveIdPreviousFrameIsAlive = g.ActiveIdPreviousFrameIsAlive;
  group_data.EmitItem = true;

  window->DC.GroupOffset.x = window->DC.CursorPos[0] - window->Pos[0] - window->DC.ColumnsOffset.x;
  window->DC.Indent = window->DC.GroupOffset;
  window->DC.CursorMaxPos = window->DC.CursorPos;
  window->DC.CurrLineSize = wabi::GfVec2f(0.0f, 0.0f);
  if (g.LogEnabled)
    g.LogLinePosY = -FLT_MAX;  // To enforce a carriage return
}

void ANCHOR::EndGroup()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  ANCHOR_ASSERT(g.GroupStack.Size > 0);  // Mismatched BeginGroup()/EndGroup() calls

  AnchorGroupData &group_data = g.GroupStack.back();
  ANCHOR_ASSERT(group_data.WindowID == window->ID);  // EndGroup() in wrong window?

  AnchorBBox group_bb(group_data.BackupCursorPos,
                      AnchorMax(window->DC.CursorMaxPos, group_data.BackupCursorPos));

  window->DC.CursorPos = group_data.BackupCursorPos;
  window->DC.CursorMaxPos = AnchorMax(group_data.BackupCursorMaxPos, window->DC.CursorMaxPos);
  window->DC.Indent = group_data.BackupIndent;
  window->DC.GroupOffset = group_data.BackupGroupOffset;
  window->DC.CurrLineSize = group_data.BackupCurrLineSize;
  window->DC.CurrLineTextBaseOffset = group_data.BackupCurrLineTextBaseOffset;
  if (g.LogEnabled)
    g.LogLinePosY = -FLT_MAX;  // To enforce a carriage return

  if (!group_data.EmitItem) {
    g.GroupStack.pop_back();
    return;
  }

  window->DC.CurrLineTextBaseOffset = AnchorMax(
    window->DC.PrevLineTextBaseOffset,
    group_data.BackupCurrLineTextBaseOffset);  // FIXME: Incorrect, we should grab the base
                                               // offset from the *first line* of the group but
                                               // it is hard to obtain now.
  ItemSize(group_bb.GetSize());
  ItemAdd(group_bb, 0);

  // If the current ActiveId was declared within the boundary of our group, we copy it to
  // LastItemId so IsItemActive(), IsItemDeactivated() etc. will be functional on the entire group.
  // It would be be neater if we replaced window.DC.LastItemId by e.g. 'bool LastItemIsActive', but
  // would put a little more burden on individual widgets. Also if you grep for LastItemId you'll
  // notice it is only used in that context. (The two tests not the same because ActiveIdIsAlive is
  // an ID itself, in order to be able to handle ActiveId being overwritten during the frame.)
  const bool group_contains_curr_active_id = (group_data.BackupActiveIdIsAlive != g.ActiveId) &&
                                             (g.ActiveIdIsAlive == g.ActiveId) && g.ActiveId;
  const bool group_contains_prev_active_id = (group_data.BackupActiveIdPreviousFrameIsAlive ==
                                              false) &&
                                             (g.ActiveIdPreviousFrameIsAlive == true);
  if (group_contains_curr_active_id)
    window->DC.LastItemId = g.ActiveId;
  else if (group_contains_prev_active_id)
    window->DC.LastItemId = g.ActiveIdPreviousFrame;
  window->DC.LastItemRect = group_bb;

  // Forward Hovered flag
  const bool group_contains_curr_hovered_id = (group_data.BackupHoveredIdIsAlive == false) &&
                                              g.HoveredId != 0;
  if (group_contains_curr_hovered_id)
    window->DC.LastItemStatusFlags |= AnchorItemStatusFlags_HoveredWindow;

  // Forward Edited flag
  if (group_contains_curr_active_id && g.ActiveIdHasBeenEditedThisFrame)
    window->DC.LastItemStatusFlags |= AnchorItemStatusFlags_Edited;

  // Forward Deactivated flag
  window->DC.LastItemStatusFlags |= AnchorItemStatusFlags_HasDeactivated;
  if (group_contains_prev_active_id && g.ActiveId != g.ActiveIdPreviousFrame)
    window->DC.LastItemStatusFlags |= AnchorItemStatusFlags_Deactivated;

  g.GroupStack.pop_back();
  // window->DrawList->AddRect(group_bb.Min, group_bb.Max, ANCHOR_COL32(255,0,255,255));   //
  // [Debug]
}

//-----------------------------------------------------------------------------
// [SECTION] SCROLLING
//-----------------------------------------------------------------------------

// Helper to snap on edges when aiming at an item very close to the edge,
// So the difference between WindowPadding and ItemSpacing will be in the visible area after
// scrolling. When we refactor the scrolling API this may be configurable with a flag? Note that
// the effect for this won't be visible on X axis with default Style settings as WindowPadding[0]
// == ItemSpacing[0] by default.
static float CalcScrollEdgeSnap(float target,
                                float snap_min,
                                float snap_max,
                                float snap_threshold,
                                float center_ratio)
{
  if (target <= snap_min + snap_threshold)
    return AnchorLerp(snap_min, target, center_ratio);
  if (target >= snap_max - snap_threshold)
    return AnchorLerp(target, snap_max, center_ratio);
  return target;
}

static wabi::GfVec2f CalcNextScrollFromScrollTargetAndClamp(AnchorWindow *window)
{
  wabi::GfVec2f scroll = window->Scroll;
  if (window->ScrollTarget[0] < FLT_MAX) {
    float decoration_total_width = window->ScrollbarSizes[0];
    float center_x_ratio = window->ScrollTargetCenterRatio[0];
    float scroll_target_x = window->ScrollTarget[0];
    if (window->ScrollTargetEdgeSnapDist[0] > 0.0f) {
      float snap_x_min = 0.0f;
      float snap_x_max = window->ScrollMax[0] + window->SizeFull[0] - decoration_total_width;
      scroll_target_x = CalcScrollEdgeSnap(scroll_target_x,
                                           snap_x_min,
                                           snap_x_max,
                                           window->ScrollTargetEdgeSnapDist[0],
                                           center_x_ratio);
    }
    scroll[0] = scroll_target_x - center_x_ratio * (window->SizeFull[0] - decoration_total_width);
  }
  if (window->ScrollTarget[1] < FLT_MAX) {
    float decoration_total_height = window->TitleBarHeight() + window->MenuBarHeight() +
                                    window->ScrollbarSizes[1];
    float center_y_ratio = window->ScrollTargetCenterRatio[1];
    float scroll_target_y = window->ScrollTarget[1];
    if (window->ScrollTargetEdgeSnapDist[1] > 0.0f) {
      float snap_y_min = 0.0f;
      float snap_y_max = window->ScrollMax[1] + window->SizeFull[1] - decoration_total_height;
      scroll_target_y = CalcScrollEdgeSnap(scroll_target_y,
                                           snap_y_min,
                                           snap_y_max,
                                           window->ScrollTargetEdgeSnapDist[1],
                                           center_y_ratio);
    }
    scroll[1] = scroll_target_y - center_y_ratio * (window->SizeFull[1] - decoration_total_height);
  }
  scroll[0] = ANCHOR_FLOOR(AnchorMax(scroll[0], 0.0f));
  scroll[1] = ANCHOR_FLOOR(AnchorMax(scroll[1], 0.0f));
  if (!window->Collapsed && !window->SkipItems) {
    scroll[0] = AnchorMin(scroll[0], window->ScrollMax[0]);
    scroll[1] = AnchorMin(scroll[1], window->ScrollMax[1]);
  }
  return scroll;
}

// Scroll to keep newly navigated item fully into view
wabi::GfVec2f ANCHOR::ScrollToBringRectIntoView(AnchorWindow *window, const AnchorBBox &item_rect)
{
  AnchorContext &g = *G_CTX;
  AnchorBBox window_rect(window->InnerRect.Min - wabi::GfVec2f(1, 1),
                         window->InnerRect.Max + wabi::GfVec2f(1, 1));
  // GetForegroundDrawList(window)->AddRect(window_rect.Min, window_rect.Max, ANCHOR_COL32_WHITE);
  // // [DEBUG]

  wabi::GfVec2f delta_scroll;
  if (!window_rect.Contains(item_rect)) {
    if (window->ScrollbarX && item_rect.Min[0] < window_rect.Min[0])
      SetScrollFromPosX(window, item_rect.Min[0] - window->Pos[0] - g.Style.ItemSpacing[0], 0.0f);
    else if (window->ScrollbarX && item_rect.Max[0] >= window_rect.Max[0])
      SetScrollFromPosX(window, item_rect.Max[0] - window->Pos[0] + g.Style.ItemSpacing[0], 1.0f);
    if (item_rect.Min[1] < window_rect.Min[1])
      SetScrollFromPosY(window, item_rect.Min[1] - window->Pos[1] - g.Style.ItemSpacing[1], 0.0f);
    else if (item_rect.Max[1] >= window_rect.Max[1])
      SetScrollFromPosY(window, item_rect.Max[1] - window->Pos[1] + g.Style.ItemSpacing[1], 1.0f);

    wabi::GfVec2f next_scroll = CalcNextScrollFromScrollTargetAndClamp(window);
    delta_scroll = next_scroll - window->Scroll;
  }

  // Also scroll parent window to keep us into view if necessary
  if (window->Flags & AnchorWindowFlags_ChildWindow)
    delta_scroll += ScrollToBringRectIntoView(
      window->ParentWindow,
      AnchorBBox(item_rect.Min - delta_scroll, item_rect.Max - delta_scroll));

  return delta_scroll;
}

float ANCHOR::GetScrollX()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->Scroll[0];
}

float ANCHOR::GetScrollY()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->Scroll[1];
}

float ANCHOR::GetScrollMaxX()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->ScrollMax[0];
}

float ANCHOR::GetScrollMaxY()
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  return window->ScrollMax[1];
}

void ANCHOR::SetScrollX(AnchorWindow *window, float scroll_x)
{
  window->ScrollTarget[0] = scroll_x;
  window->ScrollTargetCenterRatio[0] = 0.0f;
  window->ScrollTargetEdgeSnapDist[0] = 0.0f;
}

void ANCHOR::SetScrollY(AnchorWindow *window, float scroll_y)
{
  window->ScrollTarget[1] = scroll_y;
  window->ScrollTargetCenterRatio[1] = 0.0f;
  window->ScrollTargetEdgeSnapDist[1] = 0.0f;
}

void ANCHOR::SetScrollX(float scroll_x)
{
  AnchorContext &g = *G_CTX;
  SetScrollX(g.CurrentWindow, scroll_x);
}

void ANCHOR::SetScrollY(float scroll_y)
{
  AnchorContext &g = *G_CTX;
  SetScrollY(g.CurrentWindow, scroll_y);
}

// Note that a local position will vary depending on initial scroll value,
// This is a little bit confusing so bear with us:
//  - local_pos = (absolution_pos - window->Pos)
//  - So local_x/local_y are 0.0f for a position at the upper-left corner of a window,
//    and generally local_x/local_y are >(padding+decoration) && <(size-padding-decoration) when in
//    the visible area.
//  - They mostly exists because of legacy API.
// Following the rules above, when trying to work with scrolling code, consider that:
//  - SetScrollFromPosY(0.0f) == SetScrollY(0.0f + scroll[1]) == has no effect!
//  - SetScrollFromPosY(-scroll[1]) == SetScrollY(-scroll[1] + scroll[1]) == SetScrollY(0.0f) ==
//  reset scroll. Of course writing SetScrollY(0.0f) directly then makes more sense
// We store a target position so centering and clamping can occur on the next frame when we are
// guaranteed to have a known window size
void ANCHOR::SetScrollFromPosX(AnchorWindow *window, float local_x, float center_x_ratio)
{
  ANCHOR_ASSERT(center_x_ratio >= 0.0f && center_x_ratio <= 1.0f);
  window->ScrollTarget[0] = ANCHOR_FLOOR(
    local_x + window->Scroll[0]);  // Convert local position to scroll offset
  window->ScrollTargetCenterRatio[0] = center_x_ratio;
  window->ScrollTargetEdgeSnapDist[0] = 0.0f;
}

void ANCHOR::SetScrollFromPosY(AnchorWindow *window, float local_y, float center_y_ratio)
{
  ANCHOR_ASSERT(center_y_ratio >= 0.0f && center_y_ratio <= 1.0f);
  const float decoration_up_height =
    window->TitleBarHeight() +
    window->MenuBarHeight();  // FIXME: Would be nice to have a more standardized
                              // access to our scrollable/client rect;
  local_y -= decoration_up_height;
  window->ScrollTarget[1] = ANCHOR_FLOOR(
    local_y + window->Scroll[1]);  // Convert local position to scroll offset
  window->ScrollTargetCenterRatio[1] = center_y_ratio;
  window->ScrollTargetEdgeSnapDist[1] = 0.0f;
}

void ANCHOR::SetScrollFromPosX(float local_x, float center_x_ratio)
{
  AnchorContext &g = *G_CTX;
  SetScrollFromPosX(g.CurrentWindow, local_x, center_x_ratio);
}

void ANCHOR::SetScrollFromPosY(float local_y, float center_y_ratio)
{
  AnchorContext &g = *G_CTX;
  SetScrollFromPosY(g.CurrentWindow, local_y, center_y_ratio);
}

// center_x_ratio: 0.0f left of last item, 0.5f horizontal center of last item, 1.0f right of last
// item.
void ANCHOR::SetScrollHereX(float center_x_ratio)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  float spacing_x = AnchorMax(window->WindowPadding[0], g.Style.ItemSpacing[0]);
  float target_pos_x = AnchorLerp(window->DC.LastItemRect.Min[0] - spacing_x,
                                  window->DC.LastItemRect.Max[0] + spacing_x,
                                  center_x_ratio);
  SetScrollFromPosX(window,
                    target_pos_x - window->Pos[0],
                    center_x_ratio);  // Convert from absolute to local pos

  // Tweak: snap on edges when aiming at an item very close to the edge
  window->ScrollTargetEdgeSnapDist[0] = AnchorMax(0.0f, window->WindowPadding[0] - spacing_x);
}

// center_y_ratio: 0.0f top of last item, 0.5f vertical center of last item, 1.0f bottom of last
// item.
void ANCHOR::SetScrollHereY(float center_y_ratio)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  float spacing_y = AnchorMax(window->WindowPadding[1], g.Style.ItemSpacing[1]);
  float target_pos_y = AnchorLerp(window->DC.CursorPosPrevLine[1] - spacing_y,
                                  window->DC.CursorPosPrevLine[1] + window->DC.PrevLineSize[1] +
                                    spacing_y,
                                  center_y_ratio);
  SetScrollFromPosY(window,
                    target_pos_y - window->Pos[1],
                    center_y_ratio);  // Convert from absolute to local pos

  // Tweak: snap on edges when aiming at an item very close to the edge
  window->ScrollTargetEdgeSnapDist[1] = AnchorMax(0.0f, window->WindowPadding[1] - spacing_y);
}

//-----------------------------------------------------------------------------
// [SECTION] TOOLTIPS
//-----------------------------------------------------------------------------

void ANCHOR::BeginTooltip()
{
  BeginTooltipEx(AnchorWindowFlags_None, AnchorTooltipFlags_None);
}

void ANCHOR::BeginTooltipEx(AnchorWindowFlags extra_flags, AnchorTooltipFlags tooltip_flags)
{
  AnchorContext &g = *G_CTX;

  if (g.DragDropWithinSource || g.DragDropWithinTarget) {
    // The default tooltip position is a little offset to give space to see the context menu (it's
    // also clamped within the current viewport/monitor) In the context of a dragging tooltip we
    // try to reduce that offset and we enforce following the cursor. Whatever we do we want to
    // call SetNextWindowPos() to enforce a tooltip position and disable clipping the tooltip
    // without our display area, like regular tooltip do.
    // wabi::GfVec2f tooltip_pos = g.IO.MousePos - g.ActiveIdClickOffset - g.Style.WindowPadding;
    wabi::GfVec2f tooltip_pos = g.IO.MousePos +
                          wabi::GfVec2f(16 * g.Style.MouseCursorScale, 8 * g.Style.MouseCursorScale);
    SetNextWindowPos(tooltip_pos);
    SetNextWindowBgAlpha(g.Style.Colors[AnchorCol_PopupBg][3] * 0.60f);
    // PushStyleVar(AnchorStyleVar_Alpha, g.Style.Alpha * 0.60f); // This would be nice but e.g
    // ColorButton with checkboard has issue with transparent colors :(
    tooltip_flags |= AnchorTooltipFlags_OverridePreviousTooltip;
  }

  char window_name[16];
  AnchorFormatString(window_name,
                     ANCHOR_ARRAYSIZE(window_name),
                     "##Tooltip_%02d",
                     g.TooltipOverrideCount);
  if (tooltip_flags & AnchorTooltipFlags_OverridePreviousTooltip)
    if (AnchorWindow *window = FindWindowByName(window_name))
      if (window->Active) {
        // Hide previous tooltip from being displayed. We can't easily "reset" the content of a
        // window so we create a new one.
        window->Hidden = true;
        window->HiddenFramesCanSkipItems = 1;  // FIXME: This may not be necessary?
        AnchorFormatString(window_name,
                           ANCHOR_ARRAYSIZE(window_name),
                           "##Tooltip_%02d",
                           ++g.TooltipOverrideCount);
      }
  AnchorWindowFlags flags = AnchorWindowFlags_Tooltip | AnchorWindowFlags_NoInputs |
                            AnchorWindowFlags_NoTitleBar | AnchorWindowFlags_NoMove |
                            AnchorWindowFlags_NoResize | AnchorWindowFlags_NoSavedSettings |
                            AnchorWindowFlags_AlwaysAutoResize;
  Begin(window_name, NULL, flags | extra_flags);
}

void ANCHOR::EndTooltip()
{
  ANCHOR_ASSERT(GetCurrentWindowRead()->Flags &
                AnchorWindowFlags_Tooltip);  // Mismatched BeginTooltip()/EndTooltip() calls
  End();
}

void ANCHOR::SetTooltipV(const char *fmt, va_list args)
{
  BeginTooltipEx(0, AnchorTooltipFlags_OverridePreviousTooltip);
  TextV(fmt, args);
  EndTooltip();
}

void ANCHOR::SetTooltip(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  SetTooltipV(fmt, args);
  va_end(args);
}

//-----------------------------------------------------------------------------
// [SECTION] POPUPS
//-----------------------------------------------------------------------------

// Supported flags: AnchorPopupFlags_AnyPopupId, AnchorPopupFlags_AnyPopupLevel
bool ANCHOR::IsPopupOpen(ANCHOR_ID id, AnchorPopupFlags popup_flags)
{
  AnchorContext &g = *G_CTX;
  if (popup_flags & AnchorPopupFlags_AnyPopupId) {
    // Return true if any popup is open at the current BeginPopup() level of the popup stack
    // This may be used to e.g. test for another popups already opened to handle popups priorities
    // at the same level.
    ANCHOR_ASSERT(id == 0);
    if (popup_flags & AnchorPopupFlags_AnyPopupLevel)
      return g.OpenPopupStack.Size > 0;
    else
      return g.OpenPopupStack.Size > g.BeginPopupStack.Size;
  } else {
    if (popup_flags & AnchorPopupFlags_AnyPopupLevel) {
      // Return true if the popup is open anywhere in the popup stack
      for (int n = 0; n < g.OpenPopupStack.Size; n++)
        if (g.OpenPopupStack[n].PopupId == id)
          return true;
      return false;
    } else {
      // Return true if the popup is open at the current BeginPopup() level of the popup stack
      // (this is the most-common query)
      return g.OpenPopupStack.Size > g.BeginPopupStack.Size &&
             g.OpenPopupStack[g.BeginPopupStack.Size].PopupId == id;
    }
  }
}

bool ANCHOR::IsPopupOpen(const char *str_id, AnchorPopupFlags popup_flags)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ID id = (popup_flags & AnchorPopupFlags_AnyPopupId) ? 0 : g.CurrentWindow->GetID(str_id);
  if ((popup_flags & AnchorPopupFlags_AnyPopupLevel) && id != 0)
    ANCHOR_ASSERT(
      0 &&
      "Cannot use IsPopupOpen() with a string id and AnchorPopupFlags_AnyPopupLevel.");  // But
                                                                                         // non-string
                                                                                         // version
                                                                                         // is
                                                                                         // legal
                                                                                         // and
                                                                                         // used
                                                                                         // internally
  return IsPopupOpen(id, popup_flags);
}

AnchorWindow *ANCHOR::GetTopMostPopupModal()
{
  AnchorContext &g = *G_CTX;
  for (int n = g.OpenPopupStack.Size - 1; n >= 0; n--)
    if (AnchorWindow *popup = g.OpenPopupStack.Data[n].Window)
      if (popup->Flags & AnchorWindowFlags_Modal)
        return popup;
  return NULL;
}

void ANCHOR::OpenPopup(const char *str_id, AnchorPopupFlags popup_flags)
{
  AnchorContext &g = *G_CTX;
  OpenPopupEx(g.CurrentWindow->GetID(str_id), popup_flags);
}

void ANCHOR::OpenPopup(ANCHOR_ID id, AnchorPopupFlags popup_flags)
{
  OpenPopupEx(id, popup_flags);
}

// Mark popup as open (toggle toward open state).
// Popups are closed when user click outside, or activate a pressable item, or CloseCurrentPopup()
// is called within a BeginPopup()/EndPopup() block. Popup identifiers are relative to the current
// ID-stack (so OpenPopup and BeginPopup needs to be at the same level). One open popup per level
// of the popup hierarchy (NB: when assigning we reset the Window member of ANCHORPopupRef to NULL)
void ANCHOR::OpenPopupEx(ANCHOR_ID id, AnchorPopupFlags popup_flags)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *parent_window = g.CurrentWindow;
  const int current_stack_size = g.BeginPopupStack.Size;

  if (popup_flags & AnchorPopupFlags_NoOpenOverExistingPopup)
    if (IsPopupOpen(0u, AnchorPopupFlags_AnyPopupId))
      return;

  AnchorPopupData popup_ref;  // Tagged as new ref as Window will be set back to NULL if we write
                              // this into OpenPopupStack.
  popup_ref.PopupId = id;
  popup_ref.Window = NULL;
  popup_ref.SourceWindow = g.NavWindow;
  popup_ref.OpenFrameCount = g.FrameCount;
  popup_ref.OpenParentId = parent_window->IDStack.back();
  popup_ref.OpenPopupPos = NavCalcPreferredRefPos();
  popup_ref.OpenMousePos = IsMousePosValid(&g.IO.MousePos) ? g.IO.MousePos :
                                                             popup_ref.OpenPopupPos;

  ANCHOR_DEBUG_LOG_POPUP("OpenPopupEx(0x%08X)\n", id);
  if (g.OpenPopupStack.Size < current_stack_size + 1) {
    g.OpenPopupStack.push_back(popup_ref);
  } else {
    // Gently handle the user mistakenly calling OpenPopup() every frame. It is a programming
    // mistake! However, if we were to run the regular code path, the ui would become completely
    // unusable because the popup will always be in hidden-while-calculating-size state _while_
    // claiming focus. Which would be a very confusing situation for the programmer. Instead, we
    // silently allow the popup to proceed, it will keep reappearing and the programming error will
    // be more obvious to understand.
    if (g.OpenPopupStack[current_stack_size].PopupId == id &&
        g.OpenPopupStack[current_stack_size].OpenFrameCount == g.FrameCount - 1) {
      g.OpenPopupStack[current_stack_size].OpenFrameCount = popup_ref.OpenFrameCount;
    } else {
      // Close child popups if any, then flag popup for open/reopen
      ClosePopupToLevel(current_stack_size, false);
      g.OpenPopupStack.push_back(popup_ref);
    }

    // When reopening a popup we first refocus its parent, otherwise if its parent is itself a
    // popup it would get closed by ClosePopupsOverWindow(). This is equivalent to what
    // ClosePopupToLevel() does.
    // if (g.OpenPopupStack[current_stack_size].PopupId == id)
    //    FocusWindow(parent_window);
  }
}

// When popups are stacked, clicking on a lower level popups puts focus back to it and close popups
// above it. This function closes any popups that are over 'ref_window'.
void ANCHOR::ClosePopupsOverWindow(AnchorWindow *ref_window,
                                   bool restore_focus_to_window_under_popup)
{
  AnchorContext &g = *G_CTX;
  if (g.OpenPopupStack.Size == 0)
    return;

  // Don't close our own child popup windows.
  int popup_count_to_keep = 0;
  if (ref_window) {
    // Find the highest popup which is a descendant of the reference window (generally reference
    // window = NavWindow)
    for (; popup_count_to_keep < g.OpenPopupStack.Size; popup_count_to_keep++) {
      AnchorPopupData &popup = g.OpenPopupStack[popup_count_to_keep];
      if (!popup.Window)
        continue;
      ANCHOR_ASSERT((popup.Window->Flags & AnchorWindowFlags_Popup) != 0);
      if (popup.Window->Flags & AnchorWindowFlags_ChildWindow)
        continue;

      // Trim the stack unless the popup is a direct parent of the reference window (the reference
      // window is often the NavWindow)
      // - With this stack of window, clicking/focusing Popup1 will close Popup2 and Popup3:
      //     Window -> Popup1 -> Popup2 -> Popup3
      // - Each popups may contain child windows, which is why we compare ->RootWindow!
      //     Window -> Popup1 -> Popup1_Child -> Popup2 -> Popup2_Child
      bool ref_window_is_descendent_of_popup = false;
      for (int n = popup_count_to_keep; n < g.OpenPopupStack.Size; n++)
        if (AnchorWindow *popup_window = g.OpenPopupStack[n].Window)
          if (popup_window->RootWindow == ref_window->RootWindow) {
            ref_window_is_descendent_of_popup = true;
            break;
          }
      if (!ref_window_is_descendent_of_popup)
        break;
    }
  }
  if (popup_count_to_keep <
      g.OpenPopupStack.Size)  // This test is not required but it allows to set a
                              // convenient breakpoint on the statement below
  {
    ANCHOR_DEBUG_LOG_POPUP("ClosePopupsOverWindow(\"%s\") -> ClosePopupToLevel(%d)\n",
                           ref_window->Name,
                           popup_count_to_keep);
    ClosePopupToLevel(popup_count_to_keep, restore_focus_to_window_under_popup);
  }
}

void ANCHOR::ClosePopupToLevel(int remaining, bool restore_focus_to_window_under_popup)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_DEBUG_LOG_POPUP("ClosePopupToLevel(%d), restore_focus_to_window_under_popup=%d\n",
                         remaining,
                         restore_focus_to_window_under_popup);
  ANCHOR_ASSERT(remaining >= 0 && remaining < g.OpenPopupStack.Size);

  // Trim open popup stack
  AnchorWindow *focus_window = g.OpenPopupStack[remaining].SourceWindow;
  AnchorWindow *popup_window = g.OpenPopupStack[remaining].Window;
  g.OpenPopupStack.resize(remaining);

  if (restore_focus_to_window_under_popup) {
    if (focus_window && !focus_window->WasActive && popup_window) {
      // Fallback
      FocusTopMostWindowUnderOne(popup_window, NULL);
    } else {
      if (g.NavLayer == ANCHORNavLayer_Main && focus_window)
        focus_window = NavRestoreLastChildNavWindow(focus_window);
      FocusWindow(focus_window);
    }
  }
}

// Close the popup we have begin-ed into.
void ANCHOR::CloseCurrentPopup()
{
  AnchorContext &g = *G_CTX;
  int popup_idx = g.BeginPopupStack.Size - 1;
  if (popup_idx < 0 || popup_idx >= g.OpenPopupStack.Size ||
      g.BeginPopupStack[popup_idx].PopupId != g.OpenPopupStack[popup_idx].PopupId)
    return;

  // Closing a menu closes its top-most parent popup (unless a modal)
  while (popup_idx > 0) {
    AnchorWindow *popup_window = g.OpenPopupStack[popup_idx].Window;
    AnchorWindow *parent_popup_window = g.OpenPopupStack[popup_idx - 1].Window;
    bool close_parent = false;
    if (popup_window && (popup_window->Flags & AnchorWindowFlags_ChildMenu))
      if (parent_popup_window == NULL || !(parent_popup_window->Flags & AnchorWindowFlags_Modal))
        close_parent = true;
    if (!close_parent)
      break;
    popup_idx--;
  }
  ANCHOR_DEBUG_LOG_POPUP("CloseCurrentPopup %d -> %d\n", g.BeginPopupStack.Size - 1, popup_idx);
  ClosePopupToLevel(popup_idx, true);

  // A common pattern is to close a popup when selecting a menu item/selectable that will open
  // another window. To improve this usage pattern, we avoid nav highlight for a single frame in
  // the parent window. Similarly, we could avoid mouse hover highlight in this window but it is
  // less visually problematic.
  if (AnchorWindow *window = g.NavWindow)
    window->DC.NavHideHighlightOneFrame = true;
}

// Attention! BeginPopup() adds default flags which BeginPopupEx()!
bool ANCHOR::BeginPopupEx(ANCHOR_ID id, AnchorWindowFlags flags)
{
  AnchorContext &g = *G_CTX;
  if (!IsPopupOpen(id, AnchorPopupFlags_None)) {
    g.NextWindowData.ClearFlags();  // We behave like Begin() and need to consume those values
    return false;
  }

  char name[20];
  if (flags & AnchorWindowFlags_ChildMenu)
    AnchorFormatString(name,
                       ANCHOR_ARRAYSIZE(name),
                       "##Menu_%02d",
                       g.BeginPopupStack.Size);  // Recycle windows based on depth
  else
    AnchorFormatString(name,
                       ANCHOR_ARRAYSIZE(name),
                       "##Popup_%08x",
                       id);  // Not recycling, so we can close/open during the same frame

  flags |= AnchorWindowFlags_Popup;
  bool is_open = Begin(name, NULL, flags);
  if (!is_open)  // NB: Begin can return false when the popup is completely clipped (e.g. zero size
                 // display)
    EndPopup();

  return is_open;
}

bool ANCHOR::BeginPopup(const char *str_id, AnchorWindowFlags flags)
{
  AnchorContext &g = *G_CTX;
  if (g.OpenPopupStack.Size <= g.BeginPopupStack.Size)  // Early out for performance
  {
    g.NextWindowData.ClearFlags();  // We behave like Begin() and need to consume those values
    return false;
  }
  flags |= AnchorWindowFlags_AlwaysAutoResize | AnchorWindowFlags_NoTitleBar |
           AnchorWindowFlags_NoSavedSettings;
  return BeginPopupEx(g.CurrentWindow->GetID(str_id), flags);
}

// If 'p_open' is specified for a modal popup window, the popup will have a regular close button
// which will close the popup. Note that popup visibility status is owned by ANCHOR (and
// manipulated with e.g. OpenPopup) so the actual value of *p_open is meaningless here.
bool ANCHOR::BeginPopupModal(const char *name, bool *p_open, AnchorWindowFlags flags)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  const ANCHOR_ID id = window->GetID(name);
  if (!IsPopupOpen(id, AnchorPopupFlags_None)) {
    g.NextWindowData.ClearFlags();  // We behave like Begin() and need to consume those values
    return false;
  }

  // Center modal windows by default for increased visibility
  // (this won't really last as settings will kick in, and is mostly for backward compatibility.
  // user may do the same themselves)
  // FIXME: Should test for (PosCond & window->SetWindowPosAllowFlags) with the upcoming window.
  if ((g.NextWindowData.Flags & AnchorNextWindowDataFlags_HasPos) == 0) {
    const AnchorViewport *viewport = GetMainViewport();
    SetNextWindowPos(viewport->GetCenter(), AnchorCond_FirstUseEver, wabi::GfVec2f(0.5f, 0.5f));
  }

  flags |= AnchorWindowFlags_Popup | AnchorWindowFlags_Modal | AnchorWindowFlags_NoCollapse;
  const bool is_open = Begin(name, p_open, flags);
  if (!is_open || (p_open && !*p_open))  // NB: is_open can be 'false' when the popup is completely
                                         // clipped (e.g. zero size display)
  {
    EndPopup();
    if (is_open)
      ClosePopupToLevel(g.BeginPopupStack.Size, true);
    return false;
  }
  return is_open;
}

void ANCHOR::EndPopup()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  ANCHOR_ASSERT(window->Flags &
                AnchorWindowFlags_Popup);  // Mismatched BeginPopup()/EndPopup() calls
  ANCHOR_ASSERT(g.BeginPopupStack.Size > 0);

  // Make all menus and popups wrap around for now, may need to expose that policy.
  if (g.NavWindow == window)
    NavMoveRequestTryWrapping(window, AnchorNavMoveFlags_LoopY);

  // Child-popups don't need to be laid out
  ANCHOR_ASSERT(g.WithinEndChild == false);
  if (window->Flags & AnchorWindowFlags_ChildWindow)
    g.WithinEndChild = true;
  End();
  g.WithinEndChild = false;
}

// Helper to open a popup if mouse button is released over the item
// - This is essentially the same as BeginPopupContextItem() but without the trailing BeginPopup()
void ANCHOR::OpenPopupOnItemClick(const char *str_id, AnchorPopupFlags popup_flags)
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  int mouse_button = (popup_flags & AnchorPopupFlags_MouseButtonMask_);
  if (IsMouseReleased(mouse_button) && IsItemHovered(AnchorHoveredFlags_AllowWhenBlockedByPopup)) {
    ANCHOR_ID id =
      str_id ? window->GetID(str_id) :
               window->DC.LastItemId;  // If user hasn't passed an ID, we can use the LastItemID.
                                       // Using LastItemID as a Popup ID won't conflict!
    ANCHOR_ASSERT(id != 0);  // You cannot pass a NULL str_id if the last item has no identifier
                             // (e.g. a Text() item)
    OpenPopupEx(id, popup_flags);
  }
}

// This is a helper to handle the simplest case of associating one named popup to one given widget.
// - To create a popup associated to the last item, you generally want to pass a NULL value to
// str_id.
// - To create a popup with a specific identifier, pass it in str_id.
//    - This is useful when using using BeginPopupContextItem() on an item which doesn't have an
//    identifier, e.g. a Text() call.
//    - This is useful when multiple code locations may want to manipulate/open the same popup,
//    given an explicit id.
// - You may want to handle the whole on user side if you have specific needs (e.g. tweaking
// IsItemHovered() parameters).
//   This is essentially the same as:
//       id = str_id ? GetID(str_id) : GetItemID();
//       OpenPopupOnItemClick(str_id);
//       return BeginPopup(id);
//   Which is essentially the same as:
//       id = str_id ? GetID(str_id) : GetItemID();
//       if (IsItemHovered() && IsMouseReleased(AnchorMouseButton_Right))
//           OpenPopup(id);
//       return BeginPopup(id);
//   The main difference being that this is tweaked to avoid computing the ID twice.
bool ANCHOR::BeginPopupContextItem(const char *str_id, AnchorPopupFlags popup_flags)
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  if (window->SkipItems)
    return false;
  ANCHOR_ID id =
    str_id ? window->GetID(str_id) :
             window->DC.LastItemId;  // If user hasn't passed an ID, we can use the LastItemID.
                                     // Using LastItemID as a Popup ID won't conflict!
  ANCHOR_ASSERT(
    id !=
    0);  // You cannot pass a NULL str_id if the last item has no identifier (e.g. a Text() item)
  int mouse_button = (popup_flags & AnchorPopupFlags_MouseButtonMask_);
  if (IsMouseReleased(mouse_button) && IsItemHovered(AnchorHoveredFlags_AllowWhenBlockedByPopup))
    OpenPopupEx(id, popup_flags);
  return BeginPopupEx(id,
                      AnchorWindowFlags_AlwaysAutoResize | AnchorWindowFlags_NoTitleBar |
                        AnchorWindowFlags_NoSavedSettings);
}

bool ANCHOR::BeginPopupContextWindow(const char *str_id, AnchorPopupFlags popup_flags)
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  if (!str_id)
    str_id = "window_context";
  ANCHOR_ID id = window->GetID(str_id);
  int mouse_button = (popup_flags & AnchorPopupFlags_MouseButtonMask_);
  if (IsMouseReleased(mouse_button) && IsWindowHovered(AnchorHoveredFlags_AllowWhenBlockedByPopup))
    if (!(popup_flags & AnchorPopupFlags_NoOpenOverItems) || !IsAnyItemHovered())
      OpenPopupEx(id, popup_flags);
  return BeginPopupEx(id,
                      AnchorWindowFlags_AlwaysAutoResize | AnchorWindowFlags_NoTitleBar |
                        AnchorWindowFlags_NoSavedSettings);
}

bool ANCHOR::BeginPopupContextVoid(const char *str_id, AnchorPopupFlags popup_flags)
{
  AnchorWindow *window = G_CTX->CurrentWindow;
  if (!str_id)
    str_id = "void_context";
  ANCHOR_ID id = window->GetID(str_id);
  int mouse_button = (popup_flags & AnchorPopupFlags_MouseButtonMask_);
  if (IsMouseReleased(mouse_button) && !IsWindowHovered(AnchorHoveredFlags_AnyWindow))
    if (GetTopMostPopupModal() == NULL)
      OpenPopupEx(id, popup_flags);
  return BeginPopupEx(id,
                      AnchorWindowFlags_AlwaysAutoResize | AnchorWindowFlags_NoTitleBar |
                        AnchorWindowFlags_NoSavedSettings);
}

// r_avoid = the rectangle to avoid (e.g. for tooltip it is a rectangle around the mouse cursor
// which we want to avoid. for popups it's a small point around the cursor.) r_outer = the visible
// area rectangle, minus safe area padding. If our popup size won't fit because of safe area
// padding we ignore it. (r_outer is usually equivalent to the viewport rectangle minus padding,
// but when multi-viewports are enabled and monitor
//  information are available, it may represent the entire platform monitor from the frame of
//  reference of the current viewport. this allows us to have tooltips/popups displayed out of the
//  parent viewport.)
wabi::GfVec2f ANCHOR::FindBestWindowPosForPopupEx(const wabi::GfVec2f &ref_pos,
                                            const wabi::GfVec2f &size,
                                            AnchorDir *last_dir,
                                            const AnchorBBox &r_outer,
                                            const AnchorBBox &r_avoid,
                                            ANCHORPopupPositionPolicy policy)
{
  wabi::GfVec2f base_pos_clamped = AnchorClamp(ref_pos, r_outer.Min, r_outer.Max - size);
  // GetForegroundDrawList()->AddRect(r_avoid.Min, r_avoid.Max, ANCHOR_COL32(255,0,0,255));
  // GetForegroundDrawList()->AddRect(r_outer.Min, r_outer.Max, ANCHOR_COL32(0,255,0,255));

  // Combo Box policy (we want a connecting edge)
  if (policy == ANCHORPopupPositionPolicy_ComboBox) {
    const AnchorDir dir_prefered_order[AnchorDir_COUNT] = {AnchorDir_Down,
                                                           AnchorDir_Right,
                                                           AnchorDir_Left,
                                                           AnchorDir_Up};
    for (int n = (*last_dir != AnchorDir_None) ? -1 : 0; n < AnchorDir_COUNT; n++) {
      const AnchorDir dir = (n == -1) ? *last_dir : dir_prefered_order[n];
      if (n != -1 && dir == *last_dir)  // Already tried this direction?
        continue;
      wabi::GfVec2f pos;
      if (dir == AnchorDir_Down)
        pos = wabi::GfVec2f(r_avoid.Min[0], r_avoid.Max[1]);  // Below, Toward Right (default)
      if (dir == AnchorDir_Right)
        pos = wabi::GfVec2f(r_avoid.Min[0], r_avoid.Min[1] - size[1]);  // Above, Toward Right
      if (dir == AnchorDir_Left)
        pos = wabi::GfVec2f(r_avoid.Max[0] - size[0], r_avoid.Max[1]);  // Below, Toward Left
      if (dir == AnchorDir_Up)
        pos = wabi::GfVec2f(r_avoid.Max[0] - size[0], r_avoid.Min[1] - size[1]);  // Above, Toward Left
      if (!r_outer.Contains(AnchorBBox(pos, pos + size)))
        continue;
      *last_dir = dir;
      return pos;
    }
  }

  // Tooltip and Default popup policy
  // (Always first try the direction we used on the last frame, if any)
  if (policy == ANCHORPopupPositionPolicy_Tooltip || policy == ANCHORPopupPositionPolicy_Default) {
    const AnchorDir dir_prefered_order[AnchorDir_COUNT] = {AnchorDir_Right,
                                                           AnchorDir_Down,
                                                           AnchorDir_Up,
                                                           AnchorDir_Left};
    for (int n = (*last_dir != AnchorDir_None) ? -1 : 0; n < AnchorDir_COUNT; n++) {
      const AnchorDir dir = (n == -1) ? *last_dir : dir_prefered_order[n];
      if (n != -1 && dir == *last_dir)  // Already tried this direction?
        continue;

      const float avail_w = (dir == AnchorDir_Left ? r_avoid.Min[0] : r_outer.Max[0]) -
                            (dir == AnchorDir_Right ? r_avoid.Max[0] : r_outer.Min[0]);
      const float avail_h = (dir == AnchorDir_Up ? r_avoid.Min[1] : r_outer.Max[1]) -
                            (dir == AnchorDir_Down ? r_avoid.Max[1] : r_outer.Min[1]);

      // If there not enough room on one axis, there's no point in positioning on a side on this
      // axis (e.g. when not enough width, use a top/bottom position to maximize available width)
      if (avail_w < size[0] && (dir == AnchorDir_Left || dir == AnchorDir_Right))
        continue;
      if (avail_h < size[1] && (dir == AnchorDir_Up || dir == AnchorDir_Down))
        continue;

      wabi::GfVec2f pos;
      pos[0] = (dir == AnchorDir_Left)  ? r_avoid.Min[0] - size[0] :
               (dir == AnchorDir_Right) ? r_avoid.Max[0] :
                                          base_pos_clamped[0];
      pos[1] = (dir == AnchorDir_Up)   ? r_avoid.Min[1] - size[1] :
               (dir == AnchorDir_Down) ? r_avoid.Max[1] :
                                         base_pos_clamped[1];

      // Clamp top-left corner of popup
      pos[0] = AnchorMax(pos[0], r_outer.Min[0]);
      pos[1] = AnchorMax(pos[1], r_outer.Min[1]);

      *last_dir = dir;
      return pos;
    }
  }

  // Fallback when not enough room:
  *last_dir = AnchorDir_None;

  // For tooltip we prefer avoiding the cursor at all cost even if it means that part of the
  // tooltip won't be visible.
  if (policy == ANCHORPopupPositionPolicy_Tooltip)
    return ref_pos + wabi::GfVec2f(2, 2);

  // Otherwise try to keep within display
  wabi::GfVec2f pos = ref_pos;
  pos[0] = AnchorMax(AnchorMin(pos[0] + size[0], r_outer.Max[0]) - size[0], r_outer.Min[0]);
  pos[1] = AnchorMax(AnchorMin(pos[1] + size[1], r_outer.Max[1]) - size[1], r_outer.Min[1]);
  return pos;
}

// Note that this is used for popups, which can overlap the non work-area of individual viewports.
AnchorBBox ANCHOR::GetPopupAllowedExtentRect(AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;
  TF_UNUSED(window);
  AnchorBBox r_screen = ((AnchorViewportP *)(void *)GetMainViewport())->GetMainRect();
  wabi::GfVec2f padding = g.Style.DisplaySafeAreaPadding;
  r_screen.Expand(wabi::GfVec2f((r_screen.GetWidth() > padding[0] * 2) ? -padding[0] : 0.0f,
                          (r_screen.GetHeight() > padding[1] * 2) ? -padding[1] : 0.0f));
  return r_screen;
}

wabi::GfVec2f ANCHOR::FindBestWindowPosForPopup(AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;

  AnchorBBox r_outer = GetPopupAllowedExtentRect(window);
  if (window->Flags & AnchorWindowFlags_ChildMenu) {
    // Child menus typically request _any_ position within the parent menu item, and then we move
    // the new menu outside the parent bounds. This is how we end up with child menus appearing
    // (most-commonly) on the right of the parent menu.
    ANCHOR_ASSERT(g.CurrentWindow == window);
    AnchorWindow *parent_window = g.CurrentWindowStack[g.CurrentWindowStack.Size - 2];
    float horizontal_overlap =
      g.Style.ItemInnerSpacing[0];  // We want some overlap to convey the relative depth of each
                                    // menu (currently the amount of overlap is hard-coded to
                                    // style.ItemSpacing[0]).
    AnchorBBox r_avoid;
    if (parent_window->DC.MenuBarAppending)
      r_avoid = AnchorBBox(-FLT_MAX,
                           parent_window->ClipRect.Min[1],
                           FLT_MAX,
                           parent_window->ClipRect
                             .Max[1]);  // Avoid parent menu-bar. If we wanted multi-line menu-bar,
                                        // we may instead want to have the calling window setup
                                        // e.g. a NextWindowData.PosConstraintAvoidRect field
    else
      r_avoid = AnchorBBox(parent_window->Pos[0] + horizontal_overlap,
                           -FLT_MAX,
                           parent_window->Pos[0] + parent_window->Size[0] - horizontal_overlap -
                             parent_window->ScrollbarSizes[0],
                           FLT_MAX);
    return FindBestWindowPosForPopupEx(window->Pos,
                                       window->Size,
                                       &window->AutoPosLastDirection,
                                       r_outer,
                                       r_avoid,
                                       ANCHORPopupPositionPolicy_Default);
  }
  if (window->Flags & AnchorWindowFlags_Popup) {
    AnchorBBox r_avoid = AnchorBBox(window->Pos[0] - 1,
                                    window->Pos[1] - 1,
                                    window->Pos[0] + 1,
                                    window->Pos[1] + 1);
    return FindBestWindowPosForPopupEx(window->Pos,
                                       window->Size,
                                       &window->AutoPosLastDirection,
                                       r_outer,
                                       r_avoid,
                                       ANCHORPopupPositionPolicy_Default);
  }
  if (window->Flags & AnchorWindowFlags_Tooltip) {
    // Position tooltip (always follows mouse)
    float sc = g.Style.MouseCursorScale;
    wabi::GfVec2f ref_pos = NavCalcPreferredRefPos();
    AnchorBBox r_avoid;
    if (!g.NavDisableHighlight && g.NavDisableMouseHover &&
        !(g.IO.ConfigFlags & AnchorConfigFlags_NavEnableSetMousePos))
      r_avoid = AnchorBBox(ref_pos[0] - 16, ref_pos[1] - 8, ref_pos[0] + 16, ref_pos[1] + 8);
    else
      r_avoid = AnchorBBox(ref_pos[0] - 16,
                           ref_pos[1] - 8,
                           ref_pos[0] + 24 * sc,
                           ref_pos[1] +
                             24 * sc);  // FIXME: Hard-coded based on mouse cursor shape
                                        // expectation. Exact dimension not very important.
    return FindBestWindowPosForPopupEx(ref_pos,
                                       window->Size,
                                       &window->AutoPosLastDirection,
                                       r_outer,
                                       r_avoid,
                                       ANCHORPopupPositionPolicy_Tooltip);
  }
  ANCHOR_ASSERT(0);
  return window->Pos;
}

//-----------------------------------------------------------------------------
// [SECTION] KEYBOARD/GAMEPAD NAVIGATION
//-----------------------------------------------------------------------------

// FIXME-NAV: The existence of SetNavID vs SetFocusID properly needs to be clarified/reworked.
void ANCHOR::SetNavID(ANCHOR_ID id,
                      ANCHORNavLayer nav_layer,
                      ANCHOR_ID focus_scope_id,
                      const AnchorBBox &rect_rel)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(g.NavWindow != NULL);
  ANCHOR_ASSERT(nav_layer == ANCHORNavLayer_Main || nav_layer == ANCHORNavLayer_Menu);
  g.NavId = id;
  g.NavLayer = nav_layer;
  g.NavFocusScopeId = focus_scope_id;
  g.NavWindow->NavLastIds[nav_layer] = id;
  g.NavWindow->NavRectRel[nav_layer] = rect_rel;
  // g.NavDisableHighlight = false;
  // g.NavDisableMouseHover = g.NavMousePosDirty = true;
}

void ANCHOR::SetFocusID(ANCHOR_ID id, AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(id != 0);

  // Assume that SetFocusID() is called in the context where its window->DC.NavLayerCurrent and
  // window->DC.NavFocusScopeIdCurrent are valid. Note that window may be != g.CurrentWindow (e.g.
  // SetFocusID call in InputTextEx for multi-line text)
  const ANCHORNavLayer nav_layer = window->DC.NavLayerCurrent;
  if (g.NavWindow != window)
    g.NavInitRequest = false;
  g.NavWindow = window;
  g.NavId = id;
  g.NavLayer = nav_layer;
  g.NavFocusScopeId = window->DC.NavFocusScopeIdCurrent;
  window->NavLastIds[nav_layer] = id;
  if (window->DC.LastItemId == id)
    window->NavRectRel[nav_layer] = AnchorBBox(window->DC.LastItemRect.Min - window->Pos,
                                               window->DC.LastItemRect.Max - window->Pos);

  if (g.ActiveIdSource == ANCHORInputSource_Nav)
    g.NavDisableMouseHover = true;
  else
    g.NavDisableHighlight = true;
}

AnchorDir AnchorGetDirQuadrantFromDelta(float dx, float dy)
{
  if (AnchorFabs(dx) > AnchorFabs(dy))
    return (dx > 0.0f) ? AnchorDir_Right : AnchorDir_Left;
  return (dy > 0.0f) ? AnchorDir_Down : AnchorDir_Up;
}

static float inline NavScoreItemDistInterval(float a0, float a1, float b0, float b1)
{
  if (a1 < b0)
    return a1 - b0;
  if (b1 < a0)
    return a0 - b1;
  return 0.0f;
}

static void inline NavClampRectToVisibleAreaForMoveDir(AnchorDir move_dir,
                                                       AnchorBBox &r,
                                                       const AnchorBBox &clip_rect)
{
  if (move_dir == AnchorDir_Left || move_dir == AnchorDir_Right) {
    r.Min[1] = AnchorClamp(r.Min[1], clip_rect.Min[1], clip_rect.Max[1]);
    r.Max[1] = AnchorClamp(r.Max[1], clip_rect.Min[1], clip_rect.Max[1]);
  } else {
    r.Min[0] = AnchorClamp(r.Min[0], clip_rect.Min[0], clip_rect.Max[0]);
    r.Max[0] = AnchorClamp(r.Max[0], clip_rect.Min[0], clip_rect.Max[0]);
  }
}

// Scoring function for gamepad/keyboard directional navigation. Based on
// https://gist.github.com/rygorous/6981057
static bool ANCHOR::NavScoreItem(AnchorNavItemData *result, AnchorBBox cand)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  if (g.NavLayer != window->DC.NavLayerCurrent)
    return false;

  const AnchorBBox &curr =
    g.NavScoringRect;  // Current modified source rect (NB: we've applied Max[0] = Min[0] in
                       // NavUpdate() to inhibit the effect of having varied item width)
  g.NavScoringCount++;

  // When entering through a NavFlattened border, we consider child window items as fully clipped
  // for scoring
  if (window->ParentWindow == g.NavWindow) {
    ANCHOR_ASSERT((window->Flags | g.NavWindow->Flags) & AnchorWindowFlags_NavFlattened);
    if (!window->ClipRect.Overlaps(cand))
      return false;
    cand.ClipWithFull(window->ClipRect);  // This allows the scored item to not overlap other
                                          // candidates in the parent window
  }

  // We perform scoring on items bounding box clipped by the current clipping rectangle on the
  // other axis (clipping on our movement axis would give us equal scores for all clipped items)
  // For example, this ensure that items in one column are not reached when moving vertically from
  // items in another column.
  NavClampRectToVisibleAreaForMoveDir(g.NavMoveClipDir, cand, window->ClipRect);

  // Compute distance between boxes
  // FIXME-NAV: Introducing biases for vertical navigation, needs to be removed.
  float dbx = NavScoreItemDistInterval(cand.Min[0], cand.Max[0], curr.Min[0], curr.Max[0]);
  float dby = NavScoreItemDistInterval(
    AnchorLerp(cand.Min[1], cand.Max[1], 0.2f),
    AnchorLerp(cand.Min[1], cand.Max[1], 0.8f),
    AnchorLerp(curr.Min[1], curr.Max[1], 0.2f),
    AnchorLerp(curr.Min[1],
               curr.Max[1],
               0.8f));  // Scale down on Y to keep using box-distance for vertically touching items
  if (dby != 0.0f && dbx != 0.0f)
    dbx = (dbx / 1000.0f) + ((dbx > 0.0f) ? +1.0f : -1.0f);
  float dist_box = AnchorFabs(dbx) + AnchorFabs(dby);

  // Compute distance between centers (this is off by a factor of 2, but we only compare center
  // distances with each other so it doesn't matter)
  float dcx = (cand.Min[0] + cand.Max[0]) - (curr.Min[0] + curr.Max[0]);
  float dcy = (cand.Min[1] + cand.Max[1]) - (curr.Min[1] + curr.Max[1]);
  float dist_center = AnchorFabs(dcx) +
                      AnchorFabs(dcy);  // L1 metric (need this for our connectedness guarantee)

  // Determine which quadrant of 'curr' our candidate item 'cand' lies in based on distance
  AnchorDir quadrant;
  float dax = 0.0f, day = 0.0f, dist_axial = 0.0f;
  if (dbx != 0.0f || dby != 0.0f) {
    // For non-overlapping boxes, use distance between boxes
    dax = dbx;
    day = dby;
    dist_axial = dist_box;
    quadrant = AnchorGetDirQuadrantFromDelta(dbx, dby);
  } else if (dcx != 0.0f || dcy != 0.0f) {
    // For overlapping boxes with different centers, use distance between centers
    dax = dcx;
    day = dcy;
    dist_axial = dist_center;
    quadrant = AnchorGetDirQuadrantFromDelta(dcx, dcy);
  } else {
    // Degenerate case: two overlapping buttons with same center, break ties arbitrarily (note that
    // LastItemId here is really the _previous_ item order, but it doesn't matter)
    quadrant = (window->DC.LastItemId < g.NavId) ? AnchorDir_Left : AnchorDir_Right;
  }

#if ANCHOR_DEBUG_NAV_SCORING
  char buf[128];
  if (IsMouseHoveringRect(cand.Min, cand.Max)) {
    AnchorFormatString(
      buf,
      ANCHOR_ARRAYSIZE(buf),
      "dbox (%.2f,%.2f->%.4f)\ndcen (%.2f,%.2f->%.4f)\nd (%.2f,%.2f->%.4f)\nnav %c, quadrant %c",
      dbx,
      dby,
      dist_box,
      dcx,
      dcy,
      dist_center,
      dax,
      day,
      dist_axial,
      "WENS"[g.NavMoveDir],
      "WENS"[quadrant]);
    AnchorDrawList *draw_list = GetForegroundDrawList(window);
    draw_list->AddRect(curr.Min, curr.Max, ANCHOR_COL32(255, 200, 0, 100));
    draw_list->AddRect(cand.Min, cand.Max, ANCHOR_COL32(255, 255, 0, 200));
    draw_list->AddRectFilled(cand.Max - wabi::GfVec2f(4, 4),
                             cand.Max + CalcTextSize(buf) + wabi::GfVec2f(4, 4),
                             ANCHOR_COL32(40, 0, 0, 150));
    draw_list->AddText(g.IO.FontDefault, 13.0f, cand.Max, ~0U, buf);
  } else if (g.IO.KeyCtrl)  // Hold to preview score in matching quadrant. Press C to rotate.
  {
    if (IsKeyPressedMap(AnchorKey_C)) {
      g.NavMoveDirLast = (AnchorDir)((g.NavMoveDirLast + 1) & 3);
      g.IO.KeysDownDuration[g.IO.KeyMap[AnchorKey_C]] = 0.01f;
    }
    if (quadrant == g.NavMoveDir) {
      AnchorFormatString(buf, ANCHOR_ARRAYSIZE(buf), "%.0f/%.0f", dist_box, dist_center);
      AnchorDrawList *draw_list = GetForegroundDrawList(window);
      draw_list->AddRectFilled(cand.Min, cand.Max, ANCHOR_COL32(255, 0, 0, 200));
      draw_list->AddText(g.IO.FontDefault, 13.0f, cand.Min, ANCHOR_COL32(255, 255, 255, 255), buf);
    }
  }
#endif

  // Is it in the quadrant we're interesting in moving to?
  bool new_best = false;
  if (quadrant == g.NavMoveDir) {
    // Does it beat the current best candidate?
    if (dist_box < result->DistBox) {
      result->DistBox = dist_box;
      result->DistCenter = dist_center;
      return true;
    }
    if (dist_box == result->DistBox) {
      // Try using distance between center points to break ties
      if (dist_center < result->DistCenter) {
        result->DistCenter = dist_center;
        new_best = true;
      } else if (dist_center == result->DistCenter) {
        // Still tied! we need to be extra-careful to make sure everything gets linked properly. We
        // consistently break ties by symbolically moving "later" items (with higher index) to the
        // right/downwards by an infinitesimal amount since we the current "best" button already
        // (so it must have a lower index), this is fairly easy. This rule ensures that all buttons
        // with dx==dy==0 will end up being linked in order of appearance along the x axis.
        if (((g.NavMoveDir == AnchorDir_Up || g.NavMoveDir == AnchorDir_Down) ? dby : dbx) <
            0.0f)  // moving bj to the right/down decreases distance
          new_best = true;
      }
    }
  }

  // Axial check: if 'curr' has no link at all in some direction and 'cand' lies roughly in that
  // direction, add a tentative link. This will only be kept if no "real" matches are found, so it
  // only augments the graph produced by the above method using extra links. (important, since it
  // doesn't guarantee strong connectedness) This is just to avoid buttons having no links in a
  // particular direction when there's a suitable neighbor. you get good graphs without this too.
  // 2017/09/29: FIXME: This now currently only enabled inside menu bars, ideally we'd disable it
  // everywhere. Menus in particular need to catch failure. For general navigation it feels
  // awkward. Disabling it may lead to disconnected graphs when nodes are very spaced out on
  // different axis. Perhaps consider offering this as an option?
  if (result->DistBox == FLT_MAX && dist_axial < result->DistAxial)  // Check axial match
    if (g.NavLayer == ANCHORNavLayer_Menu && !(g.NavWindow->Flags & AnchorWindowFlags_ChildMenu))
      if ((g.NavMoveDir == AnchorDir_Left && dax < 0.0f) ||
          (g.NavMoveDir == AnchorDir_Right && dax > 0.0f) ||
          (g.NavMoveDir == AnchorDir_Up && day < 0.0f) ||
          (g.NavMoveDir == AnchorDir_Down && day > 0.0f)) {
        result->DistAxial = dist_axial;
        new_best = true;
      }

  return new_best;
}

static void ANCHOR::NavApplyItemToResult(AnchorNavItemData *result,
                                         AnchorWindow *window,
                                         ANCHOR_ID id,
                                         const AnchorBBox &nav_bb_rel)
{
  result->Window = window;
  result->ID = id;
  result->FocusScopeId = window->DC.NavFocusScopeIdCurrent;
  result->RectRel = nav_bb_rel;
}

// We get there when either NavId == id, or when g.NavAnyRequest is set (which is updated by
// NavUpdateAnyRequestFlag above)
static void ANCHOR::NavProcessItem(AnchorWindow *window,
                                   const AnchorBBox &nav_bb,
                                   const ANCHOR_ID id)
{
  AnchorContext &g = *G_CTX;
  // if (!g.IO.NavActive)  // [2017/10/06] Removed this possibly redundant test but I am not sure
  // of all the side-effects yet. Some of the feature here will need to work regardless of using a
  // _NoNavInputs flag.
  //    return;

  const AnchorItemFlags item_flags = g.CurrentItemFlags;
  const AnchorBBox nav_bb_rel(nav_bb.Min - window->Pos, nav_bb.Max - window->Pos);

  // Process Init Request
  if (g.NavInitRequest && g.NavLayer == window->DC.NavLayerCurrent) {
    // Even if 'AnchorItemFlags_NoNavDefaultFocus' is on (typically collapse/close button) we
    // record the first ResultId so they can be used as a fallback
    if (!(item_flags & AnchorItemFlags_NoNavDefaultFocus) || g.NavInitResultId == 0) {
      g.NavInitResultId = id;
      g.NavInitResultRectRel = nav_bb_rel;
    }
    if (!(item_flags & AnchorItemFlags_NoNavDefaultFocus)) {
      g.NavInitRequest = false;  // Found a match, clear request
      NavUpdateAnyRequestFlag();
    }
  }

  // Process Move Request (scoring for navigation)
  // FIXME-NAV: Consider policy for double scoring (scoring from NavScoringRectScreen + scoring
  // from a rect wrapped according to current wrapping policy)
  if ((g.NavId != id || (g.NavMoveRequestFlags & AnchorNavMoveFlags_AllowCurrentNavId)) &&
      !(item_flags & (AnchorItemFlags_Disabled | AnchorItemFlags_NoNav))) {
    AnchorNavItemData *result = (window == g.NavWindow) ? &g.NavMoveResultLocal :
                                                          &g.NavMoveResultOther;
#if ANCHOR_DEBUG_NAV_SCORING
    // [DEBUG] Score all items in NavWindow at all times
    if (!g.NavMoveRequest)
      g.NavMoveDir = g.NavMoveDirLast;
    bool new_best = NavScoreItem(result, nav_bb) && g.NavMoveRequest;
#else
    bool new_best = g.NavMoveRequest && NavScoreItem(result, nav_bb);
#endif
    if (new_best)
      NavApplyItemToResult(result, window, id, nav_bb_rel);

    // Features like PageUp/PageDown need to maintain a separate score for the visible set of
    // items.
    const float VISIBLE_RATIO = 0.70f;
    if ((g.NavMoveRequestFlags & AnchorNavMoveFlags_AlsoScoreVisibleSet) &&
        window->ClipRect.Overlaps(nav_bb))
      if (AnchorClamp(nav_bb.Max[1], window->ClipRect.Min[1], window->ClipRect.Max[1]) -
            AnchorClamp(nav_bb.Min[1], window->ClipRect.Min[1], window->ClipRect.Max[1]) >=
          (nav_bb.Max[1] - nav_bb.Min[1]) * VISIBLE_RATIO)
        if (NavScoreItem(&g.NavMoveResultLocalVisibleSet, nav_bb))
          NavApplyItemToResult(&g.NavMoveResultLocalVisibleSet, window, id, nav_bb_rel);
  }

  // Update window-relative bounding box of navigated item
  if (g.NavId == id) {
    g.NavWindow = window;  // Always refresh g.NavWindow, because some operations such as
                           // FocusItem() don't have a window.
    g.NavLayer = window->DC.NavLayerCurrent;
    g.NavFocusScopeId = window->DC.NavFocusScopeIdCurrent;
    g.NavIdIsAlive = true;
    window->NavRectRel[window->DC.NavLayerCurrent] =
      nav_bb_rel;  // Store item bounding box (relative to window position)
  }
}

bool ANCHOR::NavMoveRequestButNoResultYet()
{
  AnchorContext &g = *G_CTX;
  return g.NavMoveRequest && g.NavMoveResultLocal.ID == 0 && g.NavMoveResultOther.ID == 0;
}

void ANCHOR::NavMoveRequestCancel()
{
  AnchorContext &g = *G_CTX;
  g.NavMoveRequest = false;
  NavUpdateAnyRequestFlag();
}

void ANCHOR::NavMoveRequestForward(AnchorDir move_dir,
                                   AnchorDir clip_dir,
                                   const AnchorBBox &bb_rel,
                                   AnchorNavMoveFlags move_flags)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(g.NavMoveRequestForward == ANCHORNavForward_None);
  NavMoveRequestCancel();
  g.NavMoveDir = move_dir;
  g.NavMoveClipDir = clip_dir;
  g.NavMoveRequestForward = ANCHORNavForward_ForwardQueued;
  g.NavMoveRequestFlags = move_flags;
  g.NavWindow->NavRectRel[g.NavLayer] = bb_rel;
}

void ANCHOR::NavMoveRequestTryWrapping(AnchorWindow *window, AnchorNavMoveFlags move_flags)
{
  AnchorContext &g = *G_CTX;

  // Navigation wrap-around logic is delayed to the end of the frame because this operation is only
  // valid after entire popup is assembled and in case of appended popups it is not clear which
  // EndPopup() call is final.
  g.NavWrapRequestWindow = window;
  g.NavWrapRequestFlags = move_flags;
}

// FIXME: This could be replaced by updating a frame number in each window when (window ==
// NavWindow) and (NavLayer == 0). This way we could find the last focused window among our
// children. It would be much less confusing this way?
static void ANCHOR::NavSaveLastChildNavWindowIntoParent(AnchorWindow *nav_window)
{
  AnchorWindow *parent = nav_window;
  while (parent && parent->RootWindow != parent &&
         (parent->Flags & (AnchorWindowFlags_Popup | AnchorWindowFlags_ChildMenu)) == 0)
    parent = parent->ParentWindow;
  if (parent && parent != nav_window)
    parent->NavLastChildNavWindow = nav_window;
}

// Restore the last focused child.
// Call when we are expected to land on the Main Layer (0) after FocusWindow()
static AnchorWindow *ANCHOR::NavRestoreLastChildNavWindow(AnchorWindow *window)
{
  if (window->NavLastChildNavWindow && window->NavLastChildNavWindow->WasActive)
    return window->NavLastChildNavWindow;
  return window;
}

void ANCHOR::NavRestoreLayer(ANCHORNavLayer layer)
{
  AnchorContext &g = *G_CTX;
  if (layer == ANCHORNavLayer_Main)
    g.NavWindow = NavRestoreLastChildNavWindow(g.NavWindow);
  AnchorWindow *window = g.NavWindow;
  if (window->NavLastIds[layer] != 0) {
    SetNavID(window->NavLastIds[layer], layer, 0, window->NavRectRel[layer]);
    g.NavDisableHighlight = false;
    g.NavDisableMouseHover = g.NavMousePosDirty = true;
  } else {
    g.NavLayer = layer;
    NavInitWindow(window, true);
  }
}

static inline void ANCHOR::NavUpdateAnyRequestFlag()
{
  AnchorContext &g = *G_CTX;
  g.NavAnyRequest = g.NavMoveRequest || g.NavInitRequest ||
                    (ANCHOR_DEBUG_NAV_SCORING && g.NavWindow != NULL);
  if (g.NavAnyRequest)
    ANCHOR_ASSERT(g.NavWindow != NULL);
}

// This needs to be called before we submit any widget (aka in or before Begin)
void ANCHOR::NavInitWindow(AnchorWindow *window, bool force_reinit)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(window == g.NavWindow);

  if (window->Flags & AnchorWindowFlags_NoNavInputs) {
    g.NavId = g.NavFocusScopeId = 0;
    return;
  }

  bool init_for_nav = false;
  if (window == window->RootWindow || (window->Flags & AnchorWindowFlags_Popup) ||
      (window->NavLastIds[0] == 0) || force_reinit)
    init_for_nav = true;
  ANCHOR_DEBUG_LOG_NAV(
    "[nav] NavInitRequest: from NavInitWindow(), init_for_nav=%d, window=\"%s\", layer=%d\n",
    init_for_nav,
    window->Name,
    g.NavLayer);
  if (init_for_nav) {
    SetNavID(0, g.NavLayer, 0, AnchorBBox());
    g.NavInitRequest = true;
    g.NavInitRequestFromMove = false;
    g.NavInitResultId = 0;
    g.NavInitResultRectRel = AnchorBBox();
    NavUpdateAnyRequestFlag();
  } else {
    g.NavId = window->NavLastIds[0];
    g.NavFocusScopeId = 0;
  }
}

static wabi::GfVec2f ANCHOR::NavCalcPreferredRefPos()
{
  AnchorContext &g = *G_CTX;
  if (g.NavDisableHighlight || !g.NavDisableMouseHover || !g.NavWindow) {
    // Mouse (we need a fallback in case the mouse becomes invalid after being used)
    if (IsMousePosValid(&g.IO.MousePos))
      return g.IO.MousePos;
    return g.LastValidMousePos;
  } else {
    // When navigation is active and mouse is disabled, decide on an arbitrary position around the
    // bottom left of the currently navigated item.
    const AnchorBBox &rect_rel = g.NavWindow->NavRectRel[g.NavLayer];
    wabi::GfVec2f pos = g.NavWindow->Pos +
                  wabi::GfVec2f(
                    rect_rel.Min[0] + AnchorMin(g.Style.FramePadding[0] * 4, rect_rel.GetWidth()),
                    rect_rel.Max[1] - AnchorMin(g.Style.FramePadding[1], rect_rel.GetHeight()));
    AnchorViewport *viewport = GetMainViewport();
    return AnchorFloor(AnchorClamp(
      pos,
      viewport->Pos,
      viewport->Pos + viewport->Size));  // AnchorFloor() is important because non-integer
                                         // mouse position application in backend might be
                                         // lossy and result in undesirable non-zero delta.
  }
}

float ANCHOR::GetNavInputAmount(AnchorNavInput n, ANCHOR_InputReadMode mode)
{
  AnchorContext &g = *G_CTX;
  if (mode == ANCHOR_InputReadMode_Down)
    return g.IO.NavInputs[n];  // Instant, read analog input (0.0f..1.0f, as provided by user)

  const float t = g.IO.NavInputsDownDuration[n];
  if (t < 0.0f && mode == ANCHOR_InputReadMode_Released)  // Return 1.0f when just released, no
                                                          // repeat, ignore analog input.
    return (g.IO.NavInputsDownDurationPrev[n] >= 0.0f ? 1.0f : 0.0f);
  if (t < 0.0f)
    return 0.0f;
  if (mode == ANCHOR_InputReadMode_Pressed)  // Return 1.0f when just pressed, no repeat, ignore
                                             // analog input.
    return (t == 0.0f) ? 1.0f : 0.0f;
  if (mode == ANCHOR_InputReadMode_Repeat)
    return (float)CalcTypematicRepeatAmount(t - g.IO.DeltaTime,
                                            t,
                                            g.IO.KeyRepeatDelay * 0.72f,
                                            g.IO.KeyRepeatRate * 0.80f);
  if (mode == ANCHOR_InputReadMode_RepeatSlow)
    return (float)CalcTypematicRepeatAmount(t - g.IO.DeltaTime,
                                            t,
                                            g.IO.KeyRepeatDelay * 1.25f,
                                            g.IO.KeyRepeatRate * 2.00f);
  if (mode == ANCHOR_InputReadMode_RepeatFast)
    return (float)CalcTypematicRepeatAmount(t - g.IO.DeltaTime,
                                            t,
                                            g.IO.KeyRepeatDelay * 0.72f,
                                            g.IO.KeyRepeatRate * 0.30f);
  return 0.0f;
}

wabi::GfVec2f ANCHOR::GetNavInputAmount2d(AnchorNavDirSourceFlags dir_sources,
                                    ANCHOR_InputReadMode mode,
                                    float slow_factor,
                                    float fast_factor)
{
  wabi::GfVec2f delta(0.0f, 0.0f);
  if (dir_sources & AnchorNavDirSourceFlags_Keyboard)
    delta += wabi::GfVec2f(GetNavInputAmount(AnchorNavInput_KeyRight_, mode) -
                       GetNavInputAmount(AnchorNavInput_KeyLeft_, mode),
                     GetNavInputAmount(AnchorNavInput_KeyDown_, mode) -
                       GetNavInputAmount(AnchorNavInput_KeyUp_, mode));
  if (dir_sources & AnchorNavDirSourceFlags_PadDPad)
    delta += wabi::GfVec2f(GetNavInputAmount(AnchorNavInput_DpadRight, mode) -
                       GetNavInputAmount(AnchorNavInput_DpadLeft, mode),
                     GetNavInputAmount(AnchorNavInput_DpadDown, mode) -
                       GetNavInputAmount(AnchorNavInput_DpadUp, mode));
  if (dir_sources & AnchorNavDirSourceFlags_PadLStick)
    delta += wabi::GfVec2f(GetNavInputAmount(AnchorNavInput_LStickRight, mode) -
                       GetNavInputAmount(AnchorNavInput_LStickLeft, mode),
                     GetNavInputAmount(AnchorNavInput_LStickDown, mode) -
                       GetNavInputAmount(AnchorNavInput_LStickUp, mode));
  if (slow_factor != 0.0f && IsNavInputDown(AnchorNavInput_TweakSlow))
    delta *= slow_factor;
  if (fast_factor != 0.0f && IsNavInputDown(AnchorNavInput_TweakFast))
    delta *= fast_factor;
  return delta;
}

static void ANCHOR::NavUpdate()
{
  AnchorContext &g = *G_CTX;
  AnchorIO &io = g.IO;

  io.WantSetMousePos = false;
  g.NavWrapRequestWindow = NULL;
  g.NavWrapRequestFlags = AnchorNavMoveFlags_None;
#if 0
    if (g.NavScoringCount > 0) ANCHOR_DEBUG_LOG("NavScoringCount %d for '%s' layer %d (Init:%d, Move:%d)\n", g.FrameCount, g.NavScoringCount, g.NavWindow ? g.NavWindow->Name : "NULL", g.NavLayer, g.NavInitRequest || g.NavInitResultId != 0, g.NavMoveRequest);
#endif

  // Set input source as Gamepad when buttons are pressed (as some features differs when used with
  // Gamepad vs Keyboard) (do it before we map Keyboard input!)
  bool nav_keyboard_active = (io.ConfigFlags & AnchorConfigFlags_NavEnableKeyboard) != 0;
  bool nav_gamepad_active = (io.ConfigFlags & AnchorConfigFlags_NavEnableGamepad) != 0 &&
                            (io.BackendFlags & AnchorBackendFlags_HasGamepad) != 0;
  if (nav_gamepad_active && g.NavInputSource != ANCHORInputSource_Gamepad) {
    if (io.NavInputs[AnchorNavInput_Activate] > 0.0f ||
        io.NavInputs[AnchorNavInput_Input] > 0.0f || io.NavInputs[AnchorNavInput_Cancel] > 0.0f ||
        io.NavInputs[AnchorNavInput_Menu] > 0.0f || io.NavInputs[AnchorNavInput_DpadLeft] > 0.0f ||
        io.NavInputs[AnchorNavInput_DpadRight] > 0.0f ||
        io.NavInputs[AnchorNavInput_DpadUp] > 0.0f || io.NavInputs[AnchorNavInput_DpadDown] > 0.0f)
      g.NavInputSource = ANCHORInputSource_Gamepad;
  }

  // Update Keyboard->Nav inputs mapping
  if (nav_keyboard_active) {
#define NAV_MAP_KEY(_KEY, _NAV_INPUT)                \
  do {                                               \
    if (IsKeyDown(io.KeyMap[_KEY])) {                \
      io.NavInputs[_NAV_INPUT] = 1.0f;               \
      g.NavInputSource = ANCHORInputSource_Keyboard; \
    }                                                \
  } while (0)
    NAV_MAP_KEY(AnchorKey_Space, AnchorNavInput_Activate);
    NAV_MAP_KEY(AnchorKey_Enter, AnchorNavInput_Input);
    NAV_MAP_KEY(AnchorKey_Escape, AnchorNavInput_Cancel);
    NAV_MAP_KEY(AnchorKey_LeftArrow, AnchorNavInput_KeyLeft_);
    NAV_MAP_KEY(AnchorKey_RightArrow, AnchorNavInput_KeyRight_);
    NAV_MAP_KEY(AnchorKey_UpArrow, AnchorNavInput_KeyUp_);
    NAV_MAP_KEY(AnchorKey_DownArrow, AnchorNavInput_KeyDown_);
    if (io.KeyCtrl)
      io.NavInputs[AnchorNavInput_TweakSlow] = 1.0f;
    if (io.KeyShift)
      io.NavInputs[AnchorNavInput_TweakFast] = 1.0f;

    // AltGR is normally Alt+Ctrl but we can't reliably detect it (not all backends/systems/layout
    // emit it as Alt+Ctrl) But also even on keyboards without AltGR we don't want Alt+Ctrl to open
    // menu anyway.
    if (io.KeyAlt && !io.KeyCtrl)
      io.NavInputs[AnchorNavInput_KeyMenu_] = 1.0f;

    // We automatically cancel toggling nav layer when any text has been typed while holding Alt.
    // (See #370)
    if (io.KeyAlt && !io.KeyCtrl && g.NavWindowingToggleLayer && io.InputQueueCharacters.Size > 0)
      g.NavWindowingToggleLayer = false;

#undef NAV_MAP_KEY
  }
  memcpy(io.NavInputsDownDurationPrev, io.NavInputsDownDuration, sizeof(io.NavInputsDownDuration));
  for (int i = 0; i < ANCHOR_ARRAYSIZE(io.NavInputs); i++)
    io.NavInputsDownDuration[i] = (io.NavInputs[i] > 0.0f) ?
                                    (io.NavInputsDownDuration[i] < 0.0f ?
                                       0.0f :
                                       io.NavInputsDownDuration[i] + io.DeltaTime) :
                                    -1.0f;

  // Process navigation init request (select first/default focus)
  if (g.NavInitResultId != 0)
    NavUpdateInitResult();
  g.NavInitRequest = false;
  g.NavInitRequestFromMove = false;
  g.NavInitResultId = 0;
  g.NavJustMovedToId = 0;

  // Process navigation move request
  if (g.NavMoveRequest)
    NavUpdateMoveResult();

  // When a forwarded move request failed, we restore the highlight that we disabled during the
  // forward frame
  if (g.NavMoveRequestForward == ANCHORNavForward_ForwardActive) {
    ANCHOR_ASSERT(g.NavMoveRequest);
    if (g.NavMoveResultLocal.ID == 0 && g.NavMoveResultOther.ID == 0)
      g.NavDisableHighlight = false;
    g.NavMoveRequestForward = ANCHORNavForward_None;
  }

  // Apply application mouse position movement, after we had a chance to process move request
  // result.
  if (g.NavMousePosDirty && g.NavIdIsAlive) {
    // Set mouse position given our knowledge of the navigated item position from last frame
    if ((io.ConfigFlags & AnchorConfigFlags_NavEnableSetMousePos) &&
        (io.BackendFlags & AnchorBackendFlags_HasSetMousePos))
      if (!g.NavDisableHighlight && g.NavDisableMouseHover && g.NavWindow) {
        io.MousePos = io.MousePosPrev = NavCalcPreferredRefPos();
        io.WantSetMousePos = true;
      }
    g.NavMousePosDirty = false;
  }
  g.NavIdIsAlive = false;
  g.NavJustTabbedId = 0;
  ANCHOR_ASSERT(g.NavLayer == 0 || g.NavLayer == 1);

  // Store our return window (for returning from Layer 1 to Layer 0) and clear it as soon as we
  // step back in our own Layer 0
  if (g.NavWindow)
    NavSaveLastChildNavWindowIntoParent(g.NavWindow);
  if (g.NavWindow && g.NavWindow->NavLastChildNavWindow != NULL &&
      g.NavLayer == ANCHORNavLayer_Main)
    g.NavWindow->NavLastChildNavWindow = NULL;

  // Update CTRL+TAB and Windowing features (hold Square to move/resize/etc.)
  NavUpdateWindowing();

  // Set output flags for user application
  io.NavActive = (nav_keyboard_active || nav_gamepad_active) && g.NavWindow &&
                 !(g.NavWindow->Flags & AnchorWindowFlags_NoNavInputs);
  io.NavVisible = (io.NavActive && g.NavId != 0 && !g.NavDisableHighlight) ||
                  (g.NavWindowingTarget != NULL);

  // Process NavCancel input (to close a popup, get back to parent, clear focus)
  if (IsNavInputTest(AnchorNavInput_Cancel, ANCHOR_InputReadMode_Pressed)) {
    ANCHOR_DEBUG_LOG_NAV("[nav] AnchorNavInput_Cancel\n");
    if (g.ActiveId != 0) {
      if (!IsActiveIdUsingNavInput(AnchorNavInput_Cancel))
        ClearActiveID();
    } else if (g.NavLayer != ANCHORNavLayer_Main) {
      // Leave the "menu" layer
      NavRestoreLayer(ANCHORNavLayer_Main);
    } else if (g.NavWindow && g.NavWindow != g.NavWindow->RootWindow &&
               !(g.NavWindow->Flags & AnchorWindowFlags_Popup) && g.NavWindow->ParentWindow) {
      // Exit child window
      AnchorWindow *child_window = g.NavWindow;
      AnchorWindow *parent_window = g.NavWindow->ParentWindow;
      ANCHOR_ASSERT(child_window->ChildId != 0);
      AnchorBBox child_rect = child_window->Rect();
      FocusWindow(parent_window);
      SetNavID(
        child_window->ChildId,
        ANCHORNavLayer_Main,
        0,
        AnchorBBox(child_rect.Min - parent_window->Pos, child_rect.Max - parent_window->Pos));
    } else if (g.OpenPopupStack.Size > 0) {
      // Close open popup/menu
      if (!(g.OpenPopupStack.back().Window->Flags & AnchorWindowFlags_Modal))
        ClosePopupToLevel(g.OpenPopupStack.Size - 1, true);
    } else {
      // Clear NavLastId for popups but keep it for regular child window so we can leave one and
      // come back where we were
      if (g.NavWindow && ((g.NavWindow->Flags & AnchorWindowFlags_Popup) ||
                          !(g.NavWindow->Flags & AnchorWindowFlags_ChildWindow)))
        g.NavWindow->NavLastIds[0] = 0;
      g.NavId = g.NavFocusScopeId = 0;
    }
  }

  // Process manual activation request
  g.NavActivateId = g.NavActivateDownId = g.NavActivatePressedId = g.NavInputId = 0;
  if (g.NavId != 0 && !g.NavDisableHighlight && !g.NavWindowingTarget && g.NavWindow &&
      !(g.NavWindow->Flags & AnchorWindowFlags_NoNavInputs)) {
    bool activate_down = IsNavInputDown(AnchorNavInput_Activate);
    bool activate_pressed = activate_down &&
                            IsNavInputTest(AnchorNavInput_Activate, ANCHOR_InputReadMode_Pressed);
    if (g.ActiveId == 0 && activate_pressed)
      g.NavActivateId = g.NavId;
    if ((g.ActiveId == 0 || g.ActiveId == g.NavId) && activate_down)
      g.NavActivateDownId = g.NavId;
    if ((g.ActiveId == 0 || g.ActiveId == g.NavId) && activate_pressed)
      g.NavActivatePressedId = g.NavId;
    if ((g.ActiveId == 0 || g.ActiveId == g.NavId) &&
        IsNavInputTest(AnchorNavInput_Input, ANCHOR_InputReadMode_Pressed))
      g.NavInputId = g.NavId;
  }
  if (g.NavWindow && (g.NavWindow->Flags & AnchorWindowFlags_NoNavInputs))
    g.NavDisableHighlight = true;
  if (g.NavActivateId != 0)
    ANCHOR_ASSERT(g.NavActivateDownId == g.NavActivateId);
  g.NavMoveRequest = false;

  // Process programmatic activation request
  if (g.NavNextActivateId != 0)
    g.NavActivateId = g.NavActivateDownId = g.NavActivatePressedId = g.NavInputId =
      g.NavNextActivateId;
  g.NavNextActivateId = 0;

  // Initiate directional inputs request
  if (g.NavMoveRequestForward == ANCHORNavForward_None) {
    g.NavMoveDir = AnchorDir_None;
    g.NavMoveRequestFlags = AnchorNavMoveFlags_None;
    if (g.NavWindow && !g.NavWindowingTarget &&
        !(g.NavWindow->Flags & AnchorWindowFlags_NoNavInputs)) {
      const ANCHOR_InputReadMode read_mode = ANCHOR_InputReadMode_Repeat;
      if (!IsActiveIdUsingNavDir(AnchorDir_Left) &&
          (IsNavInputTest(AnchorNavInput_DpadLeft, read_mode) ||
           IsNavInputTest(AnchorNavInput_KeyLeft_, read_mode))) {
        g.NavMoveDir = AnchorDir_Left;
      }
      if (!IsActiveIdUsingNavDir(AnchorDir_Right) &&
          (IsNavInputTest(AnchorNavInput_DpadRight, read_mode) ||
           IsNavInputTest(AnchorNavInput_KeyRight_, read_mode))) {
        g.NavMoveDir = AnchorDir_Right;
      }
      if (!IsActiveIdUsingNavDir(AnchorDir_Up) &&
          (IsNavInputTest(AnchorNavInput_DpadUp, read_mode) ||
           IsNavInputTest(AnchorNavInput_KeyUp_, read_mode))) {
        g.NavMoveDir = AnchorDir_Up;
      }
      if (!IsActiveIdUsingNavDir(AnchorDir_Down) &&
          (IsNavInputTest(AnchorNavInput_DpadDown, read_mode) ||
           IsNavInputTest(AnchorNavInput_KeyDown_, read_mode))) {
        g.NavMoveDir = AnchorDir_Down;
      }
    }
    g.NavMoveClipDir = g.NavMoveDir;
  } else {
    // Forwarding previous request (which has been modified, e.g. wrap around menus rewrite the
    // requests with a starting rectangle at the other side of the window) (Preserve
    // g.NavMoveRequestFlags, g.NavMoveClipDir which were set by the NavMoveRequestForward()
    // function)
    ANCHOR_ASSERT(g.NavMoveDir != AnchorDir_None && g.NavMoveClipDir != AnchorDir_None);
    ANCHOR_ASSERT(g.NavMoveRequestForward == ANCHORNavForward_ForwardQueued);
    ANCHOR_DEBUG_LOG_NAV("[nav] NavMoveRequestForward %d\n", g.NavMoveDir);
    g.NavMoveRequestForward = ANCHORNavForward_ForwardActive;
  }

  // Update PageUp/PageDown/Home/End scroll
  // FIXME-NAV: Consider enabling those keys even without the master
  // AnchorConfigFlags_NavEnableKeyboard flag?
  float nav_scoring_rect_offset_y = 0.0f;
  if (nav_keyboard_active)
    nav_scoring_rect_offset_y = NavUpdatePageUpPageDown();

  // If we initiate a movement request and have no current NavId, we initiate a InitDefautRequest
  // that will be used as a fallback if the direction fails to find a match
  if (g.NavMoveDir != AnchorDir_None) {
    g.NavMoveRequest = true;
    g.NavMoveRequestKeyMods = io.KeyMods;
    g.NavMoveDirLast = g.NavMoveDir;
  }
  if (g.NavMoveRequest && g.NavId == 0) {
    ANCHOR_DEBUG_LOG_NAV("[nav] NavInitRequest: from move, window \"%s\", layer=%d\n",
                         g.NavWindow->Name,
                         g.NavLayer);
    g.NavInitRequest = g.NavInitRequestFromMove = true;
    // Reassigning with same value, we're being explicit here.
    g.NavInitResultId = 0;  // -V1048
    g.NavDisableHighlight = false;
  }
  NavUpdateAnyRequestFlag();

  // Scrolling
  if (g.NavWindow && !(g.NavWindow->Flags & AnchorWindowFlags_NoNavInputs) &&
      !g.NavWindowingTarget) {
    // *Fallback* manual-scroll with Nav directional keys when window has no navigable item
    AnchorWindow *window = g.NavWindow;
    const float scroll_speed = IM_ROUND(
      window->CalcFontSize() * 100 * io.DeltaTime);  // We need round the scrolling speed because
                                                     // sub-pixel scroll isn't reliably supported.
    if (window->DC.NavLayersActiveMask == 0x00 && window->DC.NavHasScroll && g.NavMoveRequest) {
      if (g.NavMoveDir == AnchorDir_Left || g.NavMoveDir == AnchorDir_Right)
        SetScrollX(window,
                   AnchorFloor(window->Scroll[0] +
                               ((g.NavMoveDir == AnchorDir_Left) ? -1.0f : +1.0f) * scroll_speed));
      if (g.NavMoveDir == AnchorDir_Up || g.NavMoveDir == AnchorDir_Down)
        SetScrollY(window,
                   AnchorFloor(window->Scroll[1] +
                               ((g.NavMoveDir == AnchorDir_Up) ? -1.0f : +1.0f) * scroll_speed));
    }

    // *Normal* Manual scroll with NavScrollXXX keys
    // Next movement request will clamp the NavId reference rectangle to the visible area, so
    // navigation will resume within those bounds.
    wabi::GfVec2f scroll_dir = GetNavInputAmount2d(AnchorNavDirSourceFlags_PadLStick,
                                             ANCHOR_InputReadMode_Down,
                                             1.0f / 10.0f,
                                             10.0f);
    if (scroll_dir[0] != 0.0f && window->ScrollbarX)
      SetScrollX(window, AnchorFloor(window->Scroll[0] + scroll_dir[0] * scroll_speed));
    if (scroll_dir[1] != 0.0f)
      SetScrollY(window, AnchorFloor(window->Scroll[1] + scroll_dir[1] * scroll_speed));
  }

  // Reset search results
  g.NavMoveResultLocal.Clear();
  g.NavMoveResultLocalVisibleSet.Clear();
  g.NavMoveResultOther.Clear();

  // When using gamepad, we project the reference nav bounding box into window visible area.
  // This is to allow resuming navigation inside the visible area after doing a large amount of
  // scrolling, since with gamepad every movements are relative (can't focus a visible object like
  // we can with the mouse).
  if (g.NavMoveRequest && g.NavInputSource == ANCHORInputSource_Gamepad &&
      g.NavLayer == ANCHORNavLayer_Main) {
    AnchorWindow *window = g.NavWindow;
    AnchorBBox window_rect_rel(window->InnerRect.Min - window->Pos - wabi::GfVec2f(1, 1),
                               window->InnerRect.Max - window->Pos + wabi::GfVec2f(1, 1));
    if (!window_rect_rel.Contains(window->NavRectRel[g.NavLayer])) {
      ANCHOR_DEBUG_LOG_NAV("[nav] NavMoveRequest: clamp NavRectRel\n");
      float pad = window->CalcFontSize() * 0.5f;
      window_rect_rel.Expand(
        wabi::GfVec2f(-AnchorMin(window_rect_rel.GetWidth(), pad),
                -AnchorMin(window_rect_rel.GetHeight(),
                           pad)));  // Terrible approximation for the intent of starting
                                    // navigation from first fully visible item
      window->NavRectRel[g.NavLayer].ClipWithFull(window_rect_rel);
      g.NavId = g.NavFocusScopeId = 0;
    }
  }

  // For scoring we use a single segment on the left side our current item bounding box (not
  // touching the edge to avoid box overlap with zero-spaced items)
  AnchorBBox nav_rect_rel = g.NavWindow && !g.NavWindow->NavRectRel[g.NavLayer].IsInverted() ?
                              g.NavWindow->NavRectRel[g.NavLayer] :
                              AnchorBBox(0, 0, 0, 0);
  g.NavScoringRect = g.NavWindow ? AnchorBBox(g.NavWindow->Pos + nav_rect_rel.Min,
                                              g.NavWindow->Pos + nav_rect_rel.Max) :
                                   AnchorBBox(0, 0, 0, 0);
  g.NavScoringRect.TranslateY(nav_scoring_rect_offset_y);
  g.NavScoringRect.Min[0] = AnchorMin(g.NavScoringRect.Min[0] + 1.0f, g.NavScoringRect.Max[0]);
  g.NavScoringRect.Max[0] = g.NavScoringRect.Min[0];
  ANCHOR_ASSERT(
    !g.NavScoringRect
       .IsInverted());  // Ensure if we have a finite, non-inverted bounding box here will
                        // allows us to remove extraneous AnchorFabs() calls in NavScoreItem().
  // GetForegroundDrawList()->AddRect(g.NavScoringRectScreen.Min, g.NavScoringRectScreen.Max,
  // ANCHOR_COL32(255,200,0,255)); // [DEBUG]
  g.NavScoringCount = 0;
#if ANCHOR_DEBUG_NAV_RECTS
  if (g.NavWindow) {
    AnchorDrawList *draw_list = GetForegroundDrawList(g.NavWindow);
    if (1) {
      for (int layer = 0; layer < 2; layer++)
        draw_list->AddRect(g.NavWindow->Pos + g.NavWindow->NavRectRel[layer].Min,
                           g.NavWindow->Pos + g.NavWindow->NavRectRel[layer].Max,
                           ANCHOR_COL32(255, 200, 0, 255));
    }  // [DEBUG]
    if (1) {
      AnchorU32 col = (!g.NavWindow->Hidden) ? ANCHOR_COL32(255, 0, 255, 255) :
                                               ANCHOR_COL32(255, 0, 0, 255);
      wabi::GfVec2f p = NavCalcPreferredRefPos();
      char buf[32];
      AnchorFormatString(buf, 32, "%d", g.NavLayer);
      draw_list->AddCircleFilled(p, 3.0f, col);
      draw_list->AddText(NULL, 13.0f, p + wabi::GfVec2f(8, -4), col, buf);
    }
  }
#endif
}

static void ANCHOR::NavUpdateInitResult()
{
  // In very rare cases g.NavWindow may be null (e.g. clearing focus after requesting an init
  // request, which does happen when releasing Alt while clicking on void)
  AnchorContext &g = *G_CTX;
  if (!g.NavWindow)
    return;

  // Apply result from previous navigation init request (will typically select the first item,
  // unless SetItemDefaultFocus() has been called)
  // FIXME-NAV: On _NavFlattened windows, g.NavWindow will only be updated during subsequent frame.
  // Not a problem currently.
  ANCHOR_DEBUG_LOG_NAV("[nav] NavInitRequest: result NavID 0x%08X in Layer %d Window \"%s\"\n",
                       g.NavInitResultId,
                       g.NavLayer,
                       g.NavWindow->Name);
  SetNavID(g.NavInitResultId, g.NavLayer, 0, g.NavInitResultRectRel);
  if (g.NavInitRequestFromMove) {
    g.NavDisableHighlight = false;
    g.NavDisableMouseHover = g.NavMousePosDirty = true;
  }
}

// Apply result from previous frame navigation directional move request
static void ANCHOR::NavUpdateMoveResult()
{
  AnchorContext &g = *G_CTX;
  if (g.NavMoveResultLocal.ID == 0 && g.NavMoveResultOther.ID == 0) {
    // In a situation when there is no results but NavId != 0, re-enable the Navigation highlight
    // (because g.NavId is not considered as a possible result)
    if (g.NavId != 0) {
      g.NavDisableHighlight = false;
      g.NavDisableMouseHover = true;
    }
    return;
  }

  // Select which result to use
  AnchorNavItemData *result = (g.NavMoveResultLocal.ID != 0) ? &g.NavMoveResultLocal :
                                                               &g.NavMoveResultOther;

  // PageUp/PageDown behavior first jumps to the bottom/top mostly visible item, _otherwise_ use
  // the result from the previous/next page.
  if (g.NavMoveRequestFlags & AnchorNavMoveFlags_AlsoScoreVisibleSet)
    if (g.NavMoveResultLocalVisibleSet.ID != 0 && g.NavMoveResultLocalVisibleSet.ID != g.NavId)
      result = &g.NavMoveResultLocalVisibleSet;

  // Maybe entering a flattened child from the outside? In this case solve the tie using the
  // regular scoring rules.
  if (result != &g.NavMoveResultOther && g.NavMoveResultOther.ID != 0 &&
      g.NavMoveResultOther.Window->ParentWindow == g.NavWindow)
    if ((g.NavMoveResultOther.DistBox < result->DistBox) ||
        (g.NavMoveResultOther.DistBox == result->DistBox &&
         g.NavMoveResultOther.DistCenter < result->DistCenter))
      result = &g.NavMoveResultOther;
  ANCHOR_ASSERT(g.NavWindow && result->Window);

  // Scroll to keep newly navigated item fully into view.
  if (g.NavLayer == ANCHORNavLayer_Main) {
    wabi::GfVec2f delta_scroll;
    if (g.NavMoveRequestFlags & AnchorNavMoveFlags_ScrollToEdge) {
      float scroll_target = (g.NavMoveDir == AnchorDir_Up) ? result->Window->ScrollMax[1] : 0.0f;
      delta_scroll[1] = result->Window->Scroll[1] - scroll_target;
      SetScrollY(result->Window, scroll_target);
    } else {
      AnchorBBox rect_abs = AnchorBBox(result->RectRel.Min + result->Window->Pos,
                                       result->RectRel.Max + result->Window->Pos);
      delta_scroll = ScrollToBringRectIntoView(result->Window, rect_abs);
    }

    // Offset our result position so mouse position can be applied immediately after in NavUpdate()
    result->RectRel.TranslateX(-delta_scroll[0]);
    result->RectRel.TranslateY(-delta_scroll[1]);
  }

  ClearActiveID();
  g.NavWindow = result->Window;
  if (g.NavId != result->ID) {
    // Don't set NavJustMovedToId if just landed on the same spot (which may happen with
    // AnchorNavMoveFlags_AllowCurrentNavId)
    g.NavJustMovedToId = result->ID;
    g.NavJustMovedToFocusScopeId = result->FocusScopeId;
    g.NavJustMovedToKeyMods = g.NavMoveRequestKeyMods;
  }
  ANCHOR_DEBUG_LOG_NAV("[nav] NavMoveRequest: result NavID 0x%08X in Layer %d Window \"%s\"\n",
                       result->ID,
                       g.NavLayer,
                       g.NavWindow->Name);
  SetNavID(result->ID, g.NavLayer, result->FocusScopeId, result->RectRel);
  g.NavDisableHighlight = false;
  g.NavDisableMouseHover = g.NavMousePosDirty = true;
}

// Handle PageUp/PageDown/Home/End keys
static float ANCHOR::NavUpdatePageUpPageDown()
{
  AnchorContext &g = *G_CTX;
  AnchorIO &io = g.IO;

  if (g.NavMoveDir != AnchorDir_None || g.NavWindow == NULL)
    return 0.0f;
  if ((g.NavWindow->Flags & AnchorWindowFlags_NoNavInputs) || g.NavWindowingTarget != NULL ||
      g.NavLayer != ANCHORNavLayer_Main)
    return 0.0f;

  AnchorWindow *window = g.NavWindow;
  const bool page_up_held = IsKeyDown(io.KeyMap[AnchorKey_PageUp]) &&
                            !IsActiveIdUsingKey(AnchorKey_PageUp);
  const bool page_down_held = IsKeyDown(io.KeyMap[AnchorKey_PageDown]) &&
                              !IsActiveIdUsingKey(AnchorKey_PageDown);
  const bool home_pressed = IsKeyPressed(io.KeyMap[AnchorKey_Home]) &&
                            !IsActiveIdUsingKey(AnchorKey_Home);
  const bool end_pressed = IsKeyPressed(io.KeyMap[AnchorKey_End]) &&
                           !IsActiveIdUsingKey(AnchorKey_End);
  if (page_up_held != page_down_held ||
      home_pressed != end_pressed)  // If either (not both) are pressed
  {
    if (window->DC.NavLayersActiveMask == 0x00 && window->DC.NavHasScroll) {
      // Fallback manual-scroll when window has no navigable item
      if (IsKeyPressed(io.KeyMap[AnchorKey_PageUp], true))
        SetScrollY(window, window->Scroll[1] - window->InnerRect.GetHeight());
      else if (IsKeyPressed(io.KeyMap[AnchorKey_PageDown], true))
        SetScrollY(window, window->Scroll[1] + window->InnerRect.GetHeight());
      else if (home_pressed)
        SetScrollY(window, 0.0f);
      else if (end_pressed)
        SetScrollY(window, window->ScrollMax[1]);
    } else {
      AnchorBBox &nav_rect_rel = window->NavRectRel[g.NavLayer];
      const float page_offset_y = AnchorMax(
        0.0f,
        window->InnerRect.GetHeight() - window->CalcFontSize() * 1.0f + nav_rect_rel.GetHeight());
      float nav_scoring_rect_offset_y = 0.0f;
      if (IsKeyPressed(io.KeyMap[AnchorKey_PageUp], true)) {
        nav_scoring_rect_offset_y = -page_offset_y;
        g.NavMoveDir = AnchorDir_Down;  // Because our scoring rect is offset up, we request the
                                        // down direction (so we can always land on the last item)
        g.NavMoveClipDir = AnchorDir_Up;
        g.NavMoveRequestFlags = AnchorNavMoveFlags_AllowCurrentNavId |
                                AnchorNavMoveFlags_AlsoScoreVisibleSet;
      } else if (IsKeyPressed(io.KeyMap[AnchorKey_PageDown], true)) {
        nav_scoring_rect_offset_y = +page_offset_y;
        g.NavMoveDir = AnchorDir_Up;  // Because our scoring rect is offset down, we request the
                                      // up direction (so we can always land on the last item)
        g.NavMoveClipDir = AnchorDir_Down;
        g.NavMoveRequestFlags = AnchorNavMoveFlags_AllowCurrentNavId |
                                AnchorNavMoveFlags_AlsoScoreVisibleSet;
      } else if (home_pressed) {
        // FIXME-NAV: handling of Home/End is assuming that the top/bottom most item will be
        // visible with Scroll[1] == 0/ScrollMax[1] Scrolling will be handled via the
        // AnchorNavMoveFlags_ScrollToEdge flag, we don't scroll immediately to avoid scrolling
        // happening before nav result. Preserve current horizontal position if we have any.
        nav_rect_rel.Min[1] = nav_rect_rel.Max[1] = -window->Scroll[1];
        if (nav_rect_rel.IsInverted())
          nav_rect_rel.Min[0] = nav_rect_rel.Max[0] = 0.0f;
        g.NavMoveDir = AnchorDir_Down;
        g.NavMoveRequestFlags = AnchorNavMoveFlags_AllowCurrentNavId |
                                AnchorNavMoveFlags_ScrollToEdge;
      } else if (end_pressed) {
        nav_rect_rel.Min[1] = nav_rect_rel.Max[1] = window->ScrollMax[1] + window->SizeFull[1] -
                                                    window->Scroll[1];
        if (nav_rect_rel.IsInverted())
          nav_rect_rel.Min[0] = nav_rect_rel.Max[0] = 0.0f;
        g.NavMoveDir = AnchorDir_Up;
        g.NavMoveRequestFlags = AnchorNavMoveFlags_AllowCurrentNavId |
                                AnchorNavMoveFlags_ScrollToEdge;
      }
      return nav_scoring_rect_offset_y;
    }
  }
  return 0.0f;
}

static void ANCHOR::NavEndFrame()
{
  AnchorContext &g = *G_CTX;

  // Show CTRL+TAB list window
  if (g.NavWindowingTarget != NULL)
    NavUpdateWindowingOverlay();

  // Perform wrap-around in menus
  AnchorWindow *window = g.NavWrapRequestWindow;
  AnchorNavMoveFlags move_flags = g.NavWrapRequestFlags;
  if (window != NULL && g.NavWindow == window && NavMoveRequestButNoResultYet() &&
      g.NavMoveRequestForward == ANCHORNavForward_None && g.NavLayer == ANCHORNavLayer_Main) {
    ANCHOR_ASSERT(move_flags != 0);  // No points calling this with no wrapping
    AnchorBBox bb_rel = window->NavRectRel[0];

    AnchorDir clip_dir = g.NavMoveDir;
    if (g.NavMoveDir == AnchorDir_Left &&
        (move_flags & (AnchorNavMoveFlags_WrapX | AnchorNavMoveFlags_LoopX))) {
      bb_rel.Min[0] = bb_rel.Max[0] = AnchorMax(window->SizeFull[0],
                                                window->ContentSize[0] +
                                                  window->WindowPadding[0] * 2.0f) -
                                      window->Scroll[0];
      if (move_flags & AnchorNavMoveFlags_WrapX) {
        bb_rel.TranslateY(-bb_rel.GetHeight());
        clip_dir = AnchorDir_Up;
      }
      NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
    }
    if (g.NavMoveDir == AnchorDir_Right &&
        (move_flags & (AnchorNavMoveFlags_WrapX | AnchorNavMoveFlags_LoopX))) {
      bb_rel.Min[0] = bb_rel.Max[0] = -window->Scroll[0];
      if (move_flags & AnchorNavMoveFlags_WrapX) {
        bb_rel.TranslateY(+bb_rel.GetHeight());
        clip_dir = AnchorDir_Down;
      }
      NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
    }
    if (g.NavMoveDir == AnchorDir_Up &&
        (move_flags & (AnchorNavMoveFlags_WrapY | AnchorNavMoveFlags_LoopY))) {
      bb_rel.Min[1] = bb_rel.Max[1] = AnchorMax(window->SizeFull[1],
                                                window->ContentSize[1] +
                                                  window->WindowPadding[1] * 2.0f) -
                                      window->Scroll[1];
      if (move_flags & AnchorNavMoveFlags_WrapY) {
        bb_rel.TranslateX(-bb_rel.GetWidth());
        clip_dir = AnchorDir_Left;
      }
      NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
    }
    if (g.NavMoveDir == AnchorDir_Down &&
        (move_flags & (AnchorNavMoveFlags_WrapY | AnchorNavMoveFlags_LoopY))) {
      bb_rel.Min[1] = bb_rel.Max[1] = -window->Scroll[1];
      if (move_flags & AnchorNavMoveFlags_WrapY) {
        bb_rel.TranslateX(+bb_rel.GetWidth());
        clip_dir = AnchorDir_Right;
      }
      NavMoveRequestForward(g.NavMoveDir, clip_dir, bb_rel, move_flags);
    }
  }
}

static int ANCHOR::FindWindowFocusIndex(AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;
  TF_UNUSED(g);
  int order = window->FocusOrder;
  ANCHOR_ASSERT(g.WindowsFocusOrder[order] == window);
  return order;
}

static AnchorWindow *FindWindowNavFocusable(int i_start, int i_stop, int dir)  // FIXME-OPT O(N)
{
  AnchorContext &g = *G_CTX;
  for (int i = i_start; i >= 0 && i < g.WindowsFocusOrder.Size && i != i_stop; i += dir)
    if (ANCHOR::IsWindowNavFocusable(g.WindowsFocusOrder[i]))
      return g.WindowsFocusOrder[i];
  return NULL;
}

static void NavUpdateWindowingHighlightWindow(int focus_change_dir)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(g.NavWindowingTarget);
  if (g.NavWindowingTarget->Flags & AnchorWindowFlags_Modal)
    return;

  const int i_current = ANCHOR::FindWindowFocusIndex(g.NavWindowingTarget);
  AnchorWindow *window_target = FindWindowNavFocusable(i_current + focus_change_dir,
                                                       -INT_MAX,
                                                       focus_change_dir);
  if (!window_target)
    window_target = FindWindowNavFocusable(
      (focus_change_dir < 0) ? (g.WindowsFocusOrder.Size - 1) : 0,
      i_current,
      focus_change_dir);
  if (window_target)  // Don't reset windowing target if there's a single window in the list
    g.NavWindowingTarget = g.NavWindowingTargetAnim = window_target;
  g.NavWindowingToggleLayer = false;
}

// Windowing management mode
// Keyboard: CTRL+Tab (change focus/move/resize), Alt (toggle menu layer)
// Gamepad:  Hold Menu/Square (change focus/move/resize), Tap Menu/Square (toggle menu layer)
static void ANCHOR::NavUpdateWindowing()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *apply_focus_window = NULL;
  bool apply_toggle_layer = false;

  AnchorWindow *modal_window = GetTopMostPopupModal();
  bool allow_windowing = (modal_window == NULL);
  if (!allow_windowing)
    g.NavWindowingTarget = NULL;

  // Fade out
  if (g.NavWindowingTargetAnim && g.NavWindowingTarget == NULL) {
    g.NavWindowingHighlightAlpha = AnchorMax(g.NavWindowingHighlightAlpha - g.IO.DeltaTime * 10.0f,
                                             0.0f);
    if (g.DimBgRatio <= 0.0f && g.NavWindowingHighlightAlpha <= 0.0f)
      g.NavWindowingTargetAnim = NULL;
  }

  // Start CTRL-TAB or Square+L/R window selection
  bool start_windowing_with_gamepad = allow_windowing && !g.NavWindowingTarget &&
                                      IsNavInputTest(AnchorNavInput_Menu,
                                                     ANCHOR_InputReadMode_Pressed);
  bool start_windowing_with_keyboard = allow_windowing && !g.NavWindowingTarget && g.IO.KeyCtrl &&
                                       IsKeyPressedMap(AnchorKey_Tab) &&
                                       (g.IO.ConfigFlags & AnchorConfigFlags_NavEnableKeyboard);
  if (start_windowing_with_gamepad || start_windowing_with_keyboard)
    if (AnchorWindow *window = g.NavWindow ? g.NavWindow :
                                             FindWindowNavFocusable(g.WindowsFocusOrder.Size - 1,
                                                                    -INT_MAX,
                                                                    -1)) {
      g.NavWindowingTarget = g.NavWindowingTargetAnim =
        window->RootWindow;  // FIXME-DOCK: Will need to use RootWindowDockStop
      g.NavWindowingTimer = g.NavWindowingHighlightAlpha = 0.0f;
      g.NavWindowingToggleLayer = start_windowing_with_keyboard ? false : true;
      g.NavInputSource = start_windowing_with_keyboard ? ANCHORInputSource_Keyboard :
                                                         ANCHORInputSource_Gamepad;
    }

  // Gamepad update
  g.NavWindowingTimer += g.IO.DeltaTime;
  if (g.NavWindowingTarget && g.NavInputSource == ANCHORInputSource_Gamepad) {
    // Highlight only appears after a brief time holding the button, so that a fast tap on PadMenu
    // (to toggle NavLayer) doesn't add visual noise
    g.NavWindowingHighlightAlpha = AnchorMax(
      g.NavWindowingHighlightAlpha,
      AnchorSaturate((g.NavWindowingTimer - NAV_WINDOWING_HIGHLIGHT_DELAY) / 0.05f));

    // Select window to focus
    const int focus_change_dir = (int)IsNavInputTest(AnchorNavInput_FocusPrev,
                                                     ANCHOR_InputReadMode_RepeatSlow) -
                                 (int)IsNavInputTest(AnchorNavInput_FocusNext,
                                                     ANCHOR_InputReadMode_RepeatSlow);
    if (focus_change_dir != 0) {
      NavUpdateWindowingHighlightWindow(focus_change_dir);
      g.NavWindowingHighlightAlpha = 1.0f;
    }

    // Single press toggles NavLayer, long press with L/R apply actual focus on release (until then
    // the window was merely rendered top-most)
    if (!IsNavInputDown(AnchorNavInput_Menu)) {
      g.NavWindowingToggleLayer &= (g.NavWindowingHighlightAlpha <
                                    1.0f);  // Once button was held long enough we don't consider
                                            // it a tap-to-toggle-layer press anymore.
      if (g.NavWindowingToggleLayer && g.NavWindow)
        apply_toggle_layer = true;
      else if (!g.NavWindowingToggleLayer)
        apply_focus_window = g.NavWindowingTarget;
      g.NavWindowingTarget = NULL;
    }
  }

  // Keyboard: Focus
  if (g.NavWindowingTarget && g.NavInputSource == ANCHORInputSource_Keyboard) {
    // Visuals only appears after a brief time after pressing TAB the first time, so that a fast
    // CTRL+TAB doesn't add visual noise
    g.NavWindowingHighlightAlpha = AnchorMax(
      g.NavWindowingHighlightAlpha,
      AnchorSaturate((g.NavWindowingTimer - NAV_WINDOWING_HIGHLIGHT_DELAY) / 0.05f));  // 1.0f
    if (IsKeyPressedMap(AnchorKey_Tab, true))
      NavUpdateWindowingHighlightWindow(g.IO.KeyShift ? +1 : -1);
    if (!g.IO.KeyCtrl)
      apply_focus_window = g.NavWindowingTarget;
  }

  // Keyboard: Press and Release ALT to toggle menu layer
  // FIXME: We lack an explicit IO variable for "is the ANCHOR window focused", so compare mouse
  // validity to detect the common case of backend clearing releases all keys on ALT-TAB
  if (IsNavInputTest(AnchorNavInput_KeyMenu_, ANCHOR_InputReadMode_Pressed))
    g.NavWindowingToggleLayer = true;
  if ((g.ActiveId == 0 || g.ActiveIdAllowOverlap) && g.NavWindowingToggleLayer &&
      IsNavInputTest(AnchorNavInput_KeyMenu_, ANCHOR_InputReadMode_Released))
    if (IsMousePosValid(&g.IO.MousePos) == IsMousePosValid(&g.IO.MousePosPrev))
      apply_toggle_layer = true;

  // Move window
  if (g.NavWindowingTarget && !(g.NavWindowingTarget->Flags & AnchorWindowFlags_NoMove)) {
    wabi::GfVec2f move_delta;
    if (g.NavInputSource == ANCHORInputSource_Keyboard && !g.IO.KeyShift)
      move_delta = GetNavInputAmount2d(AnchorNavDirSourceFlags_Keyboard,
                                       ANCHOR_InputReadMode_Down);
    if (g.NavInputSource == ANCHORInputSource_Gamepad)
      move_delta = GetNavInputAmount2d(AnchorNavDirSourceFlags_PadLStick,
                                       ANCHOR_InputReadMode_Down);
    if (move_delta[0] != 0.0f || move_delta[1] != 0.0f) {
      const float NAV_MOVE_SPEED = 800.0f;
      const float move_speed = AnchorFloor(
        NAV_MOVE_SPEED * g.IO.DeltaTime *
        AnchorMin(g.IO.DisplayFramebufferScale[0],
                  g.IO.DisplayFramebufferScale[1]));  // FIXME: Doesn't handle variable framerate
                                                      // very well
      AnchorWindow *moving_window = g.NavWindowingTarget->RootWindow;
      SetWindowPos(moving_window, moving_window->Pos + move_delta * move_speed, AnchorCond_Always);
      MarkIniSettingsDirty(moving_window);
      g.NavDisableMouseHover = true;
    }
  }

  // Apply final focus
  if (apply_focus_window &&
      (g.NavWindow == NULL || apply_focus_window != g.NavWindow->RootWindow)) {
    ClearActiveID();
    g.NavDisableHighlight = false;
    g.NavDisableMouseHover = true;
    apply_focus_window = NavRestoreLastChildNavWindow(apply_focus_window);
    ClosePopupsOverWindow(apply_focus_window, false);
    FocusWindow(apply_focus_window);
    if (apply_focus_window->NavLastIds[0] == 0)
      NavInitWindow(apply_focus_window, false);

    // If the window has ONLY a menu layer (no main layer), select it directly
    // Use NavLayersActiveMaskNext since windows didn't have a chance to be Begin()-ed on this
    // frame, so CTRL+Tab where the keys are only held for 1 frame will be able to use correct
    // layers mask since the target window as already been previewed once.
    // FIXME-NAV: This should be done in NavInit.. or in FocusWindow... However in both of those
    // cases, we won't have a guarantee that windows has been visible before and therefore
    // NavLayersActiveMask* won't be valid.
    if (apply_focus_window->DC.NavLayersActiveMaskNext == (1 << ANCHORNavLayer_Menu))
      g.NavLayer = ANCHORNavLayer_Menu;
  }
  if (apply_focus_window)
    g.NavWindowingTarget = NULL;

  // Apply menu/layer toggle
  if (apply_toggle_layer && g.NavWindow) {
    ClearActiveID();

    // Move to parent menu if necessary
    AnchorWindow *new_nav_window = g.NavWindow;
    while (new_nav_window->ParentWindow &&
           (new_nav_window->DC.NavLayersActiveMask & (1 << ANCHORNavLayer_Menu)) == 0 &&
           (new_nav_window->Flags & AnchorWindowFlags_ChildWindow) != 0 &&
           (new_nav_window->Flags & (AnchorWindowFlags_Popup | AnchorWindowFlags_ChildMenu)) == 0)
      new_nav_window = new_nav_window->ParentWindow;
    if (new_nav_window != g.NavWindow) {
      AnchorWindow *old_nav_window = g.NavWindow;
      FocusWindow(new_nav_window);
      new_nav_window->NavLastChildNavWindow = old_nav_window;
    }
    g.NavDisableHighlight = false;
    g.NavDisableMouseHover = true;

    // Reinitialize navigation when entering menu bar with the Alt key.
    const ANCHORNavLayer new_nav_layer = (g.NavWindow->DC.NavLayersActiveMask &
                                          (1 << ANCHORNavLayer_Menu)) ?
                                           (ANCHORNavLayer)((int)g.NavLayer ^ 1) :
                                           ANCHORNavLayer_Main;
    if (new_nav_layer == ANCHORNavLayer_Menu)
      g.NavWindow->NavLastIds[new_nav_layer] = 0;
    NavRestoreLayer(new_nav_layer);
  }
}

// Window has already passed the IsWindowNavFocusable()
static const char *GetFallbackWindowNameForWindowingList(AnchorWindow *window)
{
  if (window->Flags & AnchorWindowFlags_Popup)
    return "(Popup)";
  if ((window->Flags & AnchorWindowFlags_MenuBar) && strcmp(window->Name, "##MainMenuBar") == 0)
    return "(Main menu bar)";
  return "(Untitled)";
}

// Overlay displayed when using CTRL+TAB. Called by EndFrame().
void ANCHOR::NavUpdateWindowingOverlay()
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(g.NavWindowingTarget != NULL);

  if (g.NavWindowingTimer < NAV_WINDOWING_LIST_APPEAR_DELAY)
    return;

  if (g.NavWindowingListWindow == NULL)
    g.NavWindowingListWindow = FindWindowByName("###NavWindowingList");
  const AnchorViewport *viewport = GetMainViewport();
  SetNextWindowSizeConstraints(wabi::GfVec2f(viewport->Size[0] * 0.20f, viewport->Size[1] * 0.20f),
                               wabi::GfVec2f(FLT_MAX, FLT_MAX));
  SetNextWindowPos(viewport->GetCenter(), AnchorCond_Always, wabi::GfVec2f(0.5f, 0.5f));
  PushStyleVar(AnchorStyleVar_WindowPadding, g.Style.WindowPadding * 2.0f);
  Begin("###NavWindowingList",
        NULL,
        AnchorWindowFlags_NoTitleBar | AnchorWindowFlags_NoFocusOnAppearing |
          AnchorWindowFlags_NoResize | AnchorWindowFlags_NoMove | AnchorWindowFlags_NoInputs |
          AnchorWindowFlags_AlwaysAutoResize | AnchorWindowFlags_NoSavedSettings);
  for (int n = g.WindowsFocusOrder.Size - 1; n >= 0; n--) {
    AnchorWindow *window = g.WindowsFocusOrder[n];
    ANCHOR_ASSERT(window != NULL);  // Fix static analyzers
    if (!IsWindowNavFocusable(window))
      continue;
    const char *label = window->Name;
    if (label == FindRenderedTextEnd(label))
      label = GetFallbackWindowNameForWindowingList(window);
    Selectable(label, g.NavWindowingTarget == window);
  }
  End();
  PopStyleVar();
}

//-----------------------------------------------------------------------------
// [SECTION] DRAG AND DROP
//-----------------------------------------------------------------------------

void ANCHOR::ClearDragDrop()
{
  AnchorContext &g = *G_CTX;
  g.DragDropActive = false;
  g.DragDropPayload.Clear();
  g.DragDropAcceptFlags = AnchorDragDropFlags_None;
  g.DragDropAcceptIdCurr = g.DragDropAcceptIdPrev = 0;
  g.DragDropAcceptIdCurrRectSurface = FLT_MAX;
  g.DragDropAcceptFrameCount = -1;

  g.DragDropPayloadBufHeap.clear();
  memset(&g.DragDropPayloadBufLocal, 0, sizeof(g.DragDropPayloadBufLocal));
}

// When this returns true you need to: a) call SetDragDropPayload() exactly once, b) you may render
// the payload visual/description, c) call EndDragDropSource() If the item has an identifier:
// - This assume/require the item to be activated (typically via ButtonBehavior).
// - Therefore if you want to use this with a mouse button other than left mouse button, it is up
// to the item itself to activate with another button.
// - We then pull and use the mouse button that was used to activate the item and use it to carry
// on the drag. If the item has no identifier:
// - Currently always assume left mouse button.
bool ANCHOR::BeginDragDropSource(AnchorDragDropFlags flags)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  // FIXME-DRAGDROP: While in the common-most "drag from non-zero active id" case we can tell the
  // mouse button, in both SourceExtern and id==0 cases we may requires something else (explicit
  // flags or some heuristic).
  AnchorMouseButton mouse_button = AnchorMouseButton_Left;

  bool source_drag_active = false;
  ANCHOR_ID source_id = 0;
  ANCHOR_ID source_parent_id = 0;
  if (!(flags & AnchorDragDropFlags_SourceExtern)) {
    source_id = window->DC.LastItemId;
    if (source_id != 0) {
      // Common path: items with ID
      if (g.ActiveId != source_id)
        return false;
      if (g.ActiveIdMouseButton != -1)
        mouse_button = g.ActiveIdMouseButton;
      if (g.IO.MouseDown[mouse_button] == false)
        return false;
      g.ActiveIdAllowOverlap = false;
    } else {
      // Uncommon path: items without ID
      if (g.IO.MouseDown[mouse_button] == false)
        return false;

      // If you want to use BeginDragDropSource() on an item with no unique identifier for
      // interaction, such as Text() or Image(), you need to: A) Read the explanation below, B) Use
      // the AnchorDragDropFlags_SourceAllowNullID flag, C) Swallow your programmer pride.
      if (!(flags & AnchorDragDropFlags_SourceAllowNullID)) {
        ANCHOR_ASSERT(0);
        return false;
      }

      // Early out
      if ((window->DC.LastItemStatusFlags & AnchorItemStatusFlags_HoveredRect) == 0 &&
          (g.ActiveId == 0 || g.ActiveIdWindow != window))
        return false;

      // Magic fallback (=somehow reprehensible) to handle items with no assigned ID, e.g. Text(),
      // Image() We build a throwaway ID based on current ID stack + relative AABB of items in
      // window. THE IDENTIFIER WON'T SURVIVE ANY REPOSITIONING OF THE WIDGET, so if your widget
      // moves your dragging operation will be canceled. We don't need to maintain/call
      // ClearActiveID() as releasing the button will early out this function and trigger
      // !ActiveIdIsAlive.
      source_id = window->DC.LastItemId = window->GetIDFromRectangle(window->DC.LastItemRect);
      bool is_hovered = ItemHoverable(window->DC.LastItemRect, source_id);
      if (is_hovered && g.IO.MouseClicked[mouse_button]) {
        SetActiveID(source_id, window);
        FocusWindow(window);
      }
      if (g.ActiveId == source_id)  // Allow the underlying widget to display/return hovered during
                                    // the mouse release frame, else we would get a flicker.
        g.ActiveIdAllowOverlap = is_hovered;
    }
    if (g.ActiveId != source_id)
      return false;
    source_parent_id = window->IDStack.back();
    source_drag_active = IsMouseDragging(mouse_button);

    // Disable navigation and key inputs while dragging
    g.ActiveIdUsingNavDirMask = ~(AnchorU32)0;
    g.ActiveIdUsingNavInputMask = ~(AnchorU32)0;
    g.ActiveIdUsingKeyInputMask = ~(AnchorU64)0;
  } else {
    window = NULL;
    source_id = AnchorHashStr("#SourceExtern");
    source_drag_active = true;
  }

  if (source_drag_active) {
    if (!g.DragDropActive) {
      ANCHOR_ASSERT(source_id != 0);
      ClearDragDrop();
      AnchorPayload &payload = g.DragDropPayload;
      payload.SourceId = source_id;
      payload.SourceParentId = source_parent_id;
      g.DragDropActive = true;
      g.DragDropSourceFlags = flags;
      g.DragDropMouseButton = mouse_button;
      if (payload.SourceId == g.ActiveId)
        g.ActiveIdNoClearOnFocusLoss = true;
    }
    g.DragDropSourceFrameCount = g.FrameCount;
    g.DragDropWithinSource = true;

    if (!(flags & AnchorDragDropFlags_SourceNoPreviewTooltip)) {
      // Target can request the Source to not display its tooltip (we use a dedicated flag to make
      // this request explicit) We unfortunately can't just modify the source flags and skip the
      // call to BeginTooltip, as caller may be emitting contents.
      BeginTooltip();
      if (g.DragDropAcceptIdPrev &&
          (g.DragDropAcceptFlags & AnchorDragDropFlags_AcceptNoPreviewTooltip)) {
        AnchorWindow *tooltip_window = g.CurrentWindow;
        tooltip_window->SkipItems = true;
        tooltip_window->HiddenFramesCanSkipItems = 1;
      }
    }

    if (!(flags & AnchorDragDropFlags_SourceNoDisableHover) &&
        !(flags & AnchorDragDropFlags_SourceExtern))
      window->DC.LastItemStatusFlags &= ~AnchorItemStatusFlags_HoveredRect;

    return true;
  }
  return false;
}

void ANCHOR::EndDragDropSource()
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(g.DragDropActive);
  ANCHOR_ASSERT(g.DragDropWithinSource && "Not after a BeginDragDropSource()?");

  if (!(g.DragDropSourceFlags & AnchorDragDropFlags_SourceNoPreviewTooltip))
    EndTooltip();

  // Discard the drag if have not called SetDragDropPayload()
  if (g.DragDropPayload.DataFrameCount == -1)
    ClearDragDrop();
  g.DragDropWithinSource = false;
}

// Use 'cond' to choose to submit payload on drag start or every frame
bool ANCHOR::SetDragDropPayload(const char *type,
                                const void *data,
                                size_t data_size,
                                AnchorCond cond)
{
  AnchorContext &g = *G_CTX;
  AnchorPayload &payload = g.DragDropPayload;
  if (cond == 0)
    cond = AnchorCond_Always;

  ANCHOR_ASSERT(type != NULL);
  ANCHOR_ASSERT(strlen(type) < ANCHOR_ARRAYSIZE(payload.DataType) &&
                "Payload type can be at most 32 characters long");
  ANCHOR_ASSERT((data != NULL && data_size > 0) || (data == NULL && data_size == 0));
  ANCHOR_ASSERT(cond == AnchorCond_Always || cond == AnchorCond_Once);
  ANCHOR_ASSERT(payload.SourceId !=
                0);  // Not called between BeginDragDropSource() and EndDragDropSource()

  if (cond == AnchorCond_Always || payload.DataFrameCount == -1) {
    // Copy payload
    AnchorStrncpy(payload.DataType, type, ANCHOR_ARRAYSIZE(payload.DataType));
    g.DragDropPayloadBufHeap.resize(0);
    if (data_size > sizeof(g.DragDropPayloadBufLocal)) {
      // Store in heap
      g.DragDropPayloadBufHeap.resize((int)data_size);
      payload.Data = g.DragDropPayloadBufHeap.Data;
      memcpy(payload.Data, data, data_size);
    } else if (data_size > 0) {
      // Store locally
      memset(&g.DragDropPayloadBufLocal, 0, sizeof(g.DragDropPayloadBufLocal));
      payload.Data = g.DragDropPayloadBufLocal;
      memcpy(payload.Data, data, data_size);
    } else {
      payload.Data = NULL;
    }
    payload.DataSize = (int)data_size;
  }
  payload.DataFrameCount = g.FrameCount;

  return (g.DragDropAcceptFrameCount == g.FrameCount) ||
         (g.DragDropAcceptFrameCount == g.FrameCount - 1);
}

bool ANCHOR::BeginDragDropTargetCustom(const AnchorBBox &bb, ANCHOR_ID id)
{
  AnchorContext &g = *G_CTX;
  if (!g.DragDropActive)
    return false;

  AnchorWindow *window = g.CurrentWindow;
  AnchorWindow *hovered_window = g.HoveredWindowUnderMovingWindow;
  if (hovered_window == NULL || window->RootWindow != hovered_window->RootWindow)
    return false;
  ANCHOR_ASSERT(id != 0);
  if (!IsMouseHoveringRect(bb.Min, bb.Max) || (id == g.DragDropPayload.SourceId))
    return false;
  if (window->SkipItems)
    return false;

  ANCHOR_ASSERT(g.DragDropWithinTarget == false);
  g.DragDropTargetRect = bb;
  g.DragDropTargetId = id;
  g.DragDropWithinTarget = true;
  return true;
}

// We don't use BeginDragDropTargetCustom() and duplicate its code because:
// 1) we use LastItemRectHoveredRect which handles items that pushes a temporarily clip rectangle
// in their code. Calling BeginDragDropTargetCustom(LastItemRect) would not handle them. 2) and
// it's faster. as this code may be very frequently called, we want to early out as fast as we can.
// Also note how the HoveredWindow test is positioned differently in both functions (in both
// functions we optimize for the cheapest early out case)
bool ANCHOR::BeginDragDropTarget()
{
  AnchorContext &g = *G_CTX;
  if (!g.DragDropActive)
    return false;

  AnchorWindow *window = g.CurrentWindow;
  if (!(window->DC.LastItemStatusFlags & AnchorItemStatusFlags_HoveredRect))
    return false;
  AnchorWindow *hovered_window = g.HoveredWindowUnderMovingWindow;
  if (hovered_window == NULL || window->RootWindow != hovered_window->RootWindow)
    return false;

  const AnchorBBox &display_rect = (window->DC.LastItemStatusFlags &
                                    AnchorItemStatusFlags_HasDisplayRect) ?
                                     window->DC.LastItemDisplayRect :
                                     window->DC.LastItemRect;
  ANCHOR_ID id = window->DC.LastItemId;
  if (id == 0)
    id = window->GetIDFromRectangle(display_rect);
  if (g.DragDropPayload.SourceId == id)
    return false;

  ANCHOR_ASSERT(g.DragDropWithinTarget == false);
  g.DragDropTargetRect = display_rect;
  g.DragDropTargetId = id;
  g.DragDropWithinTarget = true;
  return true;
}

bool ANCHOR::IsDragDropPayloadBeingAccepted()
{
  AnchorContext &g = *G_CTX;
  return g.DragDropActive && g.DragDropAcceptIdPrev != 0;
}

const AnchorPayload *ANCHOR::AcceptDragDropPayload(const char *type, AnchorDragDropFlags flags)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  AnchorPayload &payload = g.DragDropPayload;
  ANCHOR_ASSERT(
    g.DragDropActive);  // Not called between BeginDragDropTarget() and EndDragDropTarget() ?
  ANCHOR_ASSERT(payload.DataFrameCount != -1);  // Forgot to call EndDragDropTarget() ?
  if (type != NULL && !payload.IsDataType(type))
    return NULL;

  // Accept smallest drag target bounding box, this allows us to nest drag targets conveniently
  // without ordering constraints. NB: We currently accept NULL id as target. However, overlapping
  // targets requires a unique ID to function!
  const bool was_accepted_previously = (g.DragDropAcceptIdPrev == g.DragDropTargetId);
  AnchorBBox r = g.DragDropTargetRect;
  float r_surface = r.GetWidth() * r.GetHeight();
  if (r_surface <= g.DragDropAcceptIdCurrRectSurface) {
    g.DragDropAcceptFlags = flags;
    g.DragDropAcceptIdCurr = g.DragDropTargetId;
    g.DragDropAcceptIdCurrRectSurface = r_surface;
  }

  // Render default drop visuals
  payload.Preview = was_accepted_previously;
  flags |= (g.DragDropSourceFlags &
            AnchorDragDropFlags_AcceptNoDrawDefaultRect);  // Source can also inhibit the preview
                                                           // (useful for external sources that
                                                           // lives for 1 frame)
  if (!(flags & AnchorDragDropFlags_AcceptNoDrawDefaultRect) && payload.Preview) {
    // FIXME-DRAGDROP: Settle on a proper default visuals for drop target.
    r.Expand(3.5f);
    bool push_clip_rect = !window->ClipRect.Contains(r);
    if (push_clip_rect)
      window->DrawList->PushClipRect(r.Min - wabi::GfVec2f(1, 1), r.Max + wabi::GfVec2f(1, 1));
    window->DrawList->AddRect(r.Min, r.Max, GetColorU32(AnchorCol_DragDropTarget), 0.0f, 0, 2.0f);
    if (push_clip_rect)
      window->DrawList->PopClipRect();
  }

  g.DragDropAcceptFrameCount = g.FrameCount;
  payload.Delivery = was_accepted_previously &&
                     !IsMouseDown(
                       g.DragDropMouseButton);  // For extern drag sources affecting os window
                                                // focus, it's easier to just test !IsMouseDown()
                                                // instead of IsMouseReleased()
  if (!payload.Delivery && !(flags & AnchorDragDropFlags_AcceptBeforeDelivery))
    return NULL;

  return &payload;
}

const AnchorPayload *ANCHOR::GetDragDropPayload()
{
  AnchorContext &g = *G_CTX;
  return g.DragDropActive ? &g.DragDropPayload : NULL;
}

// We don't really use/need this now, but added it for the sake of consistency and because we might
// need it later.
void ANCHOR::EndDragDropTarget()
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(g.DragDropActive);
  ANCHOR_ASSERT(g.DragDropWithinTarget);
  g.DragDropWithinTarget = false;
}

//-----------------------------------------------------------------------------
// [SECTION] LOGGING/CAPTURING
//-----------------------------------------------------------------------------
// All text output from the interface can be captured into tty/file/clipboard.
// By default, tree nodes are automatically opened during logging.
//-----------------------------------------------------------------------------

// Pass text data straight to log (without being displayed)
static inline void LogTextV(AnchorContext &g, const char *fmt, va_list args)
{
  if (g.LogFile) {
    g.LogBuffer.Buf.resize(0);
    g.LogBuffer.appendfv(fmt, args);
    ImFileWrite(g.LogBuffer.c_str(), sizeof(char), (AnchorU64)g.LogBuffer.size(), g.LogFile);
  } else {
    g.LogBuffer.appendfv(fmt, args);
  }
}

void ANCHOR::LogText(const char *fmt, ...)
{
  AnchorContext &g = *G_CTX;
  if (!g.LogEnabled)
    return;

  va_list args;
  va_start(args, fmt);
  LogTextV(g, fmt, args);
  va_end(args);
}

void ANCHOR::LogTextV(const char *fmt, va_list args)
{
  AnchorContext &g = *G_CTX;
  if (!g.LogEnabled)
    return;

  LogTextV(g, fmt, args);
}

// Internal version that takes a position to decide on newline placement and pad items according to
// their depth. We split text into individual lines to add current tree level padding
// FIXME: This code is a little complicated perhaps, considering simplifying the whole system.
void ANCHOR::LogRenderedText(const wabi::GfVec2f *ref_pos, const char *text, const char *text_end)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  const char *prefix = g.LogNextPrefix;
  const char *suffix = g.LogNextSuffix;
  g.LogNextPrefix = g.LogNextSuffix = NULL;

  if (!text_end)
    text_end = FindRenderedTextEnd(text, text_end);

  const bool log_new_line = ref_pos &&
                            (ref_pos->data()[1] > g.LogLinePosY + g.Style.FramePadding[1] + 1);
  if (ref_pos)
    g.LogLinePosY = ref_pos->data()[1];
  if (log_new_line) {
    LogText(ANCHOR_NEWLINE);
    g.LogLineFirstItem = true;
  }

  if (prefix)
    LogRenderedText(ref_pos,
                    prefix,
                    prefix +
                      strlen(prefix));  // Calculate end ourself to ensure "##" are included here.

  // Re-adjust padding if we have popped out of our starting depth
  if (g.LogDepthRef > window->DC.TreeDepth)
    g.LogDepthRef = window->DC.TreeDepth;
  const int tree_depth = (window->DC.TreeDepth - g.LogDepthRef);

  const char *text_remaining = text;
  for (;;) {
    // Split the string. Each new line (after a '\n') is followed by indentation corresponding to
    // the current depth of our log entry. We don't add a trailing \n yet to allow a subsequent
    // item on the same line to be captured.
    const char *line_start = text_remaining;
    const char *line_end = AnchorStreolRange(line_start, text_end);
    const bool is_last_line = (line_end == text_end);
    if (line_start != line_end || !is_last_line) {
      const int line_length = (int)(line_end - line_start);
      const int indentation = g.LogLineFirstItem ? tree_depth * 4 : 1;
      LogText("%*s%.*s", indentation, "", line_length, line_start);
      g.LogLineFirstItem = false;
      if (*line_end == '\n') {
        LogText(ANCHOR_NEWLINE);
        g.LogLineFirstItem = true;
      }
    }
    if (is_last_line)
      break;
    text_remaining = line_end + 1;
  }

  if (suffix)
    LogRenderedText(ref_pos, suffix, suffix + strlen(suffix));
}

// Start logging/capturing text output
void ANCHOR::LogBegin(ANCHORLogType type, int auto_open_depth)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;
  ANCHOR_ASSERT(g.LogEnabled == false);
  ANCHOR_ASSERT(g.LogFile == NULL);
  ANCHOR_ASSERT(g.LogBuffer.empty());
  g.LogEnabled = true;
  g.LogType = type;
  g.LogNextPrefix = g.LogNextSuffix = NULL;
  g.LogDepthRef = window->DC.TreeDepth;
  g.LogDepthToExpand = ((auto_open_depth >= 0) ? auto_open_depth : g.LogDepthToExpandDefault);
  g.LogLinePosY = FLT_MAX;
  g.LogLineFirstItem = true;
}

// Important: doesn't copy underlying data, use carefully (prefix/suffix must be in scope at the
// time of the next LogRenderedText)
void ANCHOR::LogSetNextTextDecoration(const char *prefix, const char *suffix)
{
  AnchorContext &g = *G_CTX;
  g.LogNextPrefix = prefix;
  g.LogNextSuffix = suffix;
}

void ANCHOR::LogToTTY(int auto_open_depth)
{
  AnchorContext &g = *G_CTX;
  if (g.LogEnabled)
    return;
  TF_UNUSED(auto_open_depth);
#ifndef ANCHOR_DISABLE_TTY_FUNCTIONS
  LogBegin(ANCHORLogType_TTY, auto_open_depth);
  g.LogFile = stdout;
#endif
}

// Start logging/capturing text output to given file
void ANCHOR::LogToFile(int auto_open_depth, const char *filename)
{
  AnchorContext &g = *G_CTX;
  if (g.LogEnabled)
    return;

  // FIXME: We could probably open the file in text mode "at", however note that clipboard/buffer
  // logging will still be subject to outputting OS-incompatible carriage return if within strings
  // the user doesn't use ANCHOR_NEWLINE. By opening the file in binary mode "ab" we have
  // consistent output everywhere.
  if (!filename)
    filename = g.IO.LogFilename;
  if (!filename || !filename[0])
    return;
  ImFileHandle f = ImFileOpen(filename, "ab");
  if (!f) {
    ANCHOR_ASSERT(0);
    return;
  }

  LogBegin(ANCHORLogType_File, auto_open_depth);
  g.LogFile = f;
}

// Start logging/capturing text output to clipboard
void ANCHOR::LogToClipboard(int auto_open_depth)
{
  AnchorContext &g = *G_CTX;
  if (g.LogEnabled)
    return;
  LogBegin(ANCHORLogType_Clipboard, auto_open_depth);
}

void ANCHOR::LogToBuffer(int auto_open_depth)
{
  AnchorContext &g = *G_CTX;
  if (g.LogEnabled)
    return;
  LogBegin(ANCHORLogType_Buffer, auto_open_depth);
}

void ANCHOR::LogFinish()
{
  AnchorContext &g = *G_CTX;
  if (!g.LogEnabled)
    return;

  LogText(ANCHOR_NEWLINE);
  switch (g.LogType) {
    case ANCHORLogType_TTY:
#ifndef ANCHOR_DISABLE_TTY_FUNCTIONS
      fflush(g.LogFile);
#endif
      break;
    case ANCHORLogType_File:
      ImFileClose(g.LogFile);
      break;
    case ANCHORLogType_Buffer:
      break;
    case ANCHORLogType_Clipboard:
      if (!g.LogBuffer.empty())
        SetClipboardText(g.LogBuffer.begin());
      break;
    case ANCHORLogType_None:
      ANCHOR_ASSERT(0);
      break;
  }

  g.LogEnabled = false;
  g.LogType = ANCHORLogType_None;
  g.LogFile = NULL;
  g.LogBuffer.clear();
}

// Helper to display logging buttons
// FIXME-OBSOLETE: We should probably obsolete this and let the user have their own helper (this is
// one of the oldest function alive!)
void ANCHOR::LogButtons()
{
  AnchorContext &g = *G_CTX;

  PushID("LogButtons");
#ifndef ANCHOR_DISABLE_TTY_FUNCTIONS
  const bool log_to_tty = Button("Log To TTY");
  SameLine();
#else
  const bool log_to_tty = false;
#endif
  const bool log_to_file = Button("Log To File");
  SameLine();
  const bool log_to_clipboard = Button("Log To Clipboard");
  SameLine();
  PushAllowKeyboardFocus(false);
  SetNextItemWidth(80.0f);
  SliderInt("Default Depth", &g.LogDepthToExpandDefault, 0, 9, NULL);
  PopAllowKeyboardFocus();
  PopID();

  // Start logging at the end of the function so that the buttons don't appear in the log
  if (log_to_tty)
    LogToTTY();
  if (log_to_file)
    LogToFile();
  if (log_to_clipboard)
    LogToClipboard();
}

//-----------------------------------------------------------------------------
// [SECTION] SETTINGS
//-----------------------------------------------------------------------------
// - UpdateSettings() [Internal]
// - MarkIniSettingsDirty() [Internal]
// - CreateNewWindowSettings() [Internal]
// - FindWindowSettings() [Internal]
// - FindOrCreateWindowSettings() [Internal]
// - FindSettingsHandler() [Internal]
// - ClearIniSettings() [Internal]
// - LoadIniSettingsFromDisk()
// - LoadIniSettingsFromMemory()
// - SaveIniSettingsToDisk()
// - SaveIniSettingsToMemory()
// - WindowSettingsHandler_***() [Internal]
//-----------------------------------------------------------------------------

// Called by NewFrame()
void ANCHOR::UpdateSettings()
{
  // Load settings on first frame (if not explicitly loaded manually before)
  AnchorContext &g = *G_CTX;
  if (!g.SettingsLoaded) {
    ANCHOR_ASSERT(g.SettingsWindows.empty());
    // if (g.IO.IniFilename)
    //     LoadIniSettingsFromDisk(g.IO.IniFilename);
    g.SettingsLoaded = true;
  }

  // Save settings (with a delay after the last modification, so we don't spam disk too much)
  if (g.SettingsDirtyTimer > 0.0f) {
    g.SettingsDirtyTimer -= g.IO.DeltaTime;
    if (g.SettingsDirtyTimer <= 0.0f) {
      if (g.IO.IniFilename != NULL)
        SaveIniSettingsToDisk(g.IO.IniFilename);
      else
        g.IO.WantSaveIniSettings =
          true;  // Let user know they can call SaveIniSettingsToMemory(). user will
                 // need to clear io.WantSaveIniSettings themselves.
      g.SettingsDirtyTimer = 0.0f;
    }
  }
}

void ANCHOR::MarkIniSettingsDirty()
{
  AnchorContext &g = *G_CTX;
  if (g.SettingsDirtyTimer <= 0.0f)
    g.SettingsDirtyTimer = g.IO.IniSavingRate;
}

void ANCHOR::MarkIniSettingsDirty(AnchorWindow *window)
{
  AnchorContext &g = *G_CTX;
  if (!(window->Flags & AnchorWindowFlags_NoSavedSettings))
    if (g.SettingsDirtyTimer <= 0.0f)
      g.SettingsDirtyTimer = g.IO.IniSavingRate;
}

AnchorWindowSettings *ANCHOR::CreateNewWindowSettings(const char *name)
{
  AnchorContext &g = *G_CTX;

#if !ANCHOR_DEBUG_INI_SETTINGS
  // Skip to the "###" marker if any. We don't skip past to match the behavior of GetID()
  // Preserve the full string when ANCHOR_DEBUG_INI_SETTINGS is set to make .ini inspection easier.
  if (const char *p = strstr(name, "###"))
    name = p;
#endif
  const size_t name_len = strlen(name);

  // Allocate chunk
  const size_t chunk_size = sizeof(AnchorWindowSettings) + name_len + 1;
  AnchorWindowSettings *settings = g.SettingsWindows.alloc_chunk(chunk_size);
  ANCHOR_PLACEMENT_NEW(settings)
  AnchorWindowSettings();
  settings->ID = AnchorHashStr(name, name_len);
  memcpy(settings->GetName(), name, name_len + 1);  // Store with zero terminator

  return settings;
}

AnchorWindowSettings *ANCHOR::FindWindowSettings(ANCHOR_ID id)
{
  AnchorContext &g = *G_CTX;
  for (AnchorWindowSettings *settings = g.SettingsWindows.begin(); settings != NULL;
       settings = g.SettingsWindows.next_chunk(settings))
    if (settings->ID == id)
      return settings;
  return NULL;
}

AnchorWindowSettings *ANCHOR::FindOrCreateWindowSettings(const char *name)
{
  if (AnchorWindowSettings *settings = FindWindowSettings(AnchorHashStr(name)))
    return settings;
  return CreateNewWindowSettings(name);
}

AnchorSettingsHandler *ANCHOR::FindSettingsHandler(const char *type_name)
{
  AnchorContext &g = *G_CTX;
  const ANCHOR_ID type_hash = AnchorHashStr(type_name);
  for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++)
    if (g.SettingsHandlers[handler_n].TypeHash == type_hash)
      return &g.SettingsHandlers[handler_n];
  return NULL;
}

void ANCHOR::ClearIniSettings()
{
  AnchorContext &g = *G_CTX;
  g.SettingsIniData.clear();
  for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++)
    if (g.SettingsHandlers[handler_n].ClearAllFn)
      g.SettingsHandlers[handler_n].ClearAllFn(&g, &g.SettingsHandlers[handler_n]);
}

void ANCHOR::LoadIniSettingsFromDisk(const char *ini_filename)
{
  size_t file_data_size = 0;
  char *file_data = (char *)ImFileLoadToMemory(ini_filename, "rb", &file_data_size);
  if (!file_data)
    return;
  LoadIniSettingsFromMemory(file_data, (size_t)file_data_size);
  ANCHOR_FREE(file_data);
}

// Zero-tolerance, no error reporting, cheap .ini parsing
void ANCHOR::LoadIniSettingsFromMemory(const char *ini_data, size_t ini_size)
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(g.Initialized);
  // ANCHOR_ASSERT(!g.WithinFrameScope && "Cannot be called between NewFrame() and EndFrame()");
  // ANCHOR_ASSERT(g.SettingsLoaded == false && g.FrameCount == 0);

  // For user convenience, we allow passing a non zero-terminated string (hence the ini_size
  // parameter). For our convenience and to make the code simpler, we'll also write
  // zero-terminators within the buffer. So let's create a writable copy..
  if (ini_size == 0)
    ini_size = strlen(ini_data);
  g.SettingsIniData.Buf.resize((int)ini_size + 1);
  char *const buf = g.SettingsIniData.Buf.Data;
  char *const buf_end = buf + ini_size;
  memcpy(buf, ini_data, ini_size);
  buf_end[0] = 0;

  // Call pre-read handlers
  // Some types will clear their data (e.g. dock information) some types will allow merge/override
  // (window)
  for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++)
    if (g.SettingsHandlers[handler_n].ReadInitFn)
      g.SettingsHandlers[handler_n].ReadInitFn(&g, &g.SettingsHandlers[handler_n]);

  void *entry_data = NULL;
  AnchorSettingsHandler *entry_handler = NULL;

  char *line_end = NULL;
  for (char *line = buf; line < buf_end; line = line_end + 1) {
    // Skip new lines markers, then find end of the line
    while (*line == '\n' || *line == '\r')
      line++;
    line_end = line;
    while (line_end < buf_end && *line_end != '\n' && *line_end != '\r')
      line_end++;
    line_end[0] = 0;
    if (line[0] == ';')
      continue;
    if (line[0] == '[' && line_end > line && line_end[-1] == ']') {
      // Parse "[Type][Name]". Note that 'Name' can itself contains [] characters, which is
      // acceptable with the current format and parsing code.
      line_end[-1] = 0;
      const char *name_end = line_end - 1;
      const char *type_start = line + 1;
      char *type_end = (char *)(void *)AnchorStrchrRange(type_start, name_end, ']');
      const char *name_start = type_end ? AnchorStrchrRange(type_end + 1, name_end, '[') : NULL;
      if (!type_end || !name_start)
        continue;
      *type_end = 0;  // Overwrite first ']'
      name_start++;   // Skip second '['
      entry_handler = FindSettingsHandler(type_start);
      entry_data = entry_handler ? entry_handler->ReadOpenFn(&g, entry_handler, name_start) : NULL;
    } else if (entry_handler != NULL && entry_data != NULL) {
      // Let type handler parse the line
      entry_handler->ReadLineFn(&g, entry_handler, entry_data, line);
    }
  }
  g.SettingsLoaded = true;

  // [DEBUG] Restore untouched copy so it can be browsed in Metrics (not strictly necessary)
  memcpy(buf, ini_data, ini_size);

  // Call post-read handlers
  for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++)
    if (g.SettingsHandlers[handler_n].ApplyAllFn)
      g.SettingsHandlers[handler_n].ApplyAllFn(&g, &g.SettingsHandlers[handler_n]);
}

void ANCHOR::SaveIniSettingsToDisk(const char *ini_filename)
{
  AnchorContext &g = *G_CTX;
  g.SettingsDirtyTimer = 0.0f;
  if (!ini_filename)
    return;

  size_t ini_data_size = 0;
  const char *ini_data = SaveIniSettingsToMemory(&ini_data_size);
  ImFileHandle f = ImFileOpen(ini_filename, "wt");
  if (!f)
    return;
  ImFileWrite(ini_data, sizeof(char), ini_data_size, f);
  ImFileClose(f);
}

// Call registered handlers (e.g. SettingsHandlerWindow_WriteAll() + custom handlers) to write
// their stuff into a text buffer
const char *ANCHOR::SaveIniSettingsToMemory(size_t *out_size)
{
  AnchorContext &g = *G_CTX;
  g.SettingsDirtyTimer = 0.0f;
  g.SettingsIniData.Buf.resize(0);
  g.SettingsIniData.Buf.push_back(0);
  for (int handler_n = 0; handler_n < g.SettingsHandlers.Size; handler_n++) {
    AnchorSettingsHandler *handler = &g.SettingsHandlers[handler_n];
    handler->WriteAllFn(&g, handler, &g.SettingsIniData);
  }
  if (out_size)
    *out_size = (size_t)g.SettingsIniData.size();
  return g.SettingsIniData.c_str();
}

static void WindowSettingsHandler_ClearAll(AnchorContext *ctx, AnchorSettingsHandler *)
{
  AnchorContext &g = *ctx;
  for (int i = 0; i != g.Windows.Size; i++)
    g.Windows[i]->SettingsOffset = -1;
  g.SettingsWindows.clear();
}

static void *WindowSettingsHandler_ReadOpen(AnchorContext *,
                                            AnchorSettingsHandler *,
                                            const char *name)
{
  AnchorWindowSettings *settings = ANCHOR::FindOrCreateWindowSettings(name);
  ANCHOR_ID id = settings->ID;
  *settings = AnchorWindowSettings();  // Clear existing if recycling previous entry
  settings->ID = id;
  settings->WantApply = true;
  return (void *)settings;
}

static void WindowSettingsHandler_ReadLine(AnchorContext *,
                                           AnchorSettingsHandler *,
                                           void *entry,
                                           const char *line)
{
  AnchorWindowSettings *settings = (AnchorWindowSettings *)entry;
  int x, y;
  int i;
  if (sscanf(line, "Pos=%i,%i", &x, &y) == 2) {
    settings->Pos = wabi::GfVec2h((short)x, (short)y);
  } else if (sscanf(line, "Size=%i,%i", &x, &y) == 2) {
    settings->Size = wabi::GfVec2h((short)x, (short)y);
  } else if (sscanf(line, "Collapsed=%d", &i) == 1) {
    settings->Collapsed = (i != 0);
  }
}

// Apply to existing windows (if any)
static void WindowSettingsHandler_ApplyAll(AnchorContext *ctx, AnchorSettingsHandler *)
{
  AnchorContext &g = *ctx;
  for (AnchorWindowSettings *settings = g.SettingsWindows.begin(); settings != NULL;
       settings = g.SettingsWindows.next_chunk(settings))
    if (settings->WantApply) {
      if (AnchorWindow *window = ANCHOR::FindWindowByID(settings->ID))
        ApplyWindowSettings(window, settings);
      settings->WantApply = false;
    }
}

static void WindowSettingsHandler_WriteAll(AnchorContext *ctx,
                                           AnchorSettingsHandler *handler,
                                           AnchorTextBuffer *buf)
{
  // Gather data from windows that were active during this session
  // (if a window wasn't opened in this session we preserve its settings)
  AnchorContext &g = *ctx;
  for (int i = 0; i != g.Windows.Size; i++) {
    AnchorWindow *window = g.Windows[i];
    if (window->Flags & AnchorWindowFlags_NoSavedSettings)
      continue;

    AnchorWindowSettings *settings = (window->SettingsOffset != -1) ?
                                       g.SettingsWindows.ptr_from_offset(window->SettingsOffset) :
                                       ANCHOR::FindWindowSettings(window->ID);
    if (!settings) {
      settings = ANCHOR::CreateNewWindowSettings(window->Name);
      window->SettingsOffset = g.SettingsWindows.offset_from_ptr(settings);
    }
    ANCHOR_ASSERT(settings->ID == window->ID);
    settings->Pos = wabi::GfVec2h((short)window->Pos[0], (short)window->Pos[1]);
    settings->Size = wabi::GfVec2h((short)window->SizeFull[0], (short)window->SizeFull[1]);
    settings->Collapsed = window->Collapsed;
  }

  // Write to text buffer
  buf->reserve(buf->size() + g.SettingsWindows.size() * 6);  // ballpark reserve
  for (AnchorWindowSettings *settings = g.SettingsWindows.begin(); settings != NULL;
       settings = g.SettingsWindows.next_chunk(settings)) {
    const char *settings_name = settings->GetName();
    buf->appendf("[%s][%s]\n", handler->TypeName, settings_name);
    buf->appendf("Pos=%d,%d\n", settings->Pos[0], settings->Pos[1]);
    buf->appendf("Size=%d,%d\n", settings->Size[0], settings->Size[1]);
    buf->appendf("Collapsed=%d\n", settings->Collapsed);
    buf->append("\n");
  }
}

//-----------------------------------------------------------------------------
// [SECTION] VIEWPORTS, PLATFORM WINDOWS
//-----------------------------------------------------------------------------
// - GetMainViewport()
// - UpdateViewportsNewFrame() [Internal]
// (this section is more complete in the 'docking' branch)
//-----------------------------------------------------------------------------

AnchorViewport *ANCHOR::GetMainViewport()
{
  AnchorContext &g = *G_CTX;
  return g.Viewports[0];
}

// Update viewports and monitor infos
static void ANCHOR::UpdateViewportsNewFrame()
{
  AnchorContext &g = *G_CTX;
  ANCHOR_ASSERT(g.Viewports.Size == 1);

  // Update main viewport with current platform position.
  // FIXME-VIEWPORT: Size is driven by backend/user code for backward-compatibility but we should
  // aim to make this more consistent.
  AnchorViewportP *main_viewport = g.Viewports[0];
  main_viewport->Flags = AnchorViewportFlags_IsPlatformWindow | AnchorViewportFlags_OwnedByApp;
  main_viewport->Pos = wabi::GfVec2f(0.0f, 0.0f);
  main_viewport->Size = g.IO.DisplaySize;

  for (int n = 0; n < g.Viewports.Size; n++) {
    AnchorViewportP *viewport = g.Viewports[n];

    // Lock down space taken by menu bars and status bars, reset the offset for fucntions like
    // BeginMainMenuBar() to alter them again.
    viewport->WorkOffsetMin = viewport->BuildWorkOffsetMin;
    viewport->WorkOffsetMax = viewport->BuildWorkOffsetMax;
    viewport->BuildWorkOffsetMin = viewport->BuildWorkOffsetMax = wabi::GfVec2f(0.0f, 0.0f);
    viewport->UpdateWorkRect();
  }
}

//-----------------------------------------------------------------------------
// [SECTION] DOCKING
//-----------------------------------------------------------------------------

// (this section is filled in the 'docking' branch)

//-----------------------------------------------------------------------------
// [SECTION] PLATFORM DEPENDENT HELPERS
//-----------------------------------------------------------------------------

#if defined(_WIN32) && !defined(ANCHOR_DISABLE_WIN32_FUNCTIONS) && \
  !defined(ANCHOR_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS)

#  ifdef _MSC_VER
#    pragma comment(lib, "user32")
#    pragma comment(lib, "kernel32")
#  endif

// Win32 clipboard implementation
// We use g.ClipboardHandlerData for temporary storage to ensure it is freed on Shutdown()
static const char *GetClipboardTextFn_DefaultImpl(void *)
{
  AnchorContext &g = *G_CTX;
  g.ClipboardHandlerData.clear();
  if (!::OpenClipboard(NULL))
    return NULL;
  HANDLE wbuf_handle = ::GetClipboardData(CF_UNICODETEXT);
  if (wbuf_handle == NULL) {
    ::CloseClipboard();
    return NULL;
  }
  if (const WCHAR *wbuf_global = (const WCHAR *)::GlobalLock(wbuf_handle)) {
    int buf_len = ::WideCharToMultiByte(CP_UTF8, 0, wbuf_global, -1, NULL, 0, NULL, NULL);
    g.ClipboardHandlerData.resize(buf_len);
    ::WideCharToMultiByte(CP_UTF8,
                          0,
                          wbuf_global,
                          -1,
                          g.ClipboardHandlerData.Data,
                          buf_len,
                          NULL,
                          NULL);
  }
  ::GlobalUnlock(wbuf_handle);
  ::CloseClipboard();
  return g.ClipboardHandlerData.Data;
}

static void SetClipboardTextFn_DefaultImpl(void *, const char *text)
{
  if (!::OpenClipboard(NULL))
    return;
  const int wbuf_length = ::MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
  HGLOBAL wbuf_handle = ::GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)wbuf_length * sizeof(WCHAR));
  if (wbuf_handle == NULL) {
    ::CloseClipboard();
    return;
  }
  WCHAR *wbuf_global = (WCHAR *)::GlobalLock(wbuf_handle);
  ::MultiByteToWideChar(CP_UTF8, 0, text, -1, wbuf_global, wbuf_length);
  ::GlobalUnlock(wbuf_handle);
  ::EmptyClipboard();
  if (::SetClipboardData(CF_UNICODETEXT, wbuf_handle) == NULL)
    ::GlobalFree(wbuf_handle);
  ::CloseClipboard();
}

#elif defined(__APPLE__) && TARGET_OS_OSX && defined(ANCHOR_ENABLE_OSX_DEFAULT_CLIPBOARD_FUNCTIONS)

#  include <Carbon/Carbon.h>  // Use old API to avoid need for separate .mm file
static PasteboardRef main_clipboard = 0;

// OSX clipboard implementation
// If you enable this you will need to add '-framework ApplicationServices' to your linker
// command-line!
static void SetClipboardTextFn_DefaultImpl(void *, const char *text)
{
  if (!main_clipboard)
    PasteboardCreate(kPasteboardClipboard, &main_clipboard);
  PasteboardClear(main_clipboard);
  CFDataRef cf_data = CFDataCreate(kCFAllocatorDefault, (const UInt8 *)text, strlen(text));
  if (cf_data) {
    PasteboardPutItemFlavor(main_clipboard,
                            (PasteboardItemID)1,
                            CFSTR("public.utf8-plain-text"),
                            cf_data,
                            0);
    CFRelease(cf_data);
  }
}

static const char *GetClipboardTextFn_DefaultImpl(void *)
{
  if (!main_clipboard)
    PasteboardCreate(kPasteboardClipboard, &main_clipboard);
  PasteboardSynchronize(main_clipboard);

  ItemCount item_count = 0;
  PasteboardGetItemCount(main_clipboard, &item_count);
  for (ItemCount i = 0; i < item_count; i++) {
    PasteboardItemID item_id = 0;
    PasteboardGetItemIdentifier(main_clipboard, i + 1, &item_id);
    CFArrayRef flavor_type_array = 0;
    PasteboardCopyItemFlavors(main_clipboard, item_id, &flavor_type_array);
    for (CFIndex j = 0, nj = CFArrayGetCount(flavor_type_array); j < nj; j++) {
      CFDataRef cf_data;
      if (PasteboardCopyItemFlavorData(main_clipboard,
                                       item_id,
                                       CFSTR("public.utf8-plain-text"),
                                       &cf_data) == noErr) {
        AnchorContext &g = *G_CTX;
        g.ClipboardHandlerData.clear();
        int length = (int)CFDataGetLength(cf_data);
        g.ClipboardHandlerData.resize(length + 1);
        CFDataGetBytes(cf_data, CFRangeMake(0, length), (UInt8 *)g.ClipboardHandlerData.Data);
        g.ClipboardHandlerData[length] = 0;
        CFRelease(cf_data);
        return g.ClipboardHandlerData.Data;
      }
    }
  }
  return NULL;
}

#else

// Local ANCHOR-only clipboard implementation, if user hasn't defined better clipboard handlers.
static const char *GetClipboardTextFn_DefaultImpl(void *)
{
  AnchorContext &g = *G_CTX;
  return g.ClipboardHandlerData.empty() ? NULL : g.ClipboardHandlerData.begin();
}

static void SetClipboardTextFn_DefaultImpl(void *, const char *text)
{
  AnchorContext &g = *G_CTX;
  g.ClipboardHandlerData.clear();
  const char *text_end = text + strlen(text);
  g.ClipboardHandlerData.resize((int)(text_end - text) + 1);
  memcpy(&g.ClipboardHandlerData[0], text, (size_t)(text_end - text));
  g.ClipboardHandlerData[(int)(text_end - text)] = 0;
}

#endif

// Win32 API IME support (for Asian languages, etc.)
#if defined(_WIN32) && !defined(ANCHOR_DISABLE_WIN32_FUNCTIONS) && \
  !defined(ANCHOR_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS)

#  include <imm.h>
#  ifdef _MSC_VER
#    pragma comment(lib, "imm32")
#  endif

static void ImeSetInputScreenPosFn_DefaultImpl(int x, int y)
{
  // Notify OS Input Method Editor of text input position
  AnchorIO &io = ANCHOR::GetIO();
  if (HWND hwnd = (HWND)io.ImeWindowHandle)
    if (HIMC himc = ::ImmGetContext(hwnd)) {
      COMPOSITIONFORM cf;
#  ifndef _WIN32
      cf.ptCurrentPos[0] = x;
      cf.ptCurrentPos[1] = y;
#  else
      cf.ptCurrentPos.x = x;
      cf.ptCurrentPos.y = y;
#  endif
      cf.dwStyle = CFS_FORCE_POSITION;
      ::ImmSetCompositionWindow(himc, &cf);
      ::ImmReleaseContext(hwnd, himc);
    }
}

#else

static void ImeSetInputScreenPosFn_DefaultImpl(int, int) {}

#endif

//-----------------------------------------------------------------------------
// [SECTION] METRICS/DEBUGGER WINDOW
//-----------------------------------------------------------------------------
// - RenderViewportThumbnail() [Internal]
// - RenderViewportsThumbnails() [Internal]
// - MetricsHelpMarker() [Internal]
// - ShowMetricsWindow()
// - DebugNodeColumns() [Internal]
// - DebugNodeDrawList() [Internal]
// - DebugNodeDrawCmdShowMeshAndBoundingBox() [Internal]
// - DebugNodeStorage() [Internal]
// - DebugNodeTabBar() [Internal]
// - DebugNodeViewport() [Internal]
// - DebugNodeWindow() [Internal]
// - DebugNodeWindowSettings() [Internal]
// - DebugNodeWindowsList() [Internal]
//-----------------------------------------------------------------------------

#ifndef ANCHOR_DISABLE_METRICS_WINDOW

void ANCHOR::DebugRenderViewportThumbnail(AnchorDrawList *draw_list,
                                          AnchorViewportP *viewport,
                                          const AnchorBBox &bb)
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  /** todo::check_math */
  wabi::GfVec2f scale = wabi::GfVec2f(bb.GetSize()[0] / viewport->Size[0],
                          bb.GetSize()[1] / viewport->Size[1]);
  wabi::GfVec2f off = wabi::GfVec2f(bb.Min[0] - viewport->Pos[0] * scale[0],
                        bb.Min[1] - viewport->Pos[1] * scale[1]);
  float alpha_mul = 1.0f;
  window->DrawList->AddRectFilled(bb.Min,
                                  bb.Max,
                                  GetColorU32(AnchorCol_Border, alpha_mul * 0.40f));
  for (int i = 0; i != g.Windows.Size; i++) {
    AnchorWindow *thumb_window = g.Windows[i];
    if (!thumb_window->WasActive || (thumb_window->Flags & AnchorWindowFlags_ChildWindow))
      continue;

    AnchorBBox thumb_r = thumb_window->Rect();
    AnchorBBox title_r = thumb_window->TitleBarRect();

    /** todo::check_math */
    thumb_r = AnchorBBox(
      AnchorFloor(wabi::GfVec2f(off[0] + thumb_r.Min[0] * scale[0], off[1] + thumb_r.Min[1] * scale[1])),
      AnchorFloor(
        wabi::GfVec2f(off[0] + thumb_r.Max[0] * scale[0], off[1] + thumb_r.Max[1] * scale[1])));

    title_r = AnchorBBox(
      AnchorFloor(wabi::GfVec2f(off[0] + title_r.Min[0] * scale[0], off[1] + title_r.Min[1] * scale[1])),
      AnchorFloor(wabi::GfVec2f(off[0] + wabi::GfVec2f(title_r.Max[0], title_r.Min[1])[0] * scale[0],
                          off[1] + wabi::GfVec2f(title_r.Max[0], title_r.Min[1])[1] * scale[1]) +
                  wabi::GfVec2f(0, 5)));  // Exaggerate title bar height

    thumb_r.ClipWithFull(bb);
    title_r.ClipWithFull(bb);
    const bool window_is_focused = (g.NavWindow && thumb_window->RootWindowForTitleBarHighlight ==
                                                     g.NavWindow->RootWindowForTitleBarHighlight);
    window->DrawList->AddRectFilled(thumb_r.Min,
                                    thumb_r.Max,
                                    GetColorU32(AnchorCol_WindowBg, alpha_mul));
    window->DrawList->AddRectFilled(
      title_r.Min,
      title_r.Max,
      GetColorU32(window_is_focused ? AnchorCol_TitleBgActive : AnchorCol_TitleBg, alpha_mul));
    window->DrawList->AddRect(thumb_r.Min, thumb_r.Max, GetColorU32(AnchorCol_Border, alpha_mul));
    window->DrawList->AddText(g.Font,
                              g.FontSize * 1.0f,
                              title_r.Min,
                              GetColorU32(AnchorCol_Text, alpha_mul),
                              thumb_window->Name,
                              FindRenderedTextEnd(thumb_window->Name));
  }
  draw_list->AddRect(bb.Min, bb.Max, GetColorU32(AnchorCol_Border, alpha_mul));
}

static void RenderViewportsThumbnails()
{
  AnchorContext &g = *G_CTX;
  AnchorWindow *window = g.CurrentWindow;

  // We don't display full monitor bounds (we could, but it often looks awkward), instead we
  // display just enough to cover all of our viewports.
  float SCALE = 1.0f / 8.0f;
  AnchorBBox bb_full(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);
  for (int n = 0; n < g.Viewports.Size; n++)
    bb_full.Add(g.Viewports[n]->GetMainRect());
  wabi::GfVec2f p = window->DC.CursorPos;
  wabi::GfVec2f off = p - bb_full.Min * SCALE;
  for (int n = 0; n < g.Viewports.Size; n++) {
    AnchorViewportP *viewport = g.Viewports[n];
    AnchorBBox viewport_draw_bb(off + (viewport->Pos) * SCALE,
                                off + (viewport->Pos + viewport->Size) * SCALE);
    ANCHOR::DebugRenderViewportThumbnail(window->DrawList, viewport, viewport_draw_bb);
  }
  ANCHOR::Dummy(bb_full.GetSize() * SCALE);
}

// Avoid naming collision with ANCHOR_demo.cpp's HelpMarker() for unity builds.
static void MetricsHelpMarker(const char *desc)
{
  ANCHOR::TextDisabled("(?)");
  if (ANCHOR::IsItemHovered()) {
    ANCHOR::BeginTooltip();
    ANCHOR::PushTextWrapPos(ANCHOR::GetFontSize() * 35.0f);
    ANCHOR::TextUnformatted(desc);
    ANCHOR::PopTextWrapPos();
    ANCHOR::EndTooltip();
  }
}

namespace ANCHOR
{
  void ShowFontAtlas(AnchorFontAtlas *atlas);
}

void ANCHOR::ShowMetricsWindow(bool *p_open)
{
  if (!Begin("ANCHOR Metrics/Debugger", p_open)) {
    End();
    return;
  }

  AnchorContext &g = *G_CTX;
  AnchorIO &io = g.IO;
  AnchorMetricsConfig *cfg = &g.DebugMetricsConfig;

  // Basic info
  Text("ANCHOR %s", GetVersion());
  Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
  Text("%d vertices, %d indices (%d triangles)",
       io.MetricsRenderVertices,
       io.MetricsRenderIndices,
       io.MetricsRenderIndices / 3);
  Text("%d active windows (%d visible)", io.MetricsActiveWindows, io.MetricsRenderWindows);
  Text("%d active allocations", io.MetricsActiveAllocations);
  // SameLine(); if (SmallButton("GC")) { g.GcCompactAll = true; }

  Separator();

  // Debugging enums
  enum
  {
    WRT_OuterRect,
    WRT_OuterRectClipped,
    WRT_InnerRect,
    WRT_InnerClipRect,
    WRT_WorkRect,
    WRT_Content,
    WRT_ContentIdeal,
    WRT_ContentRegionRect,
    WRT_Count
  };  // Windows Rect Type
  const char *wrt_rects_names[WRT_Count] = {"OuterRect",
                                            "OuterRectClipped",
                                            "InnerRect",
                                            "InnerClipRect",
                                            "WorkRect",
                                            "Content",
                                            "ContentIdeal",
                                            "ContentRegionRect"};
  enum
  {
    TRT_OuterRect,
    TRT_InnerRect,
    TRT_WorkRect,
    TRT_HostClipRect,
    TRT_InnerClipRect,
    TRT_BackgroundClipRect,
    TRT_ColumnsRect,
    TRT_ColumnsWorkRect,
    TRT_ColumnsClipRect,
    TRT_ColumnsContentHeadersUsed,
    TRT_ColumnsContentHeadersIdeal,
    TRT_ColumnsContentFrozen,
    TRT_ColumnsContentUnfrozen,
    TRT_Count
  };  // Tables Rect Type
  const char *trt_rects_names[TRT_Count] = {"OuterRect",
                                            "InnerRect",
                                            "WorkRect",
                                            "HostClipRect",
                                            "InnerClipRect",
                                            "BackgroundClipRect",
                                            "ColumnsRect",
                                            "ColumnsWorkRect",
                                            "ColumnsClipRect",
                                            "ColumnsContentHeadersUsed",
                                            "ColumnsContentHeadersIdeal",
                                            "ColumnsContentFrozen",
                                            "ColumnsContentUnfrozen"};
  if (cfg->ShowWindowsRectsType < 0)
    cfg->ShowWindowsRectsType = WRT_WorkRect;
  if (cfg->ShowTablesRectsType < 0)
    cfg->ShowTablesRectsType = TRT_WorkRect;

  struct Funcs
  {
    static AnchorBBox GetTableRect(AnchorTable *table, int rect_type, int n)
    {
      if (rect_type == TRT_OuterRect) {
        return table->OuterRect;
      } else if (rect_type == TRT_InnerRect) {
        return table->InnerRect;
      } else if (rect_type == TRT_WorkRect) {
        return table->WorkRect;
      } else if (rect_type == TRT_HostClipRect) {
        return table->HostClipRect;
      } else if (rect_type == TRT_InnerClipRect) {
        return table->InnerClipRect;
      } else if (rect_type == TRT_BackgroundClipRect) {
        return table->BgClipRect;
      } else if (rect_type == TRT_ColumnsRect) {
        AnchorTableColumn *c = &table->Columns[n];
        return AnchorBBox(c->MinX,
                          table->InnerClipRect.Min[1],
                          c->MaxX,
                          table->InnerClipRect.Min[1] + table->LastOuterHeight);
      } else if (rect_type == TRT_ColumnsWorkRect) {
        AnchorTableColumn *c = &table->Columns[n];
        return AnchorBBox(c->WorkMinX,
                          table->WorkRect.Min[1],
                          c->WorkMaxX,
                          table->WorkRect.Max[1]);
      } else if (rect_type == TRT_ColumnsClipRect) {
        AnchorTableColumn *c = &table->Columns[n];
        return c->ClipRect;
      } else if (rect_type == TRT_ColumnsContentHeadersUsed) {
        AnchorTableColumn *c = &table->Columns[n];
        return AnchorBBox(c->WorkMinX,
                          table->InnerClipRect.Min[1],
                          c->ContentMaxXHeadersUsed,
                          table->InnerClipRect.Min[1] + table->LastFirstRowHeight);
      }  // Note: y1/y2 not always accurate
      else if (rect_type == TRT_ColumnsContentHeadersIdeal) {
        AnchorTableColumn *c = &table->Columns[n];
        return AnchorBBox(c->WorkMinX,
                          table->InnerClipRect.Min[1],
                          c->ContentMaxXHeadersIdeal,
                          table->InnerClipRect.Min[1] + table->LastFirstRowHeight);
      } else if (rect_type == TRT_ColumnsContentFrozen) {
        AnchorTableColumn *c = &table->Columns[n];
        return AnchorBBox(c->WorkMinX,
                          table->InnerClipRect.Min[1],
                          c->ContentMaxXFrozen,
                          table->InnerClipRect.Min[1] + table->LastFirstRowHeight);
      } else if (rect_type == TRT_ColumnsContentUnfrozen) {
        AnchorTableColumn *c = &table->Columns[n];
        return AnchorBBox(c->WorkMinX,
                          table->InnerClipRect.Min[1] + table->LastFirstRowHeight,
                          c->ContentMaxXUnfrozen,
                          table->InnerClipRect.Max[1]);
      }
      ANCHOR_ASSERT(0);
      return AnchorBBox();
    }

    static AnchorBBox GetWindowRect(AnchorWindow *window, int rect_type)
    {
      if (rect_type == WRT_OuterRect) {
        return window->Rect();
      } else if (rect_type == WRT_OuterRectClipped) {
        return window->OuterRectClipped;
      } else if (rect_type == WRT_InnerRect) {
        return window->InnerRect;
      } else if (rect_type == WRT_InnerClipRect) {
        return window->InnerClipRect;
      } else if (rect_type == WRT_WorkRect) {
        return window->WorkRect;
      } else if (rect_type == WRT_Content) {
        wabi::GfVec2f min = window->InnerRect.Min - window->Scroll + window->WindowPadding;
        return AnchorBBox(min, min + window->ContentSize);
      } else if (rect_type == WRT_ContentIdeal) {
        wabi::GfVec2f min = window->InnerRect.Min - window->Scroll + window->WindowPadding;
        return AnchorBBox(min, min + window->ContentSizeIdeal);
      } else if (rect_type == WRT_ContentRegionRect) {
        return window->ContentRegionRect;
      }
      ANCHOR_ASSERT(0);
      return AnchorBBox();
    }
  };

  // Tools
  if (TreeNode("Tools")) {
    // The Item Picker tool is super useful to visually select an item and break into the
    // call-stack of where it was submitted.
    if (Button("Item Picker.."))
      DebugStartItemPicker();
    SameLine();
    MetricsHelpMarker(
      "Will call the IM_DEBUG_BREAK() macro to break in debugger.\nWarning: If you don't have a "
      "debugger attached, this will probably crash.");

    Checkbox("Show windows begin order", &cfg->ShowWindowsBeginOrder);
    Checkbox("Show windows rectangles", &cfg->ShowWindowsRects);
    SameLine();
    SetNextItemWidth(GetFontSize() * 12);
    cfg->ShowWindowsRects |= Combo("##show_windows_rect_type",
                                   &cfg->ShowWindowsRectsType,
                                   wrt_rects_names,
                                   WRT_Count,
                                   WRT_Count);
    if (cfg->ShowWindowsRects && g.NavWindow != NULL) {
      BulletText("'%s':", g.NavWindow->Name);
      Indent();
      for (int rect_n = 0; rect_n < WRT_Count; rect_n++) {
        AnchorBBox r = Funcs::GetWindowRect(g.NavWindow, rect_n);
        Text("(%6.1f,%6.1f) (%6.1f,%6.1f) Size (%6.1f,%6.1f) %s",
             r.Min[0],
             r.Min[1],
             r.Max[0],
             r.Max[1],
             r.GetWidth(),
             r.GetHeight(),
             wrt_rects_names[rect_n]);
      }
      Unindent();
    }
    Checkbox("Show AnchorDrawCmd mesh when hovering", &cfg->ShowDrawCmdMesh);
    Checkbox("Show AnchorDrawCmd bounding boxes when hovering", &cfg->ShowDrawCmdBoundingBoxes);

    Checkbox("Show tables rectangles", &cfg->ShowTablesRects);
    SameLine();
    SetNextItemWidth(GetFontSize() * 12);
    cfg->ShowTablesRects |= Combo("##show_table_rects_type",
                                  &cfg->ShowTablesRectsType,
                                  trt_rects_names,
                                  TRT_Count,
                                  TRT_Count);
    if (cfg->ShowTablesRects && g.NavWindow != NULL) {
      for (int table_n = 0; table_n < g.Tables.GetSize(); table_n++) {
        AnchorTable *table = g.Tables.GetByIndex(table_n);
        if (table->LastFrameActive < g.FrameCount - 1 ||
            (table->OuterWindow != g.NavWindow && table->InnerWindow != g.NavWindow))
          continue;

        BulletText("Table 0x%08X (%d columns, in '%s')",
                   table->ID,
                   table->ColumnsCount,
                   table->OuterWindow->Name);
        if (IsItemHovered())
          GetForegroundDrawList()->AddRect(table->OuterRect.Min - wabi::GfVec2f(1, 1),
                                           table->OuterRect.Max + wabi::GfVec2f(1, 1),
                                           ANCHOR_COL32(255, 255, 0, 255),
                                           0.0f,
                                           0,
                                           2.0f);
        Indent();
        char buf[128];
        for (int rect_n = 0; rect_n < TRT_Count; rect_n++) {
          if (rect_n >= TRT_ColumnsRect) {
            if (rect_n != TRT_ColumnsRect && rect_n != TRT_ColumnsClipRect)
              continue;
            for (int column_n = 0; column_n < table->ColumnsCount; column_n++) {
              AnchorBBox r = Funcs::GetTableRect(table, rect_n, column_n);
              AnchorFormatString(buf,
                                 ANCHOR_ARRAYSIZE(buf),
                                 "(%6.1f,%6.1f) (%6.1f,%6.1f) Size (%6.1f,%6.1f) Col %d %s",
                                 r.Min[0],
                                 r.Min[1],
                                 r.Max[0],
                                 r.Max[1],
                                 r.GetWidth(),
                                 r.GetHeight(),
                                 column_n,
                                 trt_rects_names[rect_n]);
              Selectable(buf);
              if (IsItemHovered())
                GetForegroundDrawList()->AddRect(r.Min - wabi::GfVec2f(1, 1),
                                                 r.Max + wabi::GfVec2f(1, 1),
                                                 ANCHOR_COL32(255, 255, 0, 255),
                                                 0.0f,
                                                 0,
                                                 2.0f);
            }
          } else {
            AnchorBBox r = Funcs::GetTableRect(table, rect_n, -1);
            AnchorFormatString(buf,
                               ANCHOR_ARRAYSIZE(buf),
                               "(%6.1f,%6.1f) (%6.1f,%6.1f) Size (%6.1f,%6.1f) %s",
                               r.Min[0],
                               r.Min[1],
                               r.Max[0],
                               r.Max[1],
                               r.GetWidth(),
                               r.GetHeight(),
                               trt_rects_names[rect_n]);
            Selectable(buf);
            if (IsItemHovered())
              GetForegroundDrawList()->AddRect(r.Min - wabi::GfVec2f(1, 1),
                                               r.Max + wabi::GfVec2f(1, 1),
                                               ANCHOR_COL32(255, 255, 0, 255),
                                               0.0f,
                                               0,
                                               2.0f);
          }
        }
        Unindent();
      }
    }

    TreePop();
  }

  // Windows
  DebugNodeWindowsList(&g.Windows, "Windows");
  // DebugNodeWindowsList(&g.WindowsFocusOrder, "WindowsFocusOrder");

  // DrawLists
  int drawlist_count = 0;
  for (int viewport_i = 0; viewport_i < g.Viewports.Size; viewport_i++)
    drawlist_count += g.Viewports[viewport_i]->DrawDataBuilder.GetDrawListCount();
  if (TreeNode("DrawLists", "DrawLists (%d)", drawlist_count)) {
    for (int viewport_i = 0; viewport_i < g.Viewports.Size; viewport_i++) {
      AnchorViewportP *viewport = g.Viewports[viewport_i];
      for (int layer_i = 0; layer_i < ANCHOR_ARRAYSIZE(viewport->DrawDataBuilder.Layers);
           layer_i++)
        for (int draw_list_i = 0; draw_list_i < viewport->DrawDataBuilder.Layers[layer_i].Size;
             draw_list_i++)
          DebugNodeDrawList(NULL,
                            viewport->DrawDataBuilder.Layers[layer_i][draw_list_i],
                            "DrawList");
    }
    TreePop();
  }

  // Viewports
  if (TreeNode("Viewports", "Viewports (%d)", g.Viewports.Size)) {
    Indent(GetTreeNodeToLabelSpacing());
    RenderViewportsThumbnails();
    Unindent(GetTreeNodeToLabelSpacing());
    for (int i = 0; i < g.Viewports.Size; i++)
      DebugNodeViewport(g.Viewports[i]);
    TreePop();
  }

  // Details for Popups
  if (TreeNode("Popups", "Popups (%d)", g.OpenPopupStack.Size)) {
    for (int i = 0; i < g.OpenPopupStack.Size; i++) {
      AnchorWindow *window = g.OpenPopupStack[i].Window;
      BulletText("PopupID: %08x, Window: '%s'%s%s",
                 g.OpenPopupStack[i].PopupId,
                 window ? window->Name : "NULL",
                 window && (window->Flags & AnchorWindowFlags_ChildWindow) ? " ChildWindow" : "",
                 window && (window->Flags & AnchorWindowFlags_ChildMenu) ? " ChildMenu" : "");
    }
    TreePop();
  }

  // Details for TabBars
  if (TreeNode("TabBars", "Tab Bars (%d)", g.TabBars.GetSize())) {
    for (int n = 0; n < g.TabBars.GetSize(); n++)
      DebugNodeTabBar(g.TabBars.GetByIndex(n), "TabBar");
    TreePop();
  }

  // Details for Tables
  if (TreeNode("Tables", "Tables (%d)", g.Tables.GetSize())) {
    for (int n = 0; n < g.Tables.GetSize(); n++)
      DebugNodeTable(g.Tables.GetByIndex(n));
    TreePop();
  }

  // Details for Fonts
#  ifndef ANCHOR_DISABLE_DEMO_WINDOWS
  AnchorFontAtlas *atlas = g.IO.Fonts;
  if (TreeNode("Fonts", "Fonts (%d)", atlas->Fonts.Size)) {
    ShowFontAtlas(atlas);
    TreePop();
  }
#  endif

  // Details for Docking
#  ifdef ANCHOR_HAS_DOCK
  if (TreeNode("Docking")) {
    TreePop();
  }
#  endif  // #ifdef ANCHOR_HAS_DOCK

  // Settings
  if (TreeNode("Settings")) {
    if (SmallButton("Clear"))
      ClearIniSettings();
    SameLine();
    if (SmallButton("Save to memory"))
      SaveIniSettingsToMemory();
    SameLine();
    if (SmallButton("Save to disk"))
      SaveIniSettingsToDisk(g.IO.IniFilename);
    SameLine();
    if (g.IO.IniFilename)
      Text("\"%s\"", g.IO.IniFilename);
    else
      TextUnformatted("<NULL>");
    Text("SettingsDirtyTimer %.2f", g.SettingsDirtyTimer);
    if (TreeNode("SettingsHandlers", "Settings handlers: (%d)", g.SettingsHandlers.Size)) {
      for (int n = 0; n < g.SettingsHandlers.Size; n++)
        BulletText("%s", g.SettingsHandlers[n].TypeName);
      TreePop();
    }
    if (TreeNode("SettingsWindows",
                 "Settings packed data: Windows: %d bytes",
                 g.SettingsWindows.size())) {
      for (AnchorWindowSettings *settings = g.SettingsWindows.begin(); settings != NULL;
           settings = g.SettingsWindows.next_chunk(settings))
        DebugNodeWindowSettings(settings);
      TreePop();
    }

    if (TreeNode("SettingsTables",
                 "Settings packed data: Tables: %d bytes",
                 g.SettingsTables.size())) {
      for (AnchorTableSettings *settings = g.SettingsTables.begin(); settings != NULL;
           settings = g.SettingsTables.next_chunk(settings))
        DebugNodeTableSettings(settings);
      TreePop();
    }

#  ifdef ANCHOR_HAS_DOCK
#  endif  // #ifdef ANCHOR_HAS_DOCK

    if (TreeNode("SettingsIniData",
                 "Settings unpacked data (.ini): %d bytes",
                 g.SettingsIniData.size())) {
      InputTextMultiline("##Ini",
                         (char *)(void *)g.SettingsIniData.c_str(),
                         g.SettingsIniData.Buf.Size,
                         wabi::GfVec2f(-FLT_MIN, GetTextLineHeight() * 20),
                         AnchorInputTextFlags_ReadOnly);
      TreePop();
    }
    TreePop();
  }

  // Misc Details
  if (TreeNode("Internal state")) {
    const char *input_source_names[] =
      {"None", "Mouse", "Keyboard", "Gamepad", "Nav", "Clipboard"};
    ANCHOR_ASSERT(ANCHOR_ARRAYSIZE(input_source_names) == ANCHORInputSource_COUNT);

    Text("WINDOWING");
    Indent();
    Text("HoveredWindow: '%s'", g.HoveredWindow ? g.HoveredWindow->Name : "NULL");
    Text("HoveredWindow->Root: '%s'",
         g.HoveredWindow ? g.HoveredWindow->RootWindow->Name : "NULL");
    Text("HoveredWindowUnderMovingWindow: '%s'",
         g.HoveredWindowUnderMovingWindow ? g.HoveredWindowUnderMovingWindow->Name : "NULL");
    Text("MovingWindow: '%s'", g.MovingWindow ? g.MovingWindow->Name : "NULL");
    Unindent();

    Text("ITEMS");
    Indent();
    Text("ActiveId: 0x%08X/0x%08X (%.2f sec), AllowOverlap: %d, Source: %s",
         g.ActiveId,
         g.ActiveIdPreviousFrame,
         g.ActiveIdTimer,
         g.ActiveIdAllowOverlap,
         input_source_names[g.ActiveIdSource]);
    Text("ActiveIdWindow: '%s'", g.ActiveIdWindow ? g.ActiveIdWindow->Name : "NULL");
    Text("HoveredId: 0x%08X/0x%08X (%.2f sec), AllowOverlap: %d",
         g.HoveredId,
         g.HoveredIdPreviousFrame,
         g.HoveredIdTimer,
         g.HoveredIdAllowOverlap);  // Data is "in-flight" so depending on when the Metrics window
                                    // is called we may see current frame information or not
    Text("DragDrop: %d, SourceId = 0x%08X, Payload \"%s\" (%d bytes)",
         g.DragDropActive,
         g.DragDropPayload.SourceId,
         g.DragDropPayload.DataType,
         g.DragDropPayload.DataSize);
    Unindent();

    Text("NAV,FOCUS");
    Indent();
    Text("NavWindow: '%s'", g.NavWindow ? g.NavWindow->Name : "NULL");
    Text("NavId: 0x%08X, NavLayer: %d", g.NavId, g.NavLayer);
    Text("NavInputSource: %s", input_source_names[g.NavInputSource]);
    Text("NavActive: %d, NavVisible: %d", g.IO.NavActive, g.IO.NavVisible);
    Text("NavActivateId: 0x%08X, NavInputId: 0x%08X", g.NavActivateId, g.NavInputId);
    Text("NavDisableHighlight: %d, NavDisableMouseHover: %d",
         g.NavDisableHighlight,
         g.NavDisableMouseHover);
    Text("NavFocusScopeId = 0x%08X", g.NavFocusScopeId);
    Text("NavWindowingTarget: '%s'", g.NavWindowingTarget ? g.NavWindowingTarget->Name : "NULL");
    Unindent();

    TreePop();
  }

  // Overlay: Display windows Rectangles and Begin Order
  if (cfg->ShowWindowsRects || cfg->ShowWindowsBeginOrder) {
    for (int n = 0; n < g.Windows.Size; n++) {
      AnchorWindow *window = g.Windows[n];
      if (!window->WasActive)
        continue;
      AnchorDrawList *draw_list = GetForegroundDrawList(window);
      if (cfg->ShowWindowsRects) {
        AnchorBBox r = Funcs::GetWindowRect(window, cfg->ShowWindowsRectsType);
        draw_list->AddRect(r.Min, r.Max, ANCHOR_COL32(255, 0, 128, 255));
      }
      if (cfg->ShowWindowsBeginOrder && !(window->Flags & AnchorWindowFlags_ChildWindow)) {
        char buf[32];
        AnchorFormatString(buf, ANCHOR_ARRAYSIZE(buf), "%d", window->BeginOrderWithinContext);
        float font_size = GetFontSize();
        draw_list->AddRectFilled(window->Pos,
                                 window->Pos + wabi::GfVec2f(font_size, font_size),
                                 ANCHOR_COL32(200, 100, 100, 255));
        draw_list->AddText(window->Pos, ANCHOR_COL32(255, 255, 255, 255), buf);
      }
    }
  }

  // Overlay: Display Tables Rectangles
  if (cfg->ShowTablesRects) {
    for (int table_n = 0; table_n < g.Tables.GetSize(); table_n++) {
      AnchorTable *table = g.Tables.GetByIndex(table_n);
      if (table->LastFrameActive < g.FrameCount - 1)
        continue;
      AnchorDrawList *draw_list = GetForegroundDrawList(table->OuterWindow);
      if (cfg->ShowTablesRectsType >= TRT_ColumnsRect) {
        for (int column_n = 0; column_n < table->ColumnsCount; column_n++) {
          AnchorBBox r = Funcs::GetTableRect(table, cfg->ShowTablesRectsType, column_n);
          AnchorU32 col = (table->HoveredColumnBody == column_n) ?
                            ANCHOR_COL32(255, 255, 128, 255) :
                            ANCHOR_COL32(255, 0, 128, 255);
          float thickness = (table->HoveredColumnBody == column_n) ? 3.0f : 1.0f;
          draw_list->AddRect(r.Min, r.Max, col, 0.0f, 0, thickness);
        }
      } else {
        AnchorBBox r = Funcs::GetTableRect(table, cfg->ShowTablesRectsType, -1);
        draw_list->AddRect(r.Min, r.Max, ANCHOR_COL32(255, 0, 128, 255));
      }
    }
  }

#  ifdef ANCHOR_HAS_DOCK
  // Overlay: Display Docking info
  if (show_docking_nodes && g.IO.KeyCtrl) {
  }
#  endif  // #ifdef ANCHOR_HAS_DOCK

  End();
}

// [DEBUG] List fonts in a font atlas and display its texture
void ANCHOR::ShowFontAtlas(AnchorFontAtlas *atlas)
{
  for (int i = 0; i < atlas->Fonts.Size; i++) {
    AnchorFont *font = atlas->Fonts[i];
    PushID(font);
    DebugNodeFont(font);
    PopID();
  }
  if (TreeNode("Atlas texture",
               "Atlas texture (%dx%d pixels)",
               atlas->TexWidth,
               atlas->TexHeight)) {
    wabi::GfVec4f tint_col = wabi::GfVec4f(1.0f, 1.0f, 1.0f, 1.0f);
    wabi::GfVec4f border_col = wabi::GfVec4f(1.0f, 1.0f, 1.0f, 0.5f);
    Image(atlas->TexID,
          wabi::GfVec2f((float)atlas->TexWidth, (float)atlas->TexHeight),
          wabi::GfVec2f(0.0f, 0.0f),
          wabi::GfVec2f(1.0f, 1.0f),
          tint_col,
          border_col);
    TreePop();
  }
}

// [DEBUG] Display contents of Columns
void ANCHOR::DebugNodeColumns(AnchorOldColumns *columns)
{
  if (!TreeNode((void *)(uintptr_t)columns->ID,
                "Columns Id: 0x%08X, Count: %d, Flags: 0x%04X",
                columns->ID,
                columns->Count,
                columns->Flags))
    return;
  BulletText("Width: %.1f (MinX: %.1f, MaxX: %.1f)",
             columns->OffMaxX - columns->OffMinX,
             columns->OffMinX,
             columns->OffMaxX);
  for (int column_n = 0; column_n < columns->Columns.Size; column_n++)
    BulletText("Column %02d: OffsetNorm %.3f (= %.1f px)",
               column_n,
               columns->Columns[column_n].OffsetNorm,
               GetColumnOffsetFromNorm(columns, columns->Columns[column_n].OffsetNorm));
  TreePop();
}

// [DEBUG] Display contents of AnchorDrawList
void ANCHOR::DebugNodeDrawList(AnchorWindow *window,
                               const AnchorDrawList *draw_list,
                               const char *label)
{
  AnchorContext &g = *G_CTX;
  AnchorMetricsConfig *cfg = &g.DebugMetricsConfig;
  int cmd_count = draw_list->CmdBuffer.Size;
  if (cmd_count > 0 && draw_list->CmdBuffer.back().ElemCount == 0 &&
      draw_list->CmdBuffer.back().UserCallback == NULL)
    cmd_count--;
  bool node_open = TreeNode(draw_list,
                            "%s: '%s' %d vtx, %d indices, %d cmds",
                            label,
                            draw_list->_OwnerName ? draw_list->_OwnerName : "",
                            draw_list->VtxBuffer.Size,
                            draw_list->IdxBuffer.Size,
                            cmd_count);
  if (draw_list == GetWindowDrawList()) {
    SameLine();
    TextColored(wabi::GfVec4f(1.0f, 0.4f, 0.4f, 1.0f),
                "CURRENTLY APPENDING");  // Can't display stats for active draw list! (we don't
                                         // have the data double-buffered)
    if (node_open)
      TreePop();
    return;
  }

  AnchorDrawList *fg_draw_list = GetForegroundDrawList(
    window);  // Render additional visuals into the top-most draw list
  if (window && IsItemHovered())
    fg_draw_list->AddRect(window->Pos, window->Pos + window->Size, ANCHOR_COL32(255, 255, 0, 255));
  if (!node_open)
    return;

  if (window && !window->WasActive)
    TextDisabled("Warning: owning Window is inactive. This DrawList is not being rendered!");

  for (const AnchorDrawCmd *pcmd = draw_list->CmdBuffer.Data;
       pcmd < draw_list->CmdBuffer.Data + cmd_count;
       pcmd++) {
    if (pcmd->UserCallback) {
      BulletText("Callback %p, user_data %p", pcmd->UserCallback, pcmd->UserCallbackData);
      continue;
    }

    char buf[300];
    AnchorFormatString(buf,
                       ANCHOR_ARRAYSIZE(buf),
                       "DrawCmd:%5d tris, Tex 0x%p, ClipRect (%4.0f,%4.0f)-(%4.0f,%4.0f)",
                       pcmd->ElemCount / 3,
                       (void *)(intptr_t)pcmd->TextureId,
                       pcmd->ClipRect[0],
                       pcmd->ClipRect[1],
                       pcmd->ClipRect[2],
                       pcmd->ClipRect[3]);
    bool pcmd_node_open = TreeNode((void *)(pcmd - draw_list->CmdBuffer.begin()), "%s", buf);
    if (IsItemHovered() && (cfg->ShowDrawCmdMesh || cfg->ShowDrawCmdBoundingBoxes) && fg_draw_list)
      DebugNodeDrawCmdShowMeshAndBoundingBox(fg_draw_list,
                                             draw_list,
                                             pcmd,
                                             cfg->ShowDrawCmdMesh,
                                             cfg->ShowDrawCmdBoundingBoxes);
    if (!pcmd_node_open)
      continue;

    // Calculate approximate coverage area (touched pixel count)
    // This will be in pixels squared as long there's no post-scaling happening to the renderer
    // output.
    const AnchorDrawIdx *idx_buffer = (draw_list->IdxBuffer.Size > 0) ? draw_list->IdxBuffer.Data :
                                                                        NULL;
    const AnchorDrawVert *vtx_buffer = draw_list->VtxBuffer.Data + pcmd->VtxOffset;
    float total_area = 0.0f;
    for (unsigned int idx_n = pcmd->IdxOffset; idx_n < pcmd->IdxOffset + pcmd->ElemCount;) {
      wabi::GfVec2f triangle[3];
      for (int n = 0; n < 3; n++, idx_n++)
        triangle[n] = vtx_buffer[idx_buffer ? idx_buffer[idx_n] : idx_n].pos;
      total_area += AnchorTriangleArea(triangle[0], triangle[1], triangle[2]);
    }

    // Display vertex information summary. Hover to get all triangles drawn in wire-frame
    AnchorFormatString(buf,
                       ANCHOR_ARRAYSIZE(buf),
                       "Mesh: ElemCount: %d, VtxOffset: +%d, IdxOffset: +%d, Area: ~%0.f px",
                       pcmd->ElemCount,
                       pcmd->VtxOffset,
                       pcmd->IdxOffset,
                       total_area);
    Selectable(buf);
    if (IsItemHovered() && fg_draw_list)
      DebugNodeDrawCmdShowMeshAndBoundingBox(fg_draw_list, draw_list, pcmd, true, false);

    // Display individual triangles/vertices. Hover on to get the corresponding triangle
    // highlighted.
    AnchorListClipper clipper;
    clipper.Begin(pcmd->ElemCount / 3);  // Manually coarse clip our print out of individual
                                         // vertices to save CPU, only items that may be visible.
    while (clipper.Step())
      for (int prim = clipper.DisplayStart, idx_i = pcmd->IdxOffset + clipper.DisplayStart * 3;
           prim < clipper.DisplayEnd;
           prim++) {
        char *buf_p = buf, *buf_end = buf + ANCHOR_ARRAYSIZE(buf);
        wabi::GfVec2f triangle[3];
        for (int n = 0; n < 3; n++, idx_i++) {
          const AnchorDrawVert &v = vtx_buffer[idx_buffer ? idx_buffer[idx_i] : idx_i];
          triangle[n] = v.pos;
          buf_p += AnchorFormatString(buf_p,
                                      buf_end - buf_p,
                                      "%s %04d: pos (%8.2f,%8.2f), uv (%.6f,%.6f), col %08X\n",
                                      (n == 0) ? "Vert:" : "     ",
                                      idx_i,
                                      v.pos[0],
                                      v.pos[1],
                                      v.uv[0],
                                      v.uv[1],
                                      v.col);
        }

        Selectable(buf, false);
        if (fg_draw_list && IsItemHovered()) {
          AnchorDrawListFlags backup_flags = fg_draw_list->Flags;
          fg_draw_list->Flags &=
            ~AnchorDrawListFlags_AntiAliasedLines;  // Disable AA on triangle outlines is more
                                                    // readable for very large and thin triangles.
          fg_draw_list->AddPolyline(triangle,
                                    3,
                                    ANCHOR_COL32(255, 255, 0, 255),
                                    AnchorDrawFlags_Closed,
                                    1.0f);
          fg_draw_list->Flags = backup_flags;
        }
      }
    TreePop();
  }
  TreePop();
}

// [DEBUG] Display mesh/aabb of a AnchorDrawCmd
void ANCHOR::DebugNodeDrawCmdShowMeshAndBoundingBox(AnchorDrawList *out_draw_list,
                                                    const AnchorDrawList *draw_list,
                                                    const AnchorDrawCmd *draw_cmd,
                                                    bool show_mesh,
                                                    bool show_aabb)
{
  ANCHOR_ASSERT(show_mesh || show_aabb);
  AnchorDrawIdx *idx_buffer = (draw_list->IdxBuffer.Size > 0) ? draw_list->IdxBuffer.Data : NULL;
  AnchorDrawVert *vtx_buffer = draw_list->VtxBuffer.Data + draw_cmd->VtxOffset;

  // Draw wire-frame version of all triangles
  AnchorBBox clip_rect = draw_cmd->ClipRect;
  AnchorBBox vtxs_rect(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);
  AnchorDrawListFlags backup_flags = out_draw_list->Flags;
  out_draw_list->Flags &=
    ~AnchorDrawListFlags_AntiAliasedLines;  // Disable AA on triangle outlines is more
                                            // readable for very large and thin triangles.
  for (unsigned int idx_n = draw_cmd->IdxOffset;
       idx_n < draw_cmd->IdxOffset + draw_cmd->ElemCount;) {
    wabi::GfVec2f triangle[3];
    for (int n = 0; n < 3; n++, idx_n++)
      vtxs_rect.Add((triangle[n] = vtx_buffer[idx_buffer ? idx_buffer[idx_n] : idx_n].pos));
    if (show_mesh)
      out_draw_list->AddPolyline(triangle,
                                 3,
                                 ANCHOR_COL32(255, 255, 0, 255),
                                 AnchorDrawFlags_Closed,
                                 1.0f);  // In yellow: mesh triangles
  }
  // Draw bounding boxes
  if (show_aabb) {
    out_draw_list->AddRect(
      AnchorFloor(clip_rect.Min),
      AnchorFloor(clip_rect.Max),
      ANCHOR_COL32(255, 0, 255, 255));  // In pink: clipping rectangle submitted to GPU
    out_draw_list->AddRect(AnchorFloor(vtxs_rect.Min),
                           AnchorFloor(vtxs_rect.Max),
                           ANCHOR_COL32(0, 255, 255, 255));  // In cyan: bounding box of triangles
  }
  out_draw_list->Flags = backup_flags;
}

// [DEBUG] Display details for a single font, called by ShowStyleEditor().
void ANCHOR::DebugNodeFont(AnchorFont *font)
{
  bool opened = TreeNode(font,
                         "Font: \"%s\"\n%.2f px, %d glyphs, %d file(s)",
                         font->ConfigData ? font->ConfigData[0].Name : "",
                         font->FontSize,
                         font->Glyphs.Size,
                         font->ConfigDataCount);
  SameLine();
  if (SmallButton("Set as default"))
    GetIO().FontDefault = font;
  if (!opened)
    return;

  // Display preview text
  PushFont(font);
  Text("The quick brown fox jumps over the lazy dog");
  PopFont();

  // Display details
  SetNextItemWidth(GetFontSize() * 8);
  DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");
  SameLine();
  MetricsHelpMarker(
    "Note than the default embedded font is NOT meant to be scaled.\n\n"
    "Font are currently rendered into bitmaps at a given size at the time of building the "
    "atlas. "
    "You may oversample them to get some flexibility with scaling. "
    "You can also render at multiple sizes and select which one to use at runtime.\n\n"
    "(Glimmer of hope: the atlas system will be rewritten in the future to make scaling more "
    "flexible.)");
  Text("Ascent: %f, Descent: %f, Height: %f",
       font->Ascent,
       font->Descent,
       font->Ascent - font->Descent);
  char c_str[5];
  Text("Fallback character: '%s' (U+%04X)",
       AnchorTextCharToUtf8(c_str, font->FallbackChar),
       font->FallbackChar);
  Text("Ellipsis character: '%s' (U+%04X)",
       AnchorTextCharToUtf8(c_str, font->EllipsisChar),
       font->EllipsisChar);
  const int surface_sqrt = (int)AnchorSqrt((float)font->MetricsTotalSurface);
  Text("Texture Area: about %d px ~%dx%d px",
       font->MetricsTotalSurface,
       surface_sqrt,
       surface_sqrt);
  for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
    if (font->ConfigData)
      if (const AnchorFontConfig *cfg = &font->ConfigData[config_i])
        BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d, Offset: (%.1f,%.1f)",
                   config_i,
                   cfg->Name,
                   cfg->OversampleH,
                   cfg->OversampleV,
                   cfg->PixelSnapH,
                   cfg->GlyphOffset[0],
                   cfg->GlyphOffset[1]);

  // Display all glyphs of the fonts in separate pages of 256 characters
  if (TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size)) {
    AnchorDrawList *draw_list = GetWindowDrawList();
    const AnchorU32 glyph_col = GetColorU32(AnchorCol_Text);
    const float cell_size = font->FontSize * 1;
    const float cell_spacing = GetStyle().ItemSpacing[1];
    for (unsigned int base = 0; base <= IM_UNICODE_CODEPOINT_MAX; base += 256) {
      // Skip ahead if a large bunch of glyphs are not present in the font (test in chunks of 4k)
      // This is only a small optimization to reduce the number of iterations when
      // IM_UNICODE_MAX_CODEPOINT is large // (if AnchorWChar==AnchorWChar32 we will do at least
      // about 272 queries here)
      if (!(base & 4095) && font->IsGlyphRangeUnused(base, base + 4095)) {
        base += 4096 - 256;
        continue;
      }

      int count = 0;
      for (unsigned int n = 0; n < 256; n++)
        if (font->FindGlyphNoFallback((AnchorWChar)(base + n)))
          count++;
      if (count <= 0)
        continue;
      if (!TreeNode((void *)(intptr_t)base,
                    "U+%04X..U+%04X (%d %s)",
                    base,
                    base + 255,
                    count,
                    count > 1 ? "glyphs" : "glyph"))
        continue;

      // Draw a 16x16 grid of glyphs
      wabi::GfVec2f base_pos = GetCursorScreenPos();
      for (unsigned int n = 0; n < 256; n++) {
        // We use AnchorFont::RenderChar as a shortcut because we don't have UTF-8 conversion
        // functions available here and thus cannot easily generate a zero-terminated UTF-8 encoded
        // string.
        wabi::GfVec2f cell_p1(base_pos[0] + (n % 16) * (cell_size + cell_spacing),
                        base_pos[1] + (n / 16) * (cell_size + cell_spacing));
        wabi::GfVec2f cell_p2(cell_p1[0] + cell_size, cell_p1[1] + cell_size);
        const AnchorFontGlyph *glyph = font->FindGlyphNoFallback((AnchorWChar)(base + n));
        draw_list->AddRect(cell_p1,
                           cell_p2,
                           glyph ? ANCHOR_COL32(255, 255, 255, 100) :
                                   ANCHOR_COL32(255, 255, 255, 50));
        if (glyph)
          font->RenderChar(draw_list, cell_size, cell_p1, glyph_col, (AnchorWChar)(base + n));
        if (glyph && IsMouseHoveringRect(cell_p1, cell_p2)) {
          BeginTooltip();
          Text("Codepoint: U+%04X", base + n);
          Separator();
          Text("Visible: %d", glyph->Visible);
          Text("AdvanceX: %.1f", glyph->AdvanceX);
          Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
          Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
          EndTooltip();
        }
      }
      Dummy(wabi::GfVec2f((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
      TreePop();
    }
    TreePop();
  }
  TreePop();
}

// [DEBUG] Display contents of AnchorStorage
void ANCHOR::DebugNodeStorage(AnchorStorage *storage, const char *label)
{
  if (!TreeNode(label,
                "%s: %d entries, %d bytes",
                label,
                storage->Data.Size,
                storage->Data.size_in_bytes()))
    return;
  for (int n = 0; n < storage->Data.Size; n++) {
    const AnchorStorage::AnchorStoragePair &p = storage->Data[n];
    BulletText(
      "Key 0x%08X Value { i: %d }",
      p.key,
      p.val_i);  // Important: we currently don't store a type, real value may not be integer.
  }
  TreePop();
}

// [DEBUG] Display contents of AnchorTabBar
void ANCHOR::DebugNodeTabBar(AnchorTabBar *tab_bar, const char *label)
{
  // Standalone tab bars (not associated to docking/windows functionality) currently hold no
  // discernible strings.
  char buf[256];
  char *p = buf;
  const char *buf_end = buf + ANCHOR_ARRAYSIZE(buf);
  const bool is_active = (tab_bar->PrevFrameVisible >= GetFrameCount() - 2);
  p += AnchorFormatString(p,
                          buf_end - p,
                          "%s 0x%08X (%d tabs)%s",
                          label,
                          tab_bar->ID,
                          tab_bar->Tabs.Size,
                          is_active ? "" : " *Inactive*");
  TF_UNUSED(p);
  if (!is_active) {
    PushStyleColor(AnchorCol_Text, GetStyleColorVec4(AnchorCol_TextDisabled));
  }
  bool open = TreeNode(tab_bar, "%s", buf);
  if (!is_active) {
    PopStyleColor();
  }
  if (is_active && IsItemHovered()) {
    AnchorDrawList *draw_list = GetForegroundDrawList();
    draw_list->AddRect(tab_bar->BarRect.Min, tab_bar->BarRect.Max, ANCHOR_COL32(255, 255, 0, 255));
    draw_list->AddLine(wabi::GfVec2f(tab_bar->ScrollingRectMinX, tab_bar->BarRect.Min[1]),
                       wabi::GfVec2f(tab_bar->ScrollingRectMinX, tab_bar->BarRect.Max[1]),
                       ANCHOR_COL32(0, 255, 0, 255));
    draw_list->AddLine(wabi::GfVec2f(tab_bar->ScrollingRectMaxX, tab_bar->BarRect.Min[1]),
                       wabi::GfVec2f(tab_bar->ScrollingRectMaxX, tab_bar->BarRect.Max[1]),
                       ANCHOR_COL32(0, 255, 0, 255));
  }
  if (open) {
    for (int tab_n = 0; tab_n < tab_bar->Tabs.Size; tab_n++) {
      const AnchorTabItem *tab = &tab_bar->Tabs[tab_n];
      PushID(tab);
      if (SmallButton("<")) {
        TabBarQueueReorder(tab_bar, tab, -1);
      }
      SameLine(0, 2);
      if (SmallButton(">")) {
        TabBarQueueReorder(tab_bar, tab, +1);
      }
      SameLine();
      Text("%02d%c Tab 0x%08X '%s' Offset: %.1f, Width: %.1f/%.1f",
           tab_n,
           (tab->ID == tab_bar->SelectedTabId) ? '*' : ' ',
           tab->ID,
           (tab->NameOffset != -1) ? tab_bar->GetTabName(tab) : "",
           tab->Offset,
           tab->Width,
           tab->ContentWidth);
      PopID();
    }
    TreePop();
  }
}

void ANCHOR::DebugNodeViewport(AnchorViewportP *viewport)
{
  SetNextItemOpen(true, AnchorCond_Once);
  if (TreeNode("viewport0", "Viewport #%d", 0)) {
    AnchorWindowFlags flags = viewport->Flags;
    BulletText(
      "Main Pos: (%.0f,%.0f), Size: (%.0f,%.0f)\nWorkArea Offset Left: %.0f Top: %.0f, Right: "
      "%.0f, Bottom: %.0f",
      viewport->Pos[0],
      viewport->Pos[1],
      viewport->Size[0],
      viewport->Size[1],
      viewport->WorkOffsetMin[0],
      viewport->WorkOffsetMin[1],
      viewport->WorkOffsetMax[0],
      viewport->WorkOffsetMax[1]);
    BulletText("Flags: 0x%04X =%s%s%s",
               viewport->Flags,
               (flags & AnchorViewportFlags_IsPlatformWindow) ? " IsPlatformWindow" : "",
               (flags & AnchorViewportFlags_IsPlatformMonitor) ? " IsPlatformMonitor" : "",
               (flags & AnchorViewportFlags_OwnedByApp) ? " OwnedByApp" : "");
    for (int layer_i = 0; layer_i < ANCHOR_ARRAYSIZE(viewport->DrawDataBuilder.Layers); layer_i++)
      for (int draw_list_i = 0; draw_list_i < viewport->DrawDataBuilder.Layers[layer_i].Size;
           draw_list_i++)
        DebugNodeDrawList(NULL,
                          viewport->DrawDataBuilder.Layers[layer_i][draw_list_i],
                          "DrawList");
    TreePop();
  }
}

void ANCHOR::DebugNodeWindow(AnchorWindow *window, const char *label)
{
  if (window == NULL) {
    BulletText("%s: NULL", label);
    return;
  }

  AnchorContext &g = *G_CTX;
  const bool is_active = window->WasActive;
  AnchorTreeNodeFlags tree_node_flags = (window == g.NavWindow) ? AnchorTreeNodeFlags_Selected :
                                                                  AnchorTreeNodeFlags_None;
  if (!is_active) {
    PushStyleColor(AnchorCol_Text, GetStyleColorVec4(AnchorCol_TextDisabled));
  }
  const bool open = TreeNodeEx(label,
                               tree_node_flags,
                               "%s '%s'%s",
                               label,
                               window->Name,
                               is_active ? "" : " *Inactive*");
  if (!is_active) {
    PopStyleColor();
  }
  if (IsItemHovered() && is_active)
    GetForegroundDrawList(window)->AddRect(window->Pos,
                                           window->Pos + window->Size,
                                           ANCHOR_COL32(255, 255, 0, 255));
  if (!open)
    return;

  if (window->MemoryCompacted)
    TextDisabled("Note: some memory buffers have been compacted/freed.");

  AnchorWindowFlags flags = window->Flags;
  DebugNodeDrawList(window, window->DrawList, "DrawList");
  BulletText("Pos: (%.1f,%.1f), Size: (%.1f,%.1f), ContentSize (%.1f,%.1f) Ideal (%.1f,%.1f)",
             window->Pos[0],
             window->Pos[1],
             window->Size[0],
             window->Size[1],
             window->ContentSize[0],
             window->ContentSize[1],
             window->ContentSizeIdeal[0],
             window->ContentSizeIdeal[1]);
  BulletText("Flags: 0x%08X (%s%s%s%s%s%s%s%s%s..)",
             flags,
             (flags & AnchorWindowFlags_ChildWindow) ? "Child " : "",
             (flags & AnchorWindowFlags_Tooltip) ? "Tooltip " : "",
             (flags & AnchorWindowFlags_Popup) ? "Popup " : "",
             (flags & AnchorWindowFlags_Modal) ? "Modal " : "",
             (flags & AnchorWindowFlags_ChildMenu) ? "ChildMenu " : "",
             (flags & AnchorWindowFlags_NoSavedSettings) ? "NoSavedSettings " : "",
             (flags & AnchorWindowFlags_NoMouseInputs) ? "NoMouseInputs" : "",
             (flags & AnchorWindowFlags_NoNavInputs) ? "NoNavInputs" : "",
             (flags & AnchorWindowFlags_AlwaysAutoResize) ? "AlwaysAutoResize" : "");
  BulletText("Scroll: (%.2f/%.2f,%.2f/%.2f) Scrollbar:%s%s",
             window->Scroll[0],
             window->ScrollMax[0],
             window->Scroll[1],
             window->ScrollMax[1],
             window->ScrollbarX ? "X" : "",
             window->ScrollbarY ? "Y" : "");
  BulletText("Active: %d/%d, WriteAccessed: %d, BeginOrderWithinContext: %d",
             window->Active,
             window->WasActive,
             window->WriteAccessed,
             (window->Active || window->WasActive) ? window->BeginOrderWithinContext : -1);
  BulletText("Appearing: %d, Hidden: %d (CanSkip %d Cannot %d), SkipItems: %d",
             window->Appearing,
             window->Hidden,
             window->HiddenFramesCanSkipItems,
             window->HiddenFramesCannotSkipItems,
             window->SkipItems);
  for (int layer = 0; layer < ANCHORNavLayer_COUNT; layer++) {
    AnchorBBox r = window->NavRectRel[layer];
    if (r.Min[0] >= r.Max[1] && r.Min[1] >= r.Max[1]) {
      BulletText("NavLastIds[%d]: 0x%08X", layer, window->NavLastIds[layer]);
      continue;
    }
    BulletText("NavLastIds[%d]: 0x%08X at +(%.1f,%.1f)(%.1f,%.1f)",
               layer,
               window->NavLastIds[layer],
               r.Min[0],
               r.Min[1],
               r.Max[0],
               r.Max[1]);
    if (IsItemHovered())
      GetForegroundDrawList(window)->AddRect(r.Min + window->Pos,
                                             r.Max + window->Pos,
                                             ANCHOR_COL32(255, 255, 0, 255));
  }
  BulletText("NavLayersActiveMask: %X, NavLastChildNavWindow: %s",
             window->DC.NavLayersActiveMask,
             window->NavLastChildNavWindow ? window->NavLastChildNavWindow->Name : "NULL");
  if (window->RootWindow != window) {
    DebugNodeWindow(window->RootWindow, "RootWindow");
  }
  if (window->ParentWindow != NULL) {
    DebugNodeWindow(window->ParentWindow, "ParentWindow");
  }
  if (window->DC.ChildWindows.Size > 0) {
    DebugNodeWindowsList(&window->DC.ChildWindows, "ChildWindows");
  }
  if (window->ColumnsStorage.Size > 0 &&
      TreeNode("Columns", "Columns sets (%d)", window->ColumnsStorage.Size)) {
    for (int n = 0; n < window->ColumnsStorage.Size; n++)
      DebugNodeColumns(&window->ColumnsStorage[n]);
    TreePop();
  }
  DebugNodeStorage(&window->StateStorage, "Storage");
  TreePop();
}

void ANCHOR::DebugNodeWindowSettings(AnchorWindowSettings *settings)
{
  Text("0x%08X \"%s\" Pos (%d,%d) Size (%d,%d) Collapsed=%d",
       settings->ID,
       settings->GetName(),
       settings->Pos[0],
       settings->Pos[1],
       settings->Size[0],
       settings->Size[1],
       settings->Collapsed);
}

void ANCHOR::DebugNodeWindowsList(AnchorVector<AnchorWindow *> *windows, const char *label)
{
  if (!TreeNode(label, "%s (%d)", label, windows->Size))
    return;
  Text("(In front-to-back order:)");
  for (int i = windows->Size - 1; i >= 0; i--)  // Iterate front to back
  {
    PushID((*windows)[i]);
    DebugNodeWindow((*windows)[i], "Window");
    PopID();
  }
  TreePop();
}

#else

void ANCHOR::ShowMetricsWindow(bool *) {}
void ANCHOR::ShowFontAtlas(AnchorFontAtlas *) {}
void ANCHOR::DebugNodeColumns(AnchorOldColumns *) {}
void ANCHOR::DebugNodeDrawList(AnchorWindow *, const AnchorDrawList *, const char *) {}
void ANCHOR::DebugNodeDrawCmdShowMeshAndBoundingBox(AnchorDrawList *,
                                                    const AnchorDrawList *,
                                                    const AnchorDrawCmd *,
                                                    bool,
                                                    bool)
{}
void ANCHOR::DebugNodeFont(AnchorFont *) {}
void ANCHOR::DebugNodeStorage(AnchorStorage *, const char *) {}
void ANCHOR::DebugNodeTabBar(AnchorTabBar *, const char *) {}
void ANCHOR::DebugNodeWindow(AnchorWindow *, const char *) {}
void ANCHOR::DebugNodeWindowSettings(AnchorWindowSettings *) {}
void ANCHOR::DebugNodeWindowsList(AnchorVector<AnchorWindow *> *, const char *) {}
void ANCHOR::DebugNodeViewport(AnchorViewportP *) {}

#endif

//-----------------------------------------------------------------------------

// Include ANCHOR_user.inl at the end of ANCHOR.cpp to access private data/functions that aren't
// exposed. Prefer just including ANCHOR_internal.h from your code rather than using this define.
// If a declaration is missing from ANCHOR_internal.h add it or request it on the github.
#ifdef ANCHOR_INCLUDE_ANCHOR_USER_INL
#  include "ANCHOR_user.inl"
#endif

//-----------------------------------------------------------------------------
