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

#include "ANCHOR_api.h"

#ifndef ANCHOR_DEFINE_MATH_OPERATORS
#  define ANCHOR_DEFINE_MATH_OPERATORS
#endif

#include "ANCHOR_internal.h"
#include "ANCHOR_freetype.h"

#include <stdio.h>
#if !defined(alloca)
#  if defined(__GLIBC__) || defined(__sun) || defined(__APPLE__) || defined(__NEWLIB__)
#    include <alloca.h>
#  elif defined(_WIN32)
#    include <malloc.h>
#    if !defined(alloca)
#      define alloca _alloca
#    endif
#  else
#    include <stdlib.h>
#  endif
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#  pragma warning(disable : 4127)
#  pragma warning(disable : 4505)
#  pragma warning(disable : 4996)
#  pragma warning(disable : 6255)
#  pragma warning(disable : 26451)
#  pragma warning(disable : 26812)
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#  if __has_warning("-Wunknown-warning-option")
#    pragma clang diagnostic ignored "-Wunknown-warning-option"
#  endif
#  if __has_warning("-Walloca")
#    pragma clang diagnostic ignored "-Walloca"
#  endif
#  pragma clang diagnostic ignored "-Wunknown-pragmas"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wfloat-equal"
#  pragma clang diagnostic ignored "-Wglobal-constructors"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#  pragma clang diagnostic ignored "-Wcomma"
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#  pragma clang diagnostic ignored "-Wdouble-promotion"
#  pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wpragmas"
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wdouble-promotion"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wstack-protector"
#  pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

#ifdef ANCHOR_STB_NAMESPACE
namespace ANCHOR_STB_NAMESPACE
{
#endif

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4456)
#  pragma warning(disable : 6011)
#  pragma warning(disable : 6385)
#  pragma warning(disable : 28182)
#endif

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-function"
#  pragma clang diagnostic ignored "-Wmissing-prototypes"
#  pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#  pragma clang diagnostic ignored "-Wcast-qual"
#endif

#if defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wtype-limits"
#  pragma GCC diagnostic ignored "-Wcast-qual"
#endif

#ifndef STB_RECT_PACK_IMPLEMENTATION
#  ifndef ANCHOR_DISABLE_STB_RECT_PACK_IMPLEMENTATION
#    define STBRP_STATIC
#    define STBRP_ASSERT(x) \
      do {                  \
        ANCHOR_ASSERT(x);   \
      } while (0)
#    define STBRP_SORT ImQsort
#    define STB_RECT_PACK_IMPLEMENTATION
#  endif
#  ifdef ANCHOR_STB_RECT_PACK_FILENAME
#    include ANCHOR_STB_RECT_PACK_FILENAME
#  else
#    include "ANCHOR_rectpack.h"
#  endif
#endif

#ifndef STB_TRUETYPE_IMPLEMENTATION
#  ifndef ANCHOR_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#    define STBTT_malloc(x, u) ((void)(u), ANCHOR_ALLOC(x))
#    define STBTT_free(x, u) ((void)(u), ANCHOR_FREE(x))
#    define STBTT_assert(x) \
      do {                  \
        ANCHOR_ASSERT(x);   \
      } while (0)
#    define STBTT_fmod(x, y) AnchorFmod(x, y)
#    define STBTT_sqrt(x) AnchorSqrt(x)
#    define STBTT_pow(x, y) AnchorPow(x, y)
#    define STBTT_fabs(x) AnchorFabs(x)
#    define STBTT_ifloor(x) ((int)AnchorFloorSigned(x))
#    define STBTT_iceil(x) ((int)ImCeil(x))
#    define STBTT_STATIC
#    define STB_TRUETYPE_IMPLEMENTATION
#  else
#    define STBTT_DEF extern
#  endif
#  ifdef ANCHOR_STB_TRUETYPE_FILENAME
#    include ANCHOR_STB_TRUETYPE_FILENAME
#  else
#    include "ANCHOR_truetype.h"
#  endif
#endif

#if defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

#if defined(__clang__)
#  pragma clang diagnostic pop
#endif

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#ifdef ANCHOR_STB_NAMESPACE
}  // namespace AnchorStb
using namespace ANCHOR_STB_NAMESPACE;
#endif

WABI_NAMESPACE_USING

void ANCHOR::StyleColorsDark(AnchorStyle *dst)
{
  AnchorStyle *style = dst ? dst : &ANCHOR::GetStyle();
  GfVec4f *colors = style->Colors;

  colors[AnchorCol_Text] = GfVec4f(1.00f, 1.00f, 1.00f, 1.00f);
  colors[AnchorCol_TextDisabled] = GfVec4f(0.50f, 0.50f, 0.50f, 1.00f);
  colors[AnchorCol_WindowBg] = GfVec4f(0.06f, 0.06f, 0.06f, 0.94f);
  colors[AnchorCol_ChildBg] = GfVec4f(0.00f, 0.00f, 0.00f, 0.00f);
  colors[AnchorCol_PopupBg] = GfVec4f(0.08f, 0.08f, 0.08f, 0.94f);
  colors[AnchorCol_Border] = GfVec4f(0.43f, 0.43f, 0.50f, 0.50f);
  colors[AnchorCol_BorderShadow] = GfVec4f(0.00f, 0.00f, 0.00f, 0.00f);
  colors[AnchorCol_FrameBg] = GfVec4f(0.16f, 0.29f, 0.48f, 0.54f);
  colors[AnchorCol_FrameBgHovered] = GfVec4f(0.26f, 0.59f, 0.98f, 0.40f);
  colors[AnchorCol_FrameBgActive] = GfVec4f(0.26f, 0.59f, 0.98f, 0.67f);
  colors[AnchorCol_TitleBg] = GfVec4f(0.04f, 0.04f, 0.04f, 1.00f);
  colors[AnchorCol_TitleBgActive] = GfVec4f(0.16f, 0.29f, 0.48f, 1.00f);
  colors[AnchorCol_TitleBgCollapsed] = GfVec4f(0.00f, 0.00f, 0.00f, 0.51f);
  colors[AnchorCol_MenuBarBg] = GfVec4f(0.14f, 0.14f, 0.14f, 1.00f);
  colors[AnchorCol_ScrollbarBg] = GfVec4f(0.02f, 0.02f, 0.02f, 0.53f);
  colors[AnchorCol_ScrollbarGrab] = GfVec4f(0.31f, 0.31f, 0.31f, 1.00f);
  colors[AnchorCol_ScrollbarGrabHovered] = GfVec4f(0.41f, 0.41f, 0.41f, 1.00f);
  colors[AnchorCol_ScrollbarGrabActive] = GfVec4f(0.51f, 0.51f, 0.51f, 1.00f);
  colors[AnchorCol_CheckMark] = GfVec4f(0.26f, 0.59f, 0.98f, 1.00f);
  colors[AnchorCol_SliderGrab] = GfVec4f(0.24f, 0.52f, 0.88f, 1.00f);
  colors[AnchorCol_SliderGrabActive] = GfVec4f(0.26f, 0.59f, 0.98f, 1.00f);
  colors[AnchorCol_Button] = GfVec4f(0.26f, 0.59f, 0.98f, 0.40f);
  colors[AnchorCol_ButtonHovered] = GfVec4f(0.26f, 0.59f, 0.98f, 1.00f);
  colors[AnchorCol_ButtonActive] = GfVec4f(0.06f, 0.53f, 0.98f, 1.00f);
  colors[AnchorCol_Header] = GfVec4f(0.26f, 0.59f, 0.98f, 0.31f);
  colors[AnchorCol_HeaderHovered] = GfVec4f(0.26f, 0.59f, 0.98f, 0.80f);
  colors[AnchorCol_HeaderActive] = GfVec4f(0.26f, 0.59f, 0.98f, 1.00f);
  colors[AnchorCol_Separator] = colors[AnchorCol_Border];
  colors[AnchorCol_SeparatorHovered] = GfVec4f(0.10f, 0.40f, 0.75f, 0.78f);
  colors[AnchorCol_SeparatorActive] = GfVec4f(0.10f, 0.40f, 0.75f, 1.00f);
  colors[AnchorCol_ResizeGrip] = GfVec4f(0.26f, 0.59f, 0.98f, 0.20f);
  colors[AnchorCol_ResizeGripHovered] = GfVec4f(0.26f, 0.59f, 0.98f, 0.67f);
  colors[AnchorCol_ResizeGripActive] = GfVec4f(0.26f, 0.59f, 0.98f, 0.95f);
  colors[AnchorCol_Tab] = AnchorLerp(colors[AnchorCol_Header],
                                     colors[AnchorCol_TitleBgActive],
                                     0.80f);
  colors[AnchorCol_TabHovered] = colors[AnchorCol_HeaderHovered];
  colors[AnchorCol_TabActive] = AnchorLerp(colors[AnchorCol_HeaderActive],
                                           colors[AnchorCol_TitleBgActive],
                                           0.60f);
  colors[AnchorCol_TabUnfocused] = AnchorLerp(colors[AnchorCol_Tab],
                                              colors[AnchorCol_TitleBg],
                                              0.80f);
  colors[AnchorCol_TabUnfocusedActive] = AnchorLerp(colors[AnchorCol_TabActive],
                                                    colors[AnchorCol_TitleBg],
                                                    0.40f);
  colors[AnchorCol_PlotLines] = GfVec4f(0.61f, 0.61f, 0.61f, 1.00f);
  colors[AnchorCol_PlotLinesHovered] = GfVec4f(1.00f, 0.43f, 0.35f, 1.00f);
  colors[AnchorCol_PlotHistogram] = GfVec4f(0.90f, 0.70f, 0.00f, 1.00f);
  colors[AnchorCol_PlotHistogramHovered] = GfVec4f(1.00f, 0.60f, 0.00f, 1.00f);
  colors[AnchorCol_TableHeaderBg] = GfVec4f(0.19f, 0.19f, 0.20f, 1.00f);
  colors[AnchorCol_TableBorderStrong] = GfVec4f(0.31f,
                                                0.31f,
                                                0.35f,
                                                1.00f);  // Prefer using Alpha=1.0 here
  colors[AnchorCol_TableBorderLight] = GfVec4f(0.23f,
                                               0.23f,
                                               0.25f,
                                               1.00f);  // Prefer using Alpha=1.0 here
  colors[AnchorCol_TableRowBg] = GfVec4f(0.00f, 0.00f, 0.00f, 0.00f);
  colors[AnchorCol_TableRowBgAlt] = GfVec4f(1.00f, 1.00f, 1.00f, 0.06f);
  colors[AnchorCol_TextSelectedBg] = GfVec4f(0.26f, 0.59f, 0.98f, 0.35f);
  colors[AnchorCol_DragDropTarget] = GfVec4f(1.00f, 1.00f, 0.00f, 0.90f);
  colors[AnchorCol_NavHighlight] = GfVec4f(0.26f, 0.59f, 0.98f, 1.00f);
  colors[AnchorCol_NavWindowingHighlight] = GfVec4f(1.00f, 1.00f, 1.00f, 0.70f);
  colors[AnchorCol_NavWindowingDimBg] = GfVec4f(0.80f, 0.80f, 0.80f, 0.20f);
  colors[AnchorCol_ModalWindowDimBg] = GfVec4f(0.80f, 0.80f, 0.80f, 0.35f);
}

void ANCHOR::StyleColorsDefault(AnchorStyle *dst)
{
  AnchorStyle *style = dst ? dst : &ANCHOR::GetStyle();
  GfVec4f *colors = style->Colors;

  colors[AnchorCol_Text] = GfVec4f(0.90f, 0.90f, 0.90f, 1.00f);
  colors[AnchorCol_TextDisabled] = GfVec4f(0.60f, 0.60f, 0.60f, 1.00f);
  colors[AnchorCol_WindowBg] = GfVec4f(0.00f, 0.00f, 0.00f, 0.85f);
  colors[AnchorCol_ChildBg] = GfVec4f(0.00f, 0.00f, 0.00f, 0.00f);
  colors[AnchorCol_PopupBg] = GfVec4f(0.11f, 0.11f, 0.14f, 0.92f);
  colors[AnchorCol_Border] = GfVec4f(0.50f, 0.50f, 0.50f, 0.50f);
  colors[AnchorCol_BorderShadow] = GfVec4f(0.00f, 0.00f, 0.00f, 0.00f);
  colors[AnchorCol_FrameBg] = GfVec4f(0.43f, 0.43f, 0.43f, 0.39f);
  colors[AnchorCol_FrameBgHovered] = GfVec4f(0.47f, 0.47f, 0.69f, 0.40f);
  colors[AnchorCol_FrameBgActive] = GfVec4f(0.42f, 0.41f, 0.64f, 0.69f);
  colors[AnchorCol_TitleBg] = GfVec4f(0.27f, 0.27f, 0.54f, 0.83f);
  colors[AnchorCol_TitleBgActive] = GfVec4f(0.32f, 0.32f, 0.63f, 0.87f);
  colors[AnchorCol_TitleBgCollapsed] = GfVec4f(0.40f, 0.40f, 0.80f, 0.20f);
  colors[AnchorCol_MenuBarBg] = GfVec4f(0.40f, 0.40f, 0.55f, 0.80f);
  colors[AnchorCol_ScrollbarBg] = GfVec4f(0.20f, 0.25f, 0.30f, 0.60f);
  colors[AnchorCol_ScrollbarGrab] = GfVec4f(0.40f, 0.40f, 0.80f, 0.30f);
  colors[AnchorCol_ScrollbarGrabHovered] = GfVec4f(0.40f, 0.40f, 0.80f, 0.40f);
  colors[AnchorCol_ScrollbarGrabActive] = GfVec4f(0.41f, 0.39f, 0.80f, 0.60f);
  colors[AnchorCol_CheckMark] = GfVec4f(0.90f, 0.90f, 0.90f, 0.50f);
  colors[AnchorCol_SliderGrab] = GfVec4f(1.00f, 1.00f, 1.00f, 0.30f);
  colors[AnchorCol_SliderGrabActive] = GfVec4f(0.41f, 0.39f, 0.80f, 0.60f);
  colors[AnchorCol_Button] = GfVec4f(0.35f, 0.40f, 0.61f, 0.62f);
  colors[AnchorCol_ButtonHovered] = GfVec4f(0.40f, 0.48f, 0.71f, 0.79f);
  colors[AnchorCol_ButtonActive] = GfVec4f(0.46f, 0.54f, 0.80f, 1.00f);
  colors[AnchorCol_Header] = GfVec4f(0.40f, 0.40f, 0.90f, 0.45f);
  colors[AnchorCol_HeaderHovered] = GfVec4f(0.45f, 0.45f, 0.90f, 0.80f);
  colors[AnchorCol_HeaderActive] = GfVec4f(0.53f, 0.53f, 0.87f, 0.80f);
  colors[AnchorCol_Separator] = GfVec4f(0.50f, 0.50f, 0.50f, 0.60f);
  colors[AnchorCol_SeparatorHovered] = GfVec4f(0.60f, 0.60f, 0.70f, 1.00f);
  colors[AnchorCol_SeparatorActive] = GfVec4f(0.70f, 0.70f, 0.90f, 1.00f);
  colors[AnchorCol_ResizeGrip] = GfVec4f(1.00f, 1.00f, 1.00f, 0.10f);
  colors[AnchorCol_ResizeGripHovered] = GfVec4f(0.78f, 0.82f, 1.00f, 0.60f);
  colors[AnchorCol_ResizeGripActive] = GfVec4f(0.78f, 0.82f, 1.00f, 0.90f);
  colors[AnchorCol_Tab] = AnchorLerp(colors[AnchorCol_Header],
                                     colors[AnchorCol_TitleBgActive],
                                     0.80f);
  colors[AnchorCol_TabHovered] = colors[AnchorCol_HeaderHovered];
  colors[AnchorCol_TabActive] = AnchorLerp(colors[AnchorCol_HeaderActive],
                                           colors[AnchorCol_TitleBgActive],
                                           0.60f);
  colors[AnchorCol_TabUnfocused] = AnchorLerp(colors[AnchorCol_Tab],
                                              colors[AnchorCol_TitleBg],
                                              0.80f);
  colors[AnchorCol_TabUnfocusedActive] = AnchorLerp(colors[AnchorCol_TabActive],
                                                    colors[AnchorCol_TitleBg],
                                                    0.40f);
  colors[AnchorCol_PlotLines] = GfVec4f(1.00f, 1.00f, 1.00f, 1.00f);
  colors[AnchorCol_PlotLinesHovered] = GfVec4f(0.90f, 0.70f, 0.00f, 1.00f);
  colors[AnchorCol_PlotHistogram] = GfVec4f(0.90f, 0.70f, 0.00f, 1.00f);
  colors[AnchorCol_PlotHistogramHovered] = GfVec4f(1.00f, 0.60f, 0.00f, 1.00f);
  colors[AnchorCol_TableHeaderBg] = GfVec4f(0.27f, 0.27f, 0.38f, 1.00f);
  colors[AnchorCol_TableBorderStrong] = GfVec4f(0.31f,
                                                0.31f,
                                                0.45f,
                                                1.00f);  // Prefer using Alpha=1.0 here
  colors[AnchorCol_TableBorderLight] = GfVec4f(0.26f,
                                               0.26f,
                                               0.28f,
                                               1.00f);  // Prefer using Alpha=1.0 here
  colors[AnchorCol_TableRowBg] = GfVec4f(0.00f, 0.00f, 0.00f, 0.00f);
  colors[AnchorCol_TableRowBgAlt] = GfVec4f(1.00f, 1.00f, 1.00f, 0.07f);
  colors[AnchorCol_TextSelectedBg] = GfVec4f(0.00f, 0.00f, 1.00f, 0.35f);
  colors[AnchorCol_DragDropTarget] = GfVec4f(1.00f, 1.00f, 0.00f, 0.90f);
  colors[AnchorCol_NavHighlight] = colors[AnchorCol_HeaderHovered];
  colors[AnchorCol_NavWindowingHighlight] = GfVec4f(1.00f, 1.00f, 1.00f, 0.70f);
  colors[AnchorCol_NavWindowingDimBg] = GfVec4f(0.80f, 0.80f, 0.80f, 0.20f);
  colors[AnchorCol_ModalWindowDimBg] = GfVec4f(0.20f, 0.20f, 0.20f, 0.35f);
}

// Those light colors are better suited with a thicker font than the default one + FrameBorder
void ANCHOR::StyleColorsLight(AnchorStyle *dst)
{
  AnchorStyle *style = dst ? dst : &ANCHOR::GetStyle();
  GfVec4f *colors = style->Colors;

  colors[AnchorCol_Text] = GfVec4f(0.00f, 0.00f, 0.00f, 1.00f);
  colors[AnchorCol_TextDisabled] = GfVec4f(0.60f, 0.60f, 0.60f, 1.00f);
  colors[AnchorCol_WindowBg] = GfVec4f(0.94f, 0.94f, 0.94f, 1.00f);
  colors[AnchorCol_ChildBg] = GfVec4f(0.00f, 0.00f, 0.00f, 0.00f);
  colors[AnchorCol_PopupBg] = GfVec4f(1.00f, 1.00f, 1.00f, 0.98f);
  colors[AnchorCol_Border] = GfVec4f(0.00f, 0.00f, 0.00f, 0.30f);
  colors[AnchorCol_BorderShadow] = GfVec4f(0.00f, 0.00f, 0.00f, 0.00f);
  colors[AnchorCol_FrameBg] = GfVec4f(1.00f, 1.00f, 1.00f, 1.00f);
  colors[AnchorCol_FrameBgHovered] = GfVec4f(0.26f, 0.59f, 0.98f, 0.40f);
  colors[AnchorCol_FrameBgActive] = GfVec4f(0.26f, 0.59f, 0.98f, 0.67f);
  colors[AnchorCol_TitleBg] = GfVec4f(0.96f, 0.96f, 0.96f, 1.00f);
  colors[AnchorCol_TitleBgActive] = GfVec4f(0.82f, 0.82f, 0.82f, 1.00f);
  colors[AnchorCol_TitleBgCollapsed] = GfVec4f(1.00f, 1.00f, 1.00f, 0.51f);
  colors[AnchorCol_MenuBarBg] = GfVec4f(0.86f, 0.86f, 0.86f, 1.00f);
  colors[AnchorCol_ScrollbarBg] = GfVec4f(0.98f, 0.98f, 0.98f, 0.53f);
  colors[AnchorCol_ScrollbarGrab] = GfVec4f(0.69f, 0.69f, 0.69f, 0.80f);
  colors[AnchorCol_ScrollbarGrabHovered] = GfVec4f(0.49f, 0.49f, 0.49f, 0.80f);
  colors[AnchorCol_ScrollbarGrabActive] = GfVec4f(0.49f, 0.49f, 0.49f, 1.00f);
  colors[AnchorCol_CheckMark] = GfVec4f(0.26f, 0.59f, 0.98f, 1.00f);
  colors[AnchorCol_SliderGrab] = GfVec4f(0.26f, 0.59f, 0.98f, 0.78f);
  colors[AnchorCol_SliderGrabActive] = GfVec4f(0.46f, 0.54f, 0.80f, 0.60f);
  colors[AnchorCol_Button] = GfVec4f(0.26f, 0.59f, 0.98f, 0.40f);
  colors[AnchorCol_ButtonHovered] = GfVec4f(0.26f, 0.59f, 0.98f, 1.00f);
  colors[AnchorCol_ButtonActive] = GfVec4f(0.06f, 0.53f, 0.98f, 1.00f);
  colors[AnchorCol_Header] = GfVec4f(0.26f, 0.59f, 0.98f, 0.31f);
  colors[AnchorCol_HeaderHovered] = GfVec4f(0.26f, 0.59f, 0.98f, 0.80f);
  colors[AnchorCol_HeaderActive] = GfVec4f(0.26f, 0.59f, 0.98f, 1.00f);
  colors[AnchorCol_Separator] = GfVec4f(0.39f, 0.39f, 0.39f, 0.62f);
  colors[AnchorCol_SeparatorHovered] = GfVec4f(0.14f, 0.44f, 0.80f, 0.78f);
  colors[AnchorCol_SeparatorActive] = GfVec4f(0.14f, 0.44f, 0.80f, 1.00f);
  colors[AnchorCol_ResizeGrip] = GfVec4f(0.35f, 0.35f, 0.35f, 0.17f);
  colors[AnchorCol_ResizeGripHovered] = GfVec4f(0.26f, 0.59f, 0.98f, 0.67f);
  colors[AnchorCol_ResizeGripActive] = GfVec4f(0.26f, 0.59f, 0.98f, 0.95f);
  colors[AnchorCol_Tab] = AnchorLerp(colors[AnchorCol_Header],
                                     colors[AnchorCol_TitleBgActive],
                                     0.90f);
  colors[AnchorCol_TabHovered] = colors[AnchorCol_HeaderHovered];
  colors[AnchorCol_TabActive] = AnchorLerp(colors[AnchorCol_HeaderActive],
                                           colors[AnchorCol_TitleBgActive],
                                           0.60f);
  colors[AnchorCol_TabUnfocused] = AnchorLerp(colors[AnchorCol_Tab],
                                              colors[AnchorCol_TitleBg],
                                              0.80f);
  colors[AnchorCol_TabUnfocusedActive] = AnchorLerp(colors[AnchorCol_TabActive],
                                                    colors[AnchorCol_TitleBg],
                                                    0.40f);
  colors[AnchorCol_PlotLines] = GfVec4f(0.39f, 0.39f, 0.39f, 1.00f);
  colors[AnchorCol_PlotLinesHovered] = GfVec4f(1.00f, 0.43f, 0.35f, 1.00f);
  colors[AnchorCol_PlotHistogram] = GfVec4f(0.90f, 0.70f, 0.00f, 1.00f);
  colors[AnchorCol_PlotHistogramHovered] = GfVec4f(1.00f, 0.45f, 0.00f, 1.00f);
  colors[AnchorCol_TableHeaderBg] = GfVec4f(0.78f, 0.87f, 0.98f, 1.00f);
  colors[AnchorCol_TableBorderStrong] = GfVec4f(0.57f,
                                                0.57f,
                                                0.64f,
                                                1.00f);  // Prefer using Alpha=1.0 here
  colors[AnchorCol_TableBorderLight] = GfVec4f(0.68f,
                                               0.68f,
                                               0.74f,
                                               1.00f);  // Prefer using Alpha=1.0 here
  colors[AnchorCol_TableRowBg] = GfVec4f(0.00f, 0.00f, 0.00f, 0.00f);
  colors[AnchorCol_TableRowBgAlt] = GfVec4f(0.30f, 0.30f, 0.30f, 0.09f);
  colors[AnchorCol_TextSelectedBg] = GfVec4f(0.26f, 0.59f, 0.98f, 0.35f);
  colors[AnchorCol_DragDropTarget] = GfVec4f(0.26f, 0.59f, 0.98f, 0.95f);
  colors[AnchorCol_NavHighlight] = colors[AnchorCol_HeaderHovered];
  colors[AnchorCol_NavWindowingHighlight] = GfVec4f(0.70f, 0.70f, 0.70f, 0.70f);
  colors[AnchorCol_NavWindowingDimBg] = GfVec4f(0.20f, 0.20f, 0.20f, 0.20f);
  colors[AnchorCol_ModalWindowDimBg] = GfVec4f(0.20f, 0.20f, 0.20f, 0.35f);
}

//-----------------------------------------------------------------------------
// [SECTION] AnchorDrawList
//-----------------------------------------------------------------------------

AnchorDrawListSharedData::AnchorDrawListSharedData()
{
  memset(this, 0, sizeof(*this));
  for (int i = 0; i < ANCHOR_ARRAYSIZE(ArcFastVtx); i++) {
    const float a = ((float)i * 2 * IM_PI) / (float)ANCHOR_ARRAYSIZE(ArcFastVtx);
    ArcFastVtx[i] = GfVec2f(AnchorCos(a), AnchorSin(a));
  }
  ArcFastRadiusCutoff = ANCHOR_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_R(
    ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX,
    CircleSegmentMaxError);
}

void AnchorDrawListSharedData::SetCircleTessellationMaxError(float max_error)
{
  if (CircleSegmentMaxError == max_error)
    return;

  ANCHOR_ASSERT(max_error > 0.0f);
  CircleSegmentMaxError = max_error;
  for (int i = 0; i < ANCHOR_ARRAYSIZE(CircleSegmentCounts); i++) {
    const float radius = (float)i;
    CircleSegmentCounts[i] = (AnchorU8)((i > 0) ? ANCHOR_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(
                                                    radius,
                                                    CircleSegmentMaxError) :
                                                  0);
  }
  ArcFastRadiusCutoff = ANCHOR_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_R(
    ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX,
    CircleSegmentMaxError);
}

// Initialize before use in a new frame. We always have a command ready in the buffer.
void AnchorDrawList::_ResetForNewFrame()
{
  // Verify that the AnchorDrawCmd fields we want to memcmp() are contiguous in memory.
  // (those should be IM_STATIC_ASSERT() in theory but with our pre C++11 setup the whole check
  // doesn't compile with GCC)
  ANCHOR_ASSERT(ANCHOR_OFFSETOF(AnchorDrawCmd, ClipRect) == 0);
  ANCHOR_ASSERT(ANCHOR_OFFSETOF(AnchorDrawCmd, TextureId) == sizeof(GfVec4f));
  ANCHOR_ASSERT(ANCHOR_OFFSETOF(AnchorDrawCmd, VtxOffset) ==
                sizeof(GfVec4f) + sizeof(AnchorTextureID));

  CmdBuffer.resize(0);
  IdxBuffer.resize(0);
  VtxBuffer.resize(0);
  Flags = _Data->InitialFlags;
  memset(&_CmdHeader, 0, sizeof(_CmdHeader));
  _VtxCurrentIdx = 0;
  _VtxWritePtr = NULL;
  _IdxWritePtr = NULL;
  _ClipRectStack.resize(0);
  _TextureIdStack.resize(0);
  _Path.resize(0);
  _Splitter.Clear();
  CmdBuffer.push_back(AnchorDrawCmd());
  _FringeScale = 1.0f;
}

void AnchorDrawList::_ClearFreeMemory()
{
  CmdBuffer.clear();
  IdxBuffer.clear();
  VtxBuffer.clear();
  Flags = AnchorDrawListFlags_None;
  _VtxCurrentIdx = 0;
  _VtxWritePtr = NULL;
  _IdxWritePtr = NULL;
  _ClipRectStack.clear();
  _TextureIdStack.clear();
  _Path.clear();
  _Splitter.ClearFreeMemory();
}

AnchorDrawList *AnchorDrawList::CloneOutput() const
{
  AnchorDrawList *dst = ANCHOR_NEW(AnchorDrawList(_Data));
  dst->CmdBuffer = CmdBuffer;
  dst->IdxBuffer = IdxBuffer;
  dst->VtxBuffer = VtxBuffer;
  dst->Flags = Flags;
  return dst;
}

void AnchorDrawList::AddDrawCmd()
{
  AnchorDrawCmd draw_cmd;
  draw_cmd.ClipRect = _CmdHeader.ClipRect;  // Same as calling AnchorDrawCmd_HeaderCopy()
  draw_cmd.TextureId = _CmdHeader.TextureId;
  draw_cmd.VtxOffset = _CmdHeader.VtxOffset;
  draw_cmd.IdxOffset = IdxBuffer.Size;

  ANCHOR_ASSERT(draw_cmd.ClipRect[0] <= draw_cmd.ClipRect[2] &&
                draw_cmd.ClipRect[1] <= draw_cmd.ClipRect[3]);
  CmdBuffer.push_back(draw_cmd);
}

// Pop trailing draw command (used before merging or presenting to user)
// Note that this leaves the AnchorDrawList in a state unfit for further commands, as most code
// assume that CmdBuffer.Size > 0 && CmdBuffer.back().UserCallback == NULL
void AnchorDrawList::_PopUnusedDrawCmd()
{
  if (CmdBuffer.Size == 0)
    return;
  AnchorDrawCmd *curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
  if (curr_cmd->ElemCount == 0 && curr_cmd->UserCallback == NULL)
    CmdBuffer.pop_back();
}

void AnchorDrawList::AddCallback(AnchorDrawCallback callback, void *callback_data)
{
  AnchorDrawCmd *curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
  ANCHOR_ASSERT(curr_cmd->UserCallback == NULL);
  if (curr_cmd->ElemCount != 0) {
    AddDrawCmd();
    curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
  }
  curr_cmd->UserCallback = callback;
  curr_cmd->UserCallbackData = callback_data;

  AddDrawCmd();  // Force a new command after us (see comment below)
}

// Compare ClipRect, TextureId and VtxOffset with a single memcmp()
#define AnchorDrawCmd_HeaderSize (ANCHOR_OFFSETOF(AnchorDrawCmd, VtxOffset) + sizeof(unsigned int))
#define AnchorDrawCmd_HeaderCompare(CMD_LHS, CMD_RHS) \
  (memcmp(CMD_LHS, CMD_RHS, AnchorDrawCmd_HeaderSize))  // Compare ClipRect, TextureId, VtxOffset
#define AnchorDrawCmd_HeaderCopy(CMD_DST, CMD_SRC) \
  (memcpy(CMD_DST, CMD_SRC, AnchorDrawCmd_HeaderSize))  // Copy ClipRect, TextureId, VtxOffset

// Our scheme may appears a bit unusual, basically we want the most-common calls AddLine AddRect
// etc. to not have to perform any check so we always have a command ready in the stack. The cost
// of figuring out if a new command has to be added or if we can merge is paid in those Update**
// functions only.
void AnchorDrawList::_OnChangedClipRect()
{
  // If current command is used with different settings we need to add a new command
  AnchorDrawCmd *curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
  if (curr_cmd->ElemCount != 0 &&
      memcmp(&curr_cmd->ClipRect, &_CmdHeader.ClipRect, sizeof(GfVec4f)) != 0) {
    AddDrawCmd();
    return;
  }
  ANCHOR_ASSERT(curr_cmd->UserCallback == NULL);

  // Try to merge with previous command if it matches, else use current command
  AnchorDrawCmd *prev_cmd = curr_cmd - 1;
  if (curr_cmd->ElemCount == 0 && CmdBuffer.Size > 1 &&
      AnchorDrawCmd_HeaderCompare(&_CmdHeader, prev_cmd) == 0 && prev_cmd->UserCallback == NULL) {
    CmdBuffer.pop_back();
    return;
  }

  curr_cmd->ClipRect = _CmdHeader.ClipRect;
}

void AnchorDrawList::_OnChangedTextureID()
{
  // If current command is used with different settings we need to add a new command
  AnchorDrawCmd *curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
  if (curr_cmd->ElemCount != 0 && curr_cmd->TextureId != _CmdHeader.TextureId) {
    AddDrawCmd();
    return;
  }
  ANCHOR_ASSERT(curr_cmd->UserCallback == NULL);

  // Try to merge with previous command if it matches, else use current command
  AnchorDrawCmd *prev_cmd = curr_cmd - 1;
  if (curr_cmd->ElemCount == 0 && CmdBuffer.Size > 1 &&
      AnchorDrawCmd_HeaderCompare(&_CmdHeader, prev_cmd) == 0 && prev_cmd->UserCallback == NULL) {
    CmdBuffer.pop_back();
    return;
  }

  curr_cmd->TextureId = _CmdHeader.TextureId;
}

void AnchorDrawList::_OnChangedVtxOffset()
{
  // We don't need to compare curr_cmd->VtxOffset != _CmdHeader.VtxOffset because we know it'll be
  // different at the time we call this.
  _VtxCurrentIdx = 0;
  AnchorDrawCmd *curr_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
  // ANCHOR_ASSERT(curr_cmd->VtxOffset != _CmdHeader.VtxOffset); // See #3349
  if (curr_cmd->ElemCount != 0) {
    AddDrawCmd();
    return;
  }
  ANCHOR_ASSERT(curr_cmd->UserCallback == NULL);
  curr_cmd->VtxOffset = _CmdHeader.VtxOffset;
}

int AnchorDrawList::_CalcCircleAutoSegmentCount(float radius) const
{
  // Automatic segment count
  const int radius_idx = (int)(radius + 0.999999f);  // ceil to never reduce accuracy
  if (radius_idx < ANCHOR_ARRAYSIZE(_Data->CircleSegmentCounts))
    return _Data->CircleSegmentCounts[radius_idx];  // Use cached value
  else
    return ANCHOR_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(radius, _Data->CircleSegmentMaxError);
}

// Render-level scissoring. This is passed down to your render function but not used for CPU-side
// coarse clipping. Prefer using higher-level ANCHOR::PushClipRect() to affect logic (hit-testing
// and widget culling)
void AnchorDrawList::PushClipRect(GfVec2f cr_min,
                                  GfVec2f cr_max,
                                  bool intersect_with_current_clip_rect)
{
  GfVec4f cr(cr_min[0], cr_min[1], cr_max[0], cr_max[1]);
  if (intersect_with_current_clip_rect) {
    GfVec4f current = _CmdHeader.ClipRect;
    if (cr[0] < current[0])
      cr[0] = current[0];
    if (cr[1] < current[1])
      cr[1] = current[1];
    if (cr[2] > current[2])
      cr[2] = current[2];
    if (cr[3] > current[3])
      cr[3] = current[3];
  }
  cr[2] = AnchorMax(cr[0], cr[2]);
  cr[3] = AnchorMax(cr[1], cr[3]);

  _ClipRectStack.push_back(cr);
  _CmdHeader.ClipRect = cr;
  _OnChangedClipRect();
}

void AnchorDrawList::PushClipRectFullScreen()
{
  PushClipRect(GfVec2f(_Data->ClipRectFullscreen[0], _Data->ClipRectFullscreen[1]),
               GfVec2f(_Data->ClipRectFullscreen[2], _Data->ClipRectFullscreen[3]));
}

void AnchorDrawList::PopClipRect()
{
  _ClipRectStack.pop_back();
  _CmdHeader.ClipRect = (_ClipRectStack.Size == 0) ? _Data->ClipRectFullscreen :
                                                     _ClipRectStack.Data[_ClipRectStack.Size - 1];
  _OnChangedClipRect();
}

void AnchorDrawList::PushTextureID(AnchorTextureID texture_id)
{
  _TextureIdStack.push_back(texture_id);
  _CmdHeader.TextureId = texture_id;
  _OnChangedTextureID();
}

