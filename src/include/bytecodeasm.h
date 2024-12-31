
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C"
{

  typedef void *addr;
  extern void done();
  extern void iadd();
  extern void loadi();
  extern void iconst();
  extern void idiv();
  extern void imul();
  extern void isub();
  extern void printi();
  extern void storei();
  extern void trampoline(u1 *, u1 *, u1 *, u1 *);
#endif

#ifdef __cplusplus
}
#endif

typedef void (*ins)();

class AsmInterpreter
{
private:
  ins _bc_entry[1 << 8];
  addr _stack;
  int _stack_size;
  addr _local_vars;
  int _local_vars_size;
  u1 *_code;

public:
  AsmInterpreter(const char *code, int code_size, int stack_size,
                 int local_var_size);

  __attribute__((no_sanitize("address", "undefined", "leak"))) void Run();

  ~AsmInterpreter();
};