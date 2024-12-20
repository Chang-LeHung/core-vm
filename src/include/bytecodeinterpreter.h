

#pragma once
#include "types.h"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>

class ByteCodeInterpreter
{

private:
  char *_code_buf;
  u8 *_stack;
  u8 *_local_var_table;
  int _var_size;
  int _stack_size;
  int _code_size;
  int _pc;
  int _top;

public:
  explicit ByteCodeInterpreter(const char *code, int len, int stack_size,
                               int var_size)
      : _code_size(len), _pc(0), _stack_size(stack_size), _top(0),
        _var_size(var_size)
  {
    _stack = (u8 *)malloc(stack_size);
    _code_buf = (char *)malloc(len);
    std::memcpy(_code_buf, code, len);
    assert((var_size << 3) > 0);
    _local_var_table = (u8 *)malloc(var_size << 3);
  }

  virtual ~ByteCodeInterpreter()
  {
    if (_stack != NULL)
      free(_stack);
    if (_code_buf != NULL)
      free(_code_buf);
    if (_local_var_table != NULL)
      free(_local_var_table);
  }

  __attribute__((no_sanitize("address", "undefined", "leak"))) void Run();
};