void AnchorDrawList::PopTextureID()
{
  _TextureIdStack.pop_back();
  _CmdHeader.TextureId = (_TextureIdStack.Size == 0) ?
                           (AnchorTextureID)NULL :
                           _TextureIdStack.Data[_TextureIdStack.Size - 1];
  _OnChangedTextureID();
}

// Reserve space for a number of vertices and indices.
// You must finish filling your reserved data before calling PrimReserve() again, as it may
// reallocate or submit the intermediate results. PrimUnreserve() can be used to release unused
// allocations.
void AnchorDrawList::PrimReserve(int idx_count, int vtx_count)
{
  // Large mesh support (when enabled)
  ANCHOR_ASSERT_PARANOID(idx_count >= 0 && vtx_count >= 0);
  if (sizeof(AnchorDrawIdx) == 2 && (_VtxCurrentIdx + vtx_count >= (1 << 16)) &&
      (Flags & AnchorDrawListFlags_AllowVtxOffset)) {
    // FIXME: In theory we should be testing that vtx_count <64k here.
    // In practice, RenderText() relies on reserving ahead for a worst case scenario so it is
    // currently useful for us to not make that check until we rework the text functions to handle
    // clipping and large horizontal lines better.
    _CmdHeader.VtxOffset = VtxBuffer.Size;
    _OnChangedVtxOffset();
  }

  AnchorDrawCmd *draw_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
  draw_cmd->ElemCount += idx_count;

  int vtx_buffer_old_size = VtxBuffer.Size;
  VtxBuffer.resize(vtx_buffer_old_size + vtx_count);
  _VtxWritePtr = VtxBuffer.Data + vtx_buffer_old_size;

  int idx_buffer_old_size = IdxBuffer.Size;
  IdxBuffer.resize(idx_buffer_old_size + idx_count);
  _IdxWritePtr = IdxBuffer.Data + idx_buffer_old_size;
}

// Release the a number of reserved vertices/indices from the end of the last reservation made with
// PrimReserve().
void AnchorDrawList::PrimUnreserve(int idx_count, int vtx_count)
{
  ANCHOR_ASSERT_PARANOID(idx_count >= 0 && vtx_count >= 0);

  AnchorDrawCmd *draw_cmd = &CmdBuffer.Data[CmdBuffer.Size - 1];
  draw_cmd->ElemCount -= idx_count;
  VtxBuffer.shrink(VtxBuffer.Size - vtx_count);
  IdxBuffer.shrink(IdxBuffer.Size - idx_count);
}

// Fully unrolled with inline call to keep our debug builds decently fast.
void AnchorDrawList::PrimRect(const GfVec2f &a, const GfVec2f &c, AnchorU32 col)
{
  GfVec2f b(c[0], a[1]), d(a[0], c[1]), uv(_Data->TexUvWhitePixel);
  AnchorDrawIdx idx = (AnchorDrawIdx)_VtxCurrentIdx;
  _IdxWritePtr[0] = idx;
  _IdxWritePtr[1] = (AnchorDrawIdx)(idx + 1);
  _IdxWritePtr[2] = (AnchorDrawIdx)(idx + 2);
  _IdxWritePtr[3] = idx;
  _IdxWritePtr[4] = (AnchorDrawIdx)(idx + 2);
  _IdxWritePtr[5] = (AnchorDrawIdx)(idx + 3);
  _VtxWritePtr[0].pos = a;
  _VtxWritePtr[0].uv = uv;
  _VtxWritePtr[0].col = col;
  _VtxWritePtr[1].pos = b;
  _VtxWritePtr[1].uv = uv;
  _VtxWritePtr[1].col = col;
  _VtxWritePtr[2].pos = c;
  _VtxWritePtr[2].uv = uv;
  _VtxWritePtr[2].col = col;
  _VtxWritePtr[3].pos = d;
  _VtxWritePtr[3].uv = uv;
  _VtxWritePtr[3].col = col;
  _VtxWritePtr += 4;
  _VtxCurrentIdx += 4;
  _IdxWritePtr += 6;
}

void AnchorDrawList::PrimRectUV(const GfVec2f &a,
                                const GfVec2f &c,
                                const GfVec2f &uv_a,
                                const GfVec2f &uv_c,
                                AnchorU32 col)
{
  GfVec2f b(c[0], a[1]), d(a[0], c[1]), uv_b(uv_c[0], uv_a[1]), uv_d(uv_a[0], uv_c[1]);
  AnchorDrawIdx idx = (AnchorDrawIdx)_VtxCurrentIdx;
  _IdxWritePtr[0] = idx;
  _IdxWritePtr[1] = (AnchorDrawIdx)(idx + 1);
  _IdxWritePtr[2] = (AnchorDrawIdx)(idx + 2);
  _IdxWritePtr[3] = idx;
  _IdxWritePtr[4] = (AnchorDrawIdx)(idx + 2);
  _IdxWritePtr[5] = (AnchorDrawIdx)(idx + 3);
  _VtxWritePtr[0].pos = a;
  _VtxWritePtr[0].uv = uv_a;
  _VtxWritePtr[0].col = col;
  _VtxWritePtr[1].pos = b;
  _VtxWritePtr[1].uv = uv_b;
  _VtxWritePtr[1].col = col;
  _VtxWritePtr[2].pos = c;
  _VtxWritePtr[2].uv = uv_c;
  _VtxWritePtr[2].col = col;
  _VtxWritePtr[3].pos = d;
  _VtxWritePtr[3].uv = uv_d;
  _VtxWritePtr[3].col = col;
  _VtxWritePtr += 4;
  _VtxCurrentIdx += 4;
  _IdxWritePtr += 6;
}

void AnchorDrawList::PrimQuadUV(const GfVec2f &a,
                                const GfVec2f &b,
                                const GfVec2f &c,
                                const GfVec2f &d,
                                const GfVec2f &uv_a,
                                const GfVec2f &uv_b,
                                const GfVec2f &uv_c,
                                const GfVec2f &uv_d,
                                AnchorU32 col)
{
  AnchorDrawIdx idx = (AnchorDrawIdx)_VtxCurrentIdx;
  _IdxWritePtr[0] = idx;
  _IdxWritePtr[1] = (AnchorDrawIdx)(idx + 1);
  _IdxWritePtr[2] = (AnchorDrawIdx)(idx + 2);
  _IdxWritePtr[3] = idx;
  _IdxWritePtr[4] = (AnchorDrawIdx)(idx + 2);
  _IdxWritePtr[5] = (AnchorDrawIdx)(idx + 3);
  _VtxWritePtr[0].pos = a;
  _VtxWritePtr[0].uv = uv_a;
  _VtxWritePtr[0].col = col;
  _VtxWritePtr[1].pos = b;
  _VtxWritePtr[1].uv = uv_b;
  _VtxWritePtr[1].col = col;
  _VtxWritePtr[2].pos = c;
  _VtxWritePtr[2].uv = uv_c;
  _VtxWritePtr[2].col = col;
  _VtxWritePtr[3].pos = d;
  _VtxWritePtr[3].uv = uv_d;
  _VtxWritePtr[3].col = col;
  _VtxWritePtr += 4;
  _VtxCurrentIdx += 4;
  _IdxWritePtr += 6;
}

// On AddPolyline() and AddConvexPolyFilled() we intentionally avoid using GfVec2f and superfluous
// function calls to optimize debug/non-inlined builds. Those macros expects l-values.
#define IM_NORMALIZE2F_OVER_ZERO(VX, VY) \
  do {                                   \
    float d2 = VX * VX + VY * VY;        \
    if (d2 > 0.0f) {                     \
      float inv_len = AnchorRsqrt(d2);   \
      VX *= inv_len;                     \
      VY *= inv_len;                     \
    }                                    \
  } while (0)
#define IM_FIXNORMAL2F_MAX_INVLEN2 100.0f  // 500.0f (see #4053, #3366)
#define IM_FIXNORMAL2F(VX, VY)                   \
  do {                                           \
    float d2 = VX * VX + VY * VY;                \
    if (d2 > 0.000001f) {                        \
      float inv_len2 = 1.0f / d2;                \
      if (inv_len2 > IM_FIXNORMAL2F_MAX_INVLEN2) \
        inv_len2 = IM_FIXNORMAL2F_MAX_INVLEN2;   \
      VX *= inv_len2;                            \
      VY *= inv_len2;                            \
    }                                            \
  } while (0)

// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
// We avoid using the GfVec2f math operators here to reduce cost to a minimum for debug/non-inlined
// builds.
void AnchorDrawList::AddPolyline(const GfVec2f *points,
                                 const int points_count,
                                 AnchorU32 col,
                                 AnchorDrawFlags flags,
                                 float thickness)
{
  if (points_count < 2)
    return;

  const bool closed = (flags & AnchorDrawFlags_Closed) != 0;
  const GfVec2f opaque_uv = _Data->TexUvWhitePixel;
  const int count = closed ? points_count :
                             points_count - 1;  // The number of line segments we need to draw
  const bool thick_line = (thickness > _FringeScale);

  if (Flags & AnchorDrawListFlags_AntiAliasedLines) {
    // Anti-aliased stroke
    const float AA_SIZE = _FringeScale;
    const AnchorU32 col_trans = col & ~ANCHOR_COL32_A_MASK;

    // Thicknesses <1.0 should behave like thickness 1.0
    thickness = AnchorMax(thickness, 1.0f);
    const int integer_thickness = (int)thickness;
    const float fractional_thickness = thickness - integer_thickness;

    // Do we want to draw this line using a texture?
    // - For now, only draw integer-width lines using textures to avoid issues with the way scaling
    // occurs, could be improved.
    // - If AA_SIZE is not 1.0f we cannot use the texture path.
    const bool use_texture = (Flags & AnchorDrawListFlags_AntiAliasedLinesUseTex) &&
                             (integer_thickness < ANCHOR_DRAWLIST_TEX_LINES_WIDTH_MAX) &&
                             (fractional_thickness <= 0.00001f) && (AA_SIZE == 1.0f);

    // We should never hit this, because NewFrame() doesn't set
    // AnchorDrawListFlags_AntiAliasedLinesUseTex unless AnchorFontAtlasFlags_NoBakedLines is off
    ANCHOR_ASSERT_PARANOID(
      !use_texture || !(_Data->Font->ContainerAtlas->Flags & AnchorFontAtlasFlags_NoBakedLines));

    const int idx_count = use_texture ? (count * 6) : (thick_line ? count * 18 : count * 12);
    const int vtx_count = use_texture ? (points_count * 2) :
                                        (thick_line ? points_count * 4 : points_count * 3);
    PrimReserve(idx_count, vtx_count);

    // Temporary buffer
    // The first <points_count> items are normals at each line point, then after that there are
    // either 2 or 4 temp points for each line point
    GfVec2f *temp_normals = (GfVec2f *)alloca(
      points_count * ((use_texture || !thick_line) ? 3 : 5) * sizeof(GfVec2f));  //-V630
    GfVec2f *temp_points = temp_normals + points_count;

    // Calculate normals (tangents) for each line segment
    for (int i1 = 0; i1 < count; i1++) {
      const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
      float dx = points[i2][0] - points[i1][0];
      float dy = points[i2][1] - points[i1][1];
      IM_NORMALIZE2F_OVER_ZERO(dx, dy);
      temp_normals[i1][0] = dy;
      temp_normals[i1][1] = -dx;
    }
    if (!closed)
      temp_normals[points_count - 1] = temp_normals[points_count - 2];

    // If we are drawing a one-pixel-wide line without a texture, or a textured line of any width,
    // we only need 2 or 3 vertices per point
    if (use_texture || !thick_line) {
      // [PATH 1] Texture-based lines (thick or non-thick)
      // [PATH 2] Non texture-based lines (non-thick)

      // The width of the geometry we need to draw - this is essentially <thickness> pixels for the
      // line itself, plus "one pixel" for AA.
      // - In the texture-based path, we don't use AA_SIZE here because the +1 is tied to the
      // generated texture
      //   (see AnchorFontAtlasBuildRenderLinesTexData() function), and so alternate values won't
      //   work without changes to that code.
      // - In the non texture-based paths, we would allow AA_SIZE to potentially be != 1.0f with a
      // patch (e.g. fringe_scale patch to
      //   allow scaling geometry while preserving one-screen-pixel AA fringe).
      const float half_draw_size = use_texture ? ((thickness * 0.5f) + 1) : AA_SIZE;

      // If line is not closed, the first and last points need to be generated differently as there
      // are no normals to blend
      if (!closed) {
        temp_points[0] = points[0] + temp_normals[0] * half_draw_size;
        temp_points[1] = points[0] - temp_normals[0] * half_draw_size;
        temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] +
                                                  temp_normals[points_count - 1] * half_draw_size;
        temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] -
                                                  temp_normals[points_count - 1] * half_draw_size;
      }

      // Generate the indices to form a number of triangles for each line segment, and the vertices
      // for the line edges This takes points n and n+1 and writes into n+1, with the first point
      // in a closed line being generated from the final one (as n+1 wraps)
      // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
      unsigned int idx1 = _VtxCurrentIdx;  // Vertex index for start of line segment
      for (int i1 = 0; i1 < count; i1++)   // i1 is the first point of the line segment
      {
        const int i2 = (i1 + 1) == points_count ?
                         0 :
                         i1 + 1;  // i2 is the second point of the line segment
        const unsigned int idx2 = ((i1 + 1) == points_count) ?
                                    _VtxCurrentIdx :
                                    (idx1 +
                                     (use_texture ? 2 : 3));  // Vertex index for end of segment

        // Average normals
        float dm_x = (temp_normals[i1][0] + temp_normals[i2][0]) * 0.5f;
        float dm_y = (temp_normals[i1][1] + temp_normals[i2][1]) * 0.5f;
        IM_FIXNORMAL2F(dm_x, dm_y);
        dm_x *= half_draw_size;  // dm_x, dm_y are offset to the outer edge of the AA area
        dm_y *= half_draw_size;

        // Add temporary vertexes for the outer edges
        GfVec2f *out_vtx = &temp_points[i2 * 2];
        out_vtx[0][0] = points[i2][0] + dm_x;
        out_vtx[0][1] = points[i2][1] + dm_y;
        out_vtx[1][0] = points[i2][0] - dm_x;
        out_vtx[1][1] = points[i2][1] - dm_y;

        if (use_texture) {
          // Add indices for two triangles
          _IdxWritePtr[0] = (AnchorDrawIdx)(idx2 + 0);
          _IdxWritePtr[1] = (AnchorDrawIdx)(idx1 + 0);
          _IdxWritePtr[2] = (AnchorDrawIdx)(idx1 + 1);  // Right tri
          _IdxWritePtr[3] = (AnchorDrawIdx)(idx2 + 1);
          _IdxWritePtr[4] = (AnchorDrawIdx)(idx1 + 1);
          _IdxWritePtr[5] = (AnchorDrawIdx)(idx2 + 0);  // Left tri
          _IdxWritePtr += 6;
        } else {
          // Add indexes for four triangles
          _IdxWritePtr[0] = (AnchorDrawIdx)(idx2 + 0);
          _IdxWritePtr[1] = (AnchorDrawIdx)(idx1 + 0);
          _IdxWritePtr[2] = (AnchorDrawIdx)(idx1 + 2);  // Right tri 1
          _IdxWritePtr[3] = (AnchorDrawIdx)(idx1 + 2);
          _IdxWritePtr[4] = (AnchorDrawIdx)(idx2 + 2);
          _IdxWritePtr[5] = (AnchorDrawIdx)(idx2 + 0);  // Right tri 2
          _IdxWritePtr[6] = (AnchorDrawIdx)(idx2 + 1);
          _IdxWritePtr[7] = (AnchorDrawIdx)(idx1 + 1);
          _IdxWritePtr[8] = (AnchorDrawIdx)(idx1 + 0);  // Left tri 1
          _IdxWritePtr[9] = (AnchorDrawIdx)(idx1 + 0);
          _IdxWritePtr[10] = (AnchorDrawIdx)(idx2 + 0);
          _IdxWritePtr[11] = (AnchorDrawIdx)(idx2 + 1);  // Left tri 2
          _IdxWritePtr += 12;
        }

        idx1 = idx2;
      }

      // Add vertexes for each point on the line
      if (use_texture) {
        // If we're using textures we only need to emit the left/right edge vertices
        GfVec4f tex_uvs = _Data->TexUvLines[integer_thickness];
        /*if (fractional_thickness != 0.0f) // Currently always zero when use_texture==false!
        {
            const GfVec4f tex_uvs_1 = _Data->TexUvLines[integer_thickness + 1];
            tex_uvs[0] = tex_uvs[0] + (tex_uvs_1[0] - tex_uvs[0]) * fractional_thickness; //
        inlined AnchorLerp() tex_uvs[1] = tex_uvs[1] + (tex_uvs_1[1] - tex_uvs[1]) *
        fractional_thickness; tex_uvs[2] = tex_uvs[2] + (tex_uvs_1[2] - tex_uvs[2]) *
        fractional_thickness; tex_uvs[3] = tex_uvs[3] + (tex_uvs_1[3] - tex_uvs[3]) *
        fractional_thickness;
        }*/
        GfVec2f tex_uv0(tex_uvs[0], tex_uvs[1]);
        GfVec2f tex_uv1(tex_uvs[2], tex_uvs[3]);
        for (int i = 0; i < points_count; i++) {
          _VtxWritePtr[0].pos = temp_points[i * 2 + 0];
          _VtxWritePtr[0].uv = tex_uv0;
          _VtxWritePtr[0].col = col;  // Left-side outer edge
          _VtxWritePtr[1].pos = temp_points[i * 2 + 1];
          _VtxWritePtr[1].uv = tex_uv1;
          _VtxWritePtr[1].col = col;  // Right-side outer edge
          _VtxWritePtr += 2;
        }
      } else {
        // If we're not using a texture, we need the center vertex as well
        for (int i = 0; i < points_count; i++) {
          _VtxWritePtr[0].pos = points[i];
          _VtxWritePtr[0].uv = opaque_uv;
          _VtxWritePtr[0].col = col;  // Center of line
          _VtxWritePtr[1].pos = temp_points[i * 2 + 0];
          _VtxWritePtr[1].uv = opaque_uv;
          _VtxWritePtr[1].col = col_trans;  // Left-side outer edge
          _VtxWritePtr[2].pos = temp_points[i * 2 + 1];
          _VtxWritePtr[2].uv = opaque_uv;
          _VtxWritePtr[2].col = col_trans;  // Right-side outer edge
          _VtxWritePtr += 3;
        }
      }
    } else {
      // [PATH 2] Non texture-based lines (thick): we need to draw the solid line core and thus
      // require four vertices per point
      const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;

      // If line is not closed, the first and last points need to be generated differently as there
      // are no normals to blend
      if (!closed) {
        const int points_last = points_count - 1;
        temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
        temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
        temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
        temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
        temp_points[points_last * 4 + 0] = points[points_last] +
                                           temp_normals[points_last] *
                                             (half_inner_thickness + AA_SIZE);
        temp_points[points_last * 4 + 1] = points[points_last] +
                                           temp_normals[points_last] * (half_inner_thickness);
        temp_points[points_last * 4 + 2] = points[points_last] -
                                           temp_normals[points_last] * (half_inner_thickness);
        temp_points[points_last * 4 + 3] = points[points_last] -
                                           temp_normals[points_last] *
                                             (half_inner_thickness + AA_SIZE);
      }

      // Generate the indices to form a number of triangles for each line segment, and the vertices
      // for the line edges This takes points n and n+1 and writes into n+1, with the first point
      // in a closed line being generated from the final one (as n+1 wraps)
      // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
      unsigned int idx1 = _VtxCurrentIdx;  // Vertex index for start of line segment
      for (int i1 = 0; i1 < count; i1++)   // i1 is the first point of the line segment
      {
        const int i2 = (i1 + 1) == points_count ?
                         0 :
                         (i1 + 1);  // i2 is the second point of the line segment
        const unsigned int idx2 = (i1 + 1) == points_count ?
                                    _VtxCurrentIdx :
                                    (idx1 + 4);  // Vertex index for end of segment

        // Average normals
        float dm_x = (temp_normals[i1][0] + temp_normals[i2][0]) * 0.5f;
        float dm_y = (temp_normals[i1][1] + temp_normals[i2][1]) * 0.5f;
        IM_FIXNORMAL2F(dm_x, dm_y);
        float dm_out_x = dm_x * (half_inner_thickness + AA_SIZE);
        float dm_out_y = dm_y * (half_inner_thickness + AA_SIZE);
        float dm_in_x = dm_x * half_inner_thickness;
        float dm_in_y = dm_y * half_inner_thickness;

        // Add temporary vertices
        GfVec2f *out_vtx = &temp_points[i2 * 4];
        out_vtx[0][0] = points[i2][0] + dm_out_x;
        out_vtx[0][1] = points[i2][1] + dm_out_y;
        out_vtx[1][0] = points[i2][0] + dm_in_x;
        out_vtx[1][1] = points[i2][1] + dm_in_y;
        out_vtx[2][0] = points[i2][0] - dm_in_x;
        out_vtx[2][1] = points[i2][1] - dm_in_y;
        out_vtx[3][0] = points[i2][0] - dm_out_x;
        out_vtx[3][1] = points[i2][1] - dm_out_y;

        // Add indexes
        _IdxWritePtr[0] = (AnchorDrawIdx)(idx2 + 1);
        _IdxWritePtr[1] = (AnchorDrawIdx)(idx1 + 1);
        _IdxWritePtr[2] = (AnchorDrawIdx)(idx1 + 2);
        _IdxWritePtr[3] = (AnchorDrawIdx)(idx1 + 2);
        _IdxWritePtr[4] = (AnchorDrawIdx)(idx2 + 2);
        _IdxWritePtr[5] = (AnchorDrawIdx)(idx2 + 1);
        _IdxWritePtr[6] = (AnchorDrawIdx)(idx2 + 1);
        _IdxWritePtr[7] = (AnchorDrawIdx)(idx1 + 1);
        _IdxWritePtr[8] = (AnchorDrawIdx)(idx1 + 0);
        _IdxWritePtr[9] = (AnchorDrawIdx)(idx1 + 0);
        _IdxWritePtr[10] = (AnchorDrawIdx)(idx2 + 0);
        _IdxWritePtr[11] = (AnchorDrawIdx)(idx2 + 1);
        _IdxWritePtr[12] = (AnchorDrawIdx)(idx2 + 2);
        _IdxWritePtr[13] = (AnchorDrawIdx)(idx1 + 2);
        _IdxWritePtr[14] = (AnchorDrawIdx)(idx1 + 3);
        _IdxWritePtr[15] = (AnchorDrawIdx)(idx1 + 3);
        _IdxWritePtr[16] = (AnchorDrawIdx)(idx2 + 3);
        _IdxWritePtr[17] = (AnchorDrawIdx)(idx2 + 2);
        _IdxWritePtr += 18;

        idx1 = idx2;
      }

      // Add vertices
      for (int i = 0; i < points_count; i++) {
        _VtxWritePtr[0].pos = temp_points[i * 4 + 0];
        _VtxWritePtr[0].uv = opaque_uv;
        _VtxWritePtr[0].col = col_trans;
        _VtxWritePtr[1].pos = temp_points[i * 4 + 1];
        _VtxWritePtr[1].uv = opaque_uv;
        _VtxWritePtr[1].col = col;
        _VtxWritePtr[2].pos = temp_points[i * 4 + 2];
        _VtxWritePtr[2].uv = opaque_uv;
        _VtxWritePtr[2].col = col;
        _VtxWritePtr[3].pos = temp_points[i * 4 + 3];
        _VtxWritePtr[3].uv = opaque_uv;
        _VtxWritePtr[3].col = col_trans;
        _VtxWritePtr += 4;
      }
    }
    _VtxCurrentIdx += (AnchorDrawIdx)vtx_count;
  } else {
    // [PATH 4] Non texture-based, Non anti-aliased lines
    const int idx_count = count * 6;
    const int vtx_count = count * 4;  // FIXME-OPT: Not sharing edges
    PrimReserve(idx_count, vtx_count);

    for (int i1 = 0; i1 < count; i1++) {
      const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
      const GfVec2f &p1 = points[i1];
      const GfVec2f &p2 = points[i2];

      float dx = p2[0] - p1[0];
      float dy = p2[1] - p1[1];
      IM_NORMALIZE2F_OVER_ZERO(dx, dy);
      dx *= (thickness * 0.5f);
      dy *= (thickness * 0.5f);

      _VtxWritePtr[0].pos[0] = p1[0] + dy;
      _VtxWritePtr[0].pos[1] = p1[1] - dx;
      _VtxWritePtr[0].uv = opaque_uv;
      _VtxWritePtr[0].col = col;
      _VtxWritePtr[1].pos[0] = p2[0] + dy;
      _VtxWritePtr[1].pos[1] = p2[1] - dx;
      _VtxWritePtr[1].uv = opaque_uv;
      _VtxWritePtr[1].col = col;
      _VtxWritePtr[2].pos[0] = p2[0] - dy;
      _VtxWritePtr[2].pos[1] = p2[1] + dx;
      _VtxWritePtr[2].uv = opaque_uv;
      _VtxWritePtr[2].col = col;
      _VtxWritePtr[3].pos[0] = p1[0] - dy;
      _VtxWritePtr[3].pos[1] = p1[1] + dx;
      _VtxWritePtr[3].uv = opaque_uv;
      _VtxWritePtr[3].col = col;
      _VtxWritePtr += 4;

      _IdxWritePtr[0] = (AnchorDrawIdx)(_VtxCurrentIdx);
      _IdxWritePtr[1] = (AnchorDrawIdx)(_VtxCurrentIdx + 1);
      _IdxWritePtr[2] = (AnchorDrawIdx)(_VtxCurrentIdx + 2);
      _IdxWritePtr[3] = (AnchorDrawIdx)(_VtxCurrentIdx);
      _IdxWritePtr[4] = (AnchorDrawIdx)(_VtxCurrentIdx + 2);
      _IdxWritePtr[5] = (AnchorDrawIdx)(_VtxCurrentIdx + 3);
      _IdxWritePtr += 6;
      _VtxCurrentIdx += 4;
    }
  }
}

// We intentionally avoid using GfVec2f and its math operators here to reduce cost to a minimum for
// debug/non-inlined builds.
void AnchorDrawList::AddConvexPolyFilled(const GfVec2f *points,
                                         const int points_count,
                                         AnchorU32 col)
{
  if (points_count < 3)
    return;

  const GfVec2f uv = _Data->TexUvWhitePixel;

  if (Flags & AnchorDrawListFlags_AntiAliasedFill) {
    // Anti-aliased Fill
    const float AA_SIZE = _FringeScale;
    const AnchorU32 col_trans = col & ~ANCHOR_COL32_A_MASK;
    const int idx_count = (points_count - 2) * 3 + points_count * 6;
    const int vtx_count = (points_count * 2);
    PrimReserve(idx_count, vtx_count);

    // Add indexes for fill
    unsigned int vtx_inner_idx = _VtxCurrentIdx;
    unsigned int vtx_outer_idx = _VtxCurrentIdx + 1;
    for (int i = 2; i < points_count; i++) {
      _IdxWritePtr[0] = (AnchorDrawIdx)(vtx_inner_idx);
      _IdxWritePtr[1] = (AnchorDrawIdx)(vtx_inner_idx + ((i - 1) << 1));
      _IdxWritePtr[2] = (AnchorDrawIdx)(vtx_inner_idx + (i << 1));
      _IdxWritePtr += 3;
    }

    // Compute normals
    GfVec2f *temp_normals = (GfVec2f *)alloca(points_count * sizeof(GfVec2f));  //-V630
    for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
      const GfVec2f &p0 = points[i0];
      const GfVec2f &p1 = points[i1];
      float dx = p1[0] - p0[0];
      float dy = p1[1] - p0[1];
      IM_NORMALIZE2F_OVER_ZERO(dx, dy);
      temp_normals[i0][0] = dy;
      temp_normals[i0][1] = -dx;
    }

    for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++) {
      // Average normals
      const GfVec2f &n0 = temp_normals[i0];
      const GfVec2f &n1 = temp_normals[i1];
      float dm_x = (n0[0] + n1[0]) * 0.5f;
      float dm_y = (n0[1] + n1[1]) * 0.5f;
      IM_FIXNORMAL2F(dm_x, dm_y);
      dm_x *= AA_SIZE * 0.5f;
      dm_y *= AA_SIZE * 0.5f;

      // Add vertices
      _VtxWritePtr[0].pos[0] = (points[i1][0] - dm_x);
      _VtxWritePtr[0].pos[1] = (points[i1][1] - dm_y);
      _VtxWritePtr[0].uv = uv;
      _VtxWritePtr[0].col = col;  // Inner
      _VtxWritePtr[1].pos[0] = (points[i1][0] + dm_x);
      _VtxWritePtr[1].pos[1] = (points[i1][1] + dm_y);
      _VtxWritePtr[1].uv = uv;
      _VtxWritePtr[1].col = col_trans;  // Outer
      _VtxWritePtr += 2;

      // Add indexes for fringes
      _IdxWritePtr[0] = (AnchorDrawIdx)(vtx_inner_idx + (i1 << 1));
      _IdxWritePtr[1] = (AnchorDrawIdx)(vtx_inner_idx + (i0 << 1));
      _IdxWritePtr[2] = (AnchorDrawIdx)(vtx_outer_idx + (i0 << 1));
      _IdxWritePtr[3] = (AnchorDrawIdx)(vtx_outer_idx + (i0 << 1));
      _IdxWritePtr[4] = (AnchorDrawIdx)(vtx_outer_idx + (i1 << 1));
      _IdxWritePtr[5] = (AnchorDrawIdx)(vtx_inner_idx + (i1 << 1));
      _IdxWritePtr += 6;
    }
    _VtxCurrentIdx += (AnchorDrawIdx)vtx_count;
  } else {
    // Non Anti-aliased Fill
    const int idx_count = (points_count - 2) * 3;
    const int vtx_count = points_count;
    PrimReserve(idx_count, vtx_count);
    for (int i = 0; i < vtx_count; i++) {
      _VtxWritePtr[0].pos = points[i];
      _VtxWritePtr[0].uv = uv;
      _VtxWritePtr[0].col = col;
      _VtxWritePtr++;
    }
    for (int i = 2; i < points_count; i++) {
      _IdxWritePtr[0] = (AnchorDrawIdx)(_VtxCurrentIdx);
      _IdxWritePtr[1] = (AnchorDrawIdx)(_VtxCurrentIdx + i - 1);
      _IdxWritePtr[2] = (AnchorDrawIdx)(_VtxCurrentIdx + i);
      _IdxWritePtr += 3;
    }
    _VtxCurrentIdx += (AnchorDrawIdx)vtx_count;
  }
}

void AnchorDrawList::_PathArcToFastEx(const GfVec2f &center,
                                      float radius,
                                      int a_min_sample,
                                      int a_max_sample,
                                      int a_step)
{
  if (radius <= 0.0f) {
    _Path.push_back(center);
    return;
  }

  // Calculate arc auto segment step size
  if (a_step <= 0)
    a_step = ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX / _CalcCircleAutoSegmentCount(radius);

  // Make sure we never do steps larger than one quarter of the circle
  a_step = AnchorClamp(a_step, 1, ANCHOR_DRAWLIST_ARCFAST_TABLE_SIZE / 4);

  const int sample_range = AnchorAbs(a_max_sample - a_min_sample);
  const int a_next_step = a_step;

  int samples = sample_range + 1;
  bool extra_max_sample = false;
  if (a_step > 1) {
    samples = sample_range / a_step + 1;
    const int overstep = sample_range % a_step;

    if (overstep > 0) {
      extra_max_sample = true;
      samples++;

      // When we have overstep to avoid awkwardly looking one long line and one tiny one at the
      // end, distribute first step range evenly between them by reducing first step size.
      if (sample_range > 0)
        a_step -= (a_step - overstep) / 2;
    }
  }

  _Path.resize(_Path.Size + samples);
  GfVec2f *out_ptr = _Path.Data + (_Path.Size - samples);

  int sample_index = a_min_sample;
  if (sample_index < 0 || sample_index >= ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX) {
    sample_index = sample_index % ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX;
    if (sample_index < 0)
      sample_index += ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX;
  }

  if (a_max_sample >= a_min_sample) {
    for (int a = a_min_sample; a <= a_max_sample;
         a += a_step, sample_index += a_step, a_step = a_next_step) {
      // a_step is clamped to ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX, so we have guaranteed that it
      // will not wrap over range twice or more
      if (sample_index >= ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX)
        sample_index -= ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX;

      const GfVec2f s = _Data->ArcFastVtx[sample_index];
      out_ptr->data()[0] = center[0] + s[0] * radius;
      out_ptr->data()[1] = center[1] + s[1] * radius;
      out_ptr++;
    }
  } else {
    for (int a = a_min_sample; a >= a_max_sample;
         a -= a_step, sample_index -= a_step, a_step = a_next_step) {
      // a_step is clamped to ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX, so we have guaranteed that it
      // will not wrap over range twice or more
      if (sample_index < 0)
        sample_index += ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX;

      const GfVec2f s = _Data->ArcFastVtx[sample_index];
      out_ptr->data()[0] = center[0] + s[0] * radius;
      out_ptr->data()[1] = center[1] + s[1] * radius;
      out_ptr++;
    }
  }

  if (extra_max_sample) {
    int normalized_max_sample = a_max_sample % ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX;
    if (normalized_max_sample < 0)
      normalized_max_sample += ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX;

    const GfVec2f s = _Data->ArcFastVtx[normalized_max_sample];
    out_ptr->data()[0] = center[0] + s[0] * radius;
    out_ptr->data()[1] = center[1] + s[1] * radius;
    out_ptr++;
  }

  ANCHOR_ASSERT_PARANOID(_Path.Data + _Path.Size == out_ptr);
}

void AnchorDrawList::_PathArcToN(const GfVec2f &center,
                                 float radius,
                                 float a_min,
                                 float a_max,
                                 int num_segments)
{
  if (radius <= 0.0f) {
    _Path.push_back(center);
    return;
  }

  // Note that we are adding a point at both a_min and a_max.
  // If you are trying to draw a full closed circle you don't want the overlapping points!
  _Path.reserve(_Path.Size + (num_segments + 1));
  for (int i = 0; i <= num_segments; i++) {
    const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
    _Path.push_back(GfVec2f(center[0] + AnchorCos(a) * radius, center[1] + AnchorSin(a) * radius));
  }
}

// 0: East, 3: South, 6: West, 9: North, 12: East
void AnchorDrawList::PathArcToFast(const GfVec2f &center,
                                   float radius,
                                   int a_min_of_12,
                                   int a_max_of_12)
{
  if (radius <= 0.0f) {
    _Path.push_back(center);
    return;
  }
  _PathArcToFastEx(center,
                   radius,
                   a_min_of_12 * ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX / 12,
                   a_max_of_12 * ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX / 12,
                   0);
}

void AnchorDrawList::PathArcTo(const GfVec2f &center,
                               float radius,
                               float a_min,
                               float a_max,
                               int num_segments)
{
  if (radius <= 0.0f) {
    _Path.push_back(center);
    return;
  }

  if (num_segments > 0) {
    _PathArcToN(center, radius, a_min, a_max, num_segments);
    return;
  }

  // Automatic segment count
  if (radius <= _Data->ArcFastRadiusCutoff) {
    const bool a_is_reverse = a_max < a_min;

    // We are going to use precomputed values for mid samples.
    // Determine first and last sample in lookup table that belong to the arc.
    const float a_min_sample_f = ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX * a_min / (IM_PI * 2.0f);
    const float a_max_sample_f = ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX * a_max / (IM_PI * 2.0f);

    const int a_min_sample = a_is_reverse ? (int)AnchorFloorSigned(a_min_sample_f) :
                                            (int)ImCeil(a_min_sample_f);
    const int a_max_sample = a_is_reverse ? (int)ImCeil(a_max_sample_f) :
                                            (int)AnchorFloorSigned(a_max_sample_f);
    const int a_mid_samples = a_is_reverse ? AnchorMax(a_min_sample - a_max_sample, 0) :
                                             AnchorMax(a_max_sample - a_min_sample, 0);

    const float a_min_segment_angle = a_min_sample * IM_PI * 2.0f /
                                      ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX;
    const float a_max_segment_angle = a_max_sample * IM_PI * 2.0f /
                                      ANCHOR_DRAWLIST_ARCFAST_SAMPLE_MAX;
    const bool a_emit_start = (a_min_segment_angle - a_min) != 0.0f;
    const bool a_emit_end = (a_max - a_max_segment_angle) != 0.0f;

    _Path.reserve(_Path.Size +
                  (a_mid_samples + 1 + (a_emit_start ? 1 : 0) + (a_emit_end ? 1 : 0)));
    if (a_emit_start)
      _Path.push_back(
        GfVec2f(center[0] + AnchorCos(a_min) * radius, center[1] + AnchorSin(a_min) * radius));
    if (a_mid_samples > 0)
      _PathArcToFastEx(center, radius, a_min_sample, a_max_sample, 0);
    if (a_emit_end)
      _Path.push_back(
        GfVec2f(center[0] + AnchorCos(a_max) * radius, center[1] + AnchorSin(a_max) * radius));
  } else {
    const float arc_length = AnchorAbs(a_max - a_min);
    const int circle_segment_count = _CalcCircleAutoSegmentCount(radius);
    const int arc_segment_count = AnchorMax(
      (int)ImCeil(circle_segment_count * arc_length / (IM_PI * 2.0f)),
      (int)(2.0f * IM_PI / arc_length));
    _PathArcToN(center, radius, a_min, a_max, arc_segment_count);
  }
}

