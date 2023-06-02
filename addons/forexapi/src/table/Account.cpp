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
#include "Account.h"

using namespace fxdaemon;

Account::Account(Status status) : Table(status)
{
  m_Balance = 0;
  m_NonTradeEquity = 0;
  m_M2MEquity = 0;
  m_LastMarginCallDate = 0;
  m_AmountLimit = 0;
  m_BaseUnitSize = 1;
  m_MaintenanceFlag = false;
  m_Equity = 0;
  m_DayPL = 0;
  m_GrossPL = 0;
  m_UsedMargin = 0;
  m_UsableMargin = 0;
  m_UsableMarginInPercentage = 0;
  m_UsableMaintMarginInPercentage = 0;
}

const std::string &Account::getAccountID() const
{
  return m_AccountID;
}

void Account::setAccountID(const char *accountID)
{
  m_AccountID = accountID;
}

const std::string &Account::getAccountName() const
{
  return m_AccountName;
}

void Account::setAccountName(const char *accountName)
{
  m_AccountName = accountName;
}

const std::string &Account::getAccountKind() const
{
  return m_AccountKind;
}

void Account::setAccountKind(const char *accountKind)
{
  m_AccountKind = accountKind;
}

double Account::getBalance() const
{
  return m_Balance;
}

void Account::setBalance(double balance)
{
  m_Balance = balance;
}

double Account::getNonTradeEquity() const
{
  return m_NonTradeEquity;
}

void Account::setNonTradeEquity(double nonTradeEquity)
{
  m_NonTradeEquity = nonTradeEquity;
}

double Account::getM2MEquity() const
{
  return m_M2MEquity;
}

void Account::setM2MEquity(double m2MEquity)
{
  m_M2MEquity = m2MEquity;
}

const std::string &Account::getMarginCallFlag() const
{
  return m_MarginCallFlag;
}

void Account::setMarginCallFlag(const char *marginCallFlag)
{
  m_MarginCallFlag = marginCallFlag;
}

time_t Account::getLastMarginCallDate() const
{
  return m_LastMarginCallDate;
}

void Account::setLastMarginCallDate(time_t lastMarginCallDate)
{
  m_LastMarginCallDate = lastMarginCallDate;
}

const std::string &Account::getMaintenanceType() const
{
  return m_MaintenanceType;
}

void Account::setMaintenanceType(const char *maintenanceType)
{
  m_MaintenanceType = maintenanceType;
}

int Account::getAmountLimit() const
{
  return m_AmountLimit;
}

void Account::setAmountLimit(int amountLimit)
{
  m_AmountLimit = amountLimit;
}

int Account::getBaseUnitSize() const
{
  return m_BaseUnitSize;
}

void Account::setBaseUnitSize(int baseUnitSize)
{
  m_BaseUnitSize = baseUnitSize;
}

bool Account::getMaintenanceFlag() const
{
  return m_MaintenanceFlag;
}

void Account::setMaintenanceFlag(bool maintenanceFlag)
{
  m_MaintenanceFlag = maintenanceFlag;
}

const std::string &Account::getManagerAccountID() const
{
  return m_ManagerAccountID;
}

void Account::setManagerAccountID(const char *managerAccountID)
{
  m_ManagerAccountID = managerAccountID;
}

const std::string &Account::getLeverageProfileID() const
{
  return m_LeverageProfileID;
}

void Account::setLeverageProfileID(const char *leverageProfileID)
{
  m_LeverageProfileID = leverageProfileID;
}

const std::string &Account::getATPID() const
{
  return m_ATPID;
}

void Account::setATPID(const char *aTPID)
{
  m_ATPID = aTPID;
}

double Account::getEquity() const
{
  return m_Equity;
}

void Account::setEquity(double equity)
{
  m_Equity = equity;
}

double Account::getDayPL() const
{
  return m_DayPL;
}

void Account::setDayPL(double dayPL)
{
  m_DayPL = dayPL;
}

