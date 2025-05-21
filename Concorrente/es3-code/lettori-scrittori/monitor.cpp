#include "monitor.h"

RWMonitor::RWMonitor() : num_readers(0), num_writers(0), readers_in_queue(0), writers_in_queue(0)
{
}

void RWMonitor::rlock()
{
	std::unique_lock<std::mutex> lock(this-> mutex);

	//Strong Writer  
	while (writers_in_queue > 0 || num_writers >0){
		readers_in_queue++;
		read_queue.wait(lock);
		readers_in_queue--;
	}
	num_readers++;
	
	/*
	//Weak Writer
	while (writers_in_queue > 0 || num_writers >0) {
		readers_in_queue++;
		queue.wait(lock);
		readers_in_queue--;
	}
	num_readers++;

	//Strong Reader
	while (num_writers > 0){
		readers_in_queue++;
		read_queue.wait(lock);
		readers_in_queue--;
	}
	num_readers++;
	

	//Strong Reader
	while (num_writers > 0){ 
		readers_in_queue++;
		queue.wait(lock);
		readers_in_queue--;
	}
	num_readers++;
	*/

}

void RWMonitor::runlock()
{
	std::unique_lock<std::mutex> lock(this-> mutex);
	num_readers--;
	
	// Strong Writer
	if (writers_in_queue > 0)
		write_queue.notify_one();
	else
		read_queue.notify_all();
	
	/*
	//Weak Writer
	if ((readers_in_queue + writers_in_queue) != 0)
		queue.notify_all();
	

	//Strong Reader
	if(readers_in_queue > 0)		//Rischio di starvation per i writer
		read_queue.notify_one();
	else
		write_queue.notify_one();
	
	//Weak Reader
	if ((readers_in_queue + writers_in_queue) != 0)
		queue.notify_all();
	*/
}

void RWMonitor::wlock()
{
	std::unique_lock<std::mutex> lock(this-> mutex);
	
	//Strong Writer
	while (num_writers > 0 || num_readers > 0){
		writers_in_queue++;
		write_queue.wait(lock);
		writers_in_queue--;
	}
	num_writers++;
	
	/*
	//Weak Writer
	while (num_writers > 0 || num_readers > 0){
		writers_in_queue++;
		queue.wait(lock);
		writers_in_queue--;
	}
	num_writers++;
	

	//Strong Reader
	while (num_writers > 0 || num_readers > 0){
		writers_in_queue++;
		write_queue.wait(lock);
		writers_in_queue--;
	}
	num_writers++;
	

	//Weak Reader
	while (num_writers > 0 || num_readers > 0){
		writers_in_queue++;
		queue.wait(lock);
		writers_in_queue--;
	}
	num_writers++;
	*/
}

void RWMonitor::wunlock()
{
	std::unique_lock<std::mutex> lock(this-> mutex);
	num_writers--;
	
	//Strong Writer
	if(writers_in_queue > 0)
		write_queue.notify_one();
	else
		read_queue.notify_all();

	/*
	//Weak Writer
	if ((readers_in_queue + writers_in_queue) != 0){
		queue.notify_all();
	}
	

	//Strong Reader
	if(readers_in_queue > 0) 		//Rischio di starvation per i writer
		read_queue.notify_one();
	else if (writers_in_queue > 0)
		write_queue.notify_one();
	

	//Weak Reader
	if ((readers_in_queue + writers_in_queue) != 0){
		queue.notify_one();
	}
	*/
}

/*
	Considerazioni:
	Nel caso degli strong reader ci può essere starvation, in questo caso specifico c'è perchè il 
	reader ha tempo di esecuzione molto più breve del writer. Si potrebbe creare un sistema che all'
	avanzare del tempo di attesa si potrebbe aumentare la priorità dei writer.
*/