GfVec2f AnchorBezierCubicCalc(const GfVec2f &p1,
                              const GfVec2f &p2,
                              const GfVec2f &p3,
                              const GfVec2f &p4,
                              float t)
{
  float u = 1.0f - t;
  float w1 = u * u * u;
  float w2 = 3 * u * u * t;
  float w3 = 3 * u * t * t;
  float w4 = t * t * t;
  return GfVec2f(w1 * p1[0] + w2 * p2[0] + w3 * p3[0] + w4 * p4[0],
                 w1 * p1[1] + w2 * p2[1] + w3 * p3[1] + w4 * p4[1]);
}

GfVec2f AnchorBezierQuadraticCalc(const GfVec2f &p1, const GfVec2f &p2, const GfVec2f &p3, float t)
{
  float u = 1.0f - t;
  float w1 = u * u;
  float w2 = 2 * u * t;
  float w3 = t * t;
  return GfVec2f(w1 * p1[0] + w2 * p2[0] + w3 * p3[0], w1 * p1[1] + w2 * p2[1] + w3 * p3[1]);
}

// Closely mimics AnchorBezierCubicClosestPointCasteljau() in ANCHOR.cpp
static void PathBezierCubicCurveToCasteljau(AnchorVector<GfVec2f> *path,
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
  float d2 = (x2 - x4) * dy - (y2 - y4) * dx;
  float d3 = (x3 - x4) * dy - (y3 - y4) * dx;
  d2 = (d2 >= 0) ? d2 : -d2;
  d3 = (d3 >= 0) ? d3 : -d3;
  if ((d2 + d3) * (d2 + d3) < tess_tol * (dx * dx + dy * dy)) {
    path->push_back(GfVec2f(x4, y4));
  } else if (level < 10) {
    float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
    float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
    float x34 = (x3 + x4) * 0.5f, y34 = (y3 + y4) * 0.5f;
    float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
    float x234 = (x23 + x34) * 0.5f, y234 = (y23 + y34) * 0.5f;
    float x1234 = (x123 + x234) * 0.5f, y1234 = (y123 + y234) * 0.5f;
    PathBezierCubicCurveToCasteljau(path,
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
    PathBezierCubicCurveToCasteljau(path,
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

static void PathBezierQuadraticCurveToCasteljau(AnchorVector<GfVec2f> *path,
                                                float x1,
                                                float y1,
                                                float x2,
                                                float y2,
                                                float x3,
                                                float y3,
                                                float tess_tol,
                                                int level)
{
  float dx = x3 - x1, dy = y3 - y1;
  float det = (x2 - x3) * dy - (y2 - y3) * dx;
  if (det * det * 4.0f < tess_tol * (dx * dx + dy * dy)) {
    path->push_back(GfVec2f(x3, y3));
  } else if (level < 10) {
    float x12 = (x1 + x2) * 0.5f, y12 = (y1 + y2) * 0.5f;
    float x23 = (x2 + x3) * 0.5f, y23 = (y2 + y3) * 0.5f;
    float x123 = (x12 + x23) * 0.5f, y123 = (y12 + y23) * 0.5f;
    PathBezierQuadraticCurveToCasteljau(path, x1, y1, x12, y12, x123, y123, tess_tol, level + 1);
    PathBezierQuadraticCurveToCasteljau(path, x123, y123, x23, y23, x3, y3, tess_tol, level + 1);
  }
}

void AnchorDrawList::PathBezierCubicCurveTo(const GfVec2f &p2,
                                            const GfVec2f &p3,
                                            const GfVec2f &p4,
                                            int num_segments)
{
  GfVec2f p1 = _Path.back();
  if (num_segments == 0) {
    PathBezierCubicCurveToCasteljau(&_Path,
                                    p1[0],
                                    p1[1],
                                    p2[0],
                                    p2[1],
                                    p3[0],
                                    p3[1],
                                    p4[0],
                                    p4[1],
                                    _Data->CurveTessellationTol,
                                    0);  // Auto-tessellated
  } else {
    float t_step = 1.0f / (float)num_segments;
    for (int i_step = 1; i_step <= num_segments; i_step++)
      _Path.push_back(AnchorBezierCubicCalc(p1, p2, p3, p4, t_step * i_step));
  }
}

void AnchorDrawList::PathBezierQuadraticCurveTo(const GfVec2f &p2,
                                                const GfVec2f &p3,
                                                int num_segments)
{
  GfVec2f p1 = _Path.back();
  if (num_segments == 0) {
    PathBezierQuadraticCurveToCasteljau(&_Path,
                                        p1[0],
                                        p1[1],
                                        p2[0],
                                        p2[1],
                                        p3[0],
                                        p3[1],
                                        _Data->CurveTessellationTol,
                                        0);  // Auto-tessellated
  } else {
    float t_step = 1.0f / (float)num_segments;
    for (int i_step = 1; i_step <= num_segments; i_step++)
      _Path.push_back(AnchorBezierQuadraticCalc(p1, p2, p3, t_step * i_step));
  }
}

IM_STATIC_ASSERT(AnchorDrawFlags_RoundCornersTopLeft == (1 << 4));
static inline AnchorDrawFlags FixRectCornerFlags(AnchorDrawFlags flags)
{
#ifndef ANCHOR_DISABLE_OBSOLETE_FUNCTIONS
  // Legacy Support for hard coded ~0 (used to be a suggested equivalent to
  // AnchorDrawCornerFlags_All)
  //   ~0   --> AnchorDrawFlags_RoundCornersAll or 0
  if (flags == ~0)
    return AnchorDrawFlags_RoundCornersAll;

  // Legacy Support for hard coded 0x01 to 0x0F (matching 15 out of 16 old flags combinations)
  //   0x01 --> AnchorDrawFlags_RoundCornersTopLeft (VALUE 0x01 OVERLAPS AnchorDrawFlags_Closed but
  //   AnchorDrawFlags_Closed is never valid in this path!) 0x02 -->
  //   AnchorDrawFlags_RoundCornersTopRight 0x03 --> AnchorDrawFlags_RoundCornersTopLeft |
  //   AnchorDrawFlags_RoundCornersTopRight 0x04 --> AnchorDrawFlags_RoundCornersBotLeft 0x05 -->
  //   AnchorDrawFlags_RoundCornersTopLeft | AnchorDrawFlags_RoundCornersBotLeft
  //   ...
  //   0x0F --> AnchorDrawFlags_RoundCornersAll or 0
  // (See all values in AnchorDrawCornerFlags_)
  if (flags >= 0x01 && flags <= 0x0F)
    return (flags << 4);

    // We cannot support hard coded 0x00 with 'float rounding > 0.0f' --> replace with
    // AnchorDrawFlags_RoundCornersNone or use 'float rounding = 0.0f'
#endif

  // If this triggers, please update your code replacing hardcoded values with new
  // AnchorDrawFlags_RoundCorners* values. Note that AnchorDrawFlags_Closed (== 0x01) is an invalid
  // flag for AddRect(), AddRectFilled(), PathRect() etc...
  ANCHOR_ASSERT((flags & 0x0F) == 0 && "Misuse of legacy hardcoded AnchorDrawCornerFlags values!");

  if ((flags & AnchorDrawFlags_RoundCornersMask_) == 0)
    flags |= AnchorDrawFlags_RoundCornersAll;

  return flags;
}

void AnchorDrawList::PathRect(const GfVec2f &a,
                              const GfVec2f &b,
                              float rounding,
                              AnchorDrawFlags flags)
{
  flags = FixRectCornerFlags(flags);
  rounding = AnchorMin(
    rounding,
    AnchorFabs(b[0] - a[0]) *
        (((flags & AnchorDrawFlags_RoundCornersTop) == AnchorDrawFlags_RoundCornersTop) ||
             ((flags & AnchorDrawFlags_RoundCornersBottom) == AnchorDrawFlags_RoundCornersBottom) ?
           0.5f :
           1.0f) -
      1.0f);
  rounding = AnchorMin(
    rounding,
    AnchorFabs(b[1] - a[1]) *
        (((flags & AnchorDrawFlags_RoundCornersLeft) == AnchorDrawFlags_RoundCornersLeft) ||
             ((flags & AnchorDrawFlags_RoundCornersRight) == AnchorDrawFlags_RoundCornersRight) ?
           0.5f :
           1.0f) -
      1.0f);

  if (rounding <= 0.0f ||
      (flags & AnchorDrawFlags_RoundCornersMask_) == AnchorDrawFlags_RoundCornersNone) {
    PathLineTo(a);
    PathLineTo(GfVec2f(b[0], a[1]));
    PathLineTo(b);
    PathLineTo(GfVec2f(a[0], b[1]));
  } else {
    const float rounding_tl = (flags & AnchorDrawFlags_RoundCornersTopLeft) ? rounding : 0.0f;
    const float rounding_tr = (flags & AnchorDrawFlags_RoundCornersTopRight) ? rounding : 0.0f;
    const float rounding_br = (flags & AnchorDrawFlags_RoundCornersBottomRight) ? rounding : 0.0f;
    const float rounding_bl = (flags & AnchorDrawFlags_RoundCornersBottomLeft) ? rounding : 0.0f;
    PathArcToFast(GfVec2f(a[0] + rounding_tl, a[1] + rounding_tl), rounding_tl, 6, 9);
    PathArcToFast(GfVec2f(b[0] - rounding_tr, a[1] + rounding_tr), rounding_tr, 9, 12);
    PathArcToFast(GfVec2f(b[0] - rounding_br, b[1] - rounding_br), rounding_br, 0, 3);
    PathArcToFast(GfVec2f(a[0] + rounding_bl, b[1] - rounding_bl), rounding_bl, 3, 6);
  }
}

void AnchorDrawList::AddLine(const GfVec2f &p1, const GfVec2f &p2, AnchorU32 col, float thickness)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;
  PathLineTo(p1 + GfVec2f(0.5f, 0.5f));
  PathLineTo(p2 + GfVec2f(0.5f, 0.5f));
  PathStroke(col, 0, thickness);
}

// p_min = upper-left, p_max = lower-right
// Note we don't render 1 pixels sized rectangles properly.
void AnchorDrawList::AddRect(const GfVec2f &p_min,
                             const GfVec2f &p_max,
                             AnchorU32 col,
                             float rounding,
                             AnchorDrawFlags flags,
                             float thickness)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;
  if (Flags & AnchorDrawListFlags_AntiAliasedLines)
    PathRect(p_min + GfVec2f(0.50f, 0.50f), p_max - GfVec2f(0.50f, 0.50f), rounding, flags);
  else
    PathRect(p_min + GfVec2f(0.50f, 0.50f),
             p_max - GfVec2f(0.49f, 0.49f),
             rounding,
             flags);  // Better looking lower-right corner and rounded non-AA shapes.
  PathStroke(col, AnchorDrawFlags_Closed, thickness);
}

void AnchorDrawList::AddRectFilled(const GfVec2f &p_min,
                                   const GfVec2f &p_max,
                                   AnchorU32 col,
                                   float rounding,
                                   AnchorDrawFlags flags)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;
  if (rounding <= 0.0f ||
      (flags & AnchorDrawFlags_RoundCornersMask_) == AnchorDrawFlags_RoundCornersNone) {
    PrimReserve(6, 4);
    PrimRect(p_min, p_max, col);
  } else {
    PathRect(p_min, p_max, rounding, flags);
    PathFillConvex(col);
  }
}

// p_min = upper-left, p_max = lower-right
void AnchorDrawList::AddRectFilledMultiColor(const GfVec2f &p_min,
                                             const GfVec2f &p_max,
                                             AnchorU32 col_upr_left,
                                             AnchorU32 col_upr_right,
                                             AnchorU32 col_bot_right,
                                             AnchorU32 col_bot_left)
{
  if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & ANCHOR_COL32_A_MASK) == 0)
    return;

  const GfVec2f uv = _Data->TexUvWhitePixel;
  PrimReserve(6, 4);
  PrimWriteIdx((AnchorDrawIdx)(_VtxCurrentIdx));
  PrimWriteIdx((AnchorDrawIdx)(_VtxCurrentIdx + 1));
  PrimWriteIdx((AnchorDrawIdx)(_VtxCurrentIdx + 2));
  PrimWriteIdx((AnchorDrawIdx)(_VtxCurrentIdx));
  PrimWriteIdx((AnchorDrawIdx)(_VtxCurrentIdx + 2));
  PrimWriteIdx((AnchorDrawIdx)(_VtxCurrentIdx + 3));
  PrimWriteVtx(p_min, uv, col_upr_left);
  PrimWriteVtx(GfVec2f(p_max[0], p_min[1]), uv, col_upr_right);
  PrimWriteVtx(p_max, uv, col_bot_right);
  PrimWriteVtx(GfVec2f(p_min[0], p_max[1]), uv, col_bot_left);
}

void AnchorDrawList::AddQuad(const GfVec2f &p1,
                             const GfVec2f &p2,
                             const GfVec2f &p3,
                             const GfVec2f &p4,
                             AnchorU32 col,
                             float thickness)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;

  PathLineTo(p1);
  PathLineTo(p2);
  PathLineTo(p3);
  PathLineTo(p4);
  PathStroke(col, AnchorDrawFlags_Closed, thickness);
}

void AnchorDrawList::AddQuadFilled(const GfVec2f &p1,
                                   const GfVec2f &p2,
                                   const GfVec2f &p3,
                                   const GfVec2f &p4,
                                   AnchorU32 col)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;

  PathLineTo(p1);
  PathLineTo(p2);
  PathLineTo(p3);
  PathLineTo(p4);
  PathFillConvex(col);
}

void AnchorDrawList::AddTriangle(const GfVec2f &p1,
                                 const GfVec2f &p2,
                                 const GfVec2f &p3,
                                 AnchorU32 col,
                                 float thickness)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;

  PathLineTo(p1);
  PathLineTo(p2);
  PathLineTo(p3);
  PathStroke(col, AnchorDrawFlags_Closed, thickness);
}

void AnchorDrawList::AddTriangleFilled(const GfVec2f &p1,
                                       const GfVec2f &p2,
                                       const GfVec2f &p3,
                                       AnchorU32 col)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;

  PathLineTo(p1);
  PathLineTo(p2);
  PathLineTo(p3);
  PathFillConvex(col);
}

void AnchorDrawList::AddCircle(const GfVec2f &center,
                               float radius,
                               AnchorU32 col,
                               int num_segments,
                               float thickness)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0 || radius <= 0.0f)
    return;

  // Obtain segment count
  if (num_segments <= 0) {
    // Automatic segment count
    num_segments = _CalcCircleAutoSegmentCount(radius);
  } else {
    // Explicit segment count (still clamp to avoid drawing insanely tessellated shapes)
    num_segments = AnchorClamp(num_segments, 3, ANCHOR_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX);
  }

  // Because we are filling a closed shape we remove 1 from the count of segments/points
  const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
  if (num_segments == 12)
    PathArcToFast(center, radius - 0.5f, 0, 12 - 1);
  else
    PathArcTo(center, radius - 0.5f, 0.0f, a_max, num_segments - 1);
  PathStroke(col, AnchorDrawFlags_Closed, thickness);
}

void AnchorDrawList::AddCircleFilled(const GfVec2f &center,
                                     float radius,
                                     AnchorU32 col,
                                     int num_segments)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0 || radius <= 0.0f)
    return;

  // Obtain segment count
  if (num_segments <= 0) {
    // Automatic segment count
    num_segments = _CalcCircleAutoSegmentCount(radius);
  } else {
    // Explicit segment count (still clamp to avoid drawing insanely tessellated shapes)
    num_segments = AnchorClamp(num_segments, 3, ANCHOR_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX);
  }

  // Because we are filling a closed shape we remove 1 from the count of segments/points
  const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
  if (num_segments == 12)
    PathArcToFast(center, radius, 0, 12 - 1);
  else
    PathArcTo(center, radius, 0.0f, a_max, num_segments - 1);
  PathFillConvex(col);
}

// Guaranteed to honor 'num_segments'
void AnchorDrawList::AddNgon(const GfVec2f &center,
                             float radius,
                             AnchorU32 col,
                             int num_segments,
                             float thickness)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0 || num_segments <= 2)
    return;

  // Because we are filling a closed shape we remove 1 from the count of segments/points
  const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
  PathArcTo(center, radius - 0.5f, 0.0f, a_max, num_segments - 1);
  PathStroke(col, AnchorDrawFlags_Closed, thickness);
}

// Guaranteed to honor 'num_segments'
void AnchorDrawList::AddNgonFilled(const GfVec2f &center,
                                   float radius,
                                   AnchorU32 col,
                                   int num_segments)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0 || num_segments <= 2)
    return;

  // Because we are filling a closed shape we remove 1 from the count of segments/points
  const float a_max = (IM_PI * 2.0f) * ((float)num_segments - 1.0f) / (float)num_segments;
  PathArcTo(center, radius, 0.0f, a_max, num_segments - 1);
  PathFillConvex(col);
}

// Cubic Bezier takes 4 controls points
void AnchorDrawList::AddBezierCubic(const GfVec2f &p1,
                                    const GfVec2f &p2,
                                    const GfVec2f &p3,
                                    const GfVec2f &p4,
                                    AnchorU32 col,
                                    float thickness,
                                    int num_segments)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;

  PathLineTo(p1);
  PathBezierCubicCurveTo(p2, p3, p4, num_segments);
  PathStroke(col, 0, thickness);
}

// Quadratic Bezier takes 3 controls points
void AnchorDrawList::AddBezierQuadratic(const GfVec2f &p1,
                                        const GfVec2f &p2,
                                        const GfVec2f &p3,
                                        AnchorU32 col,
                                        float thickness,
                                        int num_segments)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;

  PathLineTo(p1);
  PathBezierQuadraticCurveTo(p2, p3, num_segments);
  PathStroke(col, 0, thickness);
}

void AnchorDrawList::AddText(const AnchorFont *font,
                             float font_size,
                             const GfVec2f &pos,
                             AnchorU32 col,
                             const char *text_begin,
                             const char *text_end,
                             float wrap_width,
                             const GfVec4f *cpu_fine_clip_rect)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;

  if (text_end == NULL)
    text_end = text_begin + strlen(text_begin);
  if (text_begin == text_end)
    return;

  // Pull default font/size from the shared AnchorDrawListSharedData instance
  if (font == NULL)
    font = _Data->Font;
  if (font_size == 0.0f)
    font_size = _Data->FontSize;

  ANCHOR_ASSERT(font->ContainerAtlas->TexID ==
                _CmdHeader.TextureId);  // Use high-level ANCHOR::PushFont() or low-level
                                        // AnchorDrawList::PushTextureId() to change font.

  GfVec4f clip_rect = _CmdHeader.ClipRect;
  if (cpu_fine_clip_rect) {
    clip_rect[0] = AnchorMax(clip_rect[0], cpu_fine_clip_rect->data()[0]);
    clip_rect[1] = AnchorMax(clip_rect[1], cpu_fine_clip_rect->data()[1]);
    clip_rect[2] = AnchorMin(clip_rect[2], cpu_fine_clip_rect->data()[2]);
    clip_rect[3] = AnchorMin(clip_rect[3], cpu_fine_clip_rect->data()[3]);
  }
  font->RenderText(this,
                   font_size,
                   pos,
                   col,
                   clip_rect,
                   text_begin,
                   text_end,
                   wrap_width,
                   cpu_fine_clip_rect != NULL);
}

void AnchorDrawList::AddText(const GfVec2f &pos,
                             AnchorU32 col,
                             const char *text_begin,
                             const char *text_end)
{
  AddText(NULL, 0.0f, pos, col, text_begin, text_end);
}

void AnchorDrawList::AddImage(AnchorTextureID user_texture_id,
                              const GfVec2f &p_min,
                              const GfVec2f &p_max,
                              const GfVec2f &uv_min,
                              const GfVec2f &uv_max,
                              AnchorU32 col)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;

  const bool push_texture_id = user_texture_id != _CmdHeader.TextureId;
  if (push_texture_id)
    PushTextureID(user_texture_id);

  PrimReserve(6, 4);
  PrimRectUV(p_min, p_max, uv_min, uv_max, col);

  if (push_texture_id)
    PopTextureID();
}

void AnchorDrawList::AddImageQuad(AnchorTextureID user_texture_id,
                                  const GfVec2f &p1,
                                  const GfVec2f &p2,
                                  const GfVec2f &p3,
                                  const GfVec2f &p4,
                                  const GfVec2f &uv1,
                                  const GfVec2f &uv2,
                                  const GfVec2f &uv3,
                                  const GfVec2f &uv4,
                                  AnchorU32 col)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;

  const bool push_texture_id = user_texture_id != _CmdHeader.TextureId;
  if (push_texture_id)
    PushTextureID(user_texture_id);

  PrimReserve(6, 4);
  PrimQuadUV(p1, p2, p3, p4, uv1, uv2, uv3, uv4, col);

  if (push_texture_id)
    PopTextureID();
}

void AnchorDrawList::AddImageRounded(AnchorTextureID user_texture_id,
                                     const GfVec2f &p_min,
                                     const GfVec2f &p_max,
                                     const GfVec2f &uv_min,
                                     const GfVec2f &uv_max,
                                     AnchorU32 col,
                                     float rounding,
                                     AnchorDrawFlags flags)
{
  if ((col & ANCHOR_COL32_A_MASK) == 0)
    return;

  flags = FixRectCornerFlags(flags);
  if (rounding <= 0.0f ||
      (flags & AnchorDrawFlags_RoundCornersMask_) == AnchorDrawFlags_RoundCornersNone) {
    AddImage(user_texture_id, p_min, p_max, uv_min, uv_max, col);
    return;
  }

  const bool push_texture_id = user_texture_id != _CmdHeader.TextureId;
  if (push_texture_id)
    PushTextureID(user_texture_id);

  int vert_start_idx = VtxBuffer.Size;
  PathRect(p_min, p_max, rounding, flags);
  PathFillConvex(col);
  int vert_end_idx = VtxBuffer.Size;
  ANCHOR::ShadeVertsLinearUV(this,
                             vert_start_idx,
                             vert_end_idx,
                             p_min,
                             p_max,
                             uv_min,
                             uv_max,
                             true);

  if (push_texture_id)
    PopTextureID();
}

//-----------------------------------------------------------------------------
// [SECTION] AnchorDrawListSplitter
//-----------------------------------------------------------------------------
// FIXME: This may be a little confusing, trying to be a little too low-level/optimal instead of
// just doing vector swap..
//-----------------------------------------------------------------------------

void AnchorDrawListSplitter::ClearFreeMemory()
{
  for (int i = 0; i < _Channels.Size; i++) {
    if (i == _Current)
      memset(&_Channels[i], 0, sizeof(_Channels[i]));  // Current channel is a copy of
                                                       // CmdBuffer/IdxBuffer, don't destruct again
    _Channels[i]._CmdBuffer.clear();
    _Channels[i]._IdxBuffer.clear();
  }
  _Current = 0;
  _Count = 1;
  _Channels.clear();
}

void AnchorDrawListSplitter::Split(AnchorDrawList *draw_list, int channels_count)
{
  TF_UNUSED(draw_list);
  ANCHOR_ASSERT(_Current == 0 && _Count <= 1 &&
                "Nested channel splitting is not supported. Please use separate instances of "
                "AnchorDrawListSplitter.");
  int old_channels_count = _Channels.Size;
  if (old_channels_count < channels_count) {
    _Channels.reserve(channels_count);  // Avoid over reserving since this is likely to stay stable
    _Channels.resize(channels_count);
  }
  _Count = channels_count;

  // Channels[] (24/32 bytes each) hold storage that we'll swap with
  // draw_list->_CmdBuffer/_IdxBuffer The content of Channels[0] at this point doesn't matter. We
  // clear it to make state tidy in a debugger but we don't strictly need to. When we switch to the
  // next channel, we'll copy draw_list->_CmdBuffer/_IdxBuffer into Channels[0] and then
  // Channels[1] into draw_list->CmdBuffer/_IdxBuffer
  memset(&_Channels[0], 0, sizeof(AnchorDrawChannel));
  for (int i = 1; i < channels_count; i++) {
    if (i >= old_channels_count) {
      ANCHOR_PLACEMENT_NEW(&_Channels[i])
      AnchorDrawChannel();
    } else {
      _Channels[i]._CmdBuffer.resize(0);
      _Channels[i]._IdxBuffer.resize(0);
    }
  }
}

void AnchorDrawListSplitter::Merge(AnchorDrawList *draw_list)
{
  // Note that we never use or rely on _Channels.Size because it is merely a buffer that we never
  // shrink back to 0 to keep all sub-buffers ready for use.
  if (_Count <= 1)
    return;

  SetCurrentChannel(draw_list, 0);
  draw_list->_PopUnusedDrawCmd();

  // Calculate our final buffer sizes. Also fix the incorrect IdxOffset values in each command.
  int new_cmd_buffer_count = 0;
  int new_idx_buffer_count = 0;
  AnchorDrawCmd *last_cmd = (_Count > 0 && draw_list->CmdBuffer.Size > 0) ?
                              &draw_list->CmdBuffer.back() :
                              NULL;
  int idx_offset = last_cmd ? last_cmd->IdxOffset + last_cmd->ElemCount : 0;
  for (int i = 1; i < _Count; i++) {
    AnchorDrawChannel &ch = _Channels[i];

    // Equivalent of PopUnusedDrawCmd() for this channel's cmdbuffer and except we don't need to
    // test for UserCallback.
    if (ch._CmdBuffer.Size > 0 && ch._CmdBuffer.back().ElemCount == 0)
      ch._CmdBuffer.pop_back();

    if (ch._CmdBuffer.Size > 0 && last_cmd != NULL) {
      AnchorDrawCmd *next_cmd = &ch._CmdBuffer[0];
      if (AnchorDrawCmd_HeaderCompare(last_cmd, next_cmd) == 0 && last_cmd->UserCallback == NULL &&
          next_cmd->UserCallback == NULL) {
        // Merge previous channel last draw command with current channel first draw command if
        // matching.
        last_cmd->ElemCount += next_cmd->ElemCount;
        idx_offset += next_cmd->ElemCount;
        ch._CmdBuffer.erase(ch._CmdBuffer.Data);  // FIXME-OPT: Improve for multiple merges.
      }
    }
    if (ch._CmdBuffer.Size > 0)
      last_cmd = &ch._CmdBuffer.back();
    new_cmd_buffer_count += ch._CmdBuffer.Size;
    new_idx_buffer_count += ch._IdxBuffer.Size;
    for (int cmd_n = 0; cmd_n < ch._CmdBuffer.Size; cmd_n++) {
      ch._CmdBuffer.Data[cmd_n].IdxOffset = idx_offset;
      idx_offset += ch._CmdBuffer.Data[cmd_n].ElemCount;
    }
  }
  draw_list->CmdBuffer.resize(draw_list->CmdBuffer.Size + new_cmd_buffer_count);
  draw_list->IdxBuffer.resize(draw_list->IdxBuffer.Size + new_idx_buffer_count);

  // Write commands and indices in order (they are fairly small structures, we don't copy vertices
  // only indices)
  AnchorDrawCmd *cmd_write = draw_list->CmdBuffer.Data + draw_list->CmdBuffer.Size -
                             new_cmd_buffer_count;
  AnchorDrawIdx *idx_write = draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size -
                             new_idx_buffer_count;
  for (int i = 1; i < _Count; i++) {
    AnchorDrawChannel &ch = _Channels[i];
    if (int sz = ch._CmdBuffer.Size) {
      memcpy(cmd_write, ch._CmdBuffer.Data, sz * sizeof(AnchorDrawCmd));
      cmd_write += sz;
    }
    if (int sz = ch._IdxBuffer.Size) {
      memcpy(idx_write, ch._IdxBuffer.Data, sz * sizeof(AnchorDrawIdx));
      idx_write += sz;
    }
  }
  draw_list->_IdxWritePtr = idx_write;

  // Ensure there's always a non-callback draw command trailing the command-buffer
  if (draw_list->CmdBuffer.Size == 0 || draw_list->CmdBuffer.back().UserCallback != NULL)
    draw_list->AddDrawCmd();

  // If current command is used with different settings we need to add a new command
  AnchorDrawCmd *curr_cmd = &draw_list->CmdBuffer.Data[draw_list->CmdBuffer.Size - 1];
  if (curr_cmd->ElemCount == 0)
    AnchorDrawCmd_HeaderCopy(curr_cmd,
                             &draw_list->_CmdHeader);  // Copy ClipRect, TextureId, VtxOffset
  else if (AnchorDrawCmd_HeaderCompare(curr_cmd, &draw_list->_CmdHeader) != 0)
    draw_list->AddDrawCmd();

  _Count = 1;
}

void AnchorDrawListSplitter::SetCurrentChannel(AnchorDrawList *draw_list, int idx)
{
  ANCHOR_ASSERT(idx >= 0 && idx < _Count);
  if (_Current == idx)
    return;

  // Overwrite AnchorVector (12/16 bytes), four times. This is merely a silly optimization instead
  // of doing .swap()
  memcpy(&_Channels.Data[_Current]._CmdBuffer,
         &draw_list->CmdBuffer,
         sizeof(draw_list->CmdBuffer));
  memcpy(&_Channels.Data[_Current]._IdxBuffer,
         &draw_list->IdxBuffer,
         sizeof(draw_list->IdxBuffer));
  _Current = idx;
  memcpy(&draw_list->CmdBuffer, &_Channels.Data[idx]._CmdBuffer, sizeof(draw_list->CmdBuffer));
  memcpy(&draw_list->IdxBuffer, &_Channels.Data[idx]._IdxBuffer, sizeof(draw_list->IdxBuffer));
  draw_list->_IdxWritePtr = draw_list->IdxBuffer.Data + draw_list->IdxBuffer.Size;

  // If current command is used with different settings we need to add a new command
  AnchorDrawCmd *curr_cmd = (draw_list->CmdBuffer.Size == 0) ?
                              NULL :
                              &draw_list->CmdBuffer.Data[draw_list->CmdBuffer.Size - 1];
  if (curr_cmd == NULL)
    draw_list->AddDrawCmd();
  else if (curr_cmd->ElemCount == 0)
    AnchorDrawCmd_HeaderCopy(curr_cmd,
                             &draw_list->_CmdHeader);  // Copy ClipRect, TextureId, VtxOffset
  else if (AnchorDrawCmd_HeaderCompare(curr_cmd, &draw_list->_CmdHeader) != 0)
    draw_list->AddDrawCmd();
}

//-----------------------------------------------------------------------------
// [SECTION] AnchorDrawData
//-----------------------------------------------------------------------------

// For backward compatibility: convert all buffers from indexed to de-indexed, in case you cannot
// render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed
// rendering!
void AnchorDrawData::DeIndexAllBuffers()
{
  AnchorVector<AnchorDrawVert> new_vtx_buffer;
  TotalVtxCount = TotalIdxCount = 0;
  for (int i = 0; i < CmdListsCount; i++) {
    AnchorDrawList *cmd_list = CmdLists[i];
    if (cmd_list->IdxBuffer.empty())
      continue;
    new_vtx_buffer.resize(cmd_list->IdxBuffer.Size);
    for (int j = 0; j < cmd_list->IdxBuffer.Size; j++)
      new_vtx_buffer[j] = cmd_list->VtxBuffer[cmd_list->IdxBuffer[j]];
    cmd_list->VtxBuffer.swap(new_vtx_buffer);
    cmd_list->IdxBuffer.resize(0);
    TotalVtxCount += cmd_list->VtxBuffer.Size;
  }
}

// Helper to scale the ClipRect field of each AnchorDrawCmd.
// Use if your final output buffer is at a different scale than draw_data->DisplaySize,
// or if there is a difference between your window resolution and framebuffer resolution.
void AnchorDrawData::ScaleClipRects(const GfVec2f &fb_scale)
{
  for (int i = 0; i < CmdListsCount; i++) {
    AnchorDrawList *cmd_list = CmdLists[i];
    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
      AnchorDrawCmd *cmd = &cmd_list->CmdBuffer[cmd_i];
      cmd->ClipRect = GfVec4f(cmd->ClipRect[0] * fb_scale[0],
                              cmd->ClipRect[1] * fb_scale[1],
                              cmd->ClipRect[2] * fb_scale[0],
                              cmd->ClipRect[3] * fb_scale[1]);
    }
  }
}

//-----------------------------------------------------------------------------
// [SECTION] Helpers ShadeVertsXXX functions
//-----------------------------------------------------------------------------

// Generic linear color gradient, write to RGB fields, leave A untouched.
void ANCHOR::ShadeVertsLinearColorGradientKeepAlpha(AnchorDrawList *draw_list,
                                                    int vert_start_idx,
                                                    int vert_end_idx,
                                                    GfVec2f gradient_p0,
                                                    GfVec2f gradient_p1,
                                                    AnchorU32 col0,
                                                    AnchorU32 col1)
{
  GfVec2f gradient_extent = gradient_p1 - gradient_p0;
  float gradient_inv_length2 = 1.0f / AnchorLengthSqr(gradient_extent);
  AnchorDrawVert *vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
  AnchorDrawVert *vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
  const int col0_r = (int)(col0 >> ANCHOR_COL32_R_SHIFT) & 0xFF;
  const int col0_g = (int)(col0 >> ANCHOR_COL32_G_SHIFT) & 0xFF;
  const int col0_b = (int)(col0 >> ANCHOR_COL32_B_SHIFT) & 0xFF;
  const int col_delta_r = ((int)(col1 >> ANCHOR_COL32_R_SHIFT) & 0xFF) - col0_r;
  const int col_delta_g = ((int)(col1 >> ANCHOR_COL32_G_SHIFT) & 0xFF) - col0_g;
  const int col_delta_b = ((int)(col1 >> ANCHOR_COL32_B_SHIFT) & 0xFF) - col0_b;
  for (AnchorDrawVert *vert = vert_start; vert < vert_end; vert++) {
    float d = AnchorDot(vert->pos - gradient_p0, gradient_extent);
    float t = AnchorClamp(d * gradient_inv_length2, 0.0f, 1.0f);
    int r = (int)(col0_r + col_delta_r * t);
    int g = (int)(col0_g + col_delta_g * t);
    int b = (int)(col0_b + col_delta_b * t);
    vert->col = (r << ANCHOR_COL32_R_SHIFT) | (g << ANCHOR_COL32_G_SHIFT) |
                (b << ANCHOR_COL32_B_SHIFT) | (vert->col & ANCHOR_COL32_A_MASK);
  }
}

// Distribute UV over (a, b) rectangle
void ANCHOR::ShadeVertsLinearUV(AnchorDrawList *draw_list,
                                int vert_start_idx,
                                int vert_end_idx,
                                const GfVec2f &a,
                                const GfVec2f &b,
                                const GfVec2f &uv_a,
                                const GfVec2f &uv_b,
                                bool clamp)
{
  const GfVec2f size = b - a;
  const GfVec2f uv_size = uv_b - uv_a;
  const GfVec2f scale = GfVec2f(size[0] != 0.0f ? (uv_size[0] / size[0]) : 0.0f,
                                size[1] != 0.0f ? (uv_size[1] / size[1]) : 0.0f);

  AnchorDrawVert *vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
  AnchorDrawVert *vert_end = draw_list->VtxBuffer.Data + vert_end_idx;
  if (clamp) {
    const GfVec2f min = AnchorMin(uv_a, uv_b);
    const GfVec2f max = AnchorMax(uv_a, uv_b);
    for (AnchorDrawVert *vertex = vert_start; vertex < vert_end; ++vertex)
      vertex->uv = AnchorClamp(uv_a +
                                 AnchorMul(GfVec2f(vertex->pos[0], vertex->pos[1]) - a, scale),
                               min,
                               max);
  } else {
    for (AnchorDrawVert *vertex = vert_start; vertex < vert_end; ++vertex)
      vertex->uv = uv_a + AnchorMul(GfVec2f(vertex->pos[0], vertex->pos[1]) - a, scale);
  }
}

