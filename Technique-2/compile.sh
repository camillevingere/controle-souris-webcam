#bin/bash
g++ main.cpp -o output `pkg-config --cflags --libs opencv4` -lX11
