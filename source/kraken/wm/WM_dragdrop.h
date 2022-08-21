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

#pragma once

/**
 * @file
 * Window Manager.
 * Making GUI Fly.
 */

#include "USD_wm_types.h"

#include "KKE_context.h"

WABI_NAMESPACE_BEGIN

void WM_drag_free(wmDrag *drag);
void WM_drag_free_list(std::vector<wmDrag *> &drags);

wmDrag *WM_event_start_drag(kContext *C,
                            int icon,
                            int type,
                            void *poin,
                            double value,
                            unsigned int flags);
void WM_drag_add_local_ID(wmDrag *drag, SdfPath id, SdfPath from_parent);

WABI_NAMESPACE_END