//-----------------------------------------------------------------------------
// [SECTION] AnchorFontConfig
//-----------------------------------------------------------------------------

AnchorFontConfig::AnchorFontConfig()
{
  memset(this, 0, sizeof(*this));
  FontDataOwnedByAtlas = true;
  OversampleH = 3;  // FIXME: 2 may be a better default?
  OversampleV = 1;
  GlyphMaxAdvanceX = FLT_MAX;
  RasterizerMultiply = 1.0f;
  EllipsisChar = (AnchorWChar)-1;
}

//-----------------------------------------------------------------------------
// [SECTION] AnchorFontAtlas
//-----------------------------------------------------------------------------

// A work of art lies ahead! (. = white layer, X = black layer, others are blank)
// The 2x2 white texels on the top left are the ones we'll use everywhere in ANCHOR to render
// filled shapes.
const int FONT_ATLAS_DEFAULT_TEX_DATA_W = 108;  // Actual texture will be 2 times that + 1 spacing.
const int FONT_ATLAS_DEFAULT_TEX_DATA_H = 27;
static const char FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[FONT_ATLAS_DEFAULT_TEX_DATA_W *
                                                       FONT_ATLAS_DEFAULT_TEX_DATA_H +
                                                     1] = {
  "..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX-  "
  "   XX          "
  "..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X-  "
  "  X..X         "
  "---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X-  "
  "  X..X         "
  "X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X-  "
  "  X..X         "
  "XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X-  "
  "  X..X         "
  "X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X-  "
  "  X..XXX       "
  "X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX-  "
  "  X..X..XXX    "
  "X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      -  "
  "  X..X..X..XX  "
  "X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       -  "
  "  X..X..X..X.X "
  "X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        "
  "-XXX X..X..X..X..X"
  "X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         "
  "-X..XX........X..X"
  "X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          "
  "-X...X...........X"
  "X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           - "
  "X..............X"
  "X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            -  "
  "X.............X"
  "X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           -  "
  "X.............X"
  "X......XXXXX-XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          -  "
  " X............X"
  "X...X..X    ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          -  "
  " X...........X "
  "X..X X..X   -       -XXXX.XXXX-       XXXX.XXXX       -------------------------------------  "
  "  X..........X "
  "X.X  X..X   -       -X.......X-       X.......X       -    XX           XX    -           -  "
  "  X..........X "
  "XX    X..X  -       - X.....X -        X.....X        -   X.X           X.X   -           -  "
  "   X........X  "
  "      X..X          -  X...X  -         X...X         -  X..X           X..X  -           -  "
  "   X........X  "
  "       XX           -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           -  "
  "   XXXXXXXXXX  "
  "------------        -    X    -           X           -X.....................X-           "
  "------------------"
  "                    ----------------------------------- X...XXXXXXXXXXXXX...X -              "
  "               "
  "                                                      -  X..X           X..X  -              "
  "               "
  "                                                      -   X.X           X.X   -              "
  "               "
  "                                                      -    XX           XX    -              "
  "               "};

static const GfVec2f FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[ANCHOR_StandardCursorNumCursors][3] = {
  // Pos ........ Size ......... Offset ......
  {GfVec2f(0,  3),  GfVec2f(12, 19), GfVec2f(0,  0) }, // ANCHOR_StandardCursorDefault
  {GfVec2f(13, 0),  GfVec2f(7,  16), GfVec2f(1,  8) }, // ANCHOR_StandardCursorText
  {GfVec2f(31, 0),  GfVec2f(23, 23), GfVec2f(11, 11)}, // ANCHOR_StandardCursorNSEWScroll
  {GfVec2f(21, 0),  GfVec2f(9,  23), GfVec2f(4,  11)}, // ANCHOR_StandardCursorNSScroll
  {GfVec2f(55, 18), GfVec2f(23, 9),  GfVec2f(11, 4) }, // ANCHOR_StandardCursorEWScroll
  {GfVec2f(73, 0),  GfVec2f(17, 17), GfVec2f(8,  8) }, // ANCHOR_StandardCursorBottomLeftCorner
  {GfVec2f(55, 0),  GfVec2f(17, 17), GfVec2f(8,  8) }, // ANCHOR_StandardCursorBottomRightCorner
  {GfVec2f(91, 0),  GfVec2f(17, 22), GfVec2f(5,  0) }, // ANCHOR_StandardCursorMove
};

AnchorFontAtlas::AnchorFontAtlas()
{
  memset(this, 0, sizeof(*this));
  TexGlyphPadding = 1;
  PackIdMouseCursors = PackIdLines = -1;
}

AnchorFontAtlas::~AnchorFontAtlas()
{
  ANCHOR_ASSERT(
    !Locked && "Cannot modify a locked AnchorFontAtlas between NewFrame() and EndFrame/Render()!");
  Clear();
}

void AnchorFontAtlas::ClearInputData()
{
  ANCHOR_ASSERT(
    !Locked && "Cannot modify a locked AnchorFontAtlas between NewFrame() and EndFrame/Render()!");
  for (int i = 0; i < ConfigData.Size; i++)
    if (ConfigData[i].FontData && ConfigData[i].FontDataOwnedByAtlas) {
      ANCHOR_FREE(ConfigData[i].FontData);
      ConfigData[i].FontData = NULL;
    }

  // When clearing this we lose access to the font name and other information used to build the
  // font.
  for (int i = 0; i < Fonts.Size; i++)
    if (Fonts[i]->ConfigData >= ConfigData.Data &&
        Fonts[i]->ConfigData < ConfigData.Data + ConfigData.Size) {
      Fonts[i]->ConfigData = NULL;
      Fonts[i]->ConfigDataCount = 0;
    }
  ConfigData.clear();
  CustomRects.clear();
  PackIdMouseCursors = PackIdLines = -1;
}

void AnchorFontAtlas::ClearTexData()
{
  ANCHOR_ASSERT(
    !Locked && "Cannot modify a locked AnchorFontAtlas between NewFrame() and EndFrame/Render()!");
  if (TexPixelsAlpha8)
    ANCHOR_FREE(TexPixelsAlpha8);
  if (TexPixelsRGBA32)
    ANCHOR_FREE(TexPixelsRGBA32);
  TexPixelsAlpha8 = NULL;
  TexPixelsRGBA32 = NULL;
  TexPixelsUseColors = false;
}

void AnchorFontAtlas::ClearFonts()
{
  ANCHOR_ASSERT(
    !Locked && "Cannot modify a locked AnchorFontAtlas between NewFrame() and EndFrame/Render()!");
  for (int i = 0; i < Fonts.Size; i++)
    ANCHOR_DELETE(Fonts[i]);
  Fonts.clear();
}

void AnchorFontAtlas::Clear()
{
  ClearInputData();
  ClearTexData();
  ClearFonts();
}

void AnchorFontAtlas::GetTexDataAsAlpha8(unsigned char **out_pixels,
                                         int *out_width,
                                         int *out_height,
                                         int *out_bytes_per_pixel)
{
  // Build atlas on demand
  if (TexPixelsAlpha8 == NULL) {
    if (ConfigData.empty())
      AddFontDefault();
    Build();
  }

  *out_pixels = TexPixelsAlpha8;
  if (out_width)
    *out_width = TexWidth;
  if (out_height)
    *out_height = TexHeight;
  if (out_bytes_per_pixel)
    *out_bytes_per_pixel = 1;
}

void AnchorFontAtlas::GetTexDataAsRGBA32(unsigned char **out_pixels,
                                         int *out_width,
                                         int *out_height,
                                         int *out_bytes_per_pixel)
{
  // Convert to RGBA32 format on demand
  // Although it is likely to be the most commonly used format, our font rendering is 1 channel / 8
  // bpp
  if (!TexPixelsRGBA32) {
    unsigned char *pixels = NULL;
    GetTexDataAsAlpha8(&pixels, NULL, NULL);
    if (pixels) {
      TexPixelsRGBA32 = (unsigned int *)ANCHOR_ALLOC((size_t)TexWidth * (size_t)TexHeight * 4);
      const unsigned char *src = pixels;
      unsigned int *dst = TexPixelsRGBA32;
      for (int n = TexWidth * TexHeight; n > 0; n--)
        *dst++ = ANCHOR_COL32(255, 255, 255, (unsigned int)(*src++));
    }
  }

  *out_pixels = (unsigned char *)TexPixelsRGBA32;
  if (out_width)
    *out_width = TexWidth;
  if (out_height)
    *out_height = TexHeight;
  if (out_bytes_per_pixel)
    *out_bytes_per_pixel = 4;
}

AnchorFont *AnchorFontAtlas::AddFont(const AnchorFontConfig *font_cfg)
{
  ANCHOR_ASSERT(
    !Locked && "Cannot modify a locked AnchorFontAtlas between NewFrame() and EndFrame/Render()!");
  ANCHOR_ASSERT(font_cfg->FontData != NULL && font_cfg->FontDataSize > 0);
  ANCHOR_ASSERT(font_cfg->SizePixels > 0.0f);

  // Create new font
  if (!font_cfg->MergeMode)
    Fonts.push_back(ANCHOR_NEW(AnchorFont));
  else
    ANCHOR_ASSERT(
      !Fonts.empty() &&
      "Cannot use MergeMode for the first font");  // When using MergeMode make sure that a font
                                                   // has already been added before. You can use
                                                   // ANCHOR::GetIO().Fonts->AddFontDefault() to
                                                   // add the default ANCHOR font.

  ConfigData.push_back(*font_cfg);
  AnchorFontConfig &new_font_cfg = ConfigData.back();
  if (new_font_cfg.DstFont == NULL)
    new_font_cfg.DstFont = Fonts.back();
  if (!new_font_cfg.FontDataOwnedByAtlas) {
    new_font_cfg.FontData = ANCHOR_ALLOC(new_font_cfg.FontDataSize);
    new_font_cfg.FontDataOwnedByAtlas = true;
    memcpy(new_font_cfg.FontData, font_cfg->FontData, (size_t)new_font_cfg.FontDataSize);
  }

  if (new_font_cfg.DstFont->EllipsisChar == (AnchorWChar)-1)
    new_font_cfg.DstFont->EllipsisChar = font_cfg->EllipsisChar;

  // Invalidate texture
  ClearTexData();
  return new_font_cfg.DstFont;
}

// Default font TTF is compressed with stb_compress then base85 encoded (see
// misc/fonts/binary_to_compressed_c.cpp for encoder)
static unsigned int stb_decompress_length(const unsigned char *input);
static unsigned int stb_decompress(unsigned char *output,
                                   const unsigned char *input,
                                   unsigned int length);
static const char *GetDefaultCompressedFontDataTTFBase85();
static unsigned int Decode85Byte(char c)
{
  return c >= '\\' ? c - 36 : c - 35;
}
static void Decode85(const unsigned char *src, unsigned char *dst)
{
  while (*src) {
    unsigned int tmp = Decode85Byte(src[0]) +
                       85 * (Decode85Byte(src[1]) +
                             85 * (Decode85Byte(src[2]) +
                                   85 * (Decode85Byte(src[3]) + 85 * Decode85Byte(src[4]))));
    dst[0] = ((tmp >> 0) & 0xFF);
    dst[1] = ((tmp >> 8) & 0xFF);
    dst[2] = ((tmp >> 16) & 0xFF);
    dst[3] = ((tmp >> 24) & 0xFF);  // We can't assume little-endianness.
    src += 5;
    dst += 4;
  }
}

// Load embedded ProggyClean.ttf at size 13, disable oversampling
AnchorFont *AnchorFontAtlas::AddFontDefault(const AnchorFontConfig *font_cfg_template)
{
  AnchorFontConfig font_cfg = font_cfg_template ? *font_cfg_template : AnchorFontConfig();
  if (!font_cfg_template) {
    font_cfg.OversampleH = font_cfg.OversampleV = 1;
    font_cfg.PixelSnapH = true;
  }
  if (font_cfg.SizePixels <= 0.0f)
    font_cfg.SizePixels = 13.0f * 1.0f;
  if (font_cfg.Name[0] == '\0')
    AnchorFormatString(font_cfg.Name,
                       ANCHOR_ARRAYSIZE(font_cfg.Name),
                       "ProggyClean.ttf, %dpx",
                       (int)font_cfg.SizePixels);
  font_cfg.EllipsisChar = (AnchorWChar)0x0085;
  font_cfg.GlyphOffset[1] = 1.0f * ANCHOR_FLOOR(font_cfg.SizePixels /
                                                13.0f);  // Add +1 offset per 13 units

  const char *ttf_compressed_base85 = GetDefaultCompressedFontDataTTFBase85();
  const AnchorWChar *glyph_ranges = font_cfg.GlyphRanges != NULL ? font_cfg.GlyphRanges :
                                                                   GetGlyphRangesDefault();
  AnchorFont *font = AddFontFromMemoryCompressedBase85TTF(ttf_compressed_base85,
                                                          font_cfg.SizePixels,
                                                          &font_cfg,
                                                          glyph_ranges);
  return font;
}

AnchorFont *AnchorFontAtlas::AddFontFromFileTTF(const char *filename,
                                                float size_pixels,
                                                const AnchorFontConfig *font_cfg_template,
                                                const AnchorWChar *glyph_ranges)
{
  ANCHOR_ASSERT(
    !Locked && "Cannot modify a locked AnchorFontAtlas between NewFrame() and EndFrame/Render()!");
  size_t data_size = 0;
  void *data = ImFileLoadToMemory(filename, "rb", &data_size, 0);
  if (!data) {
    ANCHOR_ASSERT_USER_ERROR(0, "Could not load font file!");
    return NULL;
  }
  AnchorFontConfig font_cfg = font_cfg_template ? *font_cfg_template : AnchorFontConfig();
  if (font_cfg.Name[0] == '\0') {
    // Store a short copy of filename into into the font name for convenience
    const char *p;
    for (p = filename + strlen(filename); p > filename && p[-1] != '/' && p[-1] != '\\'; p--) {
    }
    AnchorFormatString(font_cfg.Name,
                       ANCHOR_ARRAYSIZE(font_cfg.Name),
                       "%s, %.0fpx",
                       p,
                       size_pixels);
  }
  return AddFontFromMemoryTTF(data, (int)data_size, size_pixels, &font_cfg, glyph_ranges);
}

// NB: Transfer ownership of 'ttf_data' to AnchorFontAtlas, unless
// font_cfg_template->FontDataOwnedByAtlas == false. Owned TTF buffer will be deleted after
// Build().
AnchorFont *AnchorFontAtlas::AddFontFromMemoryTTF(void *ttf_data,
                                                  int ttf_size,
                                                  float size_pixels,
                                                  const AnchorFontConfig *font_cfg_template,
                                                  const AnchorWChar *glyph_ranges)
{
  ANCHOR_ASSERT(
    !Locked && "Cannot modify a locked AnchorFontAtlas between NewFrame() and EndFrame/Render()!");
  AnchorFontConfig font_cfg = font_cfg_template ? *font_cfg_template : AnchorFontConfig();
  ANCHOR_ASSERT(font_cfg.FontData == NULL);
  font_cfg.FontData = ttf_data;
  font_cfg.FontDataSize = ttf_size;
  font_cfg.SizePixels = size_pixels;
  if (glyph_ranges)
    font_cfg.GlyphRanges = glyph_ranges;
  return AddFont(&font_cfg);
}

AnchorFont *AnchorFontAtlas::AddFontFromMemoryCompressedTTF(
  const void *compressed_ttf_data,
  int compressed_ttf_size,
  float size_pixels,
  const AnchorFontConfig *font_cfg_template,
  const AnchorWChar *glyph_ranges)
{
  const unsigned int buf_decompressed_size = stb_decompress_length(
    (const unsigned char *)compressed_ttf_data);
  unsigned char *buf_decompressed_data = (unsigned char *)ANCHOR_ALLOC(buf_decompressed_size);
  stb_decompress(buf_decompressed_data,
                 (const unsigned char *)compressed_ttf_data,
                 (unsigned int)compressed_ttf_size);

  AnchorFontConfig font_cfg = font_cfg_template ? *font_cfg_template : AnchorFontConfig();
  ANCHOR_ASSERT(font_cfg.FontData == NULL);
  font_cfg.FontDataOwnedByAtlas = true;
  return AddFontFromMemoryTTF(buf_decompressed_data,
                              (int)buf_decompressed_size,
                              size_pixels,
                              &font_cfg,
                              glyph_ranges);
}

AnchorFont *AnchorFontAtlas::AddFontFromMemoryCompressedBase85TTF(
  const char *compressed_ttf_data_base85,
  float size_pixels,
  const AnchorFontConfig *font_cfg,
  const AnchorWChar *glyph_ranges)
{
  int compressed_ttf_size = (((int)strlen(compressed_ttf_data_base85) + 4) / 5) * 4;
  void *compressed_ttf = ANCHOR_ALLOC((size_t)compressed_ttf_size);
  Decode85((const unsigned char *)compressed_ttf_data_base85, (unsigned char *)compressed_ttf);
  AnchorFont *font = AddFontFromMemoryCompressedTTF(compressed_ttf,
                                                    compressed_ttf_size,
                                                    size_pixels,
                                                    font_cfg,
                                                    glyph_ranges);
  ANCHOR_FREE(compressed_ttf);
  return font;
}

int AnchorFontAtlas::AddCustomRectRegular(int width, int height)
{
  ANCHOR_ASSERT(width > 0 && width <= 0xFFFF);
  ANCHOR_ASSERT(height > 0 && height <= 0xFFFF);
  AnchorFontAtlasCustomRect r;
  r.Width = (unsigned short)width;
  r.Height = (unsigned short)height;
  CustomRects.push_back(r);
  return CustomRects.Size - 1;  // Return index
}

int AnchorFontAtlas::AddCustomRectFontGlyph(AnchorFont *font,
                                            AnchorWChar id,
                                            int width,
                                            int height,
                                            float advance_x,
                                            const GfVec2f &offset)
{
#ifdef ANCHOR_USE_WCHAR32
  ANCHOR_ASSERT(id <= IM_UNICODE_CODEPOINT_MAX);
#endif
  ANCHOR_ASSERT(font != NULL);
  ANCHOR_ASSERT(width > 0 && width <= 0xFFFF);
  ANCHOR_ASSERT(height > 0 && height <= 0xFFFF);
  AnchorFontAtlasCustomRect r;
  r.Width = (unsigned short)width;
  r.Height = (unsigned short)height;
  r.GlyphID = id;
  r.GlyphAdvanceX = advance_x;
  r.GlyphOffset = offset;
  r.Font = font;
  CustomRects.push_back(r);
  return CustomRects.Size - 1;  // Return index
}

void AnchorFontAtlas::CalcCustomRectUV(const AnchorFontAtlasCustomRect *rect,
                                       GfVec2f *out_uv_min,
                                       GfVec2f *out_uv_max) const
{
  ANCHOR_ASSERT(TexWidth > 0 &&
                TexHeight >
                  0);  // Font atlas needs to be built before we can calculate UV coordinates
  ANCHOR_ASSERT(rect->IsPacked());  // Make sure the rectangle has been packed
  *out_uv_min = GfVec2f((float)rect->X * TexUvScale[0], (float)rect->Y * TexUvScale[1]);
  *out_uv_max = GfVec2f((float)(rect->X + rect->Width) * TexUvScale[0],
                        (float)(rect->Y + rect->Height) * TexUvScale[1]);
}

bool AnchorFontAtlas::GetMouseCursorTexData(AnchorMouseCursor cursor_type,
                                            GfVec2f *out_offset,
                                            GfVec2f *out_size,
                                            GfVec2f out_uv_border[2],
                                            GfVec2f out_uv_fill[2])
{
  if (cursor_type <= ANCHOR_StandardCursorNone || cursor_type >= ANCHOR_StandardCursorNumCursors)
    return false;
  if (Flags & AnchorFontAtlasFlags_NoMouseCursors)
    return false;

  ANCHOR_ASSERT(PackIdMouseCursors != -1);
  AnchorFontAtlasCustomRect *r = GetCustomRectByIndex(PackIdMouseCursors);
  GfVec2f pos = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][0] +
                GfVec2f((float)r->X, (float)r->Y);
  GfVec2f size = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][1];
  *out_size = size;
  *out_offset = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[cursor_type][2];

  /** todo::check_math */
  out_uv_border[0] = GfVec2f((pos[0]) * TexUvScale[0], (pos[1]) * TexUvScale[1]);
  out_uv_border[1] = GfVec2f((pos[0] + size[0]) * TexUvScale[0],
                             (pos[1] + size[1]) * TexUvScale[1]);
  pos[0] += FONT_ATLAS_DEFAULT_TEX_DATA_W + 1;
  out_uv_fill[0] = GfVec2f((pos[0]) * TexUvScale[0], (pos[1]) * TexUvScale[1]);
  out_uv_fill[1] = GfVec2f((pos[0] + size[0]) * TexUvScale[0], (pos[1] + size[1]) * TexUvScale[1]);
  return true;
}

bool AnchorFontAtlas::Build()
{
  ANCHOR_ASSERT(
    !Locked && "Cannot modify a locked AnchorFontAtlas between NewFrame() and EndFrame/Render()!");

  // Select builder
  // - Note that we do not reassign to atlas->FontBuilderIO, since it is likely to point to static
  // data which
  //   may mess with some hot-reloading schemes. If you need to assign to this (for dynamic
  //   selection) AND are using a hot-reloading scheme that messes up static data, store your own
  //   instance of AnchorFontBuilderIO somewhere and point to it instead of pointing directly to
  //   return value of the GetBuilderXXX functions.
  const AnchorFontBuilderIO *builder_io = FontBuilderIO;
  if (builder_io == NULL) {
    builder_io = AnchorFreeType::GetBuilderForFreeType();
    // builder_io = AnchorFontAtlasGetBuilderForStbTruetype();
  }

  // Build
  return builder_io->FontBuilder_Build(this);
}

void AnchorFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256],
                                                 float in_brighten_factor)
{
  for (unsigned int i = 0; i < 256; i++) {
    unsigned int value = (unsigned int)(i * in_brighten_factor);
    out_table[i] = value > 255 ? 255 : (value & 0xFF);
  }
}

void AnchorFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256],
                                            unsigned char *pixels,
                                            int x,
                                            int y,
                                            int w,
                                            int h,
                                            int stride)
{
  unsigned char *data = pixels + x + y * stride;
  for (int j = h; j > 0; j--, data += stride)
    for (int i = 0; i < w; i++)
      data[i] = table[data[i]];
}

// Temporary data for one source font (multiple source fonts can be merged into one destination
// AnchorFont) (C++03 doesn't allow instancing AnchorVector<> with function-local types so we
// declare the type here.)
struct AnchorFontBuildSrcData
{
  stbtt_fontinfo FontInfo;
  stbtt_pack_range
    PackRange;  // Hold the list of codepoints to pack (essentially points to Codepoints.Data)
  stbrp_rect *Rects;  // Rectangle to pack. We first fill in their size and the packer will give us
                      // their position.
  stbtt_packedchar *PackedChars;  // Output glyphs
  const AnchorWChar *SrcRanges;   // Ranges as requested by user (user is allowed to request too
                                  // much, e.g. 0x0020..0xFFFF)
  int DstIndex;                   // Index into atlas->Fonts[] and dst_tmp_array[]
  int GlyphsHighest;              // Highest requested codepoint
  int GlyphsCount;  // Glyph count (excluding missing glyphs and glyphs already set by an earlier
                    // source font)
  AnchorBitVector GlyphsSet;  // Glyph bit map (random access, 1-bit per codepoint. This will be a
                              // maximum of 8KB)
  AnchorVector<int> GlyphsList;  // Glyph codepoints list (flattened version of GlyphsMap)
};

// Temporary data for one destination AnchorFont* (multiple source fonts can be merged into one
// destination AnchorFont)
struct AnchorFontBuildDstData
{
  int SrcCount;  // Number of source fonts targeting this destination font.
  int GlyphsHighest;
  int GlyphsCount;
  AnchorBitVector GlyphsSet;  // This is used to resolve collision when multiple sources are merged
                              // into a same destination font.
};

static void UnpackBitVectorToFlatIndexList(const AnchorBitVector *in, AnchorVector<int> *out)
{
  ANCHOR_ASSERT(sizeof(in->Storage.Data[0]) == sizeof(int));
  const AnchorU32 *it_begin = in->Storage.begin();
  const AnchorU32 *it_end = in->Storage.end();
  for (const AnchorU32 *it = it_begin; it < it_end; it++)
    if (AnchorU32 entries_32 = *it)
      for (AnchorU32 bit_n = 0; bit_n < 32; bit_n++)
        if (entries_32 & ((AnchorU32)1 << bit_n))
          out->push_back((int)(((it - it_begin) << 5) + bit_n));
}

static bool AnchorFontAtlasBuildWithStbTruetype(AnchorFontAtlas *atlas)
{
  ANCHOR_ASSERT(atlas->ConfigData.Size > 0);

  AnchorFontAtlasBuildInit(atlas);

  // Clear atlas
  atlas->TexID = (AnchorTextureID)NULL;
  atlas->TexWidth = atlas->TexHeight = 0;
  atlas->TexUvScale = GfVec2f(0.0f, 0.0f);
  atlas->TexUvWhitePixel = GfVec2f(0.0f, 0.0f);
  atlas->ClearTexData();

  // Temporary storage for building
  AnchorVector<AnchorFontBuildSrcData> src_tmp_array;
  AnchorVector<AnchorFontBuildDstData> dst_tmp_array;
  src_tmp_array.resize(atlas->ConfigData.Size);
  dst_tmp_array.resize(atlas->Fonts.Size);
  memset(src_tmp_array.Data, 0, (size_t)src_tmp_array.size_in_bytes());
  memset(dst_tmp_array.Data, 0, (size_t)dst_tmp_array.size_in_bytes());

  // 1. Initialize font loading structure, check font data validity
  for (int src_i = 0; src_i < atlas->ConfigData.Size; src_i++) {
    AnchorFontBuildSrcData &src_tmp = src_tmp_array[src_i];
    AnchorFontConfig &cfg = atlas->ConfigData[src_i];
    ANCHOR_ASSERT(cfg.DstFont &&
                  (!cfg.DstFont->IsLoaded() || cfg.DstFont->ContainerAtlas == atlas));

    // Find index from cfg.DstFont (we allow the user to set cfg.DstFont. Also it makes casual
    // debugging nicer than when storing indices)
    src_tmp.DstIndex = -1;
    for (int output_i = 0; output_i < atlas->Fonts.Size && src_tmp.DstIndex == -1; output_i++)
      if (cfg.DstFont == atlas->Fonts[output_i])
        src_tmp.DstIndex = output_i;
    if (src_tmp.DstIndex == -1) {
      ANCHOR_ASSERT(src_tmp.DstIndex !=
                    -1);  // cfg.DstFont not pointing within atlas->Fonts[] array?
      return false;
    }
    // Initialize helper structure for font loading and verify that the TTF/OTF data is correct
    const int font_offset = stbtt_GetFontOffsetForIndex((unsigned char *)cfg.FontData, cfg.FontNo);
    ANCHOR_ASSERT(font_offset >= 0 && "FontData is incorrect, or FontNo cannot be found.");
    if (!stbtt_InitFont(&src_tmp.FontInfo, (unsigned char *)cfg.FontData, font_offset))
      return false;

    // Measure highest codepoints
    AnchorFontBuildDstData &dst_tmp = dst_tmp_array[src_tmp.DstIndex];
    src_tmp.SrcRanges = cfg.GlyphRanges ? cfg.GlyphRanges : atlas->GetGlyphRangesDefault();
    for (const AnchorWChar *src_range = src_tmp.SrcRanges; src_range[0] && src_range[1];
         src_range += 2)
      src_tmp.GlyphsHighest = AnchorMax(src_tmp.GlyphsHighest, (int)src_range[1]);
    dst_tmp.SrcCount++;
    dst_tmp.GlyphsHighest = AnchorMax(dst_tmp.GlyphsHighest, src_tmp.GlyphsHighest);
  }

  // 2. For every requested codepoint, check for their presence in the font data, and handle
  // redundancy or overlaps between source fonts to avoid unused glyphs.
  int total_glyphs_count = 0;
  for (int src_i = 0; src_i < src_tmp_array.Size; src_i++) {
    AnchorFontBuildSrcData &src_tmp = src_tmp_array[src_i];
    AnchorFontBuildDstData &dst_tmp = dst_tmp_array[src_tmp.DstIndex];
    src_tmp.GlyphsSet.Create(src_tmp.GlyphsHighest + 1);
    if (dst_tmp.GlyphsSet.Storage.empty())
      dst_tmp.GlyphsSet.Create(dst_tmp.GlyphsHighest + 1);

    for (const AnchorWChar *src_range = src_tmp.SrcRanges; src_range[0] && src_range[1];
         src_range += 2)
      for (unsigned int codepoint = src_range[0]; codepoint <= src_range[1]; codepoint++) {
        if (dst_tmp.GlyphsSet.TestBit(
              codepoint))  // Don't overwrite existing glyphs. We could make this an
                           // option for MergeMode (e.g. MergeOverwrite==true)
          continue;
        if (!stbtt_FindGlyphIndex(&src_tmp.FontInfo, codepoint))  // It is actually in the font?
          continue;

        // Add to avail set/counters
        src_tmp.GlyphsCount++;
        dst_tmp.GlyphsCount++;
        src_tmp.GlyphsSet.SetBit(codepoint);
        dst_tmp.GlyphsSet.SetBit(codepoint);
        total_glyphs_count++;
      }
  }

  // 3. Unpack our bit map into a flat list (we now have all the Unicode points that we know are
  // requested _and_ available _and_ not overlapping another)
  for (int src_i = 0; src_i < src_tmp_array.Size; src_i++) {
    AnchorFontBuildSrcData &src_tmp = src_tmp_array[src_i];
    src_tmp.GlyphsList.reserve(src_tmp.GlyphsCount);
    UnpackBitVectorToFlatIndexList(&src_tmp.GlyphsSet, &src_tmp.GlyphsList);
    src_tmp.GlyphsSet.Clear();
    ANCHOR_ASSERT(src_tmp.GlyphsList.Size == src_tmp.GlyphsCount);
  }
  for (int dst_i = 0; dst_i < dst_tmp_array.Size; dst_i++)
    dst_tmp_array[dst_i].GlyphsSet.Clear();
  dst_tmp_array.clear();

  // Allocate packing character data and flag packed characters buffer as non-packed
  // (x0=y0=x1=y1=0) (We technically don't need to zero-clear buf_rects, but let's do it for the
  // sake of sanity)
  AnchorVector<stbrp_rect> buf_rects;
  AnchorVector<stbtt_packedchar> buf_packedchars;
  buf_rects.resize(total_glyphs_count);
  buf_packedchars.resize(total_glyphs_count);
  memset(buf_rects.Data, 0, (size_t)buf_rects.size_in_bytes());
  memset(buf_packedchars.Data, 0, (size_t)buf_packedchars.size_in_bytes());

  // 4. Gather glyphs sizes so we can pack them in our virtual canvas.
  int total_surface = 0;
  int buf_rects_out_n = 0;
  int buf_packedchars_out_n = 0;
  for (int src_i = 0; src_i < src_tmp_array.Size; src_i++) {
    AnchorFontBuildSrcData &src_tmp = src_tmp_array[src_i];
    if (src_tmp.GlyphsCount == 0)
      continue;

    src_tmp.Rects = &buf_rects[buf_rects_out_n];
    src_tmp.PackedChars = &buf_packedchars[buf_packedchars_out_n];
    buf_rects_out_n += src_tmp.GlyphsCount;
    buf_packedchars_out_n += src_tmp.GlyphsCount;

    // Convert our ranges in the format stb_truetype wants
    AnchorFontConfig &cfg = atlas->ConfigData[src_i];
    src_tmp.PackRange.font_size = cfg.SizePixels;
    src_tmp.PackRange.first_unicode_codepoint_in_range = 0;
    src_tmp.PackRange.array_of_unicode_codepoints = src_tmp.GlyphsList.Data;
    src_tmp.PackRange.num_chars = src_tmp.GlyphsList.Size;
    src_tmp.PackRange.chardata_for_range = src_tmp.PackedChars;
    src_tmp.PackRange.h_oversample = (unsigned char)cfg.OversampleH;
    src_tmp.PackRange.v_oversample = (unsigned char)cfg.OversampleV;

    // Gather the sizes of all rectangles we will need to pack (this loop is based on
    // stbtt_PackFontRangesGatherRects)
    const float scale = (cfg.SizePixels > 0) ?
                          stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels) :
                          stbtt_ScaleForMappingEmToPixels(&src_tmp.FontInfo, -cfg.SizePixels);
    const int padding = atlas->TexGlyphPadding;
    for (int glyph_i = 0; glyph_i < src_tmp.GlyphsList.Size; glyph_i++) {
      int x0, y0, x1, y1;
      const int glyph_index_in_font = stbtt_FindGlyphIndex(&src_tmp.FontInfo,
                                                           src_tmp.GlyphsList[glyph_i]);
      ANCHOR_ASSERT(glyph_index_in_font != 0);
      stbtt_GetGlyphBitmapBoxSubpixel(&src_tmp.FontInfo,
                                      glyph_index_in_font,
                                      scale * cfg.OversampleH,
                                      scale * cfg.OversampleV,
                                      0,
                                      0,
                                      &x0,
                                      &y0,
                                      &x1,
                                      &y1);
      src_tmp.Rects[glyph_i].w = (stbrp_coord)(x1 - x0 + padding + cfg.OversampleH - 1);
      src_tmp.Rects[glyph_i].h = (stbrp_coord)(y1 - y0 + padding + cfg.OversampleV - 1);
      total_surface += src_tmp.Rects[glyph_i].w * src_tmp.Rects[glyph_i].h;
    }
  }

  // We need a width for the skyline algorithm, any width!
  // The exact width doesn't really matter much, but some API/GPU have texture size limitations and
  // increasing width can decrease height. User can override TexDesiredWidth and TexGlyphPadding if
  // they wish, otherwise we use a simple heuristic to select the width based on expected surface.
  const int surface_sqrt = (int)AnchorSqrt((float)total_surface) + 1;
  atlas->TexHeight = 0;
  if (atlas->TexDesiredWidth > 0)
    atlas->TexWidth = atlas->TexDesiredWidth;
  else
    atlas->TexWidth = (surface_sqrt >= 4096 * 0.7f) ? 4096 :
                      (surface_sqrt >= 2048 * 0.7f) ? 2048 :
                      (surface_sqrt >= 1024 * 0.7f) ? 1024 :
                                                      512;

  // 5. Start packing
  // Pack our extra data rectangles first, so it will be on the upper-left corner of our texture
  // (UV will have small values).
  const int TEX_HEIGHT_MAX = 1024 * 32;
  stbtt_pack_context spc = {};
  stbtt_PackBegin(&spc, NULL, atlas->TexWidth, TEX_HEIGHT_MAX, 0, atlas->TexGlyphPadding, NULL);
  AnchorFontAtlasBuildPackCustomRects(atlas, spc.pack_info);

  // 6. Pack each source font. No rendering yet, we are working with rectangles in an infinitely
  // tall texture at this point.
  for (int src_i = 0; src_i < src_tmp_array.Size; src_i++) {
    AnchorFontBuildSrcData &src_tmp = src_tmp_array[src_i];
    if (src_tmp.GlyphsCount == 0)
      continue;

    stbrp_pack_rects((stbrp_context *)spc.pack_info, src_tmp.Rects, src_tmp.GlyphsCount);

    // Extend texture height and mark missing glyphs as non-packed so we won't render them.
    // FIXME: We are not handling packing failure here (would happen if we got off TEX_HEIGHT_MAX
    // or if a single if larger than TexWidth?)
    for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++)
      if (src_tmp.Rects[glyph_i].was_packed)
        atlas->TexHeight = AnchorMax(atlas->TexHeight,
                                     src_tmp.Rects[glyph_i].y + src_tmp.Rects[glyph_i].h);
  }

  // 7. Allocate texture
  atlas->TexHeight = (atlas->Flags & AnchorFontAtlasFlags_NoPowerOfTwoHeight) ?
                       (atlas->TexHeight + 1) :
                       ImUpperPowerOfTwo(atlas->TexHeight);
  atlas->TexUvScale = GfVec2f(1.0f / atlas->TexWidth, 1.0f / atlas->TexHeight);
  atlas->TexPixelsAlpha8 = (unsigned char *)ANCHOR_ALLOC(atlas->TexWidth * atlas->TexHeight);
  memset(atlas->TexPixelsAlpha8, 0, atlas->TexWidth * atlas->TexHeight);
  spc.pixels = atlas->TexPixelsAlpha8;
  spc.height = atlas->TexHeight;

  // 8. Render/rasterize font characters into the texture
  for (int src_i = 0; src_i < src_tmp_array.Size; src_i++) {
    AnchorFontConfig &cfg = atlas->ConfigData[src_i];
    AnchorFontBuildSrcData &src_tmp = src_tmp_array[src_i];
    if (src_tmp.GlyphsCount == 0)
      continue;

    stbtt_PackFontRangesRenderIntoRects(&spc,
                                        &src_tmp.FontInfo,
                                        &src_tmp.PackRange,
                                        1,
                                        src_tmp.Rects);

    // Apply multiply operator
    if (cfg.RasterizerMultiply != 1.0f) {
      unsigned char multiply_table[256];
      AnchorFontAtlasBuildMultiplyCalcLookupTable(multiply_table, cfg.RasterizerMultiply);
      stbrp_rect *r = &src_tmp.Rects[0];
      for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++, r++)
        if (r->was_packed)
          AnchorFontAtlasBuildMultiplyRectAlpha8(multiply_table,
                                                 atlas->TexPixelsAlpha8,
                                                 r->x,
                                                 r->y,
                                                 r->w,
                                                 r->h,
                                                 atlas->TexWidth * 1);
    }
    src_tmp.Rects = NULL;
  }

  // End packing
  stbtt_PackEnd(&spc);
  buf_rects.clear();

  // 9. Setup AnchorFont and glyphs for runtime
  for (int src_i = 0; src_i < src_tmp_array.Size; src_i++) {
    AnchorFontBuildSrcData &src_tmp = src_tmp_array[src_i];
    if (src_tmp.GlyphsCount == 0)
      continue;

    // When merging fonts with MergeMode=true:
    // - We can have multiple input fonts writing into a same destination font.
    // - dst_font->ConfigData is != from cfg which is our source configuration.
    AnchorFontConfig &cfg = atlas->ConfigData[src_i];
    AnchorFont *dst_font = cfg.DstFont;

    const float font_scale = stbtt_ScaleForPixelHeight(&src_tmp.FontInfo, cfg.SizePixels);
    int unscaled_ascent, unscaled_descent, unscaled_line_gap;
    stbtt_GetFontVMetrics(&src_tmp.FontInfo,
                          &unscaled_ascent,
                          &unscaled_descent,
                          &unscaled_line_gap);

    const float ascent = AnchorFloor(unscaled_ascent * font_scale +
                                     ((unscaled_ascent > 0.0f) ? +1 : -1));
    const float descent = AnchorFloor(unscaled_descent * font_scale +
                                      ((unscaled_descent > 0.0f) ? +1 : -1));
    AnchorFontAtlasBuildSetupFont(atlas, dst_font, &cfg, ascent, descent);
    const float font_off_x = cfg.GlyphOffset[0];
    const float font_off_y = cfg.GlyphOffset[1] + IM_ROUND(dst_font->Ascent);

    for (int glyph_i = 0; glyph_i < src_tmp.GlyphsCount; glyph_i++) {
      // Register glyph
      const int codepoint = src_tmp.GlyphsList[glyph_i];
      const stbtt_packedchar &pc = src_tmp.PackedChars[glyph_i];
      stbtt_aligned_quad q;
      float unused_x = 0.0f, unused_y = 0.0f;
      stbtt_GetPackedQuad(src_tmp.PackedChars,
                          atlas->TexWidth,
                          atlas->TexHeight,
                          glyph_i,
                          &unused_x,
                          &unused_y,
                          &q,
                          0);
      dst_font->AddGlyph(&cfg,
                         (AnchorWChar)codepoint,
                         q.x0 + font_off_x,
                         q.y0 + font_off_y,
                         q.x1 + font_off_x,
                         q.y1 + font_off_y,
                         q.s0,
                         q.t0,
                         q.s1,
                         q.t1,
                         pc.xadvance);
    }
  }

  // Cleanup temporary (AnchorVector doesn't honor destructor)
  for (int src_i = 0; src_i < src_tmp_array.Size; src_i++)
    src_tmp_array[src_i].~AnchorFontBuildSrcData();

  AnchorFontAtlasBuildFinish(atlas);
  return true;
}

