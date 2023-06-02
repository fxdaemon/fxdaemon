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
#include "Message.h"

using namespace fxdaemon;

Message::Message(Status status) : Table(status)
{
  m_Time = 0;
  m_HTMLFragmentFlag = false;
}

const std::string &Message::getMsgID() const
{
  return m_MsgID;
}

void Message::setMsgID(const char *msgID)
{
  m_MsgID = msgID;
}

time_t Message::Message::getTime() const
{
  return m_Time;
}

void Message::setTime(time_t time)
{
  m_Time = time;
}

const std::string &Message::getFrom() const
{
  return m_From;
}

void Message::setFrom(const char *from)
{
  m_From = from;
}

const std::string &Message::getType() const
{
  return m_Type;
}

void Message::setType(const char *type)
{
  m_Type = type;
}

const std::string &Message::getFeature() const
{
  return m_Feature;
}

void Message::setFeature(const char *feature)
{
  m_Feature = feature;
}

const std::string &Message::getText() const
{
  return m_Text;
}

void Message::setText(const char *text)
{
  m_Text = text;
}

const std::string &Message::getSubject() const
{
  return m_Subject;
}

void Message::setSubject(const char *subject)
{
  m_Subject = subject;
}

bool Message::getHTMLFragmentFlag() const
{
  return m_HTMLFragmentFlag;
}

void Message::setHTMLFragmentFlag(bool htmlFragmentFlag)
{
  m_HTMLFragmentFlag = htmlFragmentFlag;
}

Napi::Value Message::toValue(const Napi::Env &env) const
{
  Napi::Object obj = Napi::Object::New(env);
  obj.Set(Napi::String::New(env, "msg_id"), Napi::String::New(env, m_MsgID));
  obj.Set(Napi::String::New(env, "time"), Napi::Number::New(env, m_Time));
  obj.Set(Napi::String::New(env, "from"), Napi::String::New(env, m_From));
  obj.Set(Napi::String::New(env, "type"), Napi::String::New(env, m_Type));
  obj.Set(Napi::String::New(env, "feature"), Napi::String::New(env, m_Feature));
  obj.Set(Napi::String::New(env, "text"), Napi::String::New(env, m_Text));
  obj.Set(Napi::String::New(env, "subject"), Napi::String::New(env, m_Subject));
  obj.Set(Napi::String::New(env, "html_fragment_flag"), Napi::Boolean::New(env, m_HTMLFragmentFlag));
  obj.Set(Napi::String::New(env, "status"), Napi::Number::New(env, static_cast<int>(m_Status)));
  return obj;
}
