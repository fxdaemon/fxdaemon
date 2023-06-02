#ifndef TRADE_H_230509_
#define TRADE_H_230509_

#include "Table.h"

namespace fxdaemon
{
  class Trade : public Table
  {
  public:
    Trade(Status status);
    const std::string &getTradeID() const;
    void setTradeID(const char *tradeID);
    const std::string &getAccountID() const;
    void setAccountID(const char *accountID);
    const std::string &getOfferID() const;
    void setOfferID(const char *offerID);
    double getAmount() const;
    void setAmount(double amount);
    const std::string &getBS() const;
    void setBS(const char *bs);
    double getOpenRate() const;
    void setOpenRate(double openRate);
    time_t getOpenTime() const;
    void setOpenTime(time_t openTime);
    double getUsedMargin() const;
    void setUsedMargin(double usedMargin);
    double getPL() const;
    void setPL(double pl);
    double getGrossPL() const;
    void setGrossPL(double grossPL);
    double getClose() const;
    void setClose(double close);
    double getStop() const;
    void setStop(double stop);
    double getLimit() const;
    void setLimit(double limit);
    const std::string &getOpenQuoteID() const;
    void setOpenQuoteID(const char *openQuoteID);
    const std::string &getOpenOrderID() const;
    void setOpenOrderID(const char *openOrderID);
    const std::string &getOpenOrderReqID() const;
    void setOpenOrderReqID(const char *openOrderReqID);
    const std::string &getOpenOrderRequestTXT() const;
    void setOpenOrderRequestTXT(const char *openOrderRequestTXT);
    double getCommission() const;
    void setCommission(double commission);
    double getRolloverInterest() const;
    void setRolloverInterest(double rolloverInterest);
    const std::string &getTradeIDOrigin() const;
    void setTradeIDOrigin(const char *tradeIDOrigin);
    const std::string &getValueDate() const;
    void setValueDate(const char *valueDate);
    double getDividends() const;
    void setDividends(double dividends);

    virtual Napi::Value toValue(const Napi::Env &env) const;

  protected:
    bool m_Closed;
    std::string m_TradeID;
    std::string m_AccountID;
    std::string m_OfferID;
    double m_Amount;
    std::string m_BS;           // B, S	
    double m_OpenRate;
    time_t m_OpenTime;
    double m_UsedMargin;
    double m_PL;
    double m_GrossPL;
    double m_Close;
    double m_Stop;
    double m_Limit;
    std::string m_OpenQuoteID;
    std::string m_OpenOrderID;
    std::string m_OpenOrderReqID;
    std::string m_OpenOrderRequestTXT;
    double m_Commission;
    double m_RolloverInterest;
    std::string m_TradeIDOrigin;
    std::string m_ValueDate;
    double m_Dividends;
  };

  class ClosedTrade : public Trade
  {
  public:
    ClosedTrade(Status status);
    double getCloseRate() const;
    void setCloseRate(double closeRate);
    time_t getCloseTime() const;
    void setCloseTime(time_t closeTime);
    double getNetPL() const;
    void setNetPL(double netPL);
    const std::string &getCloseQuoteID() const;
    void setCloseQuoteID(const char *closeQuoteID);
    const std::string &getCloseOrderID() const;
    void setCloseOrderID(const char *closeOrderID);
    const std::string &getCloseOrderReqID() const;
    void setCloseOrderReqID(const char *closeOrderReqID);
    const std::string &getCloseOrderRequestTXT() const;
    void setCloseOrderRequestTXT(const char *closeOrderRequestTXT);
    const std::string &getTradeIDRemain() const;
    void setTradeIDRemain(const char *tradeIDRemain);

    virtual Napi::Value toValue(const Napi::Env &env) const;

  protected:
    double m_CloseRate;
    time_t m_CloseTime;
    double m_NetPL;
    std::string m_CloseQuoteID;
    std::string m_CloseOrderID;
    std::string m_CloseOrderReqID;
    std::string m_CloseOrderRequestTXT;
    std::string m_TradeIDRemain;
  };
}

#endif
