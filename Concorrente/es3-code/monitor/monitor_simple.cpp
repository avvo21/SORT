#include <iostream>
#include <thread>
#include <deque>
#include <list>
#include <chrono>
#include <sstream>
#include <string>
#include <mutex>
#include <condition_variable>

// Uncomment to enable RT priorities for threads A & B
#define USE_RT_PRIO

#ifdef USE_RT_PRIO
#include "rt/priority.h"
#include "rt/affinity.h"
#endif

const char CAESAR_SHIFT = '*';

char do_producer_stuff(const std::string & name, char data_in)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	
	char data_out = data_in - CAESAR_SHIFT;

	std::ostringstream str;
	str << name << ": '" << data_in << "'->'" << data_out << '\'' << std::endl;
	std::cout << str.str();
	
	return data_out;
}

char do_consumer_stuff(const std::string & name, char data_in)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	
	char data_out = data_in + CAESAR_SHIFT;

	std::ostringstream str;
	str << name << ": '" << data_in << "'->'" << data_out << '\'' << std::endl;
	std::cout << str.str();
	
	return data_out;
}

// Shared data ..............
std::deque<char> buffer; 
std::mutex mutex;
std::condition_variable cond;
// ..........................

const char EOD = 0;  // End Of Data

void thread_a()
{
	while (true)
	{
		char data = 0;

		{	// REGIONE CRITICA
			std::unique_lock<std::mutex> lock(mutex);

			while(buffer.empty())
				cond.wait(lock);

			data = buffer.front();
			
			if (data == EOD)
				return;

			buffer.pop_front();
		}

		do_consumer_stuff("Thread A", data);
	}
}

void thread_b()
{
	const std::string hello = "Hello World!";
	
	for (auto & v : hello)
	{
		
		char data = do_producer_stuff("Thread B", v);

		{	// REGIONE CRITICA
			std::unique_lock<std::mutex> lock(mutex);
			
			buffer.push_back(data);

			if (!buffer.empty())
				cond.notify_all();
		}

	}
	
	std::unique_lock<std::mutex> lock(mutex);
	buffer.push_back(EOD);
	cond.notify_all();
}
		
int main()
{

#ifdef USE_RT_PRIO
	rt::priority prio(rt::priority::rt_max);
	rt::affinity aff("1");

	rt::this_thread::set_affinity(aff);

	try
	{
		rt::this_thread::set_priority(prio);
	}
	catch (rt::permission_error &)
	{
		std::cerr << "Warning: RT priorities are not available"<< std::endl;
	}
#endif

	std::thread th_a(thread_a);
	std::thread th_b(thread_b);	
	
#ifdef USE_RT_PRIO
	
	rt::set_affinity(th_a, aff);
	rt::set_affinity(th_b, aff);
		
	try
	{	
		rt::set_priority(th_a, --prio);
		rt::set_priority(th_b, --prio);
	}
	catch (rt::permission_error &)
	{
	}
#endif

	th_a.join();
	th_b.join();
	
	return 0;
}

