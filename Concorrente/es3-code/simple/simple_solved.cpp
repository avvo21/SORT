#include <iostream>
#include <iomanip>
#include <list>
#include <thread>
#include <mutex>

const int N = 3;
const int M = 10;

int count = 0;

void print(unsigned int id, std::mutex& m)
{
    //NB: è buona norma creare regioni critiche più brevi
	for (int i = 0; i < N; ++i )
	{
        int old_value, new_value;

        { // Uso le parentesi per dare uno scope all'unique_lock
            std::unique_lock<std::mutex> l(m);

            old_value = count;
            new_value = count + 1;
            count = new_value;
        }

        //Sposto la print fuori dalla regione critica perchè non usa count e di conseguenza la riduco ancora
        std::cout << "<child " << std::setw(2) << id << ">: value stored="
			<< old_value << ", new value=" << new_value << std::endl;
	}
}

int main()
{   
    
	std::list<std::thread> childs;
    std::mutex m;

	std::cout << "<main>: Starting child threads..." << std::endl;

	for (int id = 0; id < M; ++id)
		childs.emplace_back(print, id, std::ref(m));

	for (auto it = childs.begin(); it != childs.end(); ++it)
		it -> join();

	std::cout << "<main>: value stored=" << count << "." << std::endl;

	if ( count == N * M )
		std::cout << "<main>:   Ok, good sons!!!" << std::endl;
	else
		std::cout << "<main>:   Uh, what's wrong?" << std::endl;

	return 0;
}

