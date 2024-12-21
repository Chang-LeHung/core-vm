

#define template_generator
#include "templategenerator.h"
#define DEF_REG
#include "arm64/arm64_regs.h"
#include "compiler.h"
#include "types.h"
#include <cassert>
#include <cstdlib>
#include <sys/mman.h>

// it'd better if recommand_addr is equal to main
// The jump range of the BL instruction is +/-128 MB.
void GenerateBCEntryTable(void *recommend_addr)
{
  Arm64Assembler arm64(4 KB, (void *)((u8)recommend_addr - 16 MB));
}

// branch link implementation reference:
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/BL--Branch-with-link-?lang=en
[[maybe_unused]] void Arm64Assembler::bl(void *label)
{
  i4 offset = (i4)((u8)label - (u8)_code_buffer.GetPos());
  i4 val = offset >> 2;
  if (offset > 0)
  {
    assert(offset < 0x03ffffff);
    _code_buffer.Emit(0x94000000 | val);
  }
  else
  {
    offset = -offset;
    assert(offset < 0x03ffffff);
    val = -val;
    _code_buffer.Emit(0x96000000 | val);
  }
}

// branch link implementation reference:
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/BLR--Branch-with-link-to-register-?lang=en
void Arm64Assembler::blr(reg dst)
{
  u4 ins = 0xd63f0000;
  ins |= (dst.no << 5);
  _code_buffer.Emit(ins);
  return;
}

// movez implementation reference:
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/MOVZ--Move-wide-with-zero-?lang=en
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/MOVK--Move-wide-with-keep-?lang=en
void Arm64Assembler::mov_imm64(const reg &dst, const u8 imm)
{
  // movz 0~16
  u4 ins = 0xd2800000;
  ins |= dst.no;
  if (imm == 0)
  {
    _code_buffer.Emit(ins);
    return;
  }
  u4 ll = imm & 0xffff;
  ins |= (ll << 5);
  _code_buffer.Emit(ins);
  // movk 16~32
  u4 lh = (imm >> 16) & 0xffff;
  if (lh != 0)
  {
    ins = 0xf2a00000;
    ins |= dst.no;
    ins |= (lh << 5);
    _code_buffer.Emit(ins);
  }
  // movk 32~48
  u4 hl = (imm >> 32) & 0xffff;
  if (hl != 0)
  {
    ins = 0xf2c00000;
    ins |= dst.no;
    ins |= (hl << 5);
    _code_buffer.Emit(ins);
  }
  // movk 48~64
  u4 hh = (imm >> 48) & 0xffff;
  if (hh != 0)
  {
    ins = 0xf2e00000;
    ins |= dst.no;
    ins |= (hh << 5);
    _code_buffer.Emit(ins);
  }
}

// movez & mvok implementation reference:
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/MOVZ--Move-wide-with-zero-?lang=en
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/MOVK--Move-wide-with-keep-?lang=en
void Arm64Assembler::mov_imm32(const reg &dst, const u4 imm)
{
  u4 ins = 0x52800000;
  ins |= dst.no;
  if (imm == 0)
  {
    _code_buffer.Emit(ins);
    return;
  }
  u2 low = imm & 0xffff;
  if (low != 0)
  {
    ins |= low << 5;
    _code_buffer.Emit(ins);
  }
  // movk
  u2 high = (imm >> 16) & 0xffff;
  if (high != 0)
  {
    ins = 0x72a00000;
    ins |= dst.no;
    ins |= high << 5;
    _code_buffer.Emit(ins);
  }
}

// Return from subroutine
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/RET--Return-from-subroutine-?lang=en
void Arm64Assembler::ret(const reg &dst)
{
  u4 ins = 0xd65f0000;
  ins |= dst.no << 5;
  _code_buffer.Emit(ins);
}

