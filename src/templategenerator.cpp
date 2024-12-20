

#define template_generator
#include "templategenerator.h"
#include "compiler.h"
#include "types.h"
#include <cassert>
#include <cstdlib>
#include <sys/mman.h>

void *bc_entry_table[1 << 8]; // 256 is engouh currently

static void GenIConst()
{
}

void GenerateBCEntryTable()
{
  Arm64Assembler arm64(4 K);
}

// branch link implementation reference:
// https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/BL--Branch-with-link-?lang=en
void Arm64Assembler::bl(void *label)
{
  i4 offset = (i4)((u8)label - (u8)_code_buffer.GetPos());
  i4 val = offset >> 2;
  if (offset > 0)
  {
    assert(offset < 0x03ffffff);
    _code_buffer.Emit<u4>(0x94000000 | val);
  }
  else
  {
    offset = -offset;
    assert(offset < 0x03ffffff);
    val = -val;
    _code_buffer.Emit<u4>(0x95000000 | val);
  }
}