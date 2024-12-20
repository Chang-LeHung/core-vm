

#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <sys/mman.h>

#define ALIGN(x, n) (((x) + (n) - 1) & ~((n) - 1))

#ifndef template_generator
extern void *bc_entry_table[]; // 256 is engouh currently
#endif

class CodeBuffer
{
private:
  void *_exact_code;
  void *_code;
  void *_pos;
  int _capacity;

public:
  CodeBuffer(int size) : _code(nullptr), _capacity(size)
  {
    assert(size > 0);
    _exact_code = mmap(NULL, _capacity, PROT_READ | PROT_WRITE | PROT_EXEC,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // round up to 2^2n arm64 instruction alignment (4 bytes)
    _code = (void *)ALIGN((uint64_t)_exact_code, 4);
    _pos = _code;
  }

  // create a new code snippet fron next legal pos(4 bytes aligned)
  void *NewCodeSnippet()
  {
    _pos = (void *)ALIGN((uint64_t)_pos, 4);
    _code = _pos;
    assert((uint64_t)_pos < (uint64_t)_exact_code + _capacity);
    return _pos;
  }

  template <typename T>
  __attribute__((no_sanitize("address", "undefined", "leak"))) void Emit(T val)
  {
    *((T *)_pos) = val;
    _pos = (void *)((uint64_t)_pos + sizeof(T));
  }

  uint64_t GetPos() { return (uint64_t)_pos; }

  virtual ~CodeBuffer() { munmap(_code, _capacity); }
};

// X0 - X7
// These registers are used for argument registers to pass parameters and
// return a result(X0).
//
// X9–15
// These are caller-saved temporary registers used to preserve values across a
// call to another function. The affected registers are saved in the stack frame
// of the caller function, allowing the callee function to modify these
// registers.
//
// X19-X28
// These are callee-saved registers that are saved in the stack frame of the
// called function, allowing the function to modify these registers but also
// requiring it to restore them before returning to the caller.
//
// X29 (FP)
// It is used as a frame pointer (FP) to keep track of the stack frame. More on
// this later.
//
// X30 (LR)
// It is the link register (LR) holding the return address of the function. More
// on this later.
class Arm64Assembler
{
private:
  CodeBuffer _code_buffer;

public:
  Arm64Assembler(int size) : _code_buffer(ALIGN(size, 4)) {}

  void bl(void *label);

  void *NewCodeSnippet() { return _code_buffer.NewCodeSnippet(); }
};