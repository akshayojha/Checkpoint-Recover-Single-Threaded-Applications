/* Copyright Generation of Miracles
   try.c -- 
*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int a =0;
  while (1) {
    printf("%d\t", a++);
    fflush(stdout);
    sleep(1);
  }
  return 0;
}
