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

#include "ANCHOR_window.h"
#include "ANCHOR_rect.h"

#include <assert.h>

AnchorSystemWindow::AnchorSystemWindow(AnchorU32 width,
                                       AnchorU32 height,
                                       eAnchorWindowState state,
                                       const bool wantStereoVisual,
                                       const bool /*exclusive*/)
  : m_drawingContextType(ANCHOR_DrawingContextTypeNone),
    m_cursorVisible(true),
    m_cursorGrab(ANCHOR_GrabDisable),
    m_cursorShape(ANCHOR_StandardCursorDefault),
    m_wantStereoVisual(wantStereoVisual)
{
  m_isUnsavedChanges = false;
  m_canAcceptDragOperation = false;

  m_progressBarVisible = false;

  m_cursorGrabAccumPos[0] = 0;
  m_cursorGrabAccumPos[1] = 0;

  m_nativePixelSize = 1.0f;

  m_fullScreen = state == AnchorWindowStateFullScreen;
  if (m_fullScreen) {
    m_fullScreenWidth = width;
    m_fullScreenHeight = height;
  }
}

AnchorSystemWindow::~AnchorSystemWindow()
{
  ANCHOR::SetCurrentContext(NULL);
}

void *AnchorSystemWindow::getOSWindow() const
{
  return NULL;
}

eAnchorStatus AnchorSystemWindow::setDrawingContextType(eAnchorDrawingContextType type)
{
  if (type != m_drawingContextType) {
    ANCHOR::SetCurrentContext(NULL);

    if (type != ANCHOR_DrawingContextTypeNone)
      newDrawingContext(type);

    if (ANCHOR::GetCurrentContext() != NULL) {
      m_drawingContextType = type;
    } else {
      ANCHOR::CreateContext();
      m_drawingContextType = ANCHOR_DrawingContextTypeNone;
    }

    return (type == m_drawingContextType) ? ANCHOR_SUCCESS : ANCHOR_FAILURE;
  } else {
    return ANCHOR_SUCCESS;
  }
}

eAnchorStatus AnchorSystemWindow::swapBuffers()
{
  return ANCHOR_SUCCESS;
}

eAnchorStatus AnchorSystemWindow::activateDrawingContext()
{
  return ANCHOR_SUCCESS;
}

eAnchorStatus AnchorSystemWindow::setModifiedState(bool isUnsavedChanges)
{
  m_isUnsavedChanges = isUnsavedChanges;

  return ANCHOR_SUCCESS;
}

eAnchorStatus AnchorSystemWindow::getCursorGrabBounds(AnchorRect &bounds)
{
  bounds = m_cursorGrabBounds;
  return (bounds.m_l == -1 && bounds.m_r == -1) ? ANCHOR_FAILURE : ANCHOR_SUCCESS;
}

bool AnchorSystemWindow::getModifiedState()
{
  return m_isUnsavedChanges;
}