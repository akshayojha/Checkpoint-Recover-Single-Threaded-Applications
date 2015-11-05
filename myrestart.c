/*
  myrestart.c -- Restart a process from checkpoint file
*/

#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// Data type for memory addresses
typedef unsigned long long ll;

// Structure which is header for each memory
// section and register data in checkpoint image
struct section{
  ll start,end;
} mysection;

// Context variable which will store register data
ucontext_t context;

// Function to parse each line of checkpoint image
void parsefile(char file[]) {
  int file_des= open(file, O_RDONLY);
  while(1) {
    int err =read(file_des, &mysection, sizeof(mysection));
    if (err == -1) {
      perror("Error while reading!\n");
      exit(0);
    }
    size_t size=0;
    if (mysection.start == 0) {
      int err =read(file_des, &context, sizeof(context));
      if (err == -1) {
        perror("Error reading context data!\n");
        exit(0);
      }
      break;
    } else {
	// Register data has been found
      size=mysection.end-mysection.start;
      void *start=(void *)mysection.start;
      if (mmap(start, size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == MAP_FAILED) {
        perror("Error in mmap!\n");
        exit(0);
      }
      if (read(file_des, start, size) != size) {
        perror("Error in reading!\n");
        exit(0);
      }
    }
  }
  close(file_des);
  return;
}
// Main function to restart a process from given checkpoint image
int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("%s\n", "No checkpoint file provided!");
    exit(0);
  }
  parsefile(argv[1]);
  if(setcontext(&context) == -1) {
    perror("Error in setting context!\n");
  }
  return 0;
}