const AnchorFontBuilderIO *AnchorFontAtlasGetBuilderForStbTruetype()
{
  static AnchorFontBuilderIO io;
  io.FontBuilder_Build = AnchorFontAtlasBuildWithStbTruetype;
  return &io;
}

void AnchorFontAtlasBuildSetupFont(AnchorFontAtlas *atlas,
                                   AnchorFont *font,
                                   AnchorFontConfig *font_config,
                                   float ascent,
                                   float descent)
{
  if (!font_config->MergeMode) {
    font->ClearOutputData();
    font->FontSize = font_config->SizePixels;
    font->ConfigData = font_config;
    font->ConfigDataCount = 0;
    font->ContainerAtlas = atlas;
    font->Ascent = ascent;
    font->Descent = descent;
  }
  font->ConfigDataCount++;
}

void AnchorFontAtlasBuildPackCustomRects(AnchorFontAtlas *atlas, void *stbrp_context_opaque)
{
  stbrp_context *pack_context = (stbrp_context *)stbrp_context_opaque;
  ANCHOR_ASSERT(pack_context != NULL);

  AnchorVector<AnchorFontAtlasCustomRect> &user_rects = atlas->CustomRects;
  ANCHOR_ASSERT(user_rects.Size >= 1);  // We expect at least the default custom rects to be
                                        // registered, else something went wrong.

  AnchorVector<stbrp_rect> pack_rects;
  pack_rects.resize(user_rects.Size);
  memset(pack_rects.Data, 0, (size_t)pack_rects.size_in_bytes());
  for (int i = 0; i < user_rects.Size; i++) {
    pack_rects[i].w = user_rects[i].Width;
    pack_rects[i].h = user_rects[i].Height;
  }
  stbrp_pack_rects(pack_context, &pack_rects[0], pack_rects.Size);
  for (int i = 0; i < pack_rects.Size; i++)
    if (pack_rects[i].was_packed) {
      user_rects[i].X = pack_rects[i].x;
      user_rects[i].Y = pack_rects[i].y;
      ANCHOR_ASSERT(pack_rects[i].w == user_rects[i].Width &&
                    pack_rects[i].h == user_rects[i].Height);
      atlas->TexHeight = AnchorMax(atlas->TexHeight, pack_rects[i].y + pack_rects[i].h);
    }
}

void AnchorFontAtlasBuildRender8bppRectFromString(AnchorFontAtlas *atlas,
                                                  int x,
                                                  int y,
                                                  int w,
                                                  int h,
                                                  const char *in_str,
                                                  char in_marker_char,
                                                  unsigned char in_marker_pixel_value)
{
  ANCHOR_ASSERT(x >= 0 && x + w <= atlas->TexWidth);
  ANCHOR_ASSERT(y >= 0 && y + h <= atlas->TexHeight);
  unsigned char *out_pixel = atlas->TexPixelsAlpha8 + x + (y * atlas->TexWidth);
  for (int off_y = 0; off_y < h; off_y++, out_pixel += atlas->TexWidth, in_str += w)
    for (int off_x = 0; off_x < w; off_x++)
      out_pixel[off_x] = (in_str[off_x] == in_marker_char) ? in_marker_pixel_value : 0x00;
}

void AnchorFontAtlasBuildRender32bppRectFromString(AnchorFontAtlas *atlas,
                                                   int x,
                                                   int y,
                                                   int w,
                                                   int h,
                                                   const char *in_str,
                                                   char in_marker_char,
                                                   unsigned int in_marker_pixel_value)
{
  ANCHOR_ASSERT(x >= 0 && x + w <= atlas->TexWidth);
  ANCHOR_ASSERT(y >= 0 && y + h <= atlas->TexHeight);
  unsigned int *out_pixel = atlas->TexPixelsRGBA32 + x + (y * atlas->TexWidth);
  for (int off_y = 0; off_y < h; off_y++, out_pixel += atlas->TexWidth, in_str += w)
    for (int off_x = 0; off_x < w; off_x++)
      out_pixel[off_x] = (in_str[off_x] == in_marker_char) ? in_marker_pixel_value :
                                                             ANCHOR_COL32_BLACK_TRANS;
}

static void AnchorFontAtlasBuildRenderDefaultTexData(AnchorFontAtlas *atlas)
{
  AnchorFontAtlasCustomRect *r = atlas->GetCustomRectByIndex(atlas->PackIdMouseCursors);
  ANCHOR_ASSERT(r->IsPacked());

  const int w = atlas->TexWidth;
  if (!(atlas->Flags & AnchorFontAtlasFlags_NoMouseCursors)) {
    // Render/copy pixels
    ANCHOR_ASSERT(r->Width == FONT_ATLAS_DEFAULT_TEX_DATA_W * 2 + 1 &&
                  r->Height == FONT_ATLAS_DEFAULT_TEX_DATA_H);
    const int x_for_white = r->X;
    const int x_for_black = r->X + FONT_ATLAS_DEFAULT_TEX_DATA_W + 1;
    if (atlas->TexPixelsAlpha8 != NULL) {
      AnchorFontAtlasBuildRender8bppRectFromString(atlas,
                                                   x_for_white,
                                                   r->Y,
                                                   FONT_ATLAS_DEFAULT_TEX_DATA_W,
                                                   FONT_ATLAS_DEFAULT_TEX_DATA_H,
                                                   FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS,
                                                   '.',
                                                   0xFF);
      AnchorFontAtlasBuildRender8bppRectFromString(atlas,
                                                   x_for_black,
                                                   r->Y,
                                                   FONT_ATLAS_DEFAULT_TEX_DATA_W,
                                                   FONT_ATLAS_DEFAULT_TEX_DATA_H,
                                                   FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS,
                                                   'X',
                                                   0xFF);
    } else {
      AnchorFontAtlasBuildRender32bppRectFromString(atlas,
                                                    x_for_white,
                                                    r->Y,
                                                    FONT_ATLAS_DEFAULT_TEX_DATA_W,
                                                    FONT_ATLAS_DEFAULT_TEX_DATA_H,
                                                    FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS,
                                                    '.',
                                                    ANCHOR_COL32_WHITE);
      AnchorFontAtlasBuildRender32bppRectFromString(atlas,
                                                    x_for_black,
                                                    r->Y,
                                                    FONT_ATLAS_DEFAULT_TEX_DATA_W,
                                                    FONT_ATLAS_DEFAULT_TEX_DATA_H,
                                                    FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS,
                                                    'X',
                                                    ANCHOR_COL32_WHITE);
    }
  } else {
    // Render 4 white pixels
    ANCHOR_ASSERT(r->Width == 2 && r->Height == 2);
    const int offset = (int)r->X + (int)r->Y * w;
    if (atlas->TexPixelsAlpha8 != NULL) {
      atlas->TexPixelsAlpha8[offset] = atlas->TexPixelsAlpha8[offset + 1] =
        atlas->TexPixelsAlpha8[offset + w] = atlas->TexPixelsAlpha8[offset + w + 1] = 0xFF;
    } else {
      atlas->TexPixelsRGBA32[offset] = atlas->TexPixelsRGBA32[offset + 1] =
        atlas->TexPixelsRGBA32[offset + w] = atlas->TexPixelsRGBA32[offset + w + 1] =
          ANCHOR_COL32_WHITE;
    }
  }
  atlas->TexUvWhitePixel = GfVec2f((r->X + 0.5f) * atlas->TexUvScale[0],
                                   (r->Y + 0.5f) * atlas->TexUvScale[1]);
}

static void AnchorFontAtlasBuildRenderLinesTexData(AnchorFontAtlas *atlas)
{
  if (atlas->Flags & AnchorFontAtlasFlags_NoBakedLines)
    return;

  // This generates a triangular shape in the texture, with the various line widths stacked on top
  // of each other to allow interpolation between them
  AnchorFontAtlasCustomRect *r = atlas->GetCustomRectByIndex(atlas->PackIdLines);
  ANCHOR_ASSERT(r->IsPacked());
  for (unsigned int n = 0; n < ANCHOR_DRAWLIST_TEX_LINES_WIDTH_MAX + 1;
       n++)  // +1 because of the zero-width row
  {
    // Each line consists of at least two empty pixels at the ends, with a line of solid pixels in
    // the middle
    unsigned int y = n;
    unsigned int line_width = n;
    unsigned int pad_left = (r->Width - line_width) / 2;
    unsigned int pad_right = r->Width - (pad_left + line_width);

    // Write each slice
    ANCHOR_ASSERT(pad_left + line_width + pad_right == r->Width &&
                  y < r->Height);  // Make sure we're inside the texture bounds before we start
                                   // writing pixels
    if (atlas->TexPixelsAlpha8 != NULL) {
      unsigned char *write_ptr = &atlas->TexPixelsAlpha8[r->X + ((r->Y + y) * atlas->TexWidth)];
      for (unsigned int i = 0; i < pad_left; i++)
        *(write_ptr + i) = 0x00;

      for (unsigned int i = 0; i < line_width; i++)
        *(write_ptr + pad_left + i) = 0xFF;

      for (unsigned int i = 0; i < pad_right; i++)
        *(write_ptr + pad_left + line_width + i) = 0x00;
    } else {
      unsigned int *write_ptr = &atlas->TexPixelsRGBA32[r->X + ((r->Y + y) * atlas->TexWidth)];
      for (unsigned int i = 0; i < pad_left; i++)
        *(write_ptr + i) = ANCHOR_COL32_BLACK_TRANS;

      for (unsigned int i = 0; i < line_width; i++)
        *(write_ptr + pad_left + i) = ANCHOR_COL32_WHITE;

      for (unsigned int i = 0; i < pad_right; i++)
        *(write_ptr + pad_left + line_width + i) = ANCHOR_COL32_BLACK_TRANS;
    }

    // Calculate UVs for this line
    /** todo::check_math */
    GfVec2f uv0 = GfVec2f(
      GfVec2f((float)(r->X + pad_left - 1), (float)(r->Y + y))[0] * atlas->TexUvScale[0],
      GfVec2f((float)(r->X + pad_left - 1), (float)(r->Y + y))[1] * atlas->TexUvScale[1]);

    GfVec2f uv1 = GfVec2f(
      GfVec2f((float)(r->X + pad_left + line_width + 1), (float)(r->Y + y + 1))[0] *
        atlas->TexUvScale[0],
      GfVec2f((float)(r->X + pad_left + line_width + 1), (float)(r->Y + y + 1))[1] *
        atlas->TexUvScale[1]);
    float half_v =
      (uv0[1] + uv1[1]) *
      0.5f;  // Calculate a constant V in the middle of the row to avoid sampling artifacts
    atlas->TexUvLines[n] = GfVec4f(uv0[0], half_v, uv1[0], half_v);
  }
}

// Note: this is called / shared by both the stb_truetype and the FreeType builder
void AnchorFontAtlasBuildInit(AnchorFontAtlas *atlas)
{
  // Register texture region for mouse cursors or standard white pixels
  if (atlas->PackIdMouseCursors < 0) {
    if (!(atlas->Flags & AnchorFontAtlasFlags_NoMouseCursors))
      atlas->PackIdMouseCursors = atlas->AddCustomRectRegular(FONT_ATLAS_DEFAULT_TEX_DATA_W * 2 +
                                                                1,
                                                              FONT_ATLAS_DEFAULT_TEX_DATA_H);
    else
      atlas->PackIdMouseCursors = atlas->AddCustomRectRegular(2, 2);
  }

  // Register texture region for thick lines
  // The +2 here is to give space for the end caps, whilst height +1 is to accommodate the fact we
  // have a zero-width row
  if (atlas->PackIdLines < 0) {
    if (!(atlas->Flags & AnchorFontAtlasFlags_NoBakedLines))
      atlas->PackIdLines = atlas->AddCustomRectRegular(ANCHOR_DRAWLIST_TEX_LINES_WIDTH_MAX + 2,
                                                       ANCHOR_DRAWLIST_TEX_LINES_WIDTH_MAX + 1);
  }
}

// This is called/shared by both the stb_truetype and the FreeType builder.
void AnchorFontAtlasBuildFinish(AnchorFontAtlas *atlas)
{
  // Render into our custom data blocks
  ANCHOR_ASSERT(atlas->TexPixelsAlpha8 != NULL || atlas->TexPixelsRGBA32 != NULL);
  AnchorFontAtlasBuildRenderDefaultTexData(atlas);
  AnchorFontAtlasBuildRenderLinesTexData(atlas);

  // Register custom rectangle glyphs
  for (int i = 0; i < atlas->CustomRects.Size; i++) {
    const AnchorFontAtlasCustomRect *r = &atlas->CustomRects[i];
    if (r->Font == NULL || r->GlyphID == 0)
      continue;

    // Will ignore AnchorFontConfig settings: GlyphMinAdvanceX, GlyphMinAdvanceY,
    // GlyphExtraSpacing, PixelSnapH
    ANCHOR_ASSERT(r->Font->ContainerAtlas == atlas);
    GfVec2f uv0, uv1;
    atlas->CalcCustomRectUV(r, &uv0, &uv1);
    r->Font->AddGlyph(NULL,
                      (AnchorWChar)r->GlyphID,
                      r->GlyphOffset[0],
                      r->GlyphOffset[1],
                      r->GlyphOffset[0] + r->Width,
                      r->GlyphOffset[1] + r->Height,
                      uv0[0],
                      uv0[1],
                      uv1[0],
                      uv1[1],
                      r->GlyphAdvanceX);
  }

  // Build all fonts lookup tables
  for (int i = 0; i < atlas->Fonts.Size; i++)
    if (atlas->Fonts[i]->DirtyLookupTables)
      atlas->Fonts[i]->BuildLookupTable();

  // Ellipsis character is required for rendering elided text. We prefer using U+2026 (horizontal
  // ellipsis). However some old fonts may contain ellipsis at U+0085. Here we auto-detect most
  // suitable ellipsis character.
  // FIXME: Also note that 0x2026 is currently seldom included in our font ranges. Because of this
  // we are more likely to use three individual dots.
  for (int i = 0; i < atlas->Fonts.size(); i++) {
    AnchorFont *font = atlas->Fonts[i];
    if (font->EllipsisChar != (AnchorWChar)-1)
      continue;
    const AnchorWChar ellipsis_variants[] = {(AnchorWChar)0x2026, (AnchorWChar)0x0085};
    for (int j = 0; j < ANCHOR_ARRAYSIZE(ellipsis_variants); j++)
      if (font->FindGlyphNoFallback(ellipsis_variants[j]) != NULL)  // Verify glyph exists
      {
        font->EllipsisChar = ellipsis_variants[j];
        break;
      }
  }
}

// Retrieve list of range (2 int per range, values are inclusive)
const AnchorWChar *AnchorFontAtlas::GetGlyphRangesDefault()
{
  static const AnchorWChar ranges[] = {
    0x0020,
    0x00FF,  // Basic Latin + Latin Supplement
    0,
  };
  return &ranges[0];
}

const AnchorWChar *AnchorFontAtlas::GetGlyphRangesKorean()
{
  static const AnchorWChar ranges[] = {
    0x0020,
    0x00FF,  // Basic Latin + Latin Supplement
    0x3131,
    0x3163,  // Korean alphabets
    0xAC00,
    0xD7A3,  // Korean characters
    0,
  };
  return &ranges[0];
}

const AnchorWChar *AnchorFontAtlas::GetGlyphRangesChineseFull()
{
  static const AnchorWChar ranges[] = {
    0x0020,
    0x00FF,  // Basic Latin + Latin Supplement
    0x2000,
    0x206F,  // General Punctuation
    0x3000,
    0x30FF,  // CJK Symbols and Punctuations, Hiragana, Katakana
    0x31F0,
    0x31FF,  // Katakana Phonetic Extensions
    0xFF00,
    0xFFEF,  // Half-width characters
    0x4e00,
    0x9FAF,  // CJK Ideograms
    0,
  };
  return &ranges[0];
}

static void UnpackAccumulativeOffsetsIntoRanges(int base_codepoint,
                                                const short *accumulative_offsets,
                                                int accumulative_offsets_count,
                                                AnchorWChar *out_ranges)
{
  for (int n = 0; n < accumulative_offsets_count; n++, out_ranges += 2) {
    out_ranges[0] = out_ranges[1] = (AnchorWChar)(base_codepoint + accumulative_offsets[n]);
    base_codepoint += accumulative_offsets[n];
  }
  out_ranges[0] = 0;
}

//-------------------------------------------------------------------------
// [SECTION] AnchorFontAtlas glyph ranges helpers
//-------------------------------------------------------------------------

const AnchorWChar *AnchorFontAtlas::GetGlyphRangesChineseSimplifiedCommon()
{
  // Store 2500 regularly used characters for Simplified Chinese.
  // Sourced from
  // https://zh.wiktionary.org/wiki/%E9%99%84%E5%BD%95:%E7%8E%B0%E4%BB%A3%E6%B1%89%E8%AF%AD%E5%B8%B8%E7%94%A8%E5%AD%97%E8%A1%A8
  // This table covers 97.97% of all characters used during the month in July, 1987.
  // You can use AnchorFontGlyphRangesBuilder to create your own ranges derived from this, by
  // merging existing ranges or adding new characters. (Stored as accumulative offsets from the
  // initial unicode codepoint 0x4E00. This encoding is designed to helps us compact the source
  // code size.)
  static const short accumulative_offsets_from_0x4E00[] = {
    0,   1,  2,  4,  1,   1,  1,  1,   2,   1,   3,   2,  1,  2,   2,   1,   1,   1,   1,   1,
    5,   2,  1,  2,  3,   3,  3,  2,   2,   4,   1,   1,  1,  2,   1,   5,   2,   3,   1,   2,
    1,   2,  1,  1,  2,   1,  1,  2,   2,   1,   4,   1,  1,  1,   1,   5,   10,  1,   2,   19,
    2,   1,  2,  1,  2,   1,  2,  1,   2,   1,   5,   1,  6,  3,   2,   1,   2,   2,   1,   1,
    1,   4,  8,  5,  1,   1,  4,  1,   1,   3,   1,   2,  1,  5,   1,   2,   1,   1,   1,   10,
    1,   1,  5,  2,  4,   6,  1,  4,   2,   2,   2,   12, 2,  1,   1,   6,   1,   1,   1,   4,
    1,   1,  4,  6,  5,   1,  4,  2,   2,   4,   10,  7,  1,  1,   4,   2,   4,   2,   1,   4,
    3,   6,  10, 12, 5,   7,  2,  14,  2,   9,   1,   1,  6,  7,   10,  4,   7,   13,  1,   5,
    4,   8,  4,  1,  1,   2,  28, 5,   6,   1,   1,   5,  2,  5,   20,  2,   2,   9,   8,   11,
    2,   9,  17, 1,  8,   6,  8,  27,  4,   6,   9,   20, 11, 27,  6,   68,  2,   2,   1,   1,
    1,   2,  1,  2,  2,   7,  6,  11,  3,   3,   1,   1,  3,  1,   2,   1,   1,   1,   1,   1,
    3,   1,  1,  8,  3,   4,  1,  5,   7,   2,   1,   4,  4,  8,   4,   2,   1,   2,   1,   1,
    4,   5,  6,  3,  6,   2,  12, 3,   1,   3,   9,   2,  4,  3,   4,   1,   5,   3,   3,   1,
    3,   7,  1,  5,  1,   1,  1,  1,   2,   3,   4,   5,  2,  3,   2,   6,   1,   1,   2,   1,
    7,   1,  7,  3,  4,   5,  15, 2,   2,   1,   5,   3,  22, 19,  2,   1,   1,   1,   1,   2,
    5,   1,  1,  1,  6,   1,  1,  12,  8,   2,   9,   18, 22, 4,   1,   1,   5,   1,   16,  1,
    2,   7,  10, 15, 1,   1,  6,  2,   4,   1,   2,   4,  1,  6,   1,   1,   3,   2,   4,   1,
    6,   4,  5,  1,  2,   1,  1,  2,   1,   10,  3,   1,  3,  2,   1,   9,   3,   2,   5,   7,
    2,   19, 4,  3,  6,   1,  1,  1,   1,   1,   4,   3,  2,  1,   1,   1,   2,   5,   3,   1,
    1,   1,  2,  2,  1,   1,  2,  1,   1,   2,   1,   3,  1,  1,   1,   3,   7,   1,   4,   1,
    1,   2,  1,  1,  2,   1,  2,  4,   4,   3,   8,   1,  1,  1,   2,   1,   3,   5,   1,   3,
    1,   3,  4,  6,  2,   2,  14, 4,   6,   6,   11,  9,  1,  15,  3,   1,   28,  5,   2,   5,
    5,   3,  1,  3,  4,   5,  4,  6,   14,  3,   2,   3,  5,  21,  2,   7,   20,  10,  1,   2,
    19,  2,  4,  28, 28,  2,  3,  2,   1,   14,  4,   1,  26, 28,  42,  12,  40,  3,   52,  79,
    5,   14, 17, 3,  2,   2,  11, 3,   4,   6,   3,   1,  8,  2,   23,  4,   5,   8,   10,  4,
    2,   7,  3,  5,  1,   1,  6,  3,   1,   2,   2,   2,  5,  28,  1,   1,   7,   7,   20,  5,
    3,   29, 3,  17, 26,  1,  8,  4,   27,  3,   6,   11, 23, 5,   3,   4,   6,   13,  24,  16,
    6,   5,  10, 25, 35,  7,  3,  2,   3,   3,   14,  3,  6,  2,   6,   1,   4,   2,   3,   8,
    2,   1,  1,  3,  3,   3,  4,  1,   1,   13,  2,   2,  4,  5,   2,   1,   14,  14,  1,   2,
    2,   1,  4,  5,  2,   3,  1,  14,  3,   12,  3,   17, 2,  16,  5,   1,   2,   1,   8,   9,
    3,   19, 4,  2,  2,   4,  17, 25,  21,  20,  28,  75, 1,  10,  29,  103, 4,   1,   2,   1,
    1,   4,  2,  4,  1,   2,  3,  24,  2,   2,   2,   1,  1,  2,   1,   3,   8,   1,   1,   1,
    2,   1,  1,  3,  1,   1,  1,  6,   1,   5,   3,   1,  1,  1,   3,   4,   1,   1,   5,   2,
    1,   5,  6,  13, 9,   16, 1,  1,   1,   1,   3,   2,  3,  2,   4,   5,   2,   5,   2,   2,
    3,   7,  13, 7,  2,   2,  1,  1,   1,   1,   2,   3,  3,  2,   1,   6,   4,   9,   2,   1,
    14,  2,  14, 2,  1,   18, 3,  4,   14,  4,   11,  41, 15, 23,  15,  23,  176, 1,   3,   4,
    1,   1,  1,  1,  5,   3,  1,  2,   3,   7,   3,   1,  1,  2,   1,   2,   4,   4,   6,   2,
    4,   1,  9,  7,  1,   10, 5,  8,   16,  29,  1,   1,  2,  2,   3,   1,   3,   5,   2,   4,
    5,   4,  1,  1,  2,   2,  3,  3,   7,   1,   6,   10, 1,  17,  1,   44,  4,   6,   2,   1,
    1,   6,  5,  4,  2,   10, 1,  6,   9,   2,   8,   1,  24, 1,   2,   13,  7,   8,   8,   2,
    1,   4,  1,  3,  1,   3,  3,  5,   2,   5,   10,  9,  4,  9,   12,  2,   1,   6,   1,   10,
    1,   1,  7,  7,  4,   10, 8,  3,   1,   13,  4,   3,  1,  6,   1,   3,   5,   2,   1,   2,
    17,  16, 5,  2,  16,  6,  1,  4,   2,   1,   3,   3,  6,  8,   5,   11,  11,  1,   3,   3,
    2,   4,  6,  10, 9,   5,  7,  4,   7,   4,   7,   1,  1,  4,   2,   1,   3,   6,   8,   7,
    1,   6,  11, 5,  5,   3,  24, 9,   4,   2,   7,   13, 5,  1,   8,   82,  16,  61,  1,   1,
    1,   4,  2,  2,  16,  10, 3,  8,   1,   1,   6,   4,  2,  1,   3,   1,   1,   1,   4,   3,
    8,   4,  2,  2,  1,   1,  1,  1,   1,   6,   3,   5,  1,  1,   4,   6,   9,   2,   1,   1,
    1,   2,  1,  7,  2,   1,  6,  1,   5,   4,   4,   3,  1,  8,   1,   3,   3,   1,   3,   2,
    2,   2,  2,  3,  1,   6,  1,  2,   1,   2,   1,   3,  7,  1,   8,   2,   1,   2,   1,   5,
    2,   5,  3,  5,  10,  1,  2,  1,   1,   3,   2,   5,  11, 3,   9,   3,   5,   1,   1,   5,
    9,   1,  2,  1,  5,   7,  9,  9,   8,   1,   3,   3,  3,  6,   8,   2,   3,   2,   1,   1,
    32,  6,  1,  2,  15,  9,  3,  7,   13,  1,   3,   10, 13, 2,   14,  1,   13,  10,  2,   1,
    3,   10, 4,  15, 2,   15, 15, 10,  1,   3,   9,   6,  9,  32,  25,  26,  47,  7,   3,   2,
    3,   1,  6,  3,  4,   3,  2,  8,   5,   4,   1,   9,  4,  2,   2,   19,  10,  6,   2,   3,
    8,   1,  2,  2,  4,   2,  1,  9,   4,   4,   4,   6,  4,  8,   9,   2,   3,   1,   1,   1,
    1,   3,  5,  5,  1,   3,  8,  4,   6,   2,   1,   4,  12, 1,   5,   3,   7,   13,  2,   5,
    8,   1,  6,  1,  2,   5,  14, 6,   1,   5,   2,   4,  8,  15,  5,   1,   23,  6,   62,  2,
    10,  1,  1,  8,  1,   2,  2,  10,  4,   2,   2,   9,  2,  1,   1,   3,   2,   3,   1,   5,
    3,   3,  2,  1,  3,   8,  1,  1,   1,   11,  3,   1,  1,  4,   3,   7,   1,   14,  1,   2,
    3,   12, 5,  2,  5,   1,  6,  7,   5,   7,   14,  11, 1,  3,   1,   8,   9,   12,  2,   1,
    11,  8,  4,  4,  2,   6,  10, 9,   13,  1,   1,   3,  1,  5,   1,   3,   2,   4,   4,   1,
    18,  2,  3,  14, 11,  4,  29, 4,   2,   7,   1,   3,  13, 9,   2,   2,   5,   3,   5,   20,
    7,   16, 8,  5,  72,  34, 6,  4,   22,  12,  12,  28, 45, 36,  9,   7,   39,  9,   191, 1,
    1,   1,  4,  11, 8,   4,  9,  2,   3,   22,  1,   1,  1,  1,   4,   17,  1,   7,   7,   1,
    11,  31, 10, 2,  4,   8,  2,  3,   2,   1,   4,   2,  16, 4,   32,  2,   3,   19,  13,  4,
    9,   1,  5,  2,  14,  8,  1,  1,   3,   6,   19,  6,  5,  1,   16,  6,   2,   10,  8,   5,
    1,   2,  3,  1,  5,   5,  1,  11,  6,   6,   1,   3,  3,  2,   6,   3,   8,   1,   1,   4,
    10,  7,  5,  7,  7,   5,  8,  9,   2,   1,   3,   4,  1,  1,   3,   1,   3,   3,   2,   6,
    16,  1,  4,  6,  3,   1,  10, 6,   1,   3,   15,  2,  9,  2,   10,  25,  13,  9,   16,  6,
    2,   2,  10, 11, 4,   3,  9,  1,   2,   6,   6,   5,  4,  30,  40,  1,   10,  7,   12,  14,
    33,  6,  3,  6,  7,   3,  1,  3,   1,   11,  14,  4,  9,  5,   12,  11,  49,  18,  51,  31,
    140, 31, 2,  2,  1,   5,  1,  8,   1,   10,  1,   4,  4,  3,   24,  1,   10,  1,   3,   6,
    6,   16, 3,  4,  5,   2,  1,  4,   2,   57,  10,  6,  22, 2,   22,  3,   7,   22,  6,   10,
    11,  36, 18, 16, 33,  36, 2,  5,   5,   1,   1,   1,  4,  10,  1,   4,   13,  2,   7,   5,
    2,   9,  3,  4,  1,   7,  43, 3,   7,   3,   9,   14, 7,  9,   1,   11,  1,   1,   3,   7,
    4,   18, 13, 1,  14,  1,  3,  6,   10,  73,  2,   2,  30, 6,   1,   11,  18,  19,  13,  22,
    3,   46, 42, 37, 89,  7,  3,  16,  34,  2,   2,   3,  9,  1,   7,   1,   1,   1,   2,   2,
    4,   10, 7,  3,  10,  3,  9,  5,   28,  9,   2,   6,  13, 7,   3,   1,   3,   10,  2,   7,
    2,   11, 3,  6,  21,  54, 85, 2,   1,   4,   2,   2,  1,  39,  3,   21,  2,   2,   5,   1,
    1,   1,  4,  1,  1,   3,  4,  15,  1,   3,   2,   4,  4,  2,   3,   8,   2,   20,  1,   8,
    7,   13, 4,  1,  26,  6,  2,  9,   34,  4,   21,  52, 10, 4,   4,   1,   5,   12,  2,   11,
    1,   7,  2,  30, 12,  44, 2,  30,  1,   1,   3,   6,  16, 9,   17,  39,  82,  2,   2,   24,
    7,   1,  7,  3,  16,  9,  14, 44,  2,   1,   2,   1,  2,  3,   5,   2,   4,   1,   6,   7,
    5,   3,  2,  6,  1,   11, 5,  11,  2,   1,   18,  19, 8,  1,   3,   24,  29,  2,   1,   3,
    5,   2,  2,  1,  13,  6,  5,  1,   46,  11,  3,   5,  1,  1,   5,   8,   2,   10,  6,   12,
    6,   3,  7,  11, 2,   4,  16, 13,  2,   5,   1,   1,  2,  2,   5,   2,   28,  5,   2,   23,
    10,  8,  4,  4,  22,  39, 95, 38,  8,   14,  9,   5,  1,  13,  5,   4,   3,   13,  12,  11,
    1,   9,  1,  27, 37,  2,  5,  4,   4,   63,  211, 95, 2,  2,   2,   1,   3,   5,   2,   1,
    1,   2,  2,  1,  1,   1,  3,  2,   4,   1,   2,   1,  1,  5,   2,   2,   1,   1,   2,   3,
    1,   3,  1,  1,  1,   3,  1,  4,   2,   1,   3,   6,  1,  1,   3,   7,   15,  5,   3,   2,
    5,   3,  9,  11, 4,   2,  22, 1,   6,   3,   8,   7,  1,  4,   28,  4,   16,  3,   3,   25,
    4,   4,  27, 27, 1,   4,  1,  2,   2,   7,   1,   3,  5,  2,   28,  8,   2,   14,  1,   8,
    6,   16, 25, 3,  3,   3,  14, 3,   3,   1,   1,   2,  1,  4,   6,   3,   8,   4,   1,   1,
    1,   2,  3,  6,  10,  6,  2,  3,   18,  3,   2,   5,  5,  4,   3,   1,   5,   2,   5,   4,
    23,  7,  6,  12, 6,   4,  17, 11,  9,   5,   1,   1,  10, 5,   12,  1,   1,   11,  26,  33,
    7,   3,  6,  1,  17,  7,  1,  5,   12,  1,   11,  2,  4,  1,   8,   14,  17,  23,  1,   2,
    1,   7,  8,  16, 11,  9,  6,  5,   2,   6,   4,   16, 2,  8,   14,  1,   11,  8,   9,   1,
    1,   1,  9,  25, 4,   11, 19, 7,   2,   15,  2,   12, 8,  52,  7,   5,   19,  2,   16,  4,
    36,  8,  1,  16, 8,   24, 26, 4,   6,   2,   9,   5,  4,  36,  3,   28,  12,  25,  15,  37,
    27,  17, 12, 59, 38,  5,  32, 127, 1,   2,   9,   17, 14, 4,   1,   2,   1,   1,   8,   11,
    50,  4,  14, 2,  19,  16, 4,  17,  5,   4,   5,   26, 12, 45,  2,   23,  45,  104, 30,  12,
    8,   3,  10, 2,  2,   3,  3,  1,   4,   20,  7,   2,  9,  6,   15,  2,   20,  1,   3,   16,
    4,   11, 15, 6,  134, 2,  5,  59,  1,   2,   2,   2,  1,  9,   17,  3,   26,  137, 10,  211,
    59,  1,  2,  4,  1,   4,  1,  1,   1,   2,   6,   2,  3,  1,   1,   2,   3,   2,   3,   1,
    3,   4,  4,  2,  3,   3,  1,  4,   3,   1,   7,   2,  2,  3,   1,   2,   1,   3,   3,   3,
    2,   2,  3,  2,  1,   3,  14, 6,   1,   3,   2,   9,  6,  15,  27,  9,   34,  145, 1,   1,
    2,   1,  1,  1,  1,   2,  1,  1,   1,   1,   2,   2,  2,  3,   1,   2,   1,   1,   1,   2,
    3,   5,  8,  3,  5,   2,  4,  1,   3,   2,   2,   2,  12, 4,   1,   1,   1,   10,  4,   5,
    1,   20, 4,  16, 1,   15, 9,  5,   12,  2,   9,   2,  5,  4,   2,   26,  19,  7,   1,   26,
    4,   30, 12, 15, 42,  1,  6,  8,   172, 1,   1,   4,  2,  1,   1,   11,  2,   2,   4,   2,
    1,   2,  1,  10, 8,   1,  2,  1,   4,   5,   1,   2,  5,  1,   8,   4,   1,   3,   4,   2,
    1,   6,  2,  1,  3,   4,  1,  2,   1,   1,   1,   1,  12, 5,   7,   2,   4,   3,   1,   1,
    1,   3,  3,  6,  1,   2,  2,  3,   3,   3,   2,   1,  2,  12,  14,  11,  6,   6,   4,   12,
    2,   8,  1,  7,  10,  1,  35, 7,   4,   13,  15,  4,  3,  23,  21,  28,  52,  5,   26,  5,
    6,   1,  7,  10, 2,   7,  53, 3,   2,   1,   1,   1,  2,  163, 532, 1,   10,  11,  1,   3,
    3,   4,  8,  2,  8,   6,  2,  2,   23,  22,  4,   2,  2,  4,   2,   1,   3,   1,   3,   3,
    5,   9,  8,  2,  1,   2,  8,  1,   10,  2,   12,  21, 20, 15,  105, 2,   3,   1,   1,   3,
    2,   3,  1,  1,  2,   5,  1,  4,   15,  11,  19,  1,  1,  1,   1,   5,   4,   5,   1,   1,
    2,   5,  3,  5,  12,  1,  2,  5,   1,   11,  1,   1,  15, 9,   1,   4,   5,   3,   26,  8,
    2,   1,  3,  1,  1,   15, 19, 2,   12,  1,   2,   5,  2,  7,   2,   19,  2,   20,  6,   26,
    7,   5,  2,  2,  7,   34, 21, 13,  70,  2,   128, 1,  1,  2,   1,   1,   2,   1,   1,   3,
    2,   2,  2,  15, 1,   4,  1,  3,   4,   42,  10,  6,  1,  49,  85,  8,   1,   2,   1,   1,
    4,   4,  2,  3,  6,   1,  5,  7,   4,   3,   211, 4,  1,  2,   1,   2,   5,   1,   2,   4,
    2,   2,  6,  5,  6,   10, 3,  4,   48,  100, 6,   2,  16, 296, 5,   27,  387, 2,   2,   3,
    7,   16, 8,  5,  38,  15, 39, 21,  9,   10,  3,   7,  59, 13,  27,  21,  47,  5,   21,  6};
  static AnchorWChar base_ranges[] =  // not zero-terminated
    {
      0x0020,
      0x00FF,  // Basic Latin + Latin Supplement
      0x2000,
      0x206F,  // General Punctuation
      0x3000,
      0x30FF,  // CJK Symbols and Punctuations, Hiragana, Katakana
      0x31F0,
      0x31FF,  // Katakana Phonetic Extensions
      0xFF00,
      0xFFEF  // Half-width characters
    };
  static AnchorWChar full_ranges[ANCHOR_ARRAYSIZE(base_ranges) +
                                 ANCHOR_ARRAYSIZE(accumulative_offsets_from_0x4E00) * 2 + 1] = {0};
  if (!full_ranges[0]) {
    memcpy(full_ranges, base_ranges, sizeof(base_ranges));
    UnpackAccumulativeOffsetsIntoRanges(0x4E00,
                                        accumulative_offsets_from_0x4E00,
                                        ANCHOR_ARRAYSIZE(accumulative_offsets_from_0x4E00),
                                        full_ranges + ANCHOR_ARRAYSIZE(base_ranges));
  }
  return &full_ranges[0];
}

