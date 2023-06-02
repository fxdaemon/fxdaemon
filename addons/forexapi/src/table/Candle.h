#ifndef CANDLE_H_230426_
#define CANDLE_H_230426_

#include "Table.h"

namespace fxdaemon
{
  class Candle : public Table
  {
  public:
    Candle(Status status);
    Candle(const char *symbol, const char *period, time_t startTime,
          double ask, double bid, int volume);
    Candle(const char *symbol, const char *period, time_t startTime,
          double askOpen, double askHigh, double askLow, double askClose,
          double bidOpen, double bidHigh, double bidLow, double bidClose, int volume);
    const std::string &getSymbol() const;
    void setSymbol(const char *symbol);
    const std::string &getPeriod() const;
    void setPeriod(const char *period);
    time_t getStartTime() const;
    void setStartTime(time_t startTime);
    double getAskOpen() const;
    void setAskOpen(double askOpen);
    double getAskHigh() const;
    void setAskHigh(double askHigh);
    double getAskLow() const;
    void setAskLow(double askLow);
    double getAskClose() const;
    void setAskClose(double askClose);
    double getBidOpen() const;
    void setBidOpen(double bidOpen);
    double getBidHigh() const;
    void setBidHigh(double bidHigh);
    double getBidLow() const;
    void setBidLow(double bidLow);
    double getBidClose() const;
    void setBidClose(double bidClose);
    int getVolume() const;
    void setVolume(int volume);
    void setAskHighLow(double ask);
    void setBidHighLow(double bid);

    virtual Napi::Value toValue(const Napi::Env &env) const;

  private:
    std::string m_Symbol;
    std::string m_Period;
    time_t m_StartTime;
    double m_AskOpen;
    double m_AskHigh;
    double m_AskLow;
    double m_AskClose;
    double m_BidOpen;
    double m_BidHigh;
    double m_BidLow;
    double m_BidClose;
    int m_Volume;
  };
}

#endif
