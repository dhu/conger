#ifndef __EVENTQUEUE_H_
#define __EVENTQUEUE_H _

// C++ headers
#include <string>                    // for std::string
#include <iostream>                  // for std::{cout,cerr}
#include <iomanip>                   // for std::{setw}
#include <queue>                     // for std::priority_queue
#include <cstdio>                    // for perror
#include <cmath>                     // for {floor,abs}
#include <ctime>                     // for {timeval,gettimeofday}

// UNIX headers
#include <pthread.h>                 // for pthread_{*}
#include <sys/time.h>                // for {gettimeofday}
#include <common.h>


BOREALIS_NAMESPACE_BEGIN

namespace threadtools
{

	typedef void* (*Threader) (void*);/**< A shorthand typedef for the thread functions. */

	typedef void (*eventfunc_t) (void*);/**< A shorthand typedef for EventQueue functions. */

	/**generates a timegen with p_secs plus 0..p_rand seconds. Yes, it should use a seed...
	*/
	inline timeval timegen(long p_secs, long p_rand)
	{
		timeval ts;
		ts.tv_sec = p_secs + (int(floor(p_rand*drand48())));
		ts.tv_usec = 0;

		return ts;
	}

	inline bool operator<(const timeval& t1, const timeval& t2)
	{
		return ((t1.tv_sec < t2.tv_sec) ||
			((t1.tv_sec == t2.tv_sec) &&
			(t1.tv_usec < t2.tv_usec)));
	}
	/**EventQueue
	* a threaded, timed event queue.
	* EventQueue:
	* 
	* Instantiate an EventQueue:
	*     EventQueue m_eq;
	*     
	*     Print out the pending events in the queue:
	*         m_eq.DumpEventQueue(p_out);
	*         
	* Insert an event "p_func" with label "p_string" to fire at time "p_time" with data "p_data".  "p_func" is of type
	* 
	* typedef void (*eventfunc_t) (void*); if p_relative_time = 1, use relative time in p_time instead of real time.
	* 
	* m_eq.AddEvent(p_string, p_time, p_func, p_data, p_relative_time); // insert an event with label p_string
	* 
	*  For example:
	*      m_eq.AddEvent(
	*                ::std::string("PingSendHandler"),
	*                timegen(IP_PING_BROADCAST_INTERVAL,IP_RAND),
	*                reinterpret_cast<eventfunc_t>(PingSendHandler),
	*                p_this);
	*                
	*                
	*/
	class EventQueue
	{
	protected:
		struct eventitem_t
		{
			timeval time;
			eventfunc_t function;
			void* data;
			::std::string name;

			eventitem_t() :
			time(timegen(0,0)),
				function(0),
				data(0),
				name("")
			{
			}

			eventitem_t(
				const ::std::string& p_name,
				const timeval& p_time,
				eventfunc_t p_func,
				void* p_data) :
			time(p_time),
				function(p_func),
				data(p_data),
				name(p_name)
			{
			}

			eventitem_t(const eventitem_t& p_e) :
			time(p_e.time),
				function(p_e.function),
				data(p_e.data),
				name(p_e.name)
			{ }

			/**this comparison is backwards for the priority_queue
			*/
			inline bool operator<(const eventitem_t& p_x) const
			{
				return (p_x.time < time);
			}

			inline void operator()()
			{
				function(data);
			}
		};

		class eventqueue_t : public ::std::priority_queue<eventitem_t>
		{

		public:
			typedef container_type::iterator iterator;
			iterator begin()
			{
				return c.begin();
			}
			iterator end()
			{
				return c.end();
			}
		};
		eventqueue_t m_queue;

		pthread_t m_eventthread;
		pthread_mutex_t m_eventmutex;
		pthread_cond_t m_hasdata;
		pthread_cond_t m_earlydata;
		timeval m_wakeuptime;

		bool m_running;

