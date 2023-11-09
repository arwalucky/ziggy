#include "colours.h"

void colours::red () {
  printf("\033[1;31m");
}

void colours::yellow () {
  printf("\033[1;33m");
}

void colours::reset () {
  printf("\033[0m");
}