// Move register value
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/MOV--register---Move-register-value--an-alias-of-ORR--shifted-register--?lang=en
void Arm64Assembler::mov32(const reg &dst, const reg &src)
{
  u4 ins = 0x2a000000;
  ins |= 0x1f << 5;
  ins |= dst.no;
  ins |= (src.no << 16);
  _code_buffer.Emit(ins);
}

void Arm64Assembler::mov64(const reg &dst, const reg &src)
{
  u4 ins = 0xaa000000;
  ins |= (0x1f << 5);
  ins |= dst.no;
  ins |= (src.no << 16);
  _code_buffer.Emit(ins);
}

// mov dst [src+offset]
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/STR--immediate---Store-register--immediate--?lang=en
void Arm64Assembler::str32(const reg &dst, const reg &src, int offset)
{
  u4 ins = 0xb9000000;
  ins |= src.no;
  ins |= (dst.no << 5);
  assert((offset & (4 - 1)) == 0);
  offset >>= 2;
  assert((offset & (~0xfff)) == 0);
  ins |= (offset << 10);
  _code_buffer.Emit(ins);
}

// mov dst [src+offset]
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/STR--immediate---Store-register--immediate--?lang=en
void Arm64Assembler::str64(const reg &dst, const reg &src, int offset)
{
  u4 ins = 0xf9000000;
  ins |= src.no;
  ins |= (dst.no << 5);
  assert((offset & (8 - 1)) == 0);
  offset >>= 3;
  assert((offset & (~0xfff)) == 0);
  ins |= (offset << 10);
  _code_buffer.Emit(ins);
}

// mov dst [src+offset]
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/STR--immediate---Store-register--immediate--?lang=en
void Arm64Assembler::str32_pre_index(const reg &dst, const reg &src, int offset)
{
  u4 ins = 0xb8000000;
  ins |= (0x3 << 10);
  ins |= src.no;
  ins |= (dst.no << 5);
  if (offset > 0)
  {
    assert(offset <= 255);
    ins |= (offset << 12);
  }
  else if (offset < 0)
  {
    assert(offset >= -256);
    if (offset == -256)
    {
      ins |= (0x1ff << 12);
    }
    else
    {
      ins |= ((offset & 0xff) << 12);
      ins |= (1 << 21);
    }
  }
  _code_buffer.Emit(ins);
}

