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
#include "CommunicatorListener.h"

CommunicatorListener::CommunicatorListener(EventQueue *eventQueue)
  : m_EventQueue(eventQueue)
{
  m_Event = CreateEvent(NULL, FALSE, FALSE, NULL);
  m_Completed = false;
  m_Response = NULL;
  m_Request = NULL;
}

CommunicatorListener::~CommunicatorListener()
{
  CloseHandle(m_Event);
  if (m_Response) {
    m_Response->release();
  }
}

bool CommunicatorListener::waitEvents()
{
  return WaitForSingleObject(m_Event, INFINITE) == WAIT_OBJECT_0;
}

bool CommunicatorListener::isCompleted() const
{
  return m_Completed;
}

pricehistorymgr::IPriceHistoryCommunicatorResponse* CommunicatorListener::getResponse() const
{
  return m_Response;
}

void CommunicatorListener::setRequest(pricehistorymgr::IPriceHistoryCommunicatorRequest *request)
{
  m_Completed = false;
  m_Response = NULL;
  m_Request = request;
  request->addRef();
}

void CommunicatorListener::onRequestCompleted(pricehistorymgr::IPriceHistoryCommunicatorRequest *request,
                                              pricehistorymgr::IPriceHistoryCommunicatorResponse *response)
{
  if (m_Request == request) {
    m_Completed = true;
    m_Response = response;
    m_Response->addRef();
    SetEvent(m_Event);
  }
}

void CommunicatorListener::onRequestFailed(pricehistorymgr::IPriceHistoryCommunicatorRequest *request,
                                           pricehistorymgr::IError *error)
{
  if (m_Request == request) {
    m_Completed = false;
    m_EventQueue->push(Event::ERR, error->getMessage());
    m_Request = NULL;
    m_Response = NULL;
    SetEvent(m_Event);
  }
}

void CommunicatorListener::onRequestCancelled(pricehistorymgr::IPriceHistoryCommunicatorRequest *request)
{
  if (m_Request == request) {
    m_Completed = false;
    m_EventQueue->push("Request cancelled.");
    m_Request = NULL;
    m_Response = NULL;
    SetEvent(m_Event);
  }
}

