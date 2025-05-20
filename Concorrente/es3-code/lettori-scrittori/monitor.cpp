#include "monitor.h"

RWMonitor::RWMonitor() : num_readers(0), num_writers(0), readers_in_queue(0), writers_in_queue(0)
{
}

void RWMonitor::rlock()
{
	std::unique_lock<std::mutex> lock(this-> mutex);
	readers_in_queue++;

	while (writers_in_queue > 0 || num_writers >0)
		read_queue.wait(lock);
		
	readers_in_queue--;
	num_readers++;
}

void RWMonitor::runlock()
{
	std::unique_lock<std::mutex> lock(this-> mutex);
	num_readers--;

	if (writers_in_queue > 0)
		write_queue.notify_one();
	else
		read_queue.notify_all();	
}

void RWMonitor::wlock()
{
	std::unique_lock<std::mutex> lock(this-> mutex);
	writers_in_queue++;

	while (num_writers > 0 || num_readers > 0)
		write_queue.wait(lock);

	writers_in_queue--;
	num_writers++;
}

void RWMonitor::wunlock()
{
	std::unique_lock<std::mutex> lock(this-> mutex);
	num_writers--;

	if(writers_in_queue > 0)
		write_queue.notify_one();
	else
		read_queue.notify_all();
}

