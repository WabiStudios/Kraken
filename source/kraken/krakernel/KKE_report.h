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
 * KRAKEN Kernel.
 * Purple Underground.
 */

#include "KKE_api.h"
#include "KKE_utils.h"

#include "USD_wm_types.h"

KRAKEN_NAMESPACE_BEGIN

void KKE_reports_init(ReportList *reports, int flag);
void KKE_reports_clear(ReportList *reports);

void KKE_report(ReportList *reports, eReportType type, const char *_message);
void KKE_reportf(ReportList *reports, eReportType type, const char *_format, ...)
  ATTR_PRINTF_FORMAT(3, 4);

char *KKE_reports_string(ReportList *reports, eReportType level);

const char *KKE_report_type_str(eReportType type);

KRAKEN_NAMESPACE_END