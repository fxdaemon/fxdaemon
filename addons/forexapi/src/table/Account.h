#ifndef ACCOUNT_H_230423_
#define ACCOUNT_H_230423_

#include "Table.h"

namespace fxdaemon
{
  class Account : public Table
  {
  public:
    Account(Status status);
    const std::string &getAccountID() const;
    void setAccountID(const char *accountID);
    const std::string &getAccountName() const;
    void setAccountName(const char *accountName);
    const std::string &getAccountKind() const;
    void setAccountKind(const char *accountKind);
    double getBalance() const;
    void setBalance(double balance);
    double getNonTradeEquity() const;
    void setNonTradeEquity(double nonTradeEquity);
    double getM2MEquity() const;
    void setM2MEquity(double m2MEquity);
    const std::string &getMarginCallFlag() const;
    void setMarginCallFlag(const char *marginCallFlag);
    time_t getLastMarginCallDate() const;
    void setLastMarginCallDate(time_t lastMarginCallDate);
    const std::string &getMaintenanceType() const;
    void setMaintenanceType(const char *maintenanceType);
    int getAmountLimit() const;
    void setAmountLimit(int amountLimit);
    int getBaseUnitSize() const;
    void setBaseUnitSize(int baseUnitSize);
    bool getMaintenanceFlag() const;
    void setMaintenanceFlag(bool maintenanceFlag);
    const std::string &getManagerAccountID() const;
    void setManagerAccountID(const char *managerAccountID);
    const std::string &getLeverageProfileID() const;
    void setLeverageProfileID(const char *leverageProfileID);
    const std::string &getATPID() const;
    void setATPID(const char *aTPID);
    double getEquity() const;
    void setEquity(double equity);
    double getDayPL() const;
    void setDayPL(double dayPL);
    double getGrossPL() const;
    void setGrossPL(double grossPL);
    double getUsedMargin() const;
    void setUsedMargin(double usedMargin);
    double getUsableMargin() const;
    void setUsableMargin(double usableMargin);
    int getUsableMarginInPercentage() const;
    void setUsableMarginInPercentage(int usableMarginInPercentage);
    int getUsableMaintMarginInPercentage() const;
    void setUsableMaintMarginInPercentage(int usableMaintMarginInPercentage);

    virtual Napi::Value toValue(const Napi::Env &env) const;

  private:
    std::string m_AccountID;
    std::string m_AccountName;
    std::string m_AccountKind;
    double m_Balance;
    double m_NonTradeEquity;
    double m_M2MEquity;
    std::string m_MarginCallFlag;   // Y, W, Q, A, N
    time_t m_LastMarginCallDate;
    std::string m_MaintenanceType;  // Y, N, O, D, F
    int m_AmountLimit;
    int m_BaseUnitSize;
    bool m_MaintenanceFlag;
    std::string m_ManagerAccountID;
    std::string m_LeverageProfileID;
    std::string m_ATPID;
    double m_Equity;
    double m_DayPL;
    double m_GrossPL;
    double m_UsedMargin;
    double m_UsableMargin;
    int m_UsableMarginInPercentage;
    int m_UsableMaintMarginInPercentage;
  };
}

#endif
