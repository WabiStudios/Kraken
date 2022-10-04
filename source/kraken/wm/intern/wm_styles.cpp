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
 * Window Manager.
 * Making GUI Fly.
 */

#include "WM_init_exit.h"

#include "USD_context.h"
#include "USD_screen.h"

#include "ANCHOR_api.h"



void WM_init_default_styles()
{
  AnchorStyle &style = ANCHOR::GetStyle();
  style.WindowPadding = GfVec2f(2.0f, 2.0f);
  style.WindowRounding = 5.0f;
  style.WindowBorderSize = 0.0f;
  style.WindowMinSize = GfVec2f(0.0f, 0.0f);
  style.WindowTitleAlign = GfVec2f(0.0f, 0.5f);
  style.WindowMenuButtonPosition = AnchorDir_Left;
}

