/*
  ckpt.c -- Create checkpoint file of a process
*/

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ucontext.h>

// Data type to hold memory address
typedef unsigned long long ll;

// Structure to serve as header for memory sections
// and register data
struct section{
  ll start, end;
} mysection;

// Char buffer to read memory section addresses
char low[17], high[17];
// Memory address represented as long long type
ll low_add, high_add;

// Function to parse each line of proc self maps
// and store its data in checkpoint image file
void parseline(FILE *pf) {
  char line[1000];
  int i, j;
  if (fgets(line, 1000, pf) != NULL) {
    for (i = 0, j = 0; line[i]!= '-'; i++, j++)
      low[j]= line[i];
    low[j]='\0';
    for (j = 0, i++; line[i] != ' '; i++, j++)
      high[j]= line[i];
    high[j]='\0';
    low_add = strtol(low, NULL, 16);
    high_add= strtol(high, NULL, 16);
    mysection.start=low_add;
    mysection.end=high_add;
    return;
  }
}
// Function to create checkpoint image 
void create_checkpoint() {
  FILE *proc_file = fopen("/proc/self/maps", "r");
  char line[1000];
  // Check number of lines in the file
  int l=0;
  while (fgets(line, 1000, proc_file) != NULL) l++;
  // Create a checkpoint image temporary file  
  int check_file =  open("myckpt.tmp", O_WRONLY|O_CREAT,0777);
  if (check_file == -1) {
    perror("Error creating checkpoint file!\n");
  }
  rewind(proc_file);
  // Parse each line ignoring the last line which is vsyscall
  while(--l) {
    parseline(proc_file);
    if(-1 == write(check_file, &mysection, sizeof(mysection)))
      perror("Write Error!\n");
    if(-1 == write(check_file, (char *) low_add, high_add-low_add))
      perror("Write Error!\n");
  }
  close(check_file);
  return;
}

// Fucntion to create checkpoint image and also save register
// data to checkpoint image file 
// This function is triggered by SIGUSR2 signal
void checkpointer(int signum) {
  create_checkpoint();
  ucontext_t reg_data;
  int err =getcontext(&reg_data);
  if (err == -1) {
    perror("Error getting context!");
    exit(0);
  }
  // A small hack to avoid rewriting of register data
  // when the restart program resumes this program	
  if (!(access("myckpt", F_OK)!=-1)) {
    mysection.start=0;
    mysection.end=0;
    rename("myckpt.tmp","myckpt");
    int check_file =  open("myckpt", O_WRONLY|O_APPEND, 0777);
    if (check_file == -1) {
      perror("Error creating checkpoint file!\n");
    }
    if(-1 == write(check_file, &mysection, sizeof(mysection)))
      perror("Write Error!\n");
    if (write(check_file, &reg_data, sizeof(reg_data))==-1)
      perror("Write Error!\n");
    close(check_file);
    exit(0);
  }
  return;
}
// Constructor function which will run at begining
// and handles SIGUSR2 signals
__attribute__((constructor)) void capture_signal() {
  signal(SIGUSR2, checkpointer);
}

