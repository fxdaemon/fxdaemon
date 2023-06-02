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
#include "ResponseBase.h"
#include "SessionStatusListener.h"

static const char *SessionStatusTexts[] = {
  "Disconnected",
  "Connecting",
  "TradingSessionRequested",
  "Connected",
  "Reconnecting",
  "Disconnecting",
  "SessionLost",
  "PriceSessionReconnecting",
  "ConnectedWithNeedToChangePassword",
  "ChartSessionReconnecting",
};

SessionStatusListener::SessionStatusListener(IO2GSession *session, LoginDataProvider *loginDataProvider, EventQueue *eventQueue)
  : m_Session(session), m_LoginDataProvider(loginDataProvider), m_EventQueue(eventQueue)
{
  m_Connected = false;
  m_Event = CreateEvent(NULL, FALSE, FALSE, NULL);	
}

SessionStatusListener::~SessionStatusListener()
{
  CloseHandle(m_Event);
  delete m_LoginDataProvider;
}

bool SessionStatusListener::waitEvents()
{
    return WaitForSingleObject(m_Event, TIMEOUT) == WAIT_OBJECT_0;
}

void SessionStatusListener::onSessionStatusChanged(O2GSessionStatus status)
{
  int statusNo = static_cast<int>(status);
  switch (status) {	
  case IO2GSessionStatus::Connected:
    m_Connected = true;
    SetEvent(m_Event);
    break;
  case IO2GSessionStatus::Disconnected:
    m_Connected = false;
    SetEvent(m_Event);
    break;
  case IO2GSessionStatus::TradingSessionRequested:
    {
      bool found = false;
      O2G2Ptr<IO2GSessionDescriptorCollection> descriptors = m_Session->getTradingSessionDescriptors();
      if (descriptors) {
        for (int i = 0; i < descriptors->size(); ++i) {
          O2G2Ptr<IO2GSessionDescriptor> descriptor = descriptors->get(i);
          if (m_LoginDataProvider->getSessionID() == descriptor->getID()) {
            found = true;
            break;
          }
        }
      }
      if (!found) {
        onLoginFailed("The specified sub session identifier is not found");
      } else {
        m_Session->setTradingSession(
          m_LoginDataProvider->getSessionID().c_str(), m_LoginDataProvider->getPin().c_str());
      }
    }
    break;
  case IO2GSessionStatus::Connecting:
  case IO2GSessionStatus::Disconnecting:
  case IO2GSessionStatus::Reconnecting:
  case IO2GSessionStatus::PriceSessionReconnecting:
  case IO2GSessionStatus::SessionLost:
    break;
  default:
    statusNo = -1;
  }

  if (statusNo >= 0) {
    std::shared_ptr<ResponseBase> data = std::make_shared<ResponseInfo>(SessionStatusTexts[statusNo]);
    m_EventQueue->push(Event::SESSION_STATUS, data);
  }
}

void SessionStatusListener::onLoginFailed(const char *error)
{
  m_Connected = false;
  m_Error = error;
  SetEvent(m_Event);
}
