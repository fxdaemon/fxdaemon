#ifndef RESPONSEBASE_H_230423_
#define RESPONSEBASE_H_230423_

class ResponseBase
{
public:
  ResponseBase(){};
  virtual ~ResponseBase(){};
  virtual Napi::Value toValue(const Napi::Env &env) const = 0;
};

class ResponseInfo : public ResponseBase
{
public:
  ResponseInfo(std::string info) : m_Info(std::move(info)){};
  Napi::Value toValue(const Napi::Env &env) const { return Napi::String::New(env, m_Info); };

private:
  std::string m_Info;
};

#endif
