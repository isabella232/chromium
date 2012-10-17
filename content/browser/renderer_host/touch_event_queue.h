// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_RENDERER_HOST_TOUCH_EVENT_QUEUE_H_
#define CONTENT_BROWSER_RENDERER_HOST_TOUCH_EVENT_QUEUE_H_

#include <deque>

#include "base/basictypes.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebInputEvent.h"

class MockRenderWidgetHost;

namespace content {

class RenderWidgetHostImpl;

// A queue for throttling and coalescing touch-events.
class TouchEventQueue {
 public:
  explicit TouchEventQueue(RenderWidgetHostImpl* host);
  virtual ~TouchEventQueue();

  // Adds an event to the queue. The event may be coalesced with previously
  // queued events (e.g. consecutive touch-move events can be coalesced into a
  // single touch-move event). The event may also be immediately forwarded to
  // the renderer (e.g. when there are no other queued touch event).
  void QueueEvent(const WebKit::WebTouchEvent& event);

  // Notifies the queue that a touch-event has been processed by the renderer.
  // At this point, the queue may send one or more gesture events and/or
  // additional queued touch-events to the renderer.
  void ProcessTouchAck(bool processed);

  // Empties the queue of touch events. This may result in any number of gesture
  // events being sent to the renderer.
  void FlushQueue();

  // Resets all internal state. This does not trigger any touch or gesture
  // events to be sent.
  void Reset();

  // Returns whether the event-queue is empty.
  bool empty() const WARN_UNUSED_RESULT {
    return touch_queue_.empty();
  }

 private:
  friend class ::MockRenderWidgetHost;

  // Pops the touch-event from the top of the queue and sends it to the
  // RenderWidgetHostView. This reduces the size of the queue by one.
  void PopTouchEventToView(bool processed);

  // The RenderWidgetHost that owns this event-queue.
  RenderWidgetHostImpl* render_widget_host_;

  typedef std::deque<WebKit::WebTouchEvent> TouchQueue;
  TouchQueue touch_queue_;

  DISALLOW_COPY_AND_ASSIGN(TouchEventQueue);
};

}  // namespace content

#endif  // CONTENT_BROWSER_RENDERER_HOST_TOUCH_EVENT_QUEUE_H_
