
#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C"
{

  typedef void *addr;
  extern addr done;
  extern addr iadd;
  extern addr iload;
  extern addr iconst;
  extern addr idiv;
  extern addr imul;
  extern addr isub;
  extern addr printi;
  extern addr storei;
  extern void trampoline(u1 *, u1 *, u1 *, u1 *);
#endif

#ifdef __cplusplus
}
#endif

class AsmInterpreter
{
private:
  addr _bc_entry[1 << 8];
  addr _stack;
  int _stack_size;
  addr _local_vars;
  int _local_vars_size;
  u1 *_code;

public:
  AsmInterpreter(const char *code, int code_size, int stack_size,
                 int local_var_size);

  void Run();

  ~AsmInterpreter();
};