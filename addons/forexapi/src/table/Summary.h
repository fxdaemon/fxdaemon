#ifndef SUMMARY_H_230509_
#define SUMMARY_H_230509_

#include "Table.h"

namespace fxdaemon
{
  class Summary : public Table
  {
  public:
    Summary(Status status);
    const std::string &getOfferID() const;
    void setOfferID(const char *offerID);
    double getSellNetPL() const;
    void setSellNetPL(double sellNetPL);
    double getSellNetPLPip() const;
    void setSellNetPLPip(double sellNetPLPip);
    double getSellAmount() const;
    void setSellAmount(double sellAmount);
    double getSellAvgOpen() const;
    void setSellAvgOpen(double sellAvgOpen);
    double getBuyClose() const;
    void setBuyClose(double buyClose);
    double getSellClose() const;
    void setSellClose(double sellClose);
    double getBuyAvgOpen() const;
    void setBuyAvgOpen(double buyAvgOpen);
    double getBuyAmount() const;
    void setBuyAmount(double buyAmount);
    double getBuyNetPL() const;
    void setBuyNetPL(double buyNetPL);
    double getBuyNetPLPip() const;
    void setBuyNetPLPip(double buyNetPLPip);
    double getAmount() const;
    void setAmount(double amount);
    double getGrossPL() const;
    void setGrossPL(double grossPL);
    double getNetPL() const;
    void setNetPL(double netPL);
    double getRolloverInterestSum() const;
    void setRolloverInterestSum(double rolloverInterestSum);
    double getUsedMargin() const;
    void setUsedMargin(double usedMargin);
    double getUsedMarginBuy() const;
    void setUsedMarginBuy(double usedMarginBuy);
    double getUsedMarginSell() const;
    void setUsedMarginSell(double usedMarginSell);
    double getCommission() const;
    void setCommission(double commission);
    double getCloseCommission() const;
    void setCloseCommission(double closeCommission);
    double getDividends() const;
    void setDividends(double dividends);

    virtual  Napi::Value toValue(const Napi::Env &env) const;

  private:
    std::string m_OfferID;
    double m_SellNetPL;
    double m_SellNetPLPip;
    double m_SellAmount;
    double m_SellAvgOpen;
    double m_BuyClose;
    double m_SellClose;
    double m_BuyAvgOpen;
    double m_BuyAmount;
    double m_BuyNetPL;
    double m_BuyNetPLPip;
    double m_Amount;
    double m_GrossPL;
    double m_NetPL;
    double m_RolloverInterestSum;
    double m_UsedMargin;
    double m_UsedMarginBuy;
    double m_UsedMarginSell;
    double m_Commission;
    double m_CloseCommission;
    double m_Dividends;
  };
}

#endif
