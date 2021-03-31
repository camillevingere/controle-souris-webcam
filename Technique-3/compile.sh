#bin/bash
g++ main_internet.cpp -o output `pkg-config --cflags --libs opencv4` -lX11
