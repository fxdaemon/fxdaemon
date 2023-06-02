/* Copyright 2011 Forex Capital Markets LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use these files except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "../stdafx.h"
#include "Mutex.h"

Mutex::Mutex()
{
#ifdef WIN32
  ::InitializeCriticalSectionAndSpinCount(&m_critSection, 4000);
#else
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&m_mutex, &attr);
  pthread_mutexattr_destroy(&attr);
#endif
}

Mutex::~Mutex()
{
#ifdef WIN32
  ::DeleteCriticalSection(&m_critSection);
#else
  pthread_mutex_destroy(&m_mutex);
#endif
}

void Mutex::lock()
{
#ifdef WIN32
  ::EnterCriticalSection(&m_critSection);
#else
  pthread_mutex_lock(&m_mutex);
#endif
}

void Mutex::unlock()
{
#ifdef WIN32
  ::LeaveCriticalSection(&m_critSection);
#else
  pthread_mutex_unlock(&m_mutex);
#endif
}
