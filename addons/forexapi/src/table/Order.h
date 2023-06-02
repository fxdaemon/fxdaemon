#ifndef ORDER_H_230509_
#define ORDER_H_230509_

#include "Table.h"

namespace fxdaemon
{
  class Order : public Table
  {
  public:
    Order(Status status);
    const std::string &getOrderID() const;
    void setOrderID(const char *orderID);
    const std::string &getRequestID() const;
    void setRequestID(const char *requestID);
    const std::string &getAccountID() const;
    void setAccountID(const char *accountID);
    const std::string &getOfferID() const;
    void setOfferID(const char *offerID);
    const std::string &getTradeID() const;
    void setTradeID(const char *tradeID);
    bool getNetQuantity() const;
    void setNetQuantity(bool netQuantity);
    const std::string &getBS() const;
    void setBS(const char *bs);
    const std::string &getStage() const;
    void setStage(const char *stage);
    const std::string &getType() const;
    void setType(const char *type);
    const std::string &getOrderStatus() const;
    void setOrderStatus(const char *orderStatus);
    double getAmount() const;
    void setAmount(double amount);
    double getRate() const;
    void setRate(double rate);
    double getExecutionRate() const;
    void setExecutionRate(double executionRate);
    double getRateMin() const;
    void setRateMin(double rateMin);
    double getRateMax() const;
    void setRateMax(double rateMax);
    double getStop() const;
    void setStop(double stop);
    double getLimit() const;
    void setLimit(double limit);
    const std::string &getTypeStop() const;
    void setTypeStop(const char *typeStop);
    const std::string &getTypeLimit() const;
    void setTypeLimit(const char *typeLimit);
    time_t getStatusTime() const;
    void setStatusTime(time_t time);
    double getAtMarket() const;
    void setAtMarket(double atMarket);
    int getTrailStep() const;
    void setTrailStep(int trailStep);
    double getTrailRate() const;
    void setTrailRate(double trailRate);
    const std::string &getTimeInForce() const;
    void setTimeInForce(const char *timeInForce);
    const std::string &getContingentOrderID() const;
    void setContingentOrderID(const char *contingentOrderID);
    const std::string &getContingencyType() const;
    void setContingencyType(const char *contingencyType);
    const std::string &getPrimaryID() const;
    void setPrimaryID(const char *primaryID);
    double getOriginAmount() const;
    void setOriginAmount(double originAmount);
    double getFilledAmount() const;
    void setFilledAmount(double filledAmount);
    bool getWorkingIndicator() const;
    void setWorkingIndicator(bool workingIndicator);
    const std::string &getPegType() const;
    void setPegType(const char *pegType);
    double getPegOffset() const;
    void setPegOffset(double pegOffset);
    time_t getExpireTime() const;
    void setExpireTime(time_t expireTime);
    const std::string &getValueDate() const;
    void setValueDate(const char *valueDate);

    virtual Napi::Value toValue(const Napi::Env &env) const;

  private:
    std::string m_OrderID;
    std::string m_RequestID;
    std::string m_AccountID;
    std::string m_OfferID;
    std::string m_TradeID;
    bool m_NetQuantity;
    std::string m_BS;           // B, S
    std::string m_Stage;        // O, C
    std::string m_Type;         // S, ST, L, SE, LE, STE, LTE, C, CM, CR, O, OM, OR, M
    std::string m_OrderStatus;  // W, P, I, Q, U, E, S, C, R, T, F, G
    double m_Amount;
    double m_Rate;
    double m_ExecutionRate;
    double m_RateMin;
    double m_RateMax;
    double m_Stop;
    double m_Limit;
    std::string m_TypeStop;
    std::string m_TypeLimit;
    time_t m_StatusTime;
    double m_AtMarket;
    int m_TrailStep;
    double m_TrailRate;
    std::string m_TimeInForce;  // GTC, IOC, FOK, DAY, GTD
    std::string m_ContingentOrderID;
    std::string m_ContingencyType;
    std::string m_PrimaryID;
    double m_OriginAmount;
    double m_FilledAmount;
    bool m_WorkingIndicator;
    std::string m_PegType;      // O, M
    double m_PegOffset;
    time_t m_ExpireTime;
    std::string m_ValueDate;
  };
}

#endif
