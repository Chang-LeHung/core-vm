

#include "util.h"
#include <cstdio>
#include <cstdlib>
#include <error.h>
#include <unistd.h>

void panic(const char *msg, const char *file, int line)
{
  fprintf(stderr, "%s:%d: %s\n", file, line, msg);
  exit(EXIT_FAILURE);
}