const AnchorWChar *AnchorFontAtlas::GetGlyphRangesJapanese()
{
  // 2999 ideograms code points for Japanese
  // - 2136 Joyo (meaning "for regular use" or "for common use") Kanji code points
  // - 863 Jinmeiyo (meaning "for personal name") Kanji code points
  // - Sourced from the character information database of the Information-technology Promotion
  // Agency, Japan
  //   - https://mojikiban.ipa.go.jp/mji/
  //   - Available under the terms of the Creative Commons Attribution-ShareAlike 2.1 Japan (CC
  //   BY-SA 2.1 JP).
  //     - https://creativecommons.org/licenses/by-sa/2.1/jp/deed.en
  //     - https://creativecommons.org/licenses/by-sa/2.1/jp/legalcode
  //   - You can generate this code by the script at:
  //     - https://github.com/vaiorabbit/everyday_use_kanji
  // - References:
  //   - List of Joyo Kanji
  //     - (Official list by the Agency for Cultural Affairs)
  //     https://www.bunka.go.jp/kokugo_nihongo/sisaku/joho/joho/kakuki/14/tosin02/index.html
  //     - (Wikipedia) https://en.wikipedia.org/wiki/List_of_j%C5%8Dy%C5%8D_kanji
  //   - List of Jinmeiyo Kanji
  //     - (Official list by the Ministry of Justice) http://www.moj.go.jp/MINJI/minji86.html
  //     - (Wikipedia) https://en.wikipedia.org/wiki/Jinmeiy%C5%8D_kanji
  // - Missing 1 Joyo Kanji: U+20B9F (Kun'yomi: Shikaru, On'yomi: Shitsu,shichi), see
  // https://github.com/ocornut/ANCHOR/pull/3627 for details. You can use
  // AnchorFontGlyphRangesBuilder to create your own ranges derived from this, by merging existing
  // ranges or adding new characters. (Stored as accumulative offsets from the initial unicode
  // codepoint 0x4E00. This encoding is designed to helps us compact the source code size.)
  static const short accumulative_offsets_from_0x4E00[] = {
    0,  1,  2,  4,  1,  1,  1,   1,  2,  1,  3,  3,   2,  2,  1,  5,   3,     5,  7,  5,  6,   1,
    2,  1,  7,  2,  6,  3,  1,   8,  1,  1,  4,  1,   1,  18, 2,  11,  2,     6,  2,  1,  2,   1,
    5,  1,  2,  1,  3,  1,  2,   1,  2,  3,  3,  1,   1,  2,  3,  1,   1,     1,  12, 7,  9,   1,
    4,  5,  1,  1,  2,  1,  10,  1,  1,  9,  2,  2,   4,  5,  6,  9,   3,     1,  1,  1,  1,   9,
    3,  18, 5,  2,  2,  2,  2,   1,  6,  3,  7,  1,   1,  1,  1,  2,   2,     4,  2,  1,  23,  2,
    10, 4,  3,  5,  2,  4,  10,  2,  4,  13, 1,  6,   1,  9,  3,  1,   1,     6,  6,  7,  6,   3,
    1,  2,  11, 3,  2,  2,  3,   2,  15, 2,  2,  5,   4,  3,  6,  4,   1,     2,  5,  2,  12,  16,
    6,  13, 9,  13, 2,  1,  1,   7,  16, 4,  7,  1,   19, 1,  5,  1,   2,     2,  7,  7,  8,   2,
    6,  5,  4,  9,  18, 7,  4,   5,  9,  13, 11, 8,   15, 2,  1,  1,   1,     2,  1,  2,  2,   1,
    2,  2,  8,  2,  9,  3,  3,   1,  1,  4,  4,  1,   1,  1,  4,  9,   1,     4,  3,  5,  5,   2,
    7,  5,  3,  4,  8,  2,  1,   13, 2,  3,  3,  1,   14, 1,  1,  4,   5,     1,  3,  6,  1,   5,
    2,  1,  1,  3,  3,  3,  3,   1,  1,  2,  7,  6,   6,  7,  1,  4,   7,     6,  1,  1,  1,   1,
    1,  12, 3,  3,  9,  5,  2,   6,  1,  5,  6,  1,   2,  3,  18, 2,   4,     14, 4,  1,  3,   6,
    1,  1,  6,  3,  5,  5,  3,   2,  2,  2,  2,  12,  3,  1,  4,  2,   3,     2,  3,  11, 1,   7,
    4,  1,  2,  1,  3,  17, 1,   9,  1,  24, 1,  1,   4,  2,  2,  4,   1,     2,  7,  1,  1,   1,
    3,  1,  2,  2,  4,  15, 1,   1,  2,  1,  1,  2,   1,  5,  2,  5,   20,    2,  5,  9,  1,   10,
    8,  7,  6,  1,  1,  1,  1,   1,  1,  6,  2,  1,   2,  8,  1,  1,   1,     1,  5,  1,  1,   3,
    1,  1,  1,  1,  3,  1,  1,   12, 4,  1,  3,  1,   1,  1,  1,  1,   10,    3,  1,  7,  5,   13,
    1,  2,  3,  4,  6,  1,  1,   30, 2,  9,  9,  1,   15, 38, 11, 3,   1,     8,  24, 7,  1,   9,
    8,  10, 2,  1,  9,  31, 2,   13, 6,  2,  9,  4,   49, 5,  2,  15,  2,     1,  10, 2,  1,   1,
    1,  2,  2,  6,  15, 30, 35,  3,  14, 18, 8,  1,   16, 10, 28, 12,  19,    45, 38, 1,  3,   2,
    3,  13, 2,  1,  7,  3,  6,   5,  3,  4,  3,  1,   5,  7,  8,  1,   5,     3,  18, 5,  3,   6,
    1,  21, 4,  24, 9,  24, 40,  3,  14, 3,  21, 3,   2,  1,  2,  4,   2,     3,  1,  15, 15,  6,
    5,  1,  1,  3,  1,  5,  6,   1,  9,  7,  3,  3,   2,  1,  4,  3,   8,     21, 5,  16, 4,   5,
    2,  10, 11, 11, 3,  6,  3,   2,  9,  3,  6,  13,  1,  2,  1,  1,   1,     1,  11, 12, 6,   6,
    1,  4,  2,  6,  5,  2,  1,   1,  3,  3,  6,  13,  3,  1,  1,  5,   1,     2,  3,  3,  14,  2,
    1,  2,  2,  2,  5,  1,  9,   5,  1,  1,  6,  12,  3,  12, 3,  4,   13,    2,  14, 2,  8,   1,
    17, 5,  1,  16, 4,  2,  2,   21, 8,  9,  6,  23,  20, 12, 25, 19,  9,     38, 8,  3,  21,  40,
    25, 33, 13, 4,  3,  1,  4,   1,  2,  4,  1,  2,   5,  26, 2,  1,   1,     2,  1,  3,  6,   2,
    1,  1,  1,  1,  1,  1,  2,   3,  1,  1,  1,  9,   2,  3,  1,  1,   1,     3,  6,  3,  2,   1,
    1,  6,  6,  1,  8,  2,  2,   2,  1,  4,  1,  2,   3,  2,  7,  3,   2,     4,  1,  2,  1,   2,
    2,  1,  1,  1,  1,  1,  3,   1,  2,  5,  4,  10,  9,  4,  9,  1,   1,     1,  1,  1,  1,   5,
    3,  2,  1,  6,  4,  9,  6,   1,  10, 2,  31, 17,  8,  3,  7,  5,   40,    1,  7,  7,  1,   6,
    5,  2,  10, 7,  8,  4,  15,  39, 25, 6,  28, 47,  18, 10, 7,  1,   3,     1,  1,  2,  1,   1,
    1,  3,  3,  3,  1,  1,  1,   3,  4,  2,  1,  4,   1,  3,  6,  10,  7,     8,  6,  2,  2,   1,
    3,  3,  2,  5,  8,  7,  9,   12, 2,  15, 1,  1,   4,  1,  2,  1,   1,     1,  3,  2,  1,   3,
    3,  5,  6,  2,  3,  2,  10,  1,  4,  2,  8,  1,   1,  1,  11, 6,   1,     21, 4,  16, 3,   1,
    3,  1,  4,  2,  3,  6,  5,   1,  3,  1,  1,  3,   3,  4,  6,  1,   1,     10, 4,  2,  7,   10,
    4,  7,  4,  2,  9,  4,  3,   1,  1,  1,  4,  1,   8,  3,  4,  1,   3,     1,  6,  1,  4,   2,
    1,  4,  7,  2,  1,  8,  1,   4,  5,  1,  1,  2,   2,  4,  6,  2,   7,     1,  10, 1,  1,   3,
    4,  11, 10, 8,  21, 4,  6,   1,  3,  5,  2,  1,   2,  28, 5,  5,   2,     3,  13, 1,  2,   3,
    1,  4,  2,  1,  5,  20, 3,   8,  11, 1,  3,  3,   3,  1,  8,  10,  9,     2,  10, 9,  2,   3,
    1,  1,  2,  4,  1,  8,  3,   6,  1,  7,  8,  6,   11, 1,  4,  29,  8,     4,  3,  1,  2,   7,
    13, 1,  4,  1,  6,  2,  6,   12, 12, 2,  20, 3,   2,  3,  6,  4,   8,     9,  2,  7,  34,  5,
    1,  18, 6,  1,  1,  4,  4,   5,  7,  9,  1,  2,   2,  4,  3,  4,   1,     7,  2,  2,  2,   6,
    2,  3,  25, 5,  3,  6,  1,   4,  6,  7,  4,  2,   1,  4,  2,  13,  6,     4,  4,  3,  1,   5,
    3,  4,  4,  3,  2,  1,  1,   4,  1,  2,  1,  1,   3,  1,  11, 1,   6,     3,  1,  7,  3,   6,
    2,  8,  8,  6,  9,  3,  4,   11, 3,  2,  10, 12,  2,  5,  11, 1,   6,     4,  5,  3,  1,   8,
    5,  4,  6,  6,  3,  5,  1,   1,  3,  2,  1,  2,   2,  6,  17, 12,  1,     10, 1,  6,  12,  1,
    6,  6,  19, 9,  6,  16, 1,   13, 4,  4,  15, 7,   17, 6,  11, 9,   15,    12, 6,  7,  2,   1,
    2,  2,  15, 9,  3,  21, 4,   6,  49, 18, 7,  3,   2,  3,  1,  6,   8,     2,  2,  6,  2,   9,
    1,  3,  6,  4,  4,  1,  2,   16, 2,  5,  2,  1,   6,  2,  3,  5,   3,     1,  2,  5,  1,   2,
    1,  9,  3,  1,  8,  6,  4,   8,  11, 3,  1,  1,   1,  1,  3,  1,   13,    8,  4,  1,  3,   2,
    2,  1,  4,  1,  11, 1,  5,   2,  1,  5,  2,  5,   8,  6,  1,  1,   7,     4,  3,  8,  3,   2,
    7,  2,  1,  5,  1,  5,  2,   4,  7,  6,  2,  8,   5,  1,  11, 4,   5,     3,  6,  18, 1,   2,
    13, 3,  3,  1,  21, 1,  1,   4,  1,  4,  1,  1,   1,  8,  1,  2,   2,     7,  1,  2,  4,   2,
    2,  9,  2,  1,  1,  1,  4,   3,  6,  3,  12, 5,   1,  1,  1,  5,   6,     3,  2,  4,  8,   2,
    2,  4,  2,  7,  1,  8,  9,   5,  2,  3,  2,  1,   3,  2,  13, 7,   14,    6,  5,  1,  1,   2,
    1,  4,  2,  23, 2,  1,  1,   6,  3,  1,  4,  1,   15, 3,  1,  7,   3,     9,  14, 1,  3,   1,
    4,  1,  1,  5,  8,  1,  3,   8,  3,  8,  15, 11,  4,  14, 4,  4,   2,     5,  5,  1,  7,   1,
    6,  14, 7,  7,  8,  5,  15,  4,  8,  6,  5,  6,   2,  1,  13, 1,   20,    15, 11, 9,  2,   5,
    6,  2,  11, 2,  6,  2,  5,   1,  5,  8,  4,  13,  19, 25, 4,  1,   1,     11, 1,  34, 2,   5,
    9,  14, 6,  2,  2,  6,  1,   1,  14, 1,  3,  14,  13, 1,  6,  12,  21,    14, 14, 6,  32,  17,
    8,  32, 9,  28, 1,  2,  4,   11, 8,  3,  1,  14,  2,  5,  15, 1,   1,     1,  1,  3,  6,   4,
    1,  3,  4,  11, 3,  1,  1,   11, 30, 1,  5,  1,   4,  1,  5,  8,   1,     1,  3,  2,  4,   3,
    17, 35, 2,  6,  12, 17, 3,   1,  6,  2,  1,  1,   12, 2,  7,  3,   3,     2,  1,  16, 2,   8,
    3,  6,  5,  4,  7,  3,  3,   8,  1,  9,  8,  5,   1,  2,  1,  3,   2,     8,  1,  2,  9,   12,
    1,  1,  2,  3,  8,  3,  24,  12, 4,  3,  7,  5,   8,  3,  3,  3,   3,     3,  3,  1,  23,  10,
    3,  1,  2,  2,  6,  3,  1,   16, 1,  16, 22, 3,   10, 4,  11, 6,   9,     7,  7,  3,  6,   2,
    2,  2,  4,  10, 2,  1,  1,   2,  8,  7,  1,  6,   4,  1,  3,  3,   3,     5,  10, 12, 12,  2,
    3,  12, 8,  15, 1,  1,  16,  6,  6,  1,  5,  9,   11, 4,  11, 4,   2,     6,  12, 1,  17,  5,
    13, 1,  4,  9,  5,  1,  11,  2,  1,  8,  1,  5,   7,  28, 8,  3,   5,     10, 2,  17, 3,   38,
    22, 1,  2,  18, 12, 10, 4,   38, 18, 1,  4,  44,  19, 4,  1,  8,   4,     1,  12, 1,  4,   31,
    12, 1,  14, 7,  75, 7,  5,   10, 6,  6,  13, 3,   2,  11, 11, 3,   2,     5,  28, 15, 6,   18,
    18, 5,  6,  4,  3,  16, 1,   7,  18, 7,  36, 3,   5,  3,  1,  7,   1,     9,  1,  10, 7,   2,
    4,  2,  6,  2,  9,  7,  4,   3,  32, 12, 3,  7,   10, 2,  23, 16,  3,     1,  12, 3,  31,  4,
    11, 1,  3,  8,  9,  5,  1,   30, 15, 6,  12, 3,   2,  2,  11, 19,  9,     14, 2,  6,  2,   3,
    19, 13, 17, 5,  3,  3,  25,  3,  14, 1,  1,  1,   36, 1,  3,  2,   19,    3,  13, 36, 9,   13,
    31, 6,  4,  16, 34, 2,  5,   4,  2,  3,  3,  5,   1,  1,  1,  4,   3,     1,  17, 3,  2,   3,
    5,  3,  1,  3,  2,  3,  5,   6,  3,  12, 11, 1,   3,  1,  2,  26,  7,     12, 7,  2,  14,  3,
    3,  7,  7,  11, 25, 25, 28,  16, 4,  36, 1,  2,   1,  6,  2,  1,   9,     3,  27, 17, 4,   3,
    4,  13, 4,  1,  3,  2,  2,   1,  10, 4,  2,  4,   6,  3,  8,  2,   1,     18, 1,  1,  24,  2,
    2,  4,  33, 2,  3,  63, 7,   1,  6,  40, 7,  3,   4,  4,  2,  4,   15,    18, 1,  16, 1,   1,
    11, 2,  41, 14, 1,  3,  18,  13, 3,  2,  4,  16,  2,  17, 7,  15,  24,    7,  18, 13, 44,  2,
    2,  3,  6,  1,  1,  7,  5,   1,  7,  1,  4,  3,   3,  5,  10, 8,   2,     3,  1,  8,  1,   1,
    27, 4,  2,  1,  12, 1,  2,   1,  10, 6,  1,  6,   7,  5,  2,  3,   7,     11, 5,  11, 3,   6,
    6,  2,  3,  15, 4,  9,  1,   1,  2,  1,  2,  11,  2,  8,  12, 8,   5,     4,  2,  3,  1,   5,
    2,  2,  1,  14, 1,  12, 11,  4,  1,  11, 17, 17,  4,  3,  2,  5,   5,     7,  3,  1,  5,   9,
    9,  8,  2,  5,  6,  6,  13,  13, 2,  1,  2,  6,   1,  2,  2,  49,  4,     9,  1,  2,  10,  16,
    7,  8,  4,  3,  2,  23, 4,   58, 3,  29, 1,  14,  19, 19, 11, 11,  2,     7,  5,  1,  3,   4,
    6,  2,  18, 5,  12, 12, 17,  17, 3,  3,  2,  4,   1,  6,  2,  3,   4,     3,  1,  1,  1,   1,
    5,  1,  1,  9,  1,  3,  1,   3,  6,  1,  8,  1,   1,  2,  6,  4,   14,    3,  1,  4,  11,  4,
    1,  3,  32, 1,  2,  4,  13,  4,  1,  2,  4,  2,   1,  3,  1,  11,  1,     4,  2,  1,  4,   4,
    6,  3,  5,  1,  6,  5,  7,   6,  3,  23, 3,  5,   3,  5,  3,  3,   13,    3,  9,  10, 1,   12,
    10, 2,  3,  18, 13, 7,  160, 52, 4,  2,  2,  3,   2,  14, 5,  4,   12,    4,  6,  4,  1,   20,
    4,  11, 6,  2,  12, 27, 1,   4,  1,  2,  2,  7,   4,  5,  2,  28,  3,     7,  25, 8,  3,   19,
    3,  6,  10, 2,  2,  1,  10,  2,  5,  4,  1,  3,   4,  1,  5,  3,   2,     6,  9,  3,  6,   2,
    16, 3,  3,  16, 4,  5,  5,   3,  2,  1,  2,  16,  15, 8,  2,  6,   21,    2,  4,  1,  22,  5,
    8,  1,  1,  21, 11, 2,  1,   11, 11, 19, 13, 12,  4,  2,  3,  2,   3,     6,  1,  8,  11,  1,
    4,  2,  9,  5,  2,  1,  11,  2,  9,  1,  1,  2,   14, 31, 9,  3,   4,     21, 14, 4,  8,   1,
    7,  2,  2,  2,  5,  1,  4,   20, 3,  3,  4,  10,  1,  11, 9,  8,   2,     1,  4,  5,  14,  12,
    14, 2,  17, 9,  6,  31, 4,   14, 1,  20, 13, 26,  5,  2,  7,  3,   6,     13, 2,  4,  2,   19,
    6,  2,  2,  18, 9,  3,  5,   12, 12, 14, 4,  6,   2,  3,  6,  9,   5,     22, 4,  5,  25,  6,
    4,  8,  5,  2,  6,  27, 2,   35, 2,  16, 3,  7,   8,  8,  6,  6,   5,     9,  17, 2,  20,  6,
    19, 2,  13, 3,  1,  1,  1,   4,  17, 12, 2,  14,  7,  1,  4,  18,  12,    38, 33, 2,  10,  1,
    1,  2,  13, 14, 17, 11, 50,  6,  33, 20, 26, 74,  16, 23, 45, 50,  13,    38, 33, 6,  6,   7,
    4,  4,  2,  1,  3,  2,  5,   8,  7,  8,  9,  3,   11, 21, 9,  13,  1,     3,  10, 6,  7,   1,
    2,  2,  18, 5,  5,  1,  9,   9,  2,  68, 9,  19,  13, 2,  5,  1,   4,     4,  7,  4,  13,  3,
    9,  10, 21, 17, 3,  26, 2,   1,  5,  2,  4,  5,   4,  1,  7,  4,   7,     3,  4,  2,  1,   6,
    1,  1,  20, 4,  1,  9,  2,   2,  1,  3,  3,  2,   3,  2,  1,  1,   1,     20, 2,  3,  1,   6,
    2,  3,  6,  2,  4,  8,  1,   3,  2,  10, 3,  5,   3,  4,  4,  3,   4,     16, 1,  6,  1,   10,
    2,  4,  2,  1,  1,  2,  10,  11, 2,  2,  3,  1,   24, 31, 4,  10,  10,    2,  5,  12, 16,  164,
    15, 4,  16, 7,  9,  15, 19,  17, 1,  2,  1,  1,   5,  1,  1,  1,   1,     1,  3,  1,  4,   3,
    1,  3,  1,  3,  1,  2,  1,   1,  3,  3,  7,  2,   8,  1,  2,  2,   2,     1,  3,  4,  3,   7,
    8,  12, 92, 2,  10, 3,  1,   3,  14, 5,  25, 16,  42, 4,  7,  7,   4,     2,  21, 5,  27,  26,
    27, 21, 25, 30, 31, 2,  1,   5,  13, 3,  22, 5,   6,  6,  11, 9,   12,    1,  5,  9,  7,   5,
    5,  22, 60, 3,  5,  13, 1,   1,  8,  1,  1,  3,   3,  2,  1,  9,   3,     3,  18, 4,  1,   2,
    3,  7,  6,  3,  1,  2,  3,   9,  1,  3,  1,  3,   2,  1,  3,  1,   1,     1,  2,  1,  11,  3,
    1,  6,  9,  1,  3,  2,  3,   1,  2,  1,  5,  1,   1,  4,  3,  4,   1,     2,  2,  4,  4,   1,
    7,  2,  1,  2,  2,  3,  5,   13, 18, 3,  4,  14,  9,  9,  4,  16,  3,     7,  5,  8,  2,   6,
    48, 28, 3,  1,  1,  4,  2,   14, 8,  2,  9,  2,   1,  15, 2,  4,   3,     2,  10, 16, 12,  8,
    7,  1,  1,  3,  1,  1,  1,   2,  7,  4,  1,  6,   4,  38, 39, 16,  23,    7,  15, 15, 3,   2,
    12, 7,  21, 37, 27, 6,  5,   4,  8,  2,  10, 8,   8,  6,  5,  1,   2,     1,  3,  24, 1,   16,
    17, 9,  23, 10, 17, 6,  1,   51, 55, 44, 13, 294, 9,  3,  6,  2,   4,     2,  2,  15, 1,   1,
    1,  13, 21, 17, 68, 14, 8,   9,  4,  1,  4,  9,   3,  11, 7,  1,   1,     1,  5,  6,  3,   2,
    1,  1,  1,  2,  3,  8,  1,   2,  2,  4,  1,  5,   5,  2,  1,  4,   3,     7,  13, 4,  1,   4,
    1,  3,  1,  1,  1,  5,  5,   10, 1,  6,  1,  5,   2,  1,  5,  2,   4,     1,  4,  5,  7,   3,
    18, 2,  9,  11, 32, 4,  3,   3,  2,  4,  7,  11,  16, 9,  11, 8,   13,    38, 32, 8,  4,   2,
    1,  1,  2,  1,  2,  4,  4,   1,  1,  1,  4,  1,   21, 3,  11, 1,   16,    1,  1,  6,  1,   3,
    2,  4,  9,  8,  57, 7,  44,  1,  3,  3,  13, 3,   10, 1,  1,  7,   5,     2,  7,  21, 47,  63,
    3,  15, 4,  7,  1,  16, 1,   1,  2,  8,  2,  3,   42, 15, 4,  1,   29,    7,  22, 10, 3,   78,
    16, 12, 20, 18, 4,  67, 11,  5,  1,  3,  15, 6,   21, 31, 32, 27,  18,    13, 71, 35, 5,   142,
    4,  10, 1,  2,  50, 19, 33,  16, 35, 37, 16, 19,  27, 7,  1,  133, 19,    1,  4,  8,  7,   20,
    1,  4,  4,  1,  10, 3,  1,   6,  1,  2,  51, 5,   40, 15, 24, 43,  22928, 11, 1,  13, 154, 70,
    3,  1,  1,  7,  4,  10, 1,   2,  1,  1,  2,  1,   2,  1,  2,  2,   1,     1,  2,  1,  1,   1,
    1,  1,  2,  1,  1,  1,  1,   1,  1,  1,  1,  1,   1,  1,  1,  1,   2,     1,  1,  1,  3,   2,
    1,  1,  1,  1,  2,  1,  1,
  };
  static AnchorWChar base_ranges[] =  // not zero-terminated
    {
      0x0020,
      0x00FF,  // Basic Latin + Latin Supplement
      0x3000,
      0x30FF,  // CJK Symbols and Punctuations, Hiragana, Katakana
      0x31F0,
      0x31FF,  // Katakana Phonetic Extensions
      0xFF00,
      0xFFEF  // Half-width characters
    };
  static AnchorWChar full_ranges[ANCHOR_ARRAYSIZE(base_ranges) +
                                 ANCHOR_ARRAYSIZE(accumulative_offsets_from_0x4E00) * 2 + 1] = {0};
  if (!full_ranges[0]) {
    memcpy(full_ranges, base_ranges, sizeof(base_ranges));
    UnpackAccumulativeOffsetsIntoRanges(0x4E00,
                                        accumulative_offsets_from_0x4E00,
                                        ANCHOR_ARRAYSIZE(accumulative_offsets_from_0x4E00),
                                        full_ranges + ANCHOR_ARRAYSIZE(base_ranges));
  }
  return &full_ranges[0];
}

const AnchorWChar *AnchorFontAtlas::GetGlyphRangesCyrillic()
{
  static const AnchorWChar ranges[] = {
    0x0020,
    0x00FF,  // Basic Latin + Latin Supplement
    0x0400,
    0x052F,  // Cyrillic + Cyrillic Supplement
    0x2DE0,
    0x2DFF,  // Cyrillic Extended-A
    0xA640,
    0xA69F,  // Cyrillic Extended-B
    0,
  };
  return &ranges[0];
}

const AnchorWChar *AnchorFontAtlas::GetGlyphRangesThai()
{
  static const AnchorWChar ranges[] = {
    0x0020,
    0x00FF,  // Basic Latin
    0x2010,
    0x205E,  // Punctuations
    0x0E00,
    0x0E7F,  // Thai
    0,
  };
  return &ranges[0];
}

const AnchorWChar *AnchorFontAtlas::GetGlyphRangesVietnamese()
{
  static const AnchorWChar ranges[] = {
    0x0020,
    0x00FF,  // Basic Latin
    0x0102,
    0x0103,
    0x0110,
    0x0111,
    0x0128,
    0x0129,
    0x0168,
    0x0169,
    0x01A0,
    0x01A1,
    0x01AF,
    0x01B0,
    0x1EA0,
    0x1EF9,
    0,
  };
  return &ranges[0];
}

//-----------------------------------------------------------------------------
// [SECTION] AnchorFontGlyphRangesBuilder
//-----------------------------------------------------------------------------

void AnchorFontGlyphRangesBuilder::AddText(const char *text, const char *text_end)
{
  while (text_end ? (text < text_end) : *text) {
    unsigned int c = 0;
    int c_len = AnchorTextCharFromUtf8(&c, text, text_end);
    text += c_len;
    if (c_len == 0)
      break;
    AddChar((AnchorWChar)c);
  }
}

void AnchorFontGlyphRangesBuilder::AddRanges(const AnchorWChar *ranges)
{
  for (; ranges[0]; ranges += 2)
    for (AnchorWChar c = ranges[0]; c <= ranges[1]; c++)
      AddChar(c);
}

void AnchorFontGlyphRangesBuilder::BuildRanges(AnchorVector<AnchorWChar> *out_ranges)
{
  const int max_codepoint = IM_UNICODE_CODEPOINT_MAX;
  for (int n = 0; n <= max_codepoint; n++)
    if (GetBit(n)) {
      out_ranges->push_back((AnchorWChar)n);
      while (n < max_codepoint && GetBit(n + 1))
        n++;
      out_ranges->push_back((AnchorWChar)n);
    }
  out_ranges->push_back(0);
}

//-----------------------------------------------------------------------------
// [SECTION] AnchorFont
//-----------------------------------------------------------------------------

AnchorFont::AnchorFont()
{
  FontSize = 0.0f;
  FallbackAdvanceX = 0.0f;
  FallbackChar = (AnchorWChar)'?';
  EllipsisChar = (AnchorWChar)-1;
  FallbackGlyph = NULL;
  ContainerAtlas = NULL;
  ConfigData = NULL;
  ConfigDataCount = 0;
  DirtyLookupTables = false;
  Scale = 1.0f;
  Ascent = Descent = 0.0f;
  MetricsTotalSurface = 0;
  memset(Used4kPagesMap, 0, sizeof(Used4kPagesMap));
}

AnchorFont::~AnchorFont()
{
  ClearOutputData();
}

void AnchorFont::ClearOutputData()
{
  FontSize = 0.0f;
  FallbackAdvanceX = 0.0f;
  Glyphs.clear();
  IndexAdvanceX.clear();
  IndexLookup.clear();
  FallbackGlyph = NULL;
  ContainerAtlas = NULL;
  DirtyLookupTables = true;
  Ascent = Descent = 0.0f;
  MetricsTotalSurface = 0;
}

void AnchorFont::BuildLookupTable()
{
  int max_codepoint = 0;
  for (int i = 0; i != Glyphs.Size; i++)
    max_codepoint = AnchorMax(max_codepoint, (int)Glyphs[i].Codepoint);

  // Build lookup table
  ANCHOR_ASSERT(Glyphs.Size < 0xFFFF);  // -1 is reserved
  IndexAdvanceX.clear();
  IndexLookup.clear();
  DirtyLookupTables = false;
  memset(Used4kPagesMap, 0, sizeof(Used4kPagesMap));
  GrowIndex(max_codepoint + 1);
  for (int i = 0; i < Glyphs.Size; i++) {
    int codepoint = (int)Glyphs[i].Codepoint;
    IndexAdvanceX[codepoint] = Glyphs[i].AdvanceX;
    IndexLookup[codepoint] = (AnchorWChar)i;

    // Mark 4K page as used
    const int page_n = codepoint / 4096;
    Used4kPagesMap[page_n >> 3] |= 1 << (page_n & 7);
  }

  // Create a glyph to handle TAB
  // FIXME: Needs proper TAB handling but it needs to be contextualized (or we could arbitrary say
  // that each string starts at "column 0" ?)
  if (FindGlyph((AnchorWChar)' ')) {
    if (Glyphs.back().Codepoint !=
        '\t')  // So we can call this function multiple times (FIXME: Flaky)
      Glyphs.resize(Glyphs.Size + 1);
    AnchorFontGlyph &tab_glyph = Glyphs.back();
    tab_glyph = *FindGlyph((AnchorWChar)' ');
    tab_glyph.Codepoint = '\t';
    tab_glyph.AdvanceX *= IM_TABSIZE;
    IndexAdvanceX[(int)tab_glyph.Codepoint] = (float)tab_glyph.AdvanceX;
    IndexLookup[(int)tab_glyph.Codepoint] = (AnchorWChar)(Glyphs.Size - 1);
  }

  // Mark special glyphs as not visible (note that AddGlyph already mark as non-visible glyphs with
  // zero-size polygons)
  SetGlyphVisible((AnchorWChar)' ', false);
  SetGlyphVisible((AnchorWChar)'\t', false);

  // Setup fall-backs
  FallbackGlyph = FindGlyphNoFallback(FallbackChar);
  FallbackAdvanceX = FallbackGlyph ? FallbackGlyph->AdvanceX : 0.0f;
  for (int i = 0; i < max_codepoint + 1; i++)
    if (IndexAdvanceX[i] < 0.0f)
      IndexAdvanceX[i] = FallbackAdvanceX;
}