// mov dst [src+offset]
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/STR--immediate---Store-register--immediate--?lang=en
void Arm64Assembler::str64_pre_index(const reg &dst, const reg &src, int offset)
{
  u4 ins = 0xf8000000;
  ins |= (0x3 << 10);
  ins |= src.no;
  ins |= (dst.no << 5);
  if (offset > 0)
  {
    assert(offset <= 255);
    ins |= (offset << 12);
  }
  else if (offset < 0)
  {
    assert(offset >= -256);
    if (offset == -256)
    {
      ins |= (0x1ff << 12);
    }
    else
    {
      ins |= ((offset & 0xff) << 12);
      ins |= (1 << 21);
    }
  }
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/LDR--immediate---Load-register--immediate--?lang=en
void Arm64Assembler::ldr32(const reg &dst, const reg &src, int offset)
{
  u4 ins = 0xb9400000;
  ins |= dst.no;
  ins |= (src.no << 5);
  assert((offset & (4 - 1)) == 0);
  offset >>= 2;
  assert((offset & (~0xfff)) == 0);
  ins |= (offset << 10);
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/LDR--immediate---Load-register--immediate--?lang=en
void Arm64Assembler::ldr64(const reg &dst, const reg &src, int offset)
{
  u4 ins = 0xf9400000;
  ins |= dst.no;
  ins |= (src.no << 5);
  assert((offset & (8 - 1)) == 0);
  offset >>= 3;
  assert((offset & (~0xfff)) == 0);
  ins |= (offset << 10);
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/ADD--shifted-register---Add-optionally-shifted-register-?lang=en
void Arm64Assembler::add32(const reg &dst, const reg &src1, const reg &src2)
{
  u4 ins = 0x0b000000;
  ins |= dst.no;
  ins |= (src1.no << 5);
  ins |= (src2.no << 16);
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/ADD--immediate---Add-immediate-value-?lang=en
void Arm64Assembler::add32(const reg &dst, const reg &src1, int imm)
{
  u4 ins = 0x11000000;
  ins |= dst.no;
  ins |= (src1.no << 5);
  assert(imm >= 0 && imm <= 0xfff);
  ins |= imm << 10;
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/ADD--immediate---Add-immediate-value-?lang=en
void Arm64Assembler::add64(const reg &dst, const reg &src1, int imm)
{
  u4 ins = 0x91000000;
  ins |= dst.no;
  ins |= (src1.no << 5);
  assert(imm >= 0 && imm <= 0xfff);
  ins |= imm << 10;
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/ADD--shifted-register---Add-optionally-shifted-register-?lang=en
void Arm64Assembler::add64(const reg &dst, const reg &src1, const reg &src2)
{
  u4 ins = 0x8b000000;
  ins |= dst.no;
  ins |= (src1.no << 5);
  ins |= (src2.no << 16);
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/SUB--shifted-register---Subtract-optionally-shifted-register-?lang=en
void Arm64Assembler::sub32(const reg &dst, const reg &src1, const reg &src2)
{
  u4 ins = 0x4b000000;
  ins |= dst.no;
  ins |= (src1.no << 5);
  ins |= (src2.no << 16);
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/SUB--shifted-register---Subtract-optionally-shifted-register-?lang=en
void Arm64Assembler::sub64(const reg &dst, const reg &src1, const reg &src2)
{
  u4 ins = 0xcb000000;
  ins |= dst.no;
  ins |= (src1.no << 5);
  ins |= (src2.no << 16);
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/SUB--immediate---Subtract-immediate-value-?lang=en
void Arm64Assembler::sub32(const reg &dst, const reg &src1, int imm)
{
  u4 ins = 0x51000000;
  ins |= dst.no;
  ins |= (src1.no << 5);
  assert(imm >= 0 && imm <= 0xfff);
  ins |= (imm << 10);
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/SUB--immediate---Subtract-immediate-value-?lang=en
void Arm64Assembler::sub64(const reg &dst, const reg &src1, int imm)
{
  u4 ins = 0xd1000000;
  ins |= dst.no;
  ins |= (src1.no << 5);
  assert(imm >= 0 && imm <= 0xfff);
  ins |= (imm << 10);
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/MUL--Multiply--an-alias-of-MADD-?lang=en
void Arm64Assembler::mul32(const reg &dst, const reg &src1, const reg &src2)
{
  u4 ins = 0x1b000000;
  ins |= (0x1f << 10);
  ins |= dst.no;
  ins |= (src1.no << 5);
  ins |= (src2.no << 16);
  _code_buffer.Emit(ins);
}

// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/MUL--Multiply--an-alias-of-MADD-?lang=en
void Arm64Assembler::mul64(const reg &dst, const reg &src1, const reg &src2)
{
  u4 ins = 0x9b000000;
  ins |= (0x1f << 10);
  ins |= dst.no;
  ins |= (src1.no << 5);
  ins |= (src2.no << 16);
  _code_buffer.Emit(ins);
}

void Arm64Assembler::div32(const reg &dst, const reg &src1, const reg &src2)
{
  u4 ins = 0x1ac00000;
  ins |= (0x3 << 10);
  ins |= dst.no;
  ins |= (src1.no << 5);
  ins |= (src2.no << 16);
  _code_buffer.Emit(ins);
}

void Arm64Assembler::div64(const reg &dst, const reg &src1, const reg &src2)
{
  u4 ins = 0x9ac00000;
  ins |= (0x3 << 10);
  ins |= dst.no;
  ins |= (src1.no << 5);
  ins |= (src2.no << 16);
  _code_buffer.Emit(ins);
}