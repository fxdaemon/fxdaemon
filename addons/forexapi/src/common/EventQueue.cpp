/*
* Copyright 2023 fxdaemon.com
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "../stdafx.h"
#include "EventQueue.h"

static const int EVENT_WAIT_TIMEOUT = 1000;

EventQueue::EventQueue()
{
  m_Event = CreateEvent(NULL, FALSE, FALSE, NULL);	
}

EventQueue::~EventQueue()
{
  CloseHandle(m_Event);
}

void EventQueue::push(Event mq)
{
  m_mutex.lock();
  m_queue.push(mq);
  m_mutex.unlock();
  SetEvent(m_Event);
}

void EventQueue::push(Event::EventType type, std::shared_ptr<ResponseBase> data)
{
  push({type, data});
}

void EventQueue::push(std::string info)
{
  push({Event::INFO, std::make_shared<ResponseInfo>(std::move(info))});
}

void EventQueue::push(Event::EventType type, const char *format, ...)
{
  char buf[4096];
  va_list args;
  va_start(args, format);
  vsprintf(buf, format, args);
  va_end(args);
  push({type, std::make_shared<ResponseInfo>(buf)});
}

Event EventQueue::pop()
{
  Mutex::Lock lock(m_mutex);
  Event event = { Event::UNKNOWN, NULL };
  if (!m_queue.empty()) {
    event = m_queue.front();
    m_queue.pop();
  }
  return event;
}

void EventQueue::clear()
{
  Mutex::Lock lock(m_mutex);
  while (!m_queue.empty()) {
    Event event = m_queue.front();
    m_queue.pop();
  }
}

bool EventQueue::waitEvents()
{
    return WaitForSingleObject(m_Event, EVENT_WAIT_TIMEOUT) == WAIT_OBJECT_0;
}
