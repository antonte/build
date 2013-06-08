#!/bin/bash
g++ main.cpp -o build -g -Wall -lboost_system -lboost_filesystem -std=c++0x && \
mv build ~/bin/build
