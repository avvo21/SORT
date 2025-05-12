cmake_minimum_required(VERSION 3.31)
project(SORT)

set(CMAKE_CXX_STANDARD 14)

include_directories(Concorrente/es1-code/matrix)
include_directories(EsercitazioniC/es1-code/matrix)
include_directories(EsercitazioniC/ex1)
include_directories(EsercitazioniC/ex2/esList)
include_directories(EsercitazioniC/ex3/esGiocatori)
include_directories(EsercitazioniC/ex3/esLibreria)
include_directories(EsercitazioniC/ex4)
include_directories(EsercitazioniC/ex5)

add_executable(SORT
        Concorrente/es1-code/hello/hello.cpp
        Concorrente/es1-code/matrix/matrix.h
        Concorrente/es1-code/matrix/matrix_serial.cpp
        EsercitazioniC/es1-code/hello/hello.cpp
        EsercitazioniC/es1-code/matrix/matrix.h
        EsercitazioniC/es1-code/matrix/matrix_serial.cpp
        EsercitazioniC/ex1/fast_trigo.c
        EsercitazioniC/ex1/fast_trigo.h
        EsercitazioniC/ex1/main.c
        EsercitazioniC/ex2/esCunc/client.c
        EsercitazioniC/ex2/esCunc/server.c
        EsercitazioniC/ex2/esList/list.c
        EsercitazioniC/ex2/esList/list.h
        EsercitazioniC/ex2/esList/prova.c
        EsercitazioniC/ex3/esGiocatori/client.c
        EsercitazioniC/ex3/esGiocatori/list.c
        EsercitazioniC/ex3/esGiocatori/list.h
        EsercitazioniC/ex3/esGiocatori/server.c
        EsercitazioniC/ex3/esLibreria/client_c.c
        EsercitazioniC/ex3/esLibreria/client_l.c
        EsercitazioniC/ex3/esLibreria/list.c
        EsercitazioniC/ex3/esLibreria/list.h
        EsercitazioniC/ex3/esLibreria/server.c
        EsercitazioniC/ex4/client_c.c
        EsercitazioniC/ex4/client_f.c
        EsercitazioniC/ex4/list.c
        EsercitazioniC/ex4/list.h
        EsercitazioniC/ex4/server.c
        EsercitazioniC/ex5/client_g.c
        EsercitazioniC/ex5/client_v.c
        EsercitazioniC/ex5/list.c
        EsercitazioniC/ex5/list.h
        EsercitazioniC/ex5/server.c)
