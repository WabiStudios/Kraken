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
 * KRAKEN Kernel.
 * Purple Underground.
 */

#include "KKE_icons.h"
#include "KKE_utils.h"

WABI_NAMESPACE_BEGIN

static RHash *gIcons = nullptr;
static std::mutex gIconMutex;

static Icon *icon_ghash_lookup(const std::string &icon_id)
{
  std::scoped_lock lock(gIconMutex);
  return (Icon *)KKE_rhash_lookup(gIcons, FIND_TOKEN(icon_id));
}

WABI_NAMESPACE_END