		static void* EventThread(EventQueue* p_this)
		{

			while (true)
			{
				pthread_mutex_lock(&p_this->m_eventmutex);

				if (p_this->m_queue.empty())
				{
					do
					{
						if (pthread_cond_wait(
							&(p_this->m_hasdata),
							&(p_this->m_eventmutex)) < 0)
							perror("EventThread pthread_cond_wait");
					} while (p_this->m_queue.empty());
				}

				eventitem_t item = p_this->m_queue.top();

				bool gotgoodevent=false;
				do
				{
					timespec sleeptime;
					sleeptime.tv_sec = item.time.tv_sec;
					sleeptime.tv_nsec = item.time.tv_usec*1000;

					p_this->m_wakeuptime = item.time;


					int val = pthread_cond_timedwait( // let the lock go
						&(p_this->m_earlydata),
						&(p_this->m_eventmutex),
						&sleeptime
						);  // got the lock back
					if (val<0) perror("EventQueue timedwait");

					item = p_this->m_queue.top();

					timeval now;
					gettimeofday(&now,0);

					if (!(now < item.time))
					{
						p_this->m_queue.pop();
						if (pthread_mutex_unlock(&p_this->m_eventmutex) < 0)
							perror("EventQueue mutex_unlock");
						gotgoodevent = true;
					}

				} while (!gotgoodevent);

				item();  // call the event handler
			}

			return 0;
		}

	public:
		EventQueue() :
		  m_running(false)
		  {
			  m_wakeuptime.tv_sec = 0;
			  m_wakeuptime.tv_usec = 0;

			  if (pthread_mutex_init(&m_eventmutex,0) < 0)
				  perror("EventQueue pthread_mutex_init");
			  if (pthread_cond_init(&m_hasdata,0) < 0)
				  perror("EventQueue pthread_cond_init");
			  if (pthread_cond_init(&m_earlydata,0) < 0)
				  perror("EventQueue pthread_cond_init");
		  }

		  void Start()
		  {
			  if (pthread_create(
				  &m_eventthread,
				  0,
				  reinterpret_cast<Threader>(EventThread),
				  static_cast<void*>(this)
				  ) < 0) perror("EventQueue pthread_create");
		  }

		  int Size()
		  {
			  if (pthread_mutex_lock(&m_eventmutex) < 0)
				  perror("EventQueue pthread_mutex_lock");
			  int size =  m_queue.size();
			  if (pthread_mutex_unlock(&m_eventmutex) < 0)
				  perror("EventQueue pthread_mutex_unlock");
			  return size;
		  }

		  void AddEvent(
			  const ::std::string& p_name,
			  const timeval& p_time,
			  eventfunc_t p_func,
			  void* p_data,
			  bool p_relative_time)
		  {

			  if (pthread_mutex_lock(&m_eventmutex) < 0)
				  perror("EventQueue pthread_mutex_lock");

			  if (!m_running)
			  {
				  m_running=true;
				  Start();  // start up the handler thread
			  }

			  timeval now;

			  if (p_relative_time)
			  {
				  gettimeofday(&now,0);
				  now.tv_sec += p_time.tv_sec;
				  now.tv_usec += p_time.tv_usec;
				  if (now.tv_usec >= 1000000)
				  {
					  now.tv_sec += 1;
					  now.tv_usec -= 1000000;
				  }
			  }
			  else
			  {
				  now.tv_sec = p_time.tv_sec;
				  now.tv_usec = p_time.tv_usec;
			  }

			  m_queue.push(eventitem_t(p_name,now,p_func,p_data));

			  if (pthread_cond_signal(&m_hasdata) < 0)
				  perror("EventQueue pthread_cond_signal");

			  if (now < m_wakeuptime)
			  {
				  if (pthread_cond_signal(&m_earlydata) < 0)
					  perror("EventQueue pthread_cond_signal");
			  }

			  if (pthread_mutex_unlock(&m_eventmutex) < 0)
				  perror("EventQueue pthread_mutex_unlock");

		  }

		  // write out the list of waiting events
		  void DumpEventQueue(::std::ostream& p_out)
		  {
			  if (pthread_mutex_lock(&m_eventmutex) < 0)
				  perror("EventQueue pthread_mutex_lock");

			  p_out << "EventQueue events waiting:" << ::std::endl;

			  p_out << "  Time:"
				  << ::std::setw(19) << "Name:"
				  << ::std::endl;
			  for (eventqueue_t::iterator i = m_queue.begin();
				  i != m_queue.end();
				  ++i)
			  {
				  p_out.setf(::std::ios::fixed,::std::ios::floatfield);
				  double time = (double)i->time.tv_sec +
					  (double)i->time.tv_usec/1000000.0;
				  p_out << "  "
					  << time
					  << "   "
					  << i->name
					  << ::std::endl;
				  p_out.setf(::std::ios::fmtflags(0),::std::ios::floatfield);
			  }

			  if (pthread_mutex_unlock(&m_eventmutex) < 0)
				  ::perror("EventQueue pthread_mutex_unlock");
		  }
	};

}

BOREALIS_NAMESPACE_END
#endif // __EVENTQUEUE_H
