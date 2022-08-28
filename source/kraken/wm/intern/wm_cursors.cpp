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

#include "KKE_context.h"

#include "WM_cursors_api.h"

#include "USD_area.h"
#include "USD_factory.h"
#include "USD_region.h"
#include "USD_screen.h"
#include "USD_window.h"

#include "ANCHOR_api.h"

KRAKEN_NAMESPACE_BEGIN

void WM_cursor_position_from_anchor(wmWindow *win, int *x, int *y)
{
  float fac = ANCHOR::GetNativePixelSize((AnchorSystemWindowHandle)win->anchorwin);

  ANCHOR::ScreenToClient((AnchorSystemWindowHandle)win->anchorwin, *x, *y, x, y);
  *x *= fac;

  GfVec2f win_size = FormFactory(win->size);

  *y = (GET_Y(win_size) - 1) - *y;
  *y *= fac;
}

void WM_cursor_position_to_anchor(wmWindow *win, int *x, int *y)
{
  float fac = ANCHOR::GetNativePixelSize((AnchorSystemWindowHandle)win->anchorwin);

  GfVec2f win_size = FormFactory(win->size);

  *x /= fac;
  *y /= fac;
  *y = GET_Y(win_size) - *y - 1;

  ANCHOR::ClientToScreen((AnchorSystemWindowHandle)win->anchorwin, *x, *y, x, y);
}

void WM_cursor_grab_enable(wmWindow *win, int wrap, bool hide, int bounds[4])
{
  /* Only grab cursor when not running debug.
   * It helps not to get a stuck WM when hitting a break-point. */
  eAnchorGrabCursorMode mode = ANCHOR_GrabNormal;
  int mode_axis = ANCHOR_GrabAxisX | ANCHOR_GrabAxisY;

  if (bounds) {
    WM_cursor_position_to_anchor(win, &bounds[0], &bounds[1]);
    WM_cursor_position_to_anchor(win, &bounds[2], &bounds[3]);
  }

  if (hide) {
    mode = ANCHOR_GrabHide;
  } else if (wrap) {
    mode = ANCHOR_GrabWrap;

    if (wrap == WM_CURSOR_WRAP_X) {
      mode_axis = ANCHOR_GrabAxisX;
    } else if (wrap == WM_CURSOR_WRAP_Y) {
      mode_axis = ANCHOR_GrabAxisY;
    }
  }
}

KRAKEN_NAMESPACE_END