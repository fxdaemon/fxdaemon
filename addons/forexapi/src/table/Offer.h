#ifndef OFFER_H_230423_
#define OFFER_H_230423_

#include "Table.h"

namespace fxdaemon
{
  class Offer : public Table
  {
  public:
    Offer(Status status);
    Offer(const char *symbol);
    const std::string &getOfferID() const;
    void setOfferID(const char *offerID);
    const std::string &getSymbol() const;
    void setSymbol(const char *symbol);
    double getBid() const;
    void setBid(double bid);
    double getAsk() const;
    void setAsk(double ask);
    double getHigh() const;
    void setHigh(double high);
    double getLow() const;
    void setLow(double low);
    int getVolume() const;
    void setVolume(int volume);
    time_t getTime() const;
    void setTime(time_t time);
    double getSellInterest() const;
    void setSellInterest(double sellInterest);
    double getBuyInterest() const;
    void setBuyInterest(double buyInterest);
    const std::string &getContractCurrency() const;
    int getDigits() const;
    void setDigits(int digits);
    void setContractCurrency(const char *contractCurrency);
    double getPipCost() const;
    void setPipCost(double pipCost);
    double getPointSize() const;
    void setPointSize(double pointSize);
    const std::string &getInstrumentType() const;
    void setInstrumentType(const char *instrumentType);
    double getContractMultiplier() const;
    void setContractMultiplier(double contractMultiplier);
    const std::string &getValueDate() const;
    void setValueDate(const char *valueDate);
    void update(Offer *offer);

    virtual Napi::Value toValue(const Napi::Env &env) const;

  private:
    std::string m_OfferID;
    std::string m_Symbol;
    double m_Bid;
    double m_Ask;
    double m_High;
    double m_Low;
    int m_Volume;
    time_t m_Time;
    double m_SellInterest;
    double m_BuyInterest;
    std::string m_ContractCurrency;
    int m_Digits;
    double m_PipCost;
    double m_PointSize;
    std::string m_InstrumentType;
    double m_ContractMultiplier;
    std::string m_ValueDate;

    void init();
  };
}

#endif
