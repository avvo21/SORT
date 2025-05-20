#include "monitor.h"

PhiMonitor::PhiMonitor(size_t num) : cond(num), state(num, THINKING)
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

	while (( this->state[this->left(phi_id)] == EATING )||( this->state[this->right(phi_id)] == EATING ))
		cond[phi_id].wait(lock);
	
	this->state[phi_id] = EATING;
}

void PhiMonitor::putdown(size_t phi_id)
{
	std::unique_lock<std::mutex> lock(this->mutex);

	this->state[phi_id] = THINKING;

	//Consizione del filosofo al sinistra
	if(this->state[this->left(phi_id)] == HUNGRY && this->state[this->left(this->left(phi_id))] != EATING)
		cond[this->left(phi_id)].notify_one();

	//Consizione del filosofo al destra
	if(this->state[this->right(phi_id)] == HUNGRY && this->state[this->right(this->right(phi_id))] != EATING)
		cond[this->right(phi_id)].notify_one();

}


