# Checkpoint-Recover-Single-Threaded-Applications
This software creates checkpoint images of single threaded C/C++ applications at runtime and continues them later using the image.

The idea is <b>simple</b> and as follows:

1) Compile the target C/C++ program application statically with our checkpointing program(ckpt.c)

2) Now run the output of the above compilation and send SIGUSR2 signal(Can be easily changed to any other type of signal in ckpt.c) to the running target process.

3) A checkpoint image file will be created which needs to be given as input to restart program(myrestart.c)

4) The restart program will then respawn the target process from the exact same point it was stopped from a signal.


<b>How does it works?</b>

Idea is pretty simple and amazing! Whenever a process is being run on Linux distributions, its memory map can be accessed by /proc/(pid of the process)/maps.
This file contains all the information we need. It contains memory address range used by stack and heap of the process and basically everything that makes it run and identifies its state. We can store the binary data in these ranges to a local file along with the memory ranges they belong to.
We can later use this file and map back all the data it had to the exact memory locations which will simply respawn the process.

For details about how to compile, compiler flags to use etc. refer to the makefile. I have also provided a sample program(hello.c) to test the software.

<b>To do:</b>

1) Add multithreading application support

2) Allow creating of multiple checkpoints in a single run

3) Use more efficient ways to parse the proc map file