double Account::getUsedMargin() const
{
  return m_UsedMargin;
}

void Account::setUsedMargin(double usedMargin)
{
  m_UsedMargin = usedMargin;
}

double Account::getUsableMargin() const
{
  return m_UsableMargin;
}

void Account::setUsableMargin(double usableMargin)
{
  m_UsableMargin = usableMargin;
}

double Account::getGrossPL() const
{
  return m_GrossPL;
}

void Account::setGrossPL(double grossPL)
{
  m_GrossPL = grossPL;
}

int Account::getUsableMarginInPercentage() const
{
  return m_UsableMarginInPercentage;
}

void Account::setUsableMarginInPercentage(int usableMarginInPercentage)
{
  m_UsableMarginInPercentage = usableMarginInPercentage;
}

int Account::getUsableMaintMarginInPercentage() const
{
  return m_UsableMaintMarginInPercentage;
}

void Account::setUsableMaintMarginInPercentage(int usableMaintMarginInPercentage)
{
  m_UsableMaintMarginInPercentage = usableMaintMarginInPercentage;
}

Napi::Value Account::toValue(const Napi::Env &env) const
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "account_id"), Napi::String::New(env, m_AccountID));
  obj.Set(Napi::String::New(env, "account_name"), Napi::String::New(env, m_AccountName));
  obj.Set(Napi::String::New(env, "account_kind"), Napi::String::New(env, m_AccountKind));
  obj.Set(Napi::String::New(env, "balance"), Napi::Number::New(env, m_Balance));
  obj.Set(Napi::String::New(env, "non_trade_equity"), Napi::Number::New(env, m_NonTradeEquity));
  obj.Set(Napi::String::New(env, "m2m_equity"), Napi::Number::New(env, m_M2MEquity));
  obj.Set(Napi::String::New(env, "margin_call_flag"), Napi::String::New(env, m_MarginCallFlag));
  obj.Set(Napi::String::New(env, "last_margin_call_date"), Napi::Number::New(env, m_LastMarginCallDate));
  obj.Set(Napi::String::New(env, "maintenance_type"), Napi::String::New(env, m_MaintenanceType));
  obj.Set(Napi::String::New(env, "amount_limit"), Napi::Number::New(env, m_AmountLimit));
  obj.Set(Napi::String::New(env, "base_unit_size"), Napi::Number::New(env, m_BaseUnitSize));
  obj.Set(Napi::String::New(env, "maintenance_flag"), Napi::Boolean::New(env, m_MaintenanceFlag));
  obj.Set(Napi::String::New(env, "manager_account_id"), Napi::String::New(env, m_ManagerAccountID));
  obj.Set(Napi::String::New(env, "leverage_profile_id"), Napi::String::New(env, m_LeverageProfileID));
  obj.Set(Napi::String::New(env, "atp_id"), Napi::String::New(env, m_ATPID));
  obj.Set(Napi::String::New(env, "equity"), Napi::Number::New(env, m_Equity));
  obj.Set(Napi::String::New(env, "day_pl"), Napi::Number::New(env, m_DayPL));
  obj.Set(Napi::String::New(env, "gross_pl"), Napi::Number::New(env, m_GrossPL));
  obj.Set(Napi::String::New(env, "used_margin"), Napi::Number::New(env, m_UsedMargin));
  obj.Set(Napi::String::New(env, "usable_margin"), Napi::Number::New(env, m_UsableMargin));
  obj.Set(Napi::String::New(env, "usable_margin_in_percentage"), Napi::Number::New(env, m_UsableMarginInPercentage));
  obj.Set(Napi::String::New(env, "Usable_maint_margin_in_percentage"), Napi::Number::New(env, m_UsableMaintMarginInPercentage));
  obj.Set(Napi::String::New(env, "status"), Napi::Number::New(env, static_cast<int>(m_Status)));
  return obj;
}