// API is designed this way to avoid exposing the 4K page size
// e.g. use with IsGlyphRangeUnused(0, 255)
bool AnchorFont::IsGlyphRangeUnused(unsigned int c_begin, unsigned int c_last)
{
  unsigned int page_begin = (c_begin / 4096);
  unsigned int page_last = (c_last / 4096);
  for (unsigned int page_n = page_begin; page_n <= page_last; page_n++)
    if ((page_n >> 3) < sizeof(Used4kPagesMap))
      if (Used4kPagesMap[page_n >> 3] & (1 << (page_n & 7)))
        return false;
  return true;
}

void AnchorFont::SetGlyphVisible(AnchorWChar c, bool visible)
{
  if (AnchorFontGlyph *glyph = (AnchorFontGlyph *)(void *)FindGlyph((AnchorWChar)c))
    glyph->Visible = visible ? 1 : 0;
}

void AnchorFont::SetFallbackChar(AnchorWChar c)
{
  FallbackChar = c;
  BuildLookupTable();
}

void AnchorFont::GrowIndex(int new_size)
{
  ANCHOR_ASSERT(IndexAdvanceX.Size == IndexLookup.Size);
  if (new_size <= IndexLookup.Size)
    return;
  IndexAdvanceX.resize(new_size, -1.0f);
  IndexLookup.resize(new_size, (AnchorWChar)-1);
}

// x0/y0/x1/y1 are offset from the character upper-left layout position, in pixels. Therefore x0/y0
// are often fairly close to zero. Not to be mistaken with texture coordinates, which are held by
// u0/v0/u1/v1 in normalized format (0.0..1.0 on each texture axis). 'cfg' is not necessarily ==
// 'this->ConfigData' because multiple source fonts+configs can be used to build one target font.
void AnchorFont::AddGlyph(const AnchorFontConfig *cfg,
                          AnchorWChar codepoint,
                          float x0,
                          float y0,
                          float x1,
                          float y1,
                          float u0,
                          float v0,
                          float u1,
                          float v1,
                          float advance_x)
{
  if (cfg != NULL) {
    // Clamp & recenter if needed
    const float advance_x_original = advance_x;
    advance_x = AnchorClamp(advance_x, cfg->GlyphMinAdvanceX, cfg->GlyphMaxAdvanceX);
    if (advance_x != advance_x_original) {
      float char_off_x = cfg->PixelSnapH ? AnchorFloor((advance_x - advance_x_original) * 0.5f) :
                                           (advance_x - advance_x_original) * 0.5f;
      x0 += char_off_x;
      x1 += char_off_x;
    }

    // Snap to pixel
    if (cfg->PixelSnapH)
      advance_x = IM_ROUND(advance_x);

    // Bake spacing
    advance_x += cfg->GlyphExtraSpacing[0];
  }

  Glyphs.resize(Glyphs.Size + 1);
  AnchorFontGlyph &glyph = Glyphs.back();
  glyph.Codepoint = (unsigned int)codepoint;
  glyph.Visible = (x0 != x1) && (y0 != y1);
  glyph.Colored = false;
  glyph.X0 = x0;
  glyph.Y0 = y0;
  glyph.X1 = x1;
  glyph.Y1 = y1;
  glyph.U0 = u0;
  glyph.V0 = v0;
  glyph.U1 = u1;
  glyph.V1 = v1;
  glyph.AdvanceX = advance_x;

  // Compute rough surface usage metrics (+1 to account for average padding, +0.99 to round)
  // We use (U1-U0)*TexWidth instead of X1-X0 to account for oversampling.
  float pad = ContainerAtlas->TexGlyphPadding + 0.99f;
  DirtyLookupTables = true;
  MetricsTotalSurface += (int)((glyph.U1 - glyph.U0) * ContainerAtlas->TexWidth + pad) *
                         (int)((glyph.V1 - glyph.V0) * ContainerAtlas->TexHeight + pad);
}

void AnchorFont::AddRemapChar(AnchorWChar dst, AnchorWChar src, bool overwrite_dst)
{
  ANCHOR_ASSERT(IndexLookup.Size >
                0);  // Currently this can only be called AFTER the font has been built,
                     // aka after calling AnchorFontAtlas::GetTexDataAs*() function.
  unsigned int index_size = (unsigned int)IndexLookup.Size;

  if (dst < index_size && IndexLookup.Data[dst] == (AnchorWChar)-1 &&
      !overwrite_dst)  // 'dst' already exists
    return;
  if (src >= index_size && dst >= index_size)  // both 'dst' and 'src' don't exist -> no-op
    return;

  GrowIndex(dst + 1);
  IndexLookup[dst] = (src < index_size) ? IndexLookup.Data[src] : (AnchorWChar)-1;
  IndexAdvanceX[dst] = (src < index_size) ? IndexAdvanceX.Data[src] : 1.0f;
}

const AnchorFontGlyph *AnchorFont::FindGlyph(AnchorWChar c) const
{
  if (c >= (size_t)IndexLookup.Size)
    return FallbackGlyph;
  const AnchorWChar i = IndexLookup.Data[c];
  if (i == (AnchorWChar)-1)
    return FallbackGlyph;
  return &Glyphs.Data[i];
}

const AnchorFontGlyph *AnchorFont::FindGlyphNoFallback(AnchorWChar c) const
{
  if (c >= (size_t)IndexLookup.Size)
    return NULL;
  const AnchorWChar i = IndexLookup.Data[c];
  if (i == (AnchorWChar)-1)
    return NULL;
  return &Glyphs.Data[i];
}

const char *AnchorFont::CalcWordWrapPositionA(float scale,
                                              const char *text,
                                              const char *text_end,
                                              float wrap_width) const
{
  // Simple word-wrapping for English, not full-featured. Please submit failing cases!
  // FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within
  // "word,,,,", more sensible support for punctuations, support for Unicode punctuations, etc.)

  // For references, possible wrap point marked with ^
  //  "aaa bbb, ccc,ddd. eee   fff. ggg!"
  //      ^    ^    ^   ^   ^__    ^    ^

  // List of hardcoded separators: .,;!?'"

  // Skip extra blanks after a line returns (that includes not counting them in width computation)
  // e.g. "Hello    world" --> "Hello" "World"

  // Cut words that cannot possibly fit within one line.
  // e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"

  float line_width = 0.0f;
  float word_width = 0.0f;
  float blank_width = 0.0f;
  wrap_width /= scale;  // We work with unscaled widths to avoid scaling every characters

  const char *word_end = text;
  const char *prev_word_end = NULL;
  bool inside_word = true;

  const char *s = text;
  while (s < text_end) {
    unsigned int c = (unsigned int)*s;
    const char *next_s;
    if (c < 0x80)
      next_s = s + 1;
    else
      next_s = s + AnchorTextCharFromUtf8(&c, s, text_end);
    if (c == 0)
      break;

    if (c < 32) {
      if (c == '\n') {
        line_width = word_width = blank_width = 0.0f;
        inside_word = true;
        s = next_s;
        continue;
      }
      if (c == '\r') {
        s = next_s;
        continue;
      }
    }

    const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] :
                                                            FallbackAdvanceX);
    if (AnchorCharIsBlankW(c)) {
      if (inside_word) {
        line_width += blank_width;
        blank_width = 0.0f;
        word_end = s;
      }
      blank_width += char_width;
      inside_word = false;
    } else {
      word_width += char_width;
      if (inside_word) {
        word_end = next_s;
      } else {
        prev_word_end = word_end;
        line_width += word_width + blank_width;
        word_width = blank_width = 0.0f;
      }

      // Allow wrapping after punctuation.
      inside_word = (c != '.' && c != ',' && c != ';' && c != '!' && c != '?' && c != '\"');
    }

    // We ignore blank width at the end of the line (they can be skipped)
    if (line_width + word_width > wrap_width) {
      // Words that cannot possibly fit within an entire line will be cut anywhere.
      if (word_width < wrap_width)
        s = prev_word_end ? prev_word_end : word_end;
      break;
    }

    s = next_s;
  }

  return s;
}

GfVec2f AnchorFont::CalcTextSizeA(float size,
                                  float max_width,
                                  float wrap_width,
                                  const char *text_begin,
                                  const char *text_end,
                                  const char **remaining) const
{
  if (!text_end)
    text_end = text_begin + strlen(text_begin);  // FIXME-OPT: Need to avoid this.

  const float line_height = size;
  const float scale = size / FontSize;

  GfVec2f text_size = GfVec2f(0, 0);
  float line_width = 0.0f;

  const bool word_wrap_enabled = (wrap_width > 0.0f);
  const char *word_wrap_eol = NULL;

  const char *s = text_begin;
  while (s < text_end) {
    if (word_wrap_enabled) {
      // Calculate how far we can render. Requires two passes on the string data but keeps the code
      // simple and not intrusive for what's essentially an uncommon feature.
      if (!word_wrap_eol) {
        word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);
        if (word_wrap_eol == s)  // Wrap_width is too small to fit anything. Force displaying 1
                                 // character to minimize the height discontinuity.
          word_wrap_eol++;  // +1 may not be a character start point in UTF-8 but it's ok because
                            // we use s >= word_wrap_eol below
      }

      if (s >= word_wrap_eol) {
        if (text_size[0] < line_width)
          text_size[0] = line_width;
        text_size[1] += line_height;
        line_width = 0.0f;
        word_wrap_eol = NULL;

        // Wrapping skips upcoming blanks
        while (s < text_end) {
          const char c = *s;
          if (AnchorCharIsBlankA(c)) {
            s++;
          } else if (c == '\n') {
            s++;
            break;
          } else {
            break;
          }
        }
        continue;
      }
    }

    // Decode and advance source
    const char *prev_s = s;
    unsigned int c = (unsigned int)*s;
    if (c < 0x80) {
      s += 1;
    } else {
      s += AnchorTextCharFromUtf8(&c, s, text_end);
      if (c == 0)  // Malformed UTF-8?
        break;
    }

    if (c < 32) {
      if (c == '\n') {
        text_size[0] = AnchorMax(text_size[0], line_width);
        text_size[1] += line_height;
        line_width = 0.0f;
        continue;
      }
      if (c == '\r')
        continue;
    }

    const float char_width = ((int)c < IndexAdvanceX.Size ? IndexAdvanceX.Data[c] :
                                                            FallbackAdvanceX) *
                             scale;
    if (line_width + char_width >= max_width) {
      s = prev_s;
      break;
    }

    line_width += char_width;
  }

  if (text_size[0] < line_width)
    text_size[0] = line_width;

  if (line_width > 0 || text_size[1] == 0.0f)
    text_size[1] += line_height;

  if (remaining)
    *remaining = s;

  return text_size;
}

// Note: as with every AnchorDrawList drawing function, this expects that the font atlas texture is
// bound.
void AnchorFont::RenderChar(AnchorDrawList *draw_list,
                            float size,
                            GfVec2f pos,
                            AnchorU32 col,
                            AnchorWChar c) const
{
  const AnchorFontGlyph *glyph = FindGlyph(c);
  if (!glyph || !glyph->Visible)
    return;
  if (glyph->Colored)
    col |= ~ANCHOR_COL32_A_MASK;
  float scale = (size >= 0.0f) ? (size / FontSize) : 1.0f;
  pos[0] = ANCHOR_FLOOR(pos[0]);
  pos[1] = ANCHOR_FLOOR(pos[1]);
  draw_list->PrimReserve(6, 4);
  draw_list->PrimRectUV(GfVec2f(pos[0] + glyph->X0 * scale, pos[1] + glyph->Y0 * scale),
                        GfVec2f(pos[0] + glyph->X1 * scale, pos[1] + glyph->Y1 * scale),
                        GfVec2f(glyph->U0, glyph->V0),
                        GfVec2f(glyph->U1, glyph->V1),
                        col);
}

// Note: as with every AnchorDrawList drawing function, this expects that the font atlas texture is
// bound.
void AnchorFont::RenderText(AnchorDrawList *draw_list,
                            float size,
                            GfVec2f pos,
                            AnchorU32 col,
                            const GfVec4f &clip_rect,
                            const char *text_begin,
                            const char *text_end,
                            float wrap_width,
                            bool cpu_fine_clip) const
{
  if (!text_end)
    text_end = text_begin +
               strlen(text_begin);  // ANCHOR:: functions generally already provides a valid
                                    // text_end, so this is merely to handle direct calls.

  // Align to be pixel perfect
  pos[0] = ANCHOR_FLOOR(pos[0]);
  pos[1] = ANCHOR_FLOOR(pos[1]);
  float x = pos[0];
  float y = pos[1];
  if (y > clip_rect[3])
    return;

  const float scale = size / FontSize;
  const float line_height = FontSize * scale;
  const bool word_wrap_enabled = (wrap_width > 0.0f);
  const char *word_wrap_eol = NULL;

  // Fast-forward to first visible line
  const char *s = text_begin;
  if (y + line_height < clip_rect[1] && !word_wrap_enabled)
    while (y + line_height < clip_rect[1] && s < text_end) {
      s = (const char *)memchr(s, '\n', text_end - s);
      s = s ? s + 1 : text_end;
      y += line_height;
    }

  // For large text, scan for the last visible line in order to avoid over-reserving in the call to
  // PrimReserve() Note that very large horizontal line will still be affected by the issue (e.g. a
  // one megabyte string buffer without a newline will likely crash atm)
  if (text_end - s > 10000 && !word_wrap_enabled) {
    const char *s_end = s;
    float y_end = y;
    while (y_end < clip_rect[3] && s_end < text_end) {
      s_end = (const char *)memchr(s_end, '\n', text_end - s_end);
      s_end = s_end ? s_end + 1 : text_end;
      y_end += line_height;
    }
    text_end = s_end;
  }
  if (s == text_end)
    return;

  // Reserve vertices for remaining worse case (over-reserving is useful and easily amortized)
  const int vtx_count_max = (int)(text_end - s) * 4;
  const int idx_count_max = (int)(text_end - s) * 6;
  const int idx_expected_size = draw_list->IdxBuffer.Size + idx_count_max;
  draw_list->PrimReserve(idx_count_max, vtx_count_max);

  AnchorDrawVert *vtx_write = draw_list->_VtxWritePtr;
  AnchorDrawIdx *idx_write = draw_list->_IdxWritePtr;
  unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;

  const AnchorU32 col_untinted = col | ~ANCHOR_COL32_A_MASK;

  while (s < text_end) {
    if (word_wrap_enabled) {
      // Calculate how far we can render. Requires two passes on the string data but keeps the code
      // simple and not intrusive for what's essentially an uncommon feature.
      if (!word_wrap_eol) {
        word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos[0]));
        if (word_wrap_eol == s)  // Wrap_width is too small to fit anything. Force displaying 1
                                 // character to minimize the height discontinuity.
          word_wrap_eol++;  // +1 may not be a character start point in UTF-8 but it's ok because
                            // we use s >= word_wrap_eol below
      }

      if (s >= word_wrap_eol) {
        x = pos[0];
        y += line_height;
        word_wrap_eol = NULL;

        // Wrapping skips upcoming blanks
        while (s < text_end) {
          const char c = *s;
          if (AnchorCharIsBlankA(c)) {
            s++;
          } else if (c == '\n') {
            s++;
            break;
          } else {
            break;
          }
        }
        continue;
      }
    }

    // Decode and advance source
    unsigned int c = (unsigned int)*s;
    if (c < 0x80) {
      s += 1;
    } else {
      s += AnchorTextCharFromUtf8(&c, s, text_end);
      if (c == 0)  // Malformed UTF-8?
        break;
    }

    if (c < 32) {
      if (c == '\n') {
        x = pos[0];
        y += line_height;
        if (y > clip_rect[3])
          break;  // break out of main loop
        continue;
      }
      if (c == '\r')
        continue;
    }

    const AnchorFontGlyph *glyph = FindGlyph((AnchorWChar)c);
    if (glyph == NULL)
      continue;

    float char_width = glyph->AdvanceX * scale;
    if (glyph->Visible) {
      // We don't do a second finer clipping test on the Y axis as we've already skipped anything
      // before clip_rect[1] and exit once we pass clip_rect[3]
      float x1 = x + glyph->X0 * scale;
      float x2 = x + glyph->X1 * scale;
      float y1 = y + glyph->Y0 * scale;
      float y2 = y + glyph->Y1 * scale;
      if (x1 <= clip_rect[2] && x2 >= clip_rect[0]) {
        // Render a character
        float u1 = glyph->U0;
        float v1 = glyph->V0;
        float u2 = glyph->U1;
        float v2 = glyph->V1;

        // CPU side clipping used to fit text in their frame when the frame is too small. Only does
        // clipping for axis aligned quads.
        if (cpu_fine_clip) {
          if (x1 < clip_rect[0]) {
            u1 = u1 + (1.0f - (x2 - clip_rect[0]) / (x2 - x1)) * (u2 - u1);
            x1 = clip_rect[0];
          }
          if (y1 < clip_rect[1]) {
            v1 = v1 + (1.0f - (y2 - clip_rect[1]) / (y2 - y1)) * (v2 - v1);
            y1 = clip_rect[1];
          }
          if (x2 > clip_rect[2]) {
            u2 = u1 + ((clip_rect[2] - x1) / (x2 - x1)) * (u2 - u1);
            x2 = clip_rect[2];
          }
          if (y2 > clip_rect[3]) {
            v2 = v1 + ((clip_rect[3] - y1) / (y2 - y1)) * (v2 - v1);
            y2 = clip_rect[3];
          }
          if (y1 >= y2) {
            x += char_width;
            continue;
          }
        }

        // Support for untinted glyphs
        AnchorU32 glyph_col = glyph->Colored ? col_untinted : col;

        // We are NOT calling PrimRectUV() here because non-inlined causes too much overhead in a
        // debug builds. Inlined here:
        {
          idx_write[0] = (AnchorDrawIdx)(vtx_current_idx);
          idx_write[1] = (AnchorDrawIdx)(vtx_current_idx + 1);
          idx_write[2] = (AnchorDrawIdx)(vtx_current_idx + 2);
          idx_write[3] = (AnchorDrawIdx)(vtx_current_idx);
          idx_write[4] = (AnchorDrawIdx)(vtx_current_idx + 2);
          idx_write[5] = (AnchorDrawIdx)(vtx_current_idx + 3);
          vtx_write[0].pos[0] = x1;
          vtx_write[0].pos[1] = y1;
          vtx_write[0].col = glyph_col;
          vtx_write[0].uv[0] = u1;
          vtx_write[0].uv[1] = v1;
          vtx_write[1].pos[0] = x2;
          vtx_write[1].pos[1] = y1;
          vtx_write[1].col = glyph_col;
          vtx_write[1].uv[0] = u2;
          vtx_write[1].uv[1] = v1;
          vtx_write[2].pos[0] = x2;
          vtx_write[2].pos[1] = y2;
          vtx_write[2].col = glyph_col;
          vtx_write[2].uv[0] = u2;
          vtx_write[2].uv[1] = v2;
          vtx_write[3].pos[0] = x1;
          vtx_write[3].pos[1] = y2;
          vtx_write[3].col = glyph_col;
          vtx_write[3].uv[0] = u1;
          vtx_write[3].uv[1] = v2;
          vtx_write += 4;
          vtx_current_idx += 4;
          idx_write += 6;
        }
      }
    }
    x += char_width;
  }

  // Give back unused vertices (clipped ones, blanks) ~ this is essentially a PrimUnreserve()
  // action.
  draw_list->VtxBuffer.Size = (int)(vtx_write -
                                    draw_list->VtxBuffer.Data);  // Same as calling shrink()
  draw_list->IdxBuffer.Size = (int)(idx_write - draw_list->IdxBuffer.Data);
  draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 1].ElemCount -= (idx_expected_size -
                                                                    draw_list->IdxBuffer.Size);
  draw_list->_VtxWritePtr = vtx_write;
  draw_list->_IdxWritePtr = idx_write;
  draw_list->_VtxCurrentIdx = vtx_current_idx;
}

//-----------------------------------------------------------------------------
// [SECTION] ANCHOR Internal Render Helpers
//-----------------------------------------------------------------------------
// Vaguely redesigned to stop accessing ANCHOR global state:
// - RenderArrow()
// - RenderBullet()
// - RenderCheckMark()
// - RenderMouseCursor()
// - RenderArrowPointingAt()
// - RenderRectFilledRangeH()
//-----------------------------------------------------------------------------
// Function in need of a redesign (legacy mess)
// - RenderColorRectWithAlphaCheckerboard()
//-----------------------------------------------------------------------------

// Render an arrow aimed to be aligned with text (p_min is a position in the same space text would
// be positioned). To e.g. denote expanded/collapsed state
void ANCHOR::RenderArrow(AnchorDrawList *draw_list,
                         GfVec2f pos,
                         AnchorU32 col,
                         AnchorDir dir,
                         float scale)
{
  const float h = draw_list->_Data->FontSize * 1.00f;
  float r = h * 0.40f * scale;
  GfVec2f center = pos + GfVec2f(h * 0.50f, h * 0.50f * scale);

  GfVec2f a, b, c;
  switch (dir) {
    case AnchorDir_Up:
    case AnchorDir_Down:
      if (dir == AnchorDir_Up)
        r = -r;
      a = GfVec2f(+0.000f, +0.750f) * r;
      b = GfVec2f(-0.866f, -0.750f) * r;
      c = GfVec2f(+0.866f, -0.750f) * r;
      break;
    case AnchorDir_Left:
    case AnchorDir_Right:
      if (dir == AnchorDir_Left)
        r = -r;
      a = GfVec2f(+0.750f, +0.000f) * r;
      b = GfVec2f(-0.750f, +0.866f) * r;
      c = GfVec2f(-0.750f, -0.866f) * r;
      break;
    case AnchorDir_None:
    case AnchorDir_COUNT:
      ANCHOR_ASSERT(0);
      break;
  }
  draw_list->AddTriangleFilled(center + a, center + b, center + c, col);
}

void ANCHOR::RenderBullet(AnchorDrawList *draw_list, GfVec2f pos, AnchorU32 col)
{
  draw_list->AddCircleFilled(pos, draw_list->_Data->FontSize * 0.20f, col, 8);
}

void ANCHOR::RenderCheckMark(AnchorDrawList *draw_list, GfVec2f pos, AnchorU32 col, float sz)
{
  float thickness = AnchorMax(sz / 5.0f, 1.0f);
  sz -= thickness * 0.5f;
  pos += GfVec2f(thickness * 0.25f, thickness * 0.25f);

  float third = sz / 3.0f;
  float bx = pos[0] + third;
  float by = pos[1] + sz - third * 0.5f;
  draw_list->PathLineTo(GfVec2f(bx - third, by - third));
  draw_list->PathLineTo(GfVec2f(bx, by));
  draw_list->PathLineTo(GfVec2f(bx + third * 2.0f, by - third * 2.0f));
  draw_list->PathStroke(col, 0, thickness);
}

void ANCHOR::RenderMouseCursor(AnchorDrawList *draw_list,
                               GfVec2f pos,
                               float scale,
                               AnchorMouseCursor mouse_cursor,
                               AnchorU32 col_fill,
                               AnchorU32 col_border,
                               AnchorU32 col_shadow)
{
  if (mouse_cursor == ANCHOR_StandardCursorNone)
    return;
  ANCHOR_ASSERT(mouse_cursor > ANCHOR_StandardCursorNone &&
                mouse_cursor < ANCHOR_StandardCursorNumCursors);

  AnchorFontAtlas *font_atlas = draw_list->_Data->Font->ContainerAtlas;
  GfVec2f offset, size, uv[4];
  if (font_atlas->GetMouseCursorTexData(mouse_cursor, &offset, &size, &uv[0], &uv[2])) {
    pos -= offset;
    AnchorTextureID tex_id = font_atlas->TexID;
    draw_list->PushTextureID(tex_id);
    draw_list->AddImage(tex_id,
                        pos + GfVec2f(1, 0) * scale,
                        pos + (GfVec2f(1, 0) + size) * scale,
                        uv[2],
                        uv[3],
                        col_shadow);
    draw_list->AddImage(tex_id,
                        pos + GfVec2f(2, 0) * scale,
                        pos + (GfVec2f(2, 0) + size) * scale,
                        uv[2],
                        uv[3],
                        col_shadow);
    draw_list->AddImage(tex_id, pos, pos + size * scale, uv[2], uv[3], col_border);
    draw_list->AddImage(tex_id, pos, pos + size * scale, uv[0], uv[1], col_fill);
    draw_list->PopTextureID();
  }
}

// Render an arrow. 'pos' is position of the arrow tip. half_sz[0] is length from base to tip.
// half_sz[1] is length on each side.
void ANCHOR::RenderArrowPointingAt(AnchorDrawList *draw_list,
                                   GfVec2f pos,
                                   GfVec2f half_sz,
                                   AnchorDir direction,
                                   AnchorU32 col)
{
  switch (direction) {
    case AnchorDir_Left:
      draw_list->AddTriangleFilled(GfVec2f(pos[0] + half_sz[0], pos[1] - half_sz[1]),
                                   GfVec2f(pos[0] + half_sz[0], pos[1] + half_sz[1]),
                                   pos,
                                   col);
      return;
    case AnchorDir_Right:
      draw_list->AddTriangleFilled(GfVec2f(pos[0] - half_sz[0], pos[1] + half_sz[1]),
                                   GfVec2f(pos[0] - half_sz[0], pos[1] - half_sz[1]),
                                   pos,
                                   col);
      return;
    case AnchorDir_Up:
      draw_list->AddTriangleFilled(GfVec2f(pos[0] + half_sz[0], pos[1] + half_sz[1]),
                                   GfVec2f(pos[0] - half_sz[0], pos[1] + half_sz[1]),
                                   pos,
                                   col);
      return;
    case AnchorDir_Down:
      draw_list->AddTriangleFilled(GfVec2f(pos[0] - half_sz[0], pos[1] - half_sz[1]),
                                   GfVec2f(pos[0] + half_sz[0], pos[1] - half_sz[1]),
                                   pos,
                                   col);
      return;
    case AnchorDir_None:
    case AnchorDir_COUNT:
      break;  // Fix warnings
  }
}

static inline float AnchorAcos01(float x)
{
  if (x <= 0.0f)
    return IM_PI * 0.5f;
  if (x >= 1.0f)
    return 0.0f;
  return AnchorAcos(x);
  // return (-0.69813170079773212f * x * x - 0.87266462599716477f) * x + 1.5707963267948966f; //
  // Cheap approximation, may be enough for what we do.
}

// FIXME: Cleanup and move code to AnchorDrawList.
void ANCHOR::RenderRectFilledRangeH(AnchorDrawList *draw_list,
                                    const AnchorBBox &rect,
                                    AnchorU32 col,
                                    float x_start_norm,
                                    float x_end_norm,
                                    float rounding)
{
  if (x_end_norm == x_start_norm)
    return;
  if (x_start_norm > x_end_norm)
    AnchorSwap(x_start_norm, x_end_norm);

  GfVec2f p0 = GfVec2f(AnchorLerp(rect.Min[0], rect.Max[0], x_start_norm), rect.Min[1]);
  GfVec2f p1 = GfVec2f(AnchorLerp(rect.Min[0], rect.Max[0], x_end_norm), rect.Max[1]);
  if (rounding == 0.0f) {
    draw_list->AddRectFilled(p0, p1, col, 0.0f);
    return;
  }

  rounding = AnchorClamp(
    AnchorMin((rect.Max[0] - rect.Min[0]) * 0.5f, (rect.Max[1] - rect.Min[1]) * 0.5f) - 1.0f,
    0.0f,
    rounding);
  const float inv_rounding = 1.0f / rounding;
  const float arc0_b = AnchorAcos01(1.0f - (p0[0] - rect.Min[0]) * inv_rounding);
  const float arc0_e = AnchorAcos01(1.0f - (p1[0] - rect.Min[0]) * inv_rounding);
  const float half_pi = IM_PI * 0.5f;  // We will == compare to this because we know this is the
                                       // exact value AnchorAcos01 can return.
  const float x0 = AnchorMax(p0[0], rect.Min[0] + rounding);
  if (arc0_b == arc0_e) {
    draw_list->PathLineTo(GfVec2f(x0, p1[1]));
    draw_list->PathLineTo(GfVec2f(x0, p0[1]));
  } else if (arc0_b == 0.0f && arc0_e == half_pi) {
    draw_list->PathArcToFast(GfVec2f(x0, p1[1] - rounding), rounding, 3, 6);  // BL
    draw_list->PathArcToFast(GfVec2f(x0, p0[1] + rounding), rounding, 6, 9);  // TR
  } else {
    draw_list->PathArcTo(GfVec2f(x0, p1[1] - rounding),
                         rounding,
                         IM_PI - arc0_e,
                         IM_PI - arc0_b,
                         3);  // BL
    draw_list->PathArcTo(GfVec2f(x0, p0[1] + rounding),
                         rounding,
                         IM_PI + arc0_b,
                         IM_PI + arc0_e,
                         3);  // TR
  }
  if (p1[0] > rect.Min[0] + rounding) {
    const float arc1_b = AnchorAcos01(1.0f - (rect.Max[0] - p1[0]) * inv_rounding);
    const float arc1_e = AnchorAcos01(1.0f - (rect.Max[0] - p0[0]) * inv_rounding);
    const float x1 = AnchorMin(p1[0], rect.Max[0] - rounding);
    if (arc1_b == arc1_e) {
      draw_list->PathLineTo(GfVec2f(x1, p0[1]));
      draw_list->PathLineTo(GfVec2f(x1, p1[1]));
    } else if (arc1_b == 0.0f && arc1_e == half_pi) {
      draw_list->PathArcToFast(GfVec2f(x1, p0[1] + rounding), rounding, 9, 12);  // TR
      draw_list->PathArcToFast(GfVec2f(x1, p1[1] - rounding), rounding, 0, 3);   // BR
    } else {
      draw_list->PathArcTo(GfVec2f(x1, p0[1] + rounding), rounding, -arc1_e, -arc1_b, 3);  // TR
      draw_list->PathArcTo(GfVec2f(x1, p1[1] - rounding), rounding, +arc1_b, +arc1_e, 3);  // BR
    }
  }
  draw_list->PathFillConvex(col);
}

void ANCHOR::RenderRectFilledWithHole(AnchorDrawList *draw_list,
                                      AnchorBBox outer,
                                      AnchorBBox inner,
                                      AnchorU32 col,
                                      float rounding)
{
  const bool fill_L = (inner.Min[0] > outer.Min[0]);
  const bool fill_R = (inner.Max[0] < outer.Max[0]);
  const bool fill_U = (inner.Min[1] > outer.Min[1]);
  const bool fill_D = (inner.Max[1] < outer.Max[1]);
  if (fill_L)
    draw_list->AddRectFilled(GfVec2f(outer.Min[0], inner.Min[1]),
                             GfVec2f(inner.Min[0], inner.Max[1]),
                             col,
                             rounding,
                             (fill_U ? 0 : AnchorDrawFlags_RoundCornersTopLeft) |
                               (fill_D ? 0 : AnchorDrawFlags_RoundCornersBottomLeft));
  if (fill_R)
    draw_list->AddRectFilled(GfVec2f(inner.Max[0], inner.Min[1]),
                             GfVec2f(outer.Max[0], inner.Max[1]),
                             col,
                             rounding,
                             (fill_U ? 0 : AnchorDrawFlags_RoundCornersTopRight) |
                               (fill_D ? 0 : AnchorDrawFlags_RoundCornersBottomRight));
  if (fill_U)
    draw_list->AddRectFilled(GfVec2f(inner.Min[0], outer.Min[1]),
                             GfVec2f(inner.Max[0], inner.Min[1]),
                             col,
                             rounding,
                             (fill_L ? 0 : AnchorDrawFlags_RoundCornersTopLeft) |
                               (fill_R ? 0 : AnchorDrawFlags_RoundCornersTopRight));
  if (fill_D)
    draw_list->AddRectFilled(GfVec2f(inner.Min[0], inner.Max[1]),
                             GfVec2f(inner.Max[0], outer.Max[1]),
                             col,
                             rounding,
                             (fill_L ? 0 : AnchorDrawFlags_RoundCornersBottomLeft) |
                               (fill_R ? 0 : AnchorDrawFlags_RoundCornersBottomRight));
  if (fill_L && fill_U)
    draw_list->AddRectFilled(GfVec2f(outer.Min[0], outer.Min[1]),
                             GfVec2f(inner.Min[0], inner.Min[1]),
                             col,
                             rounding,
                             AnchorDrawFlags_RoundCornersTopLeft);
  if (fill_R && fill_U)
    draw_list->AddRectFilled(GfVec2f(inner.Max[0], outer.Min[1]),
                             GfVec2f(outer.Max[0], inner.Min[1]),
                             col,
                             rounding,
                             AnchorDrawFlags_RoundCornersTopRight);
  if (fill_L && fill_D)
    draw_list->AddRectFilled(GfVec2f(outer.Min[0], inner.Max[1]),
                             GfVec2f(inner.Min[0], outer.Max[1]),
                             col,
                             rounding,
                             AnchorDrawFlags_RoundCornersBottomLeft);
  if (fill_R && fill_D)
    draw_list->AddRectFilled(GfVec2f(inner.Max[0], inner.Max[1]),
                             GfVec2f(outer.Max[0], outer.Max[1]),
                             col,
                             rounding,
                             AnchorDrawFlags_RoundCornersBottomRight);
}

// Helper for ColorPicker4()
// NB: This is rather brittle and will show artifact when rounding this enabled if rounded corners
// overlap multiple cells. Caller currently responsible for avoiding that. Spent a non reasonable
// amount of time trying to getting this right for ColorButton with
// rounding+anti-aliasing+AnchorColorEditFlags_HalfAlphaPreview flag + various grid sizes and
// offsets, and eventually gave up... probably more reasonable to disable rounding altogether.
// FIXME: uses ANCHOR::GetColorU32
void ANCHOR::RenderColorRectWithAlphaCheckerboard(AnchorDrawList *draw_list,
                                                  GfVec2f p_min,
                                                  GfVec2f p_max,
                                                  AnchorU32 col,
                                                  float grid_step,
                                                  GfVec2f grid_off,
                                                  float rounding,
                                                  AnchorDrawFlags flags)
{
  if ((flags & AnchorDrawFlags_RoundCornersMask_) == 0)
    flags = AnchorDrawFlags_RoundCornersDefault_;
  if (((col & ANCHOR_COL32_A_MASK) >> ANCHOR_COL32_A_SHIFT) < 0xFF) {
    AnchorU32 col_bg1 = GetColorU32(ImAlphaBlendColors(ANCHOR_COL32(204, 204, 204, 255), col));
    AnchorU32 col_bg2 = GetColorU32(ImAlphaBlendColors(ANCHOR_COL32(128, 128, 128, 255), col));
    draw_list->AddRectFilled(p_min, p_max, col_bg1, rounding, flags);

    int yi = 0;
    for (float y = p_min[1] + grid_off[1]; y < p_max[1]; y += grid_step, yi++) {
      float y1 = AnchorClamp(y, p_min[1], p_max[1]), y2 = AnchorMin(y + grid_step, p_max[1]);
      if (y2 <= y1)
        continue;
      for (float x = p_min[0] + grid_off[0] + (yi & 1) * grid_step; x < p_max[0];
           x += grid_step * 2.0f) {
        float x1 = AnchorClamp(x, p_min[0], p_max[0]), x2 = AnchorMin(x + grid_step, p_max[0]);
        if (x2 <= x1)
          continue;
        AnchorDrawFlags cell_flags = AnchorDrawFlags_RoundCornersNone;
        if (y1 <= p_min[1]) {
          if (x1 <= p_min[0])
            cell_flags |= AnchorDrawFlags_RoundCornersTopLeft;
          if (x2 >= p_max[0])
            cell_flags |= AnchorDrawFlags_RoundCornersTopRight;
        }
        if (y2 >= p_max[1]) {
          if (x1 <= p_min[0])
            cell_flags |= AnchorDrawFlags_RoundCornersBottomLeft;
          if (x2 >= p_max[0])
            cell_flags |= AnchorDrawFlags_RoundCornersBottomRight;
        }

        // Combine flags
        cell_flags = (flags == AnchorDrawFlags_RoundCornersNone ||
                      cell_flags == AnchorDrawFlags_RoundCornersNone) ?
                       AnchorDrawFlags_RoundCornersNone :
                       (cell_flags & flags);
        draw_list->AddRectFilled(GfVec2f(x1, y1), GfVec2f(x2, y2), col_bg2, rounding, cell_flags);
      }
    }
  } else {
    draw_list->AddRectFilled(p_min, p_max, col, rounding, flags);
  }
}

