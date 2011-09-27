#ifndef TIMEVAL_H
#define TIMEVAL_H

// C headers
#include <cmath>                // for floor
#include <ctime>                // for timeval gettimeofday

// Linux headers
#include <sys/time.h>                           // for gettimeofday
#include <common.h>

BOREALIS_NAMESPACE_BEGIN

namespace time_tools
{

	/**
	* The Timeval class is a wrapper of the timval class that
	* provides some convenient functions like +, - comparison etc.
	*/
	class Timeval:public timeval
	{
	public:

		/**Default Constructor
		*/
		Timeval()
		{
			tv_sec = 0;
			tv_usec = 0;
		};

		/**Constructor from a specific time
		*/
		Timeval(long sec, long usec)
		{
			tv_sec = sec;
			tv_usec = usec;
		}

		/**Construct from a timeval
		*/
		Timeval(const timeval &t)
		{
			tv_sec = t.tv_sec;
			tv_usec = t.tv_usec;
		}

		/**Construct from a double value where the integer part is second
		*/
		Timeval(double t)
		{
			tv_sec = static_cast<long>(floor(t));
			tv_usec = static_cast<long>((t - floor(t)) * 1000000.0);
		}

		/**Destructor
		*/
		~Timeval(){};

		/**Returns a double value with interger part as second
		*/
		double double_value() const
		{
			return static_cast<double>(tv_sec) +
				static_cast<double>(tv_usec) / 1000000.0;
		}

        /** Print the timestamp
         */
        void print()
        {
            cout << "The timestamp is: tv_sec(" << tv_sec << ") tv_usec(" << tv_usec << ")" << endl;
        }

		
        void operator=(const Timeval &t)
		{
			tv_sec = t.tv_sec;
			tv_usec = t.tv_usec;
		}
        

		void operator+=(const Timeval &t)
		{
			tv_sec += t.tv_sec;
			tv_usec += t.tv_usec;
			if (tv_usec >= 1000000)
			{
				tv_sec += 1;
				tv_usec -= 1000000;
			}
		}

		void operator-=(const Timeval &t)
		{
			tv_sec -= t.tv_sec;
			tv_usec -= t.tv_usec;
			if (tv_usec < 0)
			{
				tv_sec -= 1;
				tv_usec += 1000000;
			}
		}

		bool operator==(const Timeval &t) const
		{
			return ((tv_sec == t.tv_sec) &&
				(tv_usec == t.tv_usec));
		}

		bool operator<(const Timeval &t) const
		{
			return ((tv_sec < t.tv_sec) ||
				((tv_sec == t.tv_sec) &&
				(tv_usec < t.tv_usec)));
		}

		/**Get the current time and returns it as a Timeval object
		*/
		static Timeval now();
	};

	inline Timeval Timeval::now()
	{
		Timeval now;
		gettimeofday(&now, 0);
		return now;
	}

	inline Timeval operator-(const Timeval& tv1, const Timeval& tv2)
	{
		Timeval result = tv1;
		result -= tv2;
		return result;
	}


}

BOREALIS_NAMESPACE_END
#endif
