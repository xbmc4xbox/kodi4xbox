/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "threads/SingleLock.h"
#include "threads/Helpers.h"

#include <functional>
#include <windows.h>

namespace XbmcThreads
{
  /**
   * ConditionVariableXp is effectively a condition variable implementation
   *  assuming we're on Windows XP or earlier. This means we don't have
   *  access to InitializeConditionVariable and that the structure
   *  CONDITION_VARIABLE doesnt actually exist.
   *
   * This code is basically copied from SDL_syscond.c but structured to use
   * native windows threading primitives rather than other SDL primitives.
   */
  class ConditionVariable : public NonCopyable
  {
  private:
    CCriticalSection lock;
    int waiting;
    int signals;

    class Semaphore
    {
      HANDLE sem;
      volatile LONG count;

    public:
      inline Semaphore() : count(0L), sem(CreateSemaphore(NULL,0,32*1024,NULL)) {}
      inline ~Semaphore() { CloseHandle(sem); }

      inline bool wait(DWORD dwMilliseconds)
      {
        return (WAIT_OBJECT_0 == WaitForSingleObject(sem, dwMilliseconds)) ?
          (InterlockedDecrement(&count), true) : false;
      }

      inline bool post()
      {
        /* Increase the counter in the first place, because
         * after a successful release the semaphore may
         * immediately get destroyed by another thread which
         * is waiting for this semaphore.
         */
        InterlockedIncrement(&count);
        return ReleaseSemaphore(sem, 1, NULL) ? true : (InterlockedDecrement(&count), false);
      }
    };

    Semaphore wait_sem;
    Semaphore wait_done;

  public:
    inline ConditionVariable() : waiting(0), signals(0) {}

    inline ~ConditionVariable() {}

    inline void wait(CCriticalSection& mutex)
    {
      int  count = lock.count;
      lock.count = 0;
      wait(mutex,(unsigned long)-1L);
      lock.count = count;
    }

    inline bool wait(CCriticalSection& mutex, unsigned long milliseconds)
    {
      int  count = lock.count;
      lock.count = 0;
      bool success = false;
      DWORD ms = ((unsigned long)-1L) == milliseconds ? INFINITE : (DWORD)milliseconds;

      {
        std::unique_lock<CCriticalSection> l(lock);
        waiting++;
      }

      {
        CSingleExit ex(mutex);
        success = wait_sem.wait(ms);

        {
          std::unique_lock<CCriticalSection> l(lock);
          if (signals > 0)
          {
            if (!success)
              wait_sem.wait(INFINITE);
            wait_done.post();
            --signals;
          }
          --waiting;
        }
      }
      lock.count = count;
      return success;
    }


    inline void wait(std::unique_lock<CCriticalSection>& lock) { wait(*lock.mutex()); }
    inline bool wait(std::unique_lock<CCriticalSection>& lock, unsigned long milliseconds) { return wait(*lock.mutex(), milliseconds); }

    inline void notifyAll()
    {
      /* If there are waiting threads not already signalled, then
         signal the condition and wait for the thread to respond.
      */
      std::unique_lock<CCriticalSection> l(lock);
      if ( waiting > signals )
      {
        int i, num_waiting;

        num_waiting = (waiting - signals);
        signals = waiting;
        for ( i=0; i<num_waiting; ++i )
          wait_sem.post();

        /* Now all released threads are blocked here, waiting for us.
           Collect them all (and win fabulous prizes!) :-)
        */
        l.unlock();
        for ( i=0; i<num_waiting; ++i )
          wait_done.wait(INFINITE);
      }
    }

    inline void notify()
    {
      /* If there are waiting threads not already signalled, then
         signal the condition and wait for the thread to respond.
      */
      std::unique_lock<CCriticalSection> l(lock);
      if ( waiting > signals )
      {
        ++signals;
        wait_sem.post();
        l.lock();
        wait_done.wait(INFINITE);
      }
    }
  };

  /**
   * This is a condition variable along with its predicate. This allows the use of a
   *  condition variable without the spurious returns since the state being monitored
   *  is also part of the condition.
   *
   * L should implement the Lockable concept
   *
   * The requirements on P are that it can act as a predicate (that is, I can use
   *  it in an 'while(!predicate){...}' where 'predicate' is of type 'P').
   */
  class TightConditionVariable
  {
    ConditionVariable& cond;
    std::function<bool()> predicate;

  public:
    inline TightConditionVariable(ConditionVariable& cv, std::function<bool()> predicate_)
      : cond(cv), predicate(predicate_)
    {
    }

    template<typename L>
    inline void wait(L& lock)
    {
      while (!predicate())
        cond.wait(lock);
    }

    template<typename L, typename Rep, typename Period>
    inline bool wait(L& lock, std::chrono::duration<Rep, Period> duration)
    {
      bool ret = true;
      if (!predicate())
      {
        if (duration == std::chrono::duration<Rep, Period>::zero())
        {
          cond.wait(lock, std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() /* zero */);
          return !(!predicate()); // eh? I only require the ! operation on P
        }
        else
        {
          const auto start = std::chrono::steady_clock::now();

          auto end = std::chrono::steady_clock::now();
          auto elapsed = end - start;

          auto remaining = duration - elapsed;

          for (bool notdone = true; notdone && ret == true;
               ret = (notdone = (!predicate()))
                         ? (remaining > std::chrono::duration<Rep, Period>::zero())
                         : true)
          {
            cond.wait(lock, std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());

            end = std::chrono::steady_clock::now();
            elapsed = end - start;
            remaining = duration - elapsed;
          }
        }
      }
      return ret;
    }

    inline void notifyAll() { cond.notifyAll(); }
    inline void notify() { cond.notify(); }
  };
}
