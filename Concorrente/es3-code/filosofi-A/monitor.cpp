#include "monitor.h"

PhiMonitor::PhiMonitor(size_t num) : state(num, THINKING)
{
}

inline size_t PhiMonitor::left(size_t id) const
{
	return (id + size() - 1) % size();
}

inline size_t PhiMonitor::right(size_t id) const
{
	return (id + 1) % size();
}

void PhiMonitor::pickup(size_t phi_id)
{
	std::unique_lock<std::mutex> lock(this->mutex);

	this->state[phi_id] = HUNGRY;

	while (( this->state[left(phi_id)] == EATING )||( this->state[right(phi_id)] == EATING ))
		cond.wait(lock);
	
	this->state[phi_id] = EATING;

}

void PhiMonitor::putdown(size_t phi_id)
{
	std::unique_lock<std::mutex> lock(this->mutex);

	this->state[phi_id] = THINKING;

	cond.notify_all();
}


