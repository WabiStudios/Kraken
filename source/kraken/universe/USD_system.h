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
 * Universe.
 * Set the Stage.
 */

#pragma once

#include "USD_api.h"

#define MAX_SYSTEM_FILE_PATH 256
#define MAX_SYSTEM_VERSION_INFO 64



struct SystemPaths
{
  /** Executable Path, where kraken.exe resides. */
  char exe_path[MAX_SYSTEM_FILE_PATH];

  /** System Temporary Directory, where we create files at. */
  char temp_dir[MAX_SYSTEM_FILE_PATH];

  /** Icons Path, where kraken icons reside. */
  char icons_path[MAX_SYSTEM_FILE_PATH];

  /** Datafiles path, where kraken_version/datafiles resides. */
  char datafiles_path[MAX_SYSTEM_FILE_PATH];

  /** Stage path, where the current project is loaded from. */
  char stage_path[MAX_SYSTEM_FILE_PATH];
};

struct SystemVersion
{
  /** Build commit timestamp, the timestamp from this build's latest revision. */
  uint64_t build_commit_timestamp;

  /** Build hash, the commit hash from this build's latest revision. */
  char build_hash[MAX_SYSTEM_VERSION_INFO];

  /** The KRAKEN Version, as a decimal, example '1.50'. */
  char kraken_version[MAX_SYSTEM_VERSION_INFO];

  /** The PIXAR Version, as a decimal, example '21.5'. */
  char pixar_version[MAX_SYSTEM_VERSION_INFO];
};

struct SystemTime
{
  /** Returns the formatted launch time. */
  char launch_time[USD_MAX_TIME];
};

struct System
{
  SystemPaths paths;
  SystemVersion version;
  SystemTime time;
};

