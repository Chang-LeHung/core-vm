

#pragma once
#include "types.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>

class ByteCodeInterpreter
{

private:
  char *_code_buf;
  u8 *_stack;
  int _stack_size;
  int _code_size;
  int _pc;
  int _top;

public:
  explicit ByteCodeInterpreter(const char *code, int len, int stack_size)
      : _code_size(len), _pc(0), _stack_size(stack_size), _top(0)
  {
    _stack = (u8 *)malloc(stack_size);
    _code_buf = (char *)malloc(len);
    std::memcpy(_code_buf, code, len);
  }

  virtual ~ByteCodeInterpreter()
  {
    if (_stack != NULL)
      free(_stack);
    if (_code_buf != NULL)
      free(_code_buf);
  }

  __attribute__((no_sanitize("address", "undefined", "leak"))) void Run();
};
