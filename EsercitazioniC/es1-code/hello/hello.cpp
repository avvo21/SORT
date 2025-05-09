#include <iostream>
#include <chrono>
#include <thread>

void print_hello()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::cout << "Hello World!" << std::endl;
}

int main()
{
	print_hello();

	return 0;
}

