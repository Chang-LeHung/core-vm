

#pragma once

#include "types.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string>
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
  CodeBuffer(int size, void *base) : _code(nullptr), _capacity(size)
  {
    assert(size > 0);
    _exact_code = mmap(base, _capacity, PROT_READ | PROT_WRITE | PROT_EXEC,
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

  virtual ~CodeBuffer() { munmap(_exact_code, _capacity); }
};

struct reg
{
  std::string name;
  u4 no;
};

#define REG(name, no) const reg name = {#name, no};
#define DEF(name, no) extern const reg name;
#define DEC_REG
#include "arm64/arm64_regs.h"

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
// It is used as a frame pointer (FP) to keep track of the stack frame.
//
// X30 (LR)
// It is the link register (LR) holding the return address of the function.
// reference:
// https://medium.com/@tunacici7/aarch64-procedure-call-standard-aapcs64-abi-calling-conventions-machine-registers-a2c762540278
class Arm64Assembler
{
private:
  CodeBuffer _code_buffer;

public:
  Arm64Assembler(int size, void *base) : _code_buffer(ALIGN(size, 4), base) {}

  [[maybe_unused]] void bl(void *label);

  void blr(reg dst);

  void mov_imm32(const reg &dst, const u4 imm);

  void mov_imm64(const reg &dst, const u8 imm);

  void ret(const reg &dst);

  void mov32(const reg &dst, const reg &src);

  void mov64(const reg &dst, const reg &src);

  void str32(const reg &dst, const reg &src, int offset);

  void str64(const reg &dst, const reg &src, int offset);

  void ldr32(const reg &dst, const reg &src, int offset);

  void ldr64(const reg &dst, const reg &src, int offset);

  void str32_pre_index(const reg &dst, const reg &src, int offset);

  void str64_pre_index(const reg &dst, const reg &src, int offset);

  void add32(const reg &dst, const reg &src1, const reg &src2);

  void add32(const reg &dst, const reg &src1, int imm);

  void add64(const reg &dst, const reg &src1, const reg &src2);

  void add64(const reg &dst, const reg &src1, int imm);

  void *NewCodeSnippet() { return _code_buffer.NewCodeSnippet(); }
};