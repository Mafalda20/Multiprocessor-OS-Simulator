#!/bin/bash

# Script de compilação para teste SIM

cd "$(dirname "$0")"

echo "=== Compilando projeto SOMM25NM - Teste SIM ==="

# Criar diretórios
mkdir -p ../lib
mkdir -p ../bin

# Compilar core
echo "Compilando core..."
cd core
g++ -std=c++20 -I../../include -c *.cpp
ar rcs ../../lib/libcore.a *.o
rm *.o
cd ..

# Compilar frontend
echo "Compilando frontend..."
cd frontend
g++ -std=c++20 -I../../include -c *.cpp
ar rcs ../../lib/libfrontend.a *.o
rm *.o
cd ..

# Compilar group modules
echo "Compilando módulos do grupo..."

for module in pct rdy swp mem feq job sim; do
    echo "  - $module"
    cd group/$module
    g++ -std=c++20 -I../../../include -c *.cpp
    ar rcs ../../../lib/lib$module.a *.o
    rm *.o
    cd ../..
done

# Compilar main (teste SIM)
echo "Compilando teste SIM..."
g++ -std=c++20 -I../include -c main.cpp -o main.o

# Linkar
echo "Linkando..."
g++ -o ../bin/main main.o \
    -L../lib \
    -static \
    -Wl,--start-group \
    -lcore -lfrontend -lbinary -lpct -lrdy -lswp -lmem -lfeq -ljob -lsim \
    -Wl,--end-group

rm main.o

echo "=== Compilação concluída! ==="
echo "Executável criado em: ../bin/main"
