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
 * ⚓︎ Anchor.
 * Bare Metal.
 */

#include "ANCHOR_api.h"

class AnchorIEventConsumer;

class AnchorEventManager
{
 public:

  /**
   * Constructor. */
  AnchorEventManager();

  /**
   * Destructor. */
  ~AnchorEventManager();

  /**
   * Returns the number of events currently on the stack.
   * @return The number of events on the stack. */
  AnchorU32 getNumEvents();

  /**
   * Returns the number of events of a certain type currently on the stack.
   * @param type: The type of events to be counted.
   * @return The number of events on the stack of this type. */
  AnchorU32 getNumEvents(eAnchorEventType type);

  /**
   * Pushes an event on the stack.
   * To dispatch it, call dispatchEvent() or dispatchEvents().
   * Do not delete the event!
   * @param event: The event to push on the stack. */
  eAnchorStatus pushEvent(AnchorIEvent *event);

  /**
   * Dispatches the given event directly, bypassing the event stack. */
  void dispatchEvent(AnchorIEvent *event);

  /**
   * Dispatches the event at the back of the stack.
   * The event will be removed from the stack. */
  void dispatchEvent();

  /**
   * Dispatches all the events on the stack.
   * The event stack will be empty afterwards.
   */
  void dispatchEvents();

  /**
   * Adds a consumer to the list of event consumers.
   * @param consumer: The consumer added to the list.
   * @return Indication as to whether addition has succeeded. */
  eAnchorStatus addConsumer(AnchorIEventConsumer *consumer);

  /**
   * Removes a consumer from the list of event consumers.
   * @param consumer: The consumer removed from the list.
   * @return Indication as to whether removal has succeeded. */
  eAnchorStatus removeConsumer(AnchorIEventConsumer *consumer);

 protected:

  /**
   * Removes all events from the stack. */
  void destroyEvents();
  /**
   * A stack with events. */
  typedef std::deque<AnchorIEvent *> EventStack;

  /**
   * The event stack. */
  std::deque<AnchorIEvent *> m_events;
  std::deque<AnchorIEvent *> m_handled_events;

  /**
   * A vector with event consumers. */
  typedef std::vector<AnchorIEventConsumer *> ConsumerVector;

  /**
   * The list with event consumers. */
  ConsumerVector m_consumers;
};