//-----------------------------------------------------------------------------
// [SECTION] Decompression code
//-----------------------------------------------------------------------------
// Compressed with stb_compress() then converted to a C array and encoded as base85.
// Use the program in misc/fonts/binary_to_compressed_c.cpp to create the array from a TTF file.
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_
// size. Decompression from stb.h (public domain) by Sean Barrett
// https://github.com/nothings/stb/blob/master/stb.h
//-----------------------------------------------------------------------------

static unsigned int stb_decompress_length(const unsigned char *input)
{
  return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char *stb__barrier_out_e, *stb__barrier_out_b;
static const unsigned char *stb__barrier_in_b;
static unsigned char *stb__dout;
static void stb__match(const unsigned char *data, unsigned int length)
{
  // INVERSE of memmove... write each byte before copying the next...
  ANCHOR_ASSERT(stb__dout + length <= stb__barrier_out_e);
  if (stb__dout + length > stb__barrier_out_e) {
    stb__dout += length;
    return;
  }
  if (data < stb__barrier_out_b) {
    stb__dout = stb__barrier_out_e + 1;
    return;
  }
  while (length--)
    *stb__dout++ = *data++;
}

static void stb__lit(const unsigned char *data, unsigned int length)
{
  ANCHOR_ASSERT(stb__dout + length <= stb__barrier_out_e);
  if (stb__dout + length > stb__barrier_out_e) {
    stb__dout += length;
    return;
  }
  if (data < stb__barrier_in_b) {
    stb__dout = stb__barrier_out_e + 1;
    return;
  }
  memcpy(stb__dout, data, length);
  stb__dout += length;
}

#define stb__in2(x) ((i[x] << 8) + i[(x) + 1])
#define stb__in3(x) ((i[x] << 16) + stb__in2((x) + 1))
#define stb__in4(x) ((i[x] << 24) + stb__in3((x) + 1))

static const unsigned char *stb_decompress_token(const unsigned char *i)
{
  if (*i >= 0x20) {  // use fewer if's for cases that expand small
    if (*i >= 0x80)
      stb__match(stb__dout - i[1] - 1, i[0] - 0x80 + 1), i += 2;
    else if (*i >= 0x40)
      stb__match(stb__dout - (stb__in2(0) - 0x4000 + 1), i[2] + 1), i += 3;
    else /* *i >= 0x20 */
      stb__lit(i + 1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
  } else {  // more ifs for cases that expand large, since overhead is amortized
    if (*i >= 0x18)
      stb__match(stb__dout - (stb__in3(0) - 0x180000 + 1), i[3] + 1), i += 4;
    else if (*i >= 0x10)
      stb__match(stb__dout - (stb__in3(0) - 0x100000 + 1), stb__in2(3) + 1), i += 5;
    else if (*i >= 0x08)
      stb__lit(i + 2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
    else if (*i == 0x07)
      stb__lit(i + 3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
    else if (*i == 0x06)
      stb__match(stb__dout - (stb__in3(1) + 1), i[4] + 1), i += 5;
    else if (*i == 0x04)
      stb__match(stb__dout - (stb__in3(1) + 1), stb__in2(4) + 1), i += 6;
  }
  return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char *buffer, unsigned int buflen)
{
  const unsigned long ADLER_MOD = 65521;
  unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
  unsigned long blocklen = buflen % 5552;

  unsigned long i;
  while (buflen) {
    for (i = 0; i + 7 < blocklen; i += 8) {
      s1 += buffer[0], s2 += s1;
      s1 += buffer[1], s2 += s1;
      s1 += buffer[2], s2 += s1;
      s1 += buffer[3], s2 += s1;
      s1 += buffer[4], s2 += s1;
      s1 += buffer[5], s2 += s1;
      s1 += buffer[6], s2 += s1;
      s1 += buffer[7], s2 += s1;

      buffer += 8;
    }

    for (; i < blocklen; ++i)
      s1 += *buffer++, s2 += s1;

    s1 %= ADLER_MOD, s2 %= ADLER_MOD;
    buflen -= blocklen;
    blocklen = 5552;
  }
  return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

static unsigned int stb_decompress(unsigned char *output,
                                   const unsigned char *i,
                                   unsigned int /*length*/)
{
  if (stb__in4(0) != 0x57bC0000)
    return 0;
  if (stb__in4(4) != 0)
    return 0;  // error! stream is > 4GB
  const unsigned int olen = stb_decompress_length(i);
  stb__barrier_in_b = i;
  stb__barrier_out_e = output + olen;
  stb__barrier_out_b = output;
  i += 16;

  stb__dout = output;
  for (;;) {
    const unsigned char *old_i = i;
    i = stb_decompress_token(i);
    if (i == old_i) {
      if (*i == 0x05 && i[1] == 0xfa) {
        ANCHOR_ASSERT(stb__dout == output + olen);
        if (stb__dout != output + olen)
          return 0;
        if (stb_adler32(1, output, olen) != (unsigned int)stb__in4(2))
          return 0;
        return olen;
      } else {
        ANCHOR_ASSERT(0); /* NOTREACHED */
        return 0;
      }
    }
    ANCHOR_ASSERT(stb__dout <= output + olen);
    if (stb__dout > output + olen)
      return 0;
  }
}

//-----------------------------------------------------------------------------
// [SECTION] Default font data (ProggyClean.ttf)
//-----------------------------------------------------------------------------
// ProggyClean.ttf
// Copyright (c) 2004, 2005 Tristan Grimmer
// MIT license (see License.txt in http://www.upperbounds.net/download/ProggyClean.ttf.zip)
// Download and more information at http://upperbounds.net
//-----------------------------------------------------------------------------
// File: 'ProggyClean.ttf' (41208 bytes)
// Exported using misc/fonts/binary_to_compressed_c.cpp (with compression + base85 string
// encoding). The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on
// _source code_ size.
//-----------------------------------------------------------------------------
static const char proggy_clean_ttf_compressed_data_base85[11980 + 1] =
  "7])#######hV0qs'/###[),##/l:$#Q6>##5[n42>c-TH`->>#/"
  "e>11NNV=Bv(*:.F?uu#(gRU.o0XGH`$vhLG1hxt9?W`#,5LsCp#-i>.r$<$6pD>Lb';9Crc6tgXmKVeU2cD4Eo3R/"
  "2*>]b(MC;$jPfY.;h^`IWM9<Lh2TlS+f-s$o6Q<BWH`YiU.xfLq$N;$0iR/GX:U(jcW2p/"
  "W*q?-qmnUCI;jHSAiFWM.R*kU@C=GH?a9wp8f$e.-4^Qg1)Q-GL(lf(r/7GrRgwV%MS=C#"
  "`8ND>Qo#t'X#(v#Y9w0#1D$CIf;W'#pWUPXOuxXuU(H9M(1<q-UE31#^-V'8IRUo7Qf./"
  "L>=Ke$$'5F%)]0^#0X@U.a<r:QLtFsLcL6##lOj)#.Y5<-R&KgLwqJfLgN&;Q?gI^#DY2uL"
  "i@^rMl9t=cWq6##weg>$FBjVQTSDgEKnIS7EM9>ZY9w0#L;>>#Mx&4Mvt//"
  "L[MkA#W@lK.N'[0#7RL_&#w+F%HtG9M#XL`N&.,GM4Pg;-<nLENhvx>-VsM.M0rJfLH2eTM`*oJMHRC`N"
  "kfimM2J,W-jXS:)r0wK#@Fge$U>`w'N7G#$#fB#$E^$#:9:hk+eOe--6x)F7*E%?76%^GMHePW-Z5l'&GiF#$956:rS?"
  "dA#fiK:)Yr+`&#0j@'DbG&#^$PG.Ll+DNa<XCMKEV*N)LN/N"
  "*b=%Q6pia-Xg8I$<MR&,VdJe$<(7G;Ckl'&hF;;$<_=X(b.RS%%)###MPBuuE1V:v&cX&#2m#(&cV]`k9OhLMbn%s$G2,"
  "B$BfD3X*sp5#l,$R#]x_X1xKX%b5U*[r5iMfUo9U`N99hG)"
  "tm+/Us9pG)XPu`<0s-)WTt(gCRxIg(%6sfh=ktMKn3j)<6<b5Sk_/0(^]AaN#(p/"
  "L>&VZ>1i%h1S9u5o@YaaW$e+b<TWFn/Z:Oh(Cx2$lNEoN^e)#CFY@@I;BOQ*sRwZtZxRcU7uW6CX"
  "ow0i(?$Q[cjOd[P4d)]>ROPOpxTO7Stwi1::iB1q)C_=dV26J;2,]7op$]uQr@_V7$q^%lQwtuHY]=DX,n3L#"
  "0PHDO4f9>dC@O>HBuKPpP*E,N+b3L#lpR/MrTEH.IAQk.a>D[.e;mc."
  "x]Ip.PH^'/aqUO/$1WxLoW0[iLA<QT;5HKD+@qQ'NQ(3_PLhE48R.qAPSwQ0/WK?Z,[x?-J;jQTWA0X@KJ(_Y8N-:/"
  "M74:/-ZpKrUss?d#dZq]DAbkU*JqkL+nwX@@47`5>w=4h(9.`G"
  "CRUxHPeR`5Mjol(dUWxZa(>STrPkrJiWx`5U7F#.g*jrohGg`cg:lSTvEY/"
  "EV_7H4Q9[Z%cnv;JQYZ5q.l7Zeas:HOIZOB?G<Nald$qs]@]L<J7bR*>gv:[7MI2k).'2($5FNP&EQ(,)"
  "U]W]+fh18.vsai00);D3@4ku5P?DP8aJt+;qUM]=+b'8@;mViBKx0DE[-auGl8:PJ&Dj+M6OC]O^((##]`0i)drT;-7X`"
  "=-H3[igUnPG-NZlo.#k@h#=Ork$m>a>$-?Tm$UV(?#P6YY#"
  "'/###xe7q.73rI3*pP/$1>s9)W,JrM7SN]'/"
  "4C#v$U`0#V.[0>xQsH$fEmPMgY2u7Kh(G%siIfLSoS+MK2eTM$=5,M8p`A.;_R%#u[K#$x4AG8.kK/HSB==-'Ie/"
  "QTtG?-.*^N-4B/ZM"
  "_3YlQC7(p7q)&](`6_c)$/*JL(L-^(]$wIM`dPtOdGA,U3:w2M-0<q-]L_?^)1vw'.,MRsqVr.L;aN&#/"
  "EgJ)PBc[-f>+WomX2u7lqM2iEumMTcsF?-aT=Z-97UEnXglEn1K-bnEO`gu"
  "Ft(c%=;Am_Qs@jLooI&NX;]0#j4#F14;gl8-GQpgwhrq8'=l_f-b49'UOqkLu7-##oDY2L(te+Mch&gLYtJ,MEtJfLh'"
  "x'M=$CS-ZZ%P]8bZ>#S?YY#%Q&q'3^Fw&?D)UDNrocM3A76/"
  "/oL?#h7gl85[qW/"
  "NDOk%16ij;+:1a'iNIdb-ou8.P*w,v5#EI$TWS>Pot-R*H'-SEpA:g)f+O$%%`kA#G=8RMmG1&O`>to8bC]T&$,n.LoO>"
  "29sp3dt-52U%VM#q7'DHpg+#Z9%H[K<L"
  "%a2E-grWVM3@2=-k22tL]4$##6We'8UJCKE[d_=%wI;'6X-GsLX4j^SgJ$##R*w,vP3wK#iiW&#*h^D&R?jp7+/"
  "u&#(AP##XU8c$fSYW-J95_-Dp[g9wcO&#M-h1OcJlc-*vpw0xUX&#"
  "OQFKNX@QI'IoPp7nb,QU//"
  "MQ&ZDkKP)X<WSVL(68uVl&#c'[0#(s1X&xm$Y%B7*K:eDA323j998GXbA#pwMs-jgD$9QISB-A_(aN4xoFM^@C58D0+Q+"
  "q3n0#3U1InDjF682-SjMXJK)("
  "h$hxua_K]ul92%'BOU&#BRRh-slg8KDlr:%L71Ka:.A;%YULjDPmL<LYs8i#XwJOYaKPKc1h:'9Ke,g)b),78=I39B;"
  "xiY$bgGw-&.Zi9InXDuYa%G*f2Bq7mn9^#p1vv%#(Wi-;/Z5h"
  "o;#2:;%d&#x9v68C5g?ntX0X)pT`;%pB3q7mgGN)3%(P8nTd5L7GeA-GL@+%J3u2:(Yf>et`e;)f#Km8&+DC$I46>#Kr]"
  "]u-[=99tts1.qb#q72g1WJO81q+eN'03'eM>&1XxY-caEnO"
  "j%2n8)),?ILR5^.Ibn<-X-Mq7[a82Lq:F&#ce+S9wsCK*x`569E8ew'He]h:sI[2LM$[guka3ZRd6:t%IG:;$%YiJ:Nq="
  "?eAw;/:nnDq0(CYcMpG)qLN4$##&J<j$UpK<Q4a1]MupW^-"
  "sj_$%[HK%'F####QRZJ::Y3EGl4'@%FkiAOg#p[##O`gukTfBHagL<LHw%q&OV0##F=6/"
  ":chIm0@eCP8X]:kFI%hl8hgO@RcBhS-@Qb$%+m=hPDLg*%K8ln(wcf3/'DW-$.lR?n[nCH-"
  "eXOONTJlh:.RYF%3'p6sq:UIMA945&^HFS87@$EP2iG<-lCO$%c`uKGD3rC$x0BL8aFn--`ke%#HMP'vh1/"
  "R&O_J9'um,.<tx[@%wsJk&bUT2`0uMv7gg#qp/ij.L56'hl;.s5CUrxjO"
  "M7-##.l+Au'A&O:-T72L]P`&=;ctp'XScX*rU.>-XTt,%OVU4)S1+R-#dg0/"
  "Nn?Ku1^0f$B*P:Rowwm-`0PKjYDDM'3]d39VZHEl4,.j']Pk-M.h^&:0FACm$maq-&sgw0t7/6(^xtk%"
  "LuH88Fj-ekm>GA#_>568x6(OFRl-IZp`&b,_P'$M<Jnq79VsJW/mWS*PUiq76;]/NM_>hLbxfc$mj`,O;&%W2m`Zh:/"
  ")Uetw:aJ%]K9h:TcF]u_-Sj9,VK3M.*'&0D[Ca]J9gp8,kAW]"
  "%(?A%R$f<->Zts'^kn=-^@c4%-pY6qI%J%1IGxfLU9CP8cbPlXv);C=b),<2mOvP8up,UVf3839acAWAW-W?#ao/"
  "^#%KYo8fRULNd2.>%m]UK:n%r$'sw]J;5pAoO_#2mO3n,'=H5(et"
  "Hg*`+RLgv>=4U8guD$I%D:W>-r5V*%j*W:Kvej.Lp$<M-SGZ':+Q_k+uvOSLiEo(<aD/"
  "K<CCc`'Lx>'?;++O'>()jLR-^u68PHm8ZFWe+ej8h:9r6L*0//c&iH&R8pRbA#Kjm%upV1g:"
  "a_#Ur7FuA#(tRh#.Y5K+@?3<-8m0$PEn;J:rh6?I6uG<-`wMU'ircp0LaE_OtlMb&1#6T.#FDKu#1Lw%u%+GM+X'e?"
  "YLfjM[VO0MbuFp7;>Q&#WIo)0@F%q7c#4XAXN-U&VB<HFF*qL("
  "$/V,;(kXZejWO`<[5?\?ewY(*9=%wDc;,u<'9t3W-(H1th3+G]ucQ]kLs7df($/"
  "*JL]@*t7Bu_G3_7mp7<iaQjO@.kLg;x3B0lqp7Hf,^Ze7-##@/c58Mo(3;knp0%)A7?-W+eI'o8)b<"
  "nKnw'Ho8C=Y>pqB>0ie&jhZ[?iLR@@_AvA-iQC(=ksRZRVp7`.=+NpBC%rh&3]R:8XDmE5^V8O(x<<aG/"
  "1N$#FX$0V5Y6x'aErI3I$7x%E`v<-BY,)%-?Psf*l?%C3.mM(=/M0:JxG'?"
  "7WhH%o'a<-80g0NBxoO(GH<dM]n.+%q@jH?f.UsJ2Ggs&4<-e47&Kl+f//"
  "9@`b+?.TeN_&B8Ss?v;^Trk;f#YvJkl&w$]>-+k?'(<S:68tq*WoDfZu';mM?8X[ma8W%*`-=;D.(nc7/;"
  ")g:T1=^J$&BRV(-lTmNB6xqB[@0*o.erM*<SWF]u2=st-*(6v>^](H.aREZSi,#1:[IXaZFOm<-ui#qUq2$##Ri;"
  "u75OK#(RtaW-K-F`S+cF]uN`-KMQ%rP/Xri.LRcB##=YL3BgM/3M"
  "D?@f&1'BW-)Ju<L25gl8uhVm1hL$##*8###'A3/"
  "LkKW+(^rWX?5W_8g)a(m&K8P>#bmmWCMkk&#TR`C,5d>g)F;t,4:@_l8G/"
  "5h4vUd%&%950:VXD'QdWoY-F$BtUwmfe$YqL'8(PWX("
  "P?^@Po3$##`MSs?DWBZ/S>+4%>fX,VWv/w'KD`LP5IbH;rTV>n3cEK8U#bX]l-/"
  "V+^lj3;vlMb&[5YQ8#pekX9JP3XUC72L,,?+Ni&co7ApnO*5NK,((W-i:$,kp'UDAO(G0Sq7MVjJs"
  "bIu)'Z,*[>br5fX^:FPAWr-m2KgL<LUN098kTF&#lvo58=/vjDo;.;)Ka*hLR#/"
  "k=rKbxuV`>Q_nN6'8uTG&#1T5g)uLv:873UpTLgH+#FgpH'_o1780Ph8KmxQJ8#H72L4@768@Tm&Q"
  "h4CB/5OvmA&,Q&QbUoi$a_%3M01H)4x7I^&KQVgtFnV+;[Pc>[m4k//"
  ",]1?#`VY[Jr*3&&slRfLiVZJ:]?=K3Sw=[$=uRB?3xk48@aeg<Z'<$#4H)6,>e0jT6'N#(q%.O=?2S]u*(m<-"
  "V8J'(1)G][68hW$5'q[GC&5j`TE?m'esFGNRM)j,ffZ?-qx8;->g4t*:CIP/[Qap7/"
  "9'#(1sao7w-.qNUdkJ)tCF&#B^;xGvn2r9FEPFFFcL@.iFNkTve$m%#QvQS8U@)2Z+3K:AKM5i"
  "sZ88+dKQ)W6>J%CL<KE>`.d*(B`-n8D9oK<Up]c$X$(,)M8Zt7/"
  "[rdkqTgl-0cuGMv'?>-XV1q['-5k'cAZ69e;D_?$ZPP&s^+7])$*$#@QYi9,5P&#9r+$%CE=68>K8r0=dSC%%(@p7"
  ".m7jilQ02'0-VWAg<a/''3u.=4L$Y)6k/K:_[3=&jvL<L0C/"
  "2'v:^;-DIBW,B4E68:kZ;%?8(Q8BH=kO65BW?xSG&#@uU,DS*,?.+(o(#1vCS8#CHF>TlGW'b)Tq7VT9q^*^$$.:&N@@"
  "$&)WHtPm*5_rO0&e%K&#-30j(E4#'Zb.o/"
  "(Tpm$>K'f@[PvFl,hfINTNU6u'0pao7%XUp9]5.>%h`8_=VYbxuel.NTSsJfLacFu3B'lQSu/m6-Oqem8T+oE--$0a/"
  "k]uj9EwsG>%veR*"
  "hv^BFpQj:K'#SJ,sB-'#](j.Lg92rTw-*n%@/;39rrJF,l#qV%OrtBeC6/"
  ",;qB3ebNW[?,Hqj2L.1NP&GjUR=1D8QaS3Up&@*9wP?+lo7b?@%'k4`p0Z$22%K3+iCZj?XJN4Nm&+YF]u"
  "@-W$U%VEQ/,,>>#)D<h#`)h0:<Q6909ua+&VU%n2:cG3FJ-%@Bj-DgLr`Hw&HAKjKjseK</"
  "xKT*)B,N9X3]krc12t'pgTV(Lv-tL[xg_%=M_q7a^x?7Ubd>#%8cY#YZ?=,`Wdxu/ae&#"
  "w6)R89tI#6@s'(6Bf7a&?S=^ZI_kS&ai`&=tE72L_D,;^R)7[$s<Eh#c&)q.MXI%#v9ROa5FZO%sF7q7Nwb&#ptUJ:"
  "aqJe$Sl68%.D###EC><?-aF&#RNQv>o8lKN%5/$(vdfq7+ebA#"
  "u1p]ovUKW&Y%q]'>$1@-[xfn$7ZTp7mM,G,Ko7a&Gu%G[RMxJs[0MM%wci.LFDK)(<c`Q8N)jEIF*+?P2a8g%)$q]"
  "o2aH8C&<SibC/q,(e:v;-b#6[$NtDZ84Je2KNvB#$P5?tQ3nt(0"
  "d=j.LQf./"
  "Ll33+(;q3L-w=8dX$#WF&uIJ@-bfI>%:_i2B5CsR8&9Z&#=mPEnm0f`<&c)QL5uJ#%u%lJj+D-r;BoF&#4DoS97h5g)E#"
  "o:&S4weDF,9^Hoe`h*L+_a*NrLW-1pG_&2UdB8"
  "6e%B/:=>)N4xeW.*wft-;$'58-ESqr<b?UI(_%@[P46>#U`'6AQ]m&6/"
  "`Z>#S?YY#Vc;r7U2&326d=w&H####?TZ`*4?&.MK?LP8Vxg>$[QXc%QJv92.(Db*B)gb*BM9dM*hJMAo*c&#"
  "b0v=Pjer]$gG&JXDf->'StvU7505l9$AFvgYRI^&<^b68?j#q9QX4SM'RO#&sL1IM.rJfLUAj221]d##DW=m83u5;'"
  "bYx,*Sl0hL(W;;$doB&O/TQ:(Z^xBdLjL<Lni;''X.`$#8+1GD"
  ":k$YUWsbn8ogh6rxZ2Z9]%nd+>V#*8U_72Lh+2Q8Cj0i:6hp&$C/"
  ":p(HK>T8Y[gHQ4`4)'$Ab(Nof%V'8hL&#<NEdtg(n'=S1A(Q1/I&4([%dM`,Iu'1:_hL>SfD07&6D<fp8dHM7/g+"
  "tlPN9J*rKaPct&?'uBCem^jn%9_K)<,C5K3s=5g&GmJb*[SYq7K;TRLGCsM-$$;S%:Y@r7AK0pprpL<Lrh,q7e/"
  "%KWK:50I^+m'vi`3?%Zp+<-d+$L-Sv:@.o19n$s0&39;kn;S%BSq*"
  "$3WoJSCLweV[aZ'MQIjO<7;X-X;&+dMLvu#^UsGEC9WEc[X(wI7#2.(F0jV*eZf<-Qv3J-c+J5AlrB#$p(H68LvEA'"
  "q3n0#m,[`*8Ft)FcYgEud]CWfm68,(aLA$@EFTgLXoBq/UPlp7"
  ":d[/;r_ix=:TF`S5H-b<LI&HY(K=h#)]Lk$K14lVfm:x$H<3^Ql<M`$OhapBnkup'D#L$Pb_`N*g]2e;X/"
  "Dtg,bsj&K#2[-:iYr'_wgH)NUIR8a1n#S?Yej'h8^58UbZd+^FKD*T@;6A"
  "7aQC[K8d-(v6GI$x:T<&'Gp5Uf>@M.*J:;$-rv29'M]8qMv-tLp,'886iaC=Hb*YJoKJ,(j%K=H`K.v9HggqBIiZu'"
  "QvBT.#=)0ukruV&.)3=(^1`o*Pj4<-<aN((^7('#Z0wK#5GX@7"
  "u][`*S^43933A4rl][`*O4CgLEl]v$1Q3AeF37dbXk,.)vj#x'd`;qgbQR%FW,2(?LO=s%Sc68%NP'##Aotl8x=BE#"
  "j1UD([3$M(]UI2LX3RpKN@;/#f'f/&_mt&F)XdF<9t4)Qa.*kT"
  "LwQ'(TTB9.xH'>#MJ+gLq9-##@HuZPN0]u:h7.T..G:;$/"
  "Usj(T7`Q8tT72LnYl<-qx8;-HV7Q-&Xdx%1a,hC=0u+HlsV>nuIQL-5<N?)NBS)QN*_I,?&)2'IM%L3I)X((e/"
  "dl2&8'<M"
  ":^#M*Q+[T.Xri.LYS3v%fF`68h;b-X[/En'CR.q7E)p'/"
  "kle2HM,u;^%OKC-N+Ll%F9CF<Nf'^#t2L,;27W:0O@6##U6W7:$rJfLWHj$#)woqBefIZ.PK<b*t7ed;p*_m;4ExK#h@&"
  "]>"
  "_>@kXQtMacfD.m-VAb8;IReM3$wf0''hra*so568'Ip&vRs849'MRYSp%:t:h5qSgwpEr$B>Q,;s(C#$)`svQuF$##-D,"
  "##,g68@2[T;.XSdN9Qe)rpt._K-#5wF)sP'##p#C0c%-Gb%"
  "hd+<-j'Ai*x&&HMkT]C'OSl##5RG[JXaHN;d'uA#x._U;.`PU@(Z3dt4r152@:v,'R.Sj'w#0<-;kPI)FfJ&#AYJ&#//"
  ")>-k=m=*XnK$>=)72L]0I%>.G690a:$##<,);?;72#?x9+d;"
  "^V'9;jY@;)br#q^YQpx:X#Te$Z^'=-=bGhLf:D6&bNwZ9-ZD#n^9HhLMr5G;']d&6'wYmTFmL<LD)F^%[tC'8;+9E#C$"
  "g%#5Y>q9wI>P(9mI[>kC-ekLC/R&CH+s'B;K-M6$EB%is00:"
  "+A4[7xks.LrNk0&E)wILYF@2L'0Nb$+pv<(2.768/"
  "FrY&h$^3i&@+G%JT'<-,v`3;_)I9M^AE]CN?Cl2AZg+%4iTpT3<n-&%H%b<FDj2M<hH=&Eh<2Len$b*aTX=-8QxN)"
  "k11IM1c^j%"
  "9s<L<NFSo)B?+<-(GxsF,^-Eh@$4dXhN$+#rxK8'je'D7k`e;)2pYwPA'_p9&@^18ml1^[@g4t*[JOa*[=Qp7(qJ_oOL^"
  "('7fB&Hq-:sf,sNj8xq^>$U4O]GKx'm9)b@p7YsvK3w^YR-"
  "CdQ*:Ir<($u&)#(&?L9Rg3H)4fiEp^iI9O8KnTj,]H?D*r7'M;PwZ9K0E^k&-cpI;.p/"
  "6_vwoFMV<->#%Xi.LxVnrU(4&8/P+:hLSKj$#U%]49t'I:rgMi'FL@a:0Y-uA[39',(vbma*"
  "hU%<-SRF`Tt:542R_VV$p@[p8DV[A,?1839FWdF<TddF<9Ah-6&9tWoDlh]&1SpGMq>Ti1O*H&#(AL8[_P%.M>v^-))"
  "qOT*F5Cq0`Ye%+$B6i:7@0IX<N+T+0MlMBPQ*Vj>SsD<U4JHY"
  "8kD2)2fU/M#$e.)T4,_=8hLim[&);?UkK'-x?'(:siIfL<$pFM`i<?%W(mGDHM%>iWP,##P`%/"
  "L<eXi:@Z9C.7o=@(pXdAO/NLQ8lPl+HPOQa8wD8=^GlPa8TKI1CjhsCTSLJM'/Wl>-"
  "S(qw%sf/@%#B6;/"
  "U7K]uZbi^Oc^2n<bhPmUkMw>%t<)'mEVE''n`WnJra$^TKvX5B>;_aSEK',(hwa0:i4G?.Bci.(X[?b*($,=-n<.Q%`("
  "X=?+@Am*Js0&=3bh8K]mL<LoNs'6,'85`"
  "0?t/'_U59@]ddF<#LdF<eWdF<OuN/45rY<-L@&#+fm>69=Lb,OcZV/"
  ");TTm8VI;?%OtJ<(b4mq7M6:u?KRdF<gR@2L=FNU-<b[(9c/ML3m;Z[$oF3g)GAWqpARc=<ROu7cL5l;-[A]%/"
  "+fsd;l#SafT/"
  "f*W]0=O'$(Tb<[)*@e775R-:Yob%g*>l*:xP?Yb.5)%w_I?7uk5JC+FS(m#i'k.'a0i)9<7b'fs'59hq$*5Uhv##pi^8+"
  "hIEBF`nvo`;'l0.^S1<-wUK2/Coh58KKhLj"
  "M=SO*rfO`+qC`W-On.=AJ56>>i2@2LH6A:&5q`?9I3@@'04&p2/"
  "LVa*T-4<-i3;M9UvZd+N7>b*eIwg:CC)c<>nO&#<IGe;__.thjZl<%w(Wk2xmp4Q@I#I9,DF]u7-P=.-_:YJ]aS@V"
  "?6*C()dOp7:WL,b&3Rg/"
  ".cmM9&r^>$(>.Z-I&J(Q0Hd5Q%7Co-b`-c<N(6r@ip+AurK<m86QIth*#v;-OBqi+L7wDE-Ir8K['m+DDSLwK&/"
  ".?-V%U_%3:qKNu$_b*B-kp7NaD'QdWQPK"
  "Yq[@>P)hI;*_F]u`Rb[.j8_Q/<&>uu+VsH$sM9TA%?)(vmJ80),P7E>)tjD%2L=-t#fK[%`v=Q8<FfNkgg^oIbah*#8/"
  "Qt$F&:K*-(N/'+1vMB,u()-a.VUU*#[e%gAAO(S>WlA2);Sa"
  ">gXm8YB`1d@K#n]76-a$U,mF<fX]idqd)<3,]J7JmW4`6]uks=4-72L(jEk+:bJ0M^q-8Dm_Z?0olP1C9Sa&H[d&c$"
  "ooQUj]Exd*3ZM@-WGW2%s',B-_M%>%Ul:#/'xoFM9QX-$.QN'>"
  "[%$Z$uF6pA6Ki2O5:8w*vP1<-1`[G,)-m#>0`P&#eb#.3i)rtB61(o'$?X3B</"
  "R90;eZ]%Ncq;-Tl]#F>2Qft^ae_5tKL9MUe9b*sLEQ95C&`=G?@Mj=wh*'3E>=-<)Gt*Iw)'QG:`@I"
  "wOf7&]1i'S01B+Ev/Nac#9S;=;YQpg_6U`*kVY39xK,[/"
  "6Aj7:'1Bm-_1EYfa1+o&o4hp7KN_Q(OlIo@S%;jVdn0'1<Vc52=u`3^o-n1'g4v58Hj&6_t7$##?M)c<$bgQ_'SY((-"
  "xkA#"
  "Y(,p'H9rIVY-b,'%bCPF7.J<Up^,(dU1VY*5#WkTU>h19w,WQhLI)3S#f$2(eb,jr*b;3Vw]*7NH%$c4Vs,eD9>XW8?N]"
  "o+(*pgC%/72LV-u<Hp,3@e^9UB1J+ak9-TN/mhKPg+AJYd$"
  "MlvAF_jCK*.O-^(63adMT->W%iewS8W6m2rtCpo'RS1R84=@paTKt)>=%&1[)*vp'u+x,VrwN;&]kuO9JDbg=pO$J*."
  "jVe;u'm0dr9l,<*wMK*Oe=g8lV_KEBFkO'oU]^=[-792#ok,)"
  "i]lR8qQ2oA8wcRCZ^7w/Njh;?.stX?Q1>S1q4Bn$)K1<-rGdO'$Wr.Lc.CG)$/*JL4tNR/"
  ",SVO3,aUw'DJN:)Ss;wGn9A32ijw%FL+Z0Fn.U9;reSq)bmI32U==5ALuG&#Vf1398/pVo"
  "1*c-(aY168o<`JsSbk-,1N;$>0:OUas(3:8Z972LSfF8eb=c-;>SPw7.6hn3m`9^Xkn(r.qS[0;T%&Qc=+STRxX'"
  "q1BNk3&*eu2;&8q$&x>Q#Q7^Tf+6<(d%ZVmj2bDi%.3L2n+4W'$P"
  "iDDG)g,r%+?,$@?uou5tSe2aN_AQU*<h`e-GI7)?OK2A.d7_c)?wQ5AS@DL3r#7fSkgl6-++D:'A,uq7SvlB$pcpH'"
  "q3n0#_%dY#xCpr-l<F0NR@-##FEV6NTF6##$l84N1w?AO>'IAO"
  "URQ##V^Fv-XFbGM7Fl(N<3DhLGF%q.1rC$#:T__&Pi68%0xi_&[qFJ(77j_&JWoF.V735&T,[R*:xFR*K5>>#`bW-?"
  "4Ne_&6Ne_&6Ne_&n`kr-#GJcM6X;uM6X;uM(.a..^2TkL%oR(#"
  ";u.T%fAr%4tJ8&><1=GHZ_+m9/#H1F^R#SC#*N=BA9(D?v[UiFY>>^8p,KKF.W]L29uLkLlu/"
  "+4T<XoIB&hx=T1PcDaB&;HH+-AFr?(m9HZV)FKS8JCw;SD=6[^/DZUL`EUDf]GGlG&>"
  "w$)F./^n3+rlo+DB;5sIYGNk+i1t-69Jg--0pao7Sm#K)pdHW&;LuDNH@H>#/"
  "X-TI(;P>#,Gc>#0Su>#4`1?#8lC?#<xU?#@.i?#D:%@#HF7@#LRI@#P_[@#Tkn@#Xw*A#]-=A#a9OA#"
  "d<F&#*;G##.GY##2Sl##6`($#:l:$#>xL$#B.`$#F:r$#JF.%#NR@%#R_R%#Vke%#Zww%#_-4&#3^Rh%Sflr-k'MS.o?."
  "5/sWel/wpEM0%3'/1)K^f1-d>G21&v(35>V`39V7A4=onx4"
  "A1OY5EI0;6Ibgr6M$HS7Q<)58C5w,;WoA*#[%T*#`1g*#d=#+#hI5+#lUG+#pbY+#tnl+#x$),#&1;,#*=M,#.I`,#"
  "2Ur,#6b.-#;w[H#iQtA#m^0B#qjBB#uvTB##-hB#'9$C#+E6C#"
  "/QHC#3^ZC#7jmC#;v)D#?,<D#C8ND#GDaD#KPsD#O]/"
  "E#g1A5#KA*1#gC17#MGd;#8(02#L-d3#rWM4#Hga1#,<w0#T.j<#O#'2#CYN1#qa^:#_4m3#o@/"
  "=#eG8=#t8J5#`+78#4uI-#"
  "m3B2#SB[8#Q0@8#i[*9#iOn8#1Nm;#^sN9#qh<9#:=x-#P;K2#$%X9#bC+.#Rg;<#mN=.#MTF.#RZO.#2?)4#Y#(/"
  "#[)1/#b;L/#dAU/#0Sv;#lY$0#n`-0#sf60#(F24#wrH0#%/e0#"
  "TmD<#%JSMFove:CTBEXI:<eh2g)B,3h2^G3i;#d3jD>)4kMYD4lVu`4m`:&5niUA5@(A5BA1]PBB:xlBCC="
  "2CDLXMCEUtiCf&0g2'tN?PGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CP"
  "GT4CPGT4CPGT4CPGT4CPGT4CPGT4CP-qekC`.9kEg^+F$kwViFJTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&"
  "5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5o,^<-28ZI'O?;xp"
  "O?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xp;7q-#lLYI:xvD=#";

static const char *GetDefaultCompressedFontDataTTFBase85()
{
  return proggy_clean_ttf_compressed_data_base85;
}
