/*
 * main.c
 *
 * Programming 2 - Project 3 (PageRank)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for getopt
#include <string.h> // for strcmp
#include "utils.h"

int main(int argc, char *const *argv) {
  char option;
  char *file_name = NULL;

  while ((option = getopt(argc, argv, "hr:m:sp:")) != -1) {
      switch (option) {
          case 'h':
              print_helppage();
              exit(0);
          case 's': 
             break;
          case 'r':
             break;
          case 'm':
             break;
          case 'p': 
             break;
          default:
              fprintf(stderr, "Usage: %s [-h] [-r N] [-m N] [-s] [-p P] [FILENAME]\n", argv[0]);
              exit(1);
      }
  }

  // initialize the random number generator
  rand_init();

  // TODO: Implement me
  exit(0);
}
