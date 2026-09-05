#!/bin/bash

make all || exit 1

ALGORITMOS=("quicksort" "mergesort" "patiencesort" "sort")
TAMANOS=("10" "1000" "100000" "10000000")
TIPOS=("ascendente" "descendente" "aleatorio")
DOMINIOS=("D1" "D7")
MUESTRAS=("a" "b" "c")

for ALGO in "${ALGORITMOS[@]}"; do
    for N in "${TAMANOS[@]}"; do
        for TIPO in "${TIPOS[@]}"; do
            for DOM in "${DOMINIOS[@]}"; do
                for MUEST in "${MUESTRAS[@]}"; do
                    echo "Procesando: $ALGO | N=$N | Tipo=$TIPO | Dom=$DOM | Muestra=$MUEST"
                    ./sorting.exe $ALGO $N $TIPO $DOM $MUEST
                done
            done
        done
    done
done