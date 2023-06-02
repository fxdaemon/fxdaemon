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
#include "common/Utils.h"
#include "ResponseListener.h"

ResponseListener::ResponseListener(IO2GSession *session, PriceCollector *priceCollector, EventQueue *eventQueue)
  : m_Session(session), m_PriceCollector(priceCollector), m_EventQueue(eventQueue)
{
  m_Event = CreateEvent(NULL, FALSE, FALSE, NULL);
  m_Status= UNAVAILABLE;
  m_Response = NULL;
  m_CompletedCount = 0;
}

ResponseListener::~ResponseListener()
{
  CloseHandle(m_Event);
  if (m_Response) {
    m_Response->release();
  }
}

void ResponseListener::setRequestID(const char *requestID)
{
  m_RequestIDs.resize(1);
  m_RequestIDs[0] = requestID;
  reset();
}

void ResponseListener::setRequestID(std::vector<std::string> &requestIDs)
{
  m_RequestIDs.resize(requestIDs.size());
  std::copy(requestIDs.begin(), requestIDs.end(), m_RequestIDs.begin());
  reset(); 
}

bool ResponseListener::waitEvents()
{
  bool ret = WaitForSingleObject(m_Event, TIMEOUT) == WAIT_OBJECT_0;
  if (!ret) m_Error = "Response waiting timeout.";
  return ret;
}

void ResponseListener::reset()
{
  if (m_Response) {
    m_Response->release();
    m_Response = NULL;
  }
  m_Status= UNAVAILABLE;
  m_Error.clear();
  ResetEvent(m_Event);
  m_CompletedCount = 0;
}

std::string ResponseListener::getOrderID()
{
  std::string orderID;
  if (!isCompleted()) return orderID;
  O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
  if (factory) {
    O2G2Ptr<IO2GOrderResponseReader> reader = factory->createOrderResponseReader(m_Response);
    if (reader) orderID = reader->getOrderID();
  }
  return orderID;
}

void ResponseListener::getMarketDataSnapshot(MarketDataReaderCallback callback)
{
  if (!isCompleted()) {
    callback(NULL);
    return;
  }
  if (m_Response->getType() == MarketDataSnapshot) {
    O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
    if (factory) {
      O2G2Ptr<IO2GMarketDataSnapshotResponseReader> reader = factory->createMarketDataSnapshotReader(m_Response);
      if (reader) callback(reader);
    }
  } else {
    callback(NULL);
  }
}

void ResponseListener::getRefreshTable(OffersTableReaderCallback callback)
{
  if (!isCompleted()) return;
  O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
  if (factory) {
    O2G2Ptr<IO2GOffersTableResponseReader> reader = factory->createOffersTableReader(m_Response);
    if (reader) callback(reader);
  }
}

void ResponseListener::getRefreshTable(AccountsTableReaderCallback callback)
{
  if (!isCompleted()) return;
  O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
  if (factory) {
    O2G2Ptr<IO2GAccountsTableResponseReader> reader = factory->createAccountsTableReader(m_Response);
    if (reader) callback(reader);
  }
}

void ResponseListener::getRefreshTable(OrdersTableReaderCallback callback)
{
  if (!isCompleted()) return;
  O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
  if (factory) {
    O2G2Ptr<IO2GOrdersTableResponseReader> reader = factory->createOrdersTableReader(m_Response);
    if (reader) callback(reader);
  }
}

void ResponseListener::getRefreshTable(OpenTradesTableReaderCallback callback)
{
  if (!isCompleted()) return;
  O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
  if (factory) {
    O2G2Ptr<IO2GTradesTableResponseReader> reader = factory->createTradesTableReader(m_Response);
    if (reader) callback(reader);
  }
}

void ResponseListener::getRefreshTable(ClosedTradesTableReaderCallback callback)
{
  if (!isCompleted()) return;
  O2G2Ptr<IO2GResponseReaderFactory> factory = m_Session->getResponseReaderFactory();
  if (factory) {
    O2G2Ptr<IO2GClosedTradesTableResponseReader> reader = factory->createClosedTradesTableReader(m_Response);
    if (reader) callback(reader);
  }
}

void ResponseListener::onRequestCompleted(const char *requestID, IO2GResponse *response)
{
  if (response && std::find(m_RequestIDs.begin(), m_RequestIDs.end(), requestID) != m_RequestIDs.end()) {
    m_CompletedCount++;
    if (m_RequestIDs.size() == 1) {
      m_Response = response;
      m_Response->addRef();
    }
  }
  if (m_CompletedCount == m_RequestIDs.size()) {
    m_Status = COMPLETED;
    SetEvent(m_Event);
  }
}

void ResponseListener::onRequestFailed(const char *requestID , const char *error)
{
  if (std::find(m_RequestIDs.begin(), m_RequestIDs.end(), requestID) != m_RequestIDs.end()) {
    m_Status = FAILED;
    m_Error = error;
    m_EventQueue->push(Event::ERR, "The request has failed. ID:%s", requestID);
    SetEvent(m_Event);
  }
}

void ResponseListener::onTablesUpdates(IO2GResponse *data)
{
  if (data->getType() != ::TablesUpdates) {
    return;
  }

  O2G2Ptr<IO2GResponseReaderFactory> readerFactory = m_Session->getResponseReaderFactory();
  O2G2Ptr<IO2GTablesUpdatesReader> reader = readerFactory->createTablesUpdatesReader(data);
  for (int i = 0; i < reader->size(); ++i) {
    if (reader->getUpdateType(i) == ::Update && reader->getUpdateTable(i) == ::Offers) {
      O2G2Ptr<IO2GOfferRow> row = reader->getOfferRow(i);
      if (row->isInstrumentValid() && m_PriceCollector->isCollectable(row->getInstrument())) {
        std::shared_ptr<fxdaemon::Offer> offer = std::make_shared<fxdaemon::Offer>(row->getInstrument());
        if (row->isOfferIDValid()) {
          offer->setOfferID(row->getOfferID());
        }
        if (row->isTimeValid()) {
          offer->setTime(Utils::oleTime2Time(row->getTime()));
        }
        if (row->isBidValid()) {
          offer->setBid(row->getBid());
        }
        if (row->isAskValid()) {
          offer->setAsk(row->getAsk());
        }
        if (row->isLowValid()) {
          offer->setLow(row->getLow());
        }
        if (row->isHighValid()) {
          offer->setHigh(row->getHigh());
        }
        if (row->isVolumeValid()) {
          offer->setVolume(row->getVolume());
        }
        m_PriceCollector->onOfferUpdate(offer);
      }
    }
  }
}
