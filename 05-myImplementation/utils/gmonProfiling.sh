#!/bin/bash


# g++ main.cpp -o main -g -pg -DFEATURE -O2 -fno-inline-functions -fno-inline-functions-called-once -fno-optimize-sibling-calls -fno-inline -fno-default-inline

# First check if gmon.out exists
if [ ! -f "gmon.out" ]; then
    echo "gmon.out not found. Please run the program first."
    exit 1
fi

gprof main gmon.out > gmon.txt

python3 utils/gprof2dot.py -n0.1 -s gmon.txt > gmon.dot

dot -Tpng -Gdpi=300 gmon.dot -o gmon.png

rm gmon.txt gmon.dot