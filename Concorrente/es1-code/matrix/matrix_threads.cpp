/* matrix_serial.c */

#include <vector> 
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

#include "matrix.h"

using namespace std;

/* dimensione standard della matrice */
const size_t STD_SIZE = 10;

/* dimensione max per stampare le matrici a video */
const size_t MAX_PRINT_SIZE = 30;

/* Questa funzione moltiplica una riga di M1 per una colonna di M2
    posizionando il risultato in Res[row][column]
*/
void mult(size_t row, size_t column, const matrix & M1, const matrix & M2, matrix & Res)
{
    Res(row,column) = 0;

    for(size_t position = 0; position < Res.size(); ++position)
    {
        Res(row,column) += M1(row,position) * M2(position,column);
    }
}

void mul_row(size_t start, size_t end, const matrix & M1, const matrix & M2, matrix & Res, size_t size) {
    for (size_t i = start; i < end; i++)
        for (size_t j = 0; j < size; ++j)
            mult(i, j, M1, M2, Res);
}

int main(int argc, char * argv[])
{
    size_t size = STD_SIZE;

    if (argc > 1)
    {
        istringstream iss(argv[1]);
        iss >> size;
    }

    matrix MA(size), MB(size), MC(size);

    /* Riempimento delle matrici MA e MB: valori di esempio */
    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j)
        {
            MA(i,j) = 1;
            MB(i,j) = i + j + 1;
        }
    
    if (size < MAX_PRINT_SIZE)
    {
        cout << "Matrix Ma:" << endl << MA;
        cout << "Matrix Mb:" << endl << MB;
    }

    vector<thread> th;

    int core = thread::hardware_concurrency(); // Numero di core presenti sul dispositivo
    size_t delta = (size + core -1) / core; // Upper di n/p 

    auto start_time = chrono::high_resolution_clock::now();

    /* Calcolo del prodotto .............................. */
    for(size_t i = 0; i < core; ++i){

        //Divido le righe in base ai core e do ad ogni thread delta righe 
        size_t start = i * delta;
        size_t end = min((i + 1) * delta, size);

        // Passandolo come ref devo stare attento che la sua "vita" duri più dell'esecuzione dei thread
        thread t(mul_row, start, end, cref(MA), cref(MB), ref(MC), size);
        th.push_back(std::move(t));
    }
	
	// Anche questa parte è moltiplicazione, non includendola nel tempo avrei solo il lancio dei thread
	for (size_t i = 0; i < th.size(); i++)
        th[i].join();
    /* ................................................... */

    auto stop_time = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> elapsed(stop_time - start_time);
    
    if (size < MAX_PRINT_SIZE)
    {
        cout << "Matrix Mc (Ma + Mb):" << endl << MC << endl;
    }
    cout << "Number of Cores: " << core << endl;
    cout << "Elapsed [ms]: " << elapsed.count() << endl;

    return 0;
}