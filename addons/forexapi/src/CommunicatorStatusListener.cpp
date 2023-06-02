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
#include "stdafx.h"
#include "common/EventQueue.h"
#include "CommunicatorStatusListener.h"

CommunicatorStatusListener::CommunicatorStatusListener(EventQueue *eventQueue)
  : m_EventQueue(eventQueue)
{
  m_Ready = false;
  m_Event = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CommunicatorStatusListener::~CommunicatorStatusListener()
{
  CloseHandle(m_Event);
}

bool CommunicatorStatusListener::isReady() const
{
  return m_Ready;
}

bool CommunicatorStatusListener::waitEvents()
{
  return WaitForSingleObject(m_Event, TIMEOUT) == WAIT_OBJECT_0;
}

void CommunicatorStatusListener::onCommunicatorStatusChanged(bool ready)
{
  m_Ready = ready;
  SetEvent(m_Event);
}

void CommunicatorStatusListener::onCommunicatorInitFailed(pricehistorymgr::IError *error)
{
  m_Ready = false;
  m_EventQueue->push(Event::ERR, error->getMessage());
  SetEvent(m_Event);
}
