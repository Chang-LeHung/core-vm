

#include "templategenerator.h"
#include <cassert>
#include <cstdio>
#include <fcntl.h>

static void dump(u8 base, int cnt)
{
  while (cnt--)
  {
    u4 val = *(u4 *)base;
    printf("%08x\n", val);
    base += 4;
  }
}

static void test_move_imm32()
{
  printf("start test_move_imm32\n");
  Arm64Assembler assembler(1024, nullptr);

  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.mov_imm32(X0, 0x12345678);
  dump(base, 2);
  assert(0x528acf00 == *(u4 *)base);
  base += 4;
  assert(0x72a24680 == *(u4 *)base);
}

static void test_ret()
{
  printf("start test_ret\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.ret(X30);
  dump(base, 1);
  assert(0xd65f03c0 == *(u4 *)base);
}

static void test_move_imm64()
{
  printf("start test_move_imm64\n");
  Arm64Assembler assembler(1024, nullptr);

  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.mov_imm64(X0, 0x1234567890098765);
  dump(base, 4);
  assert(0xd290eca0 == *(u4 *)base);
  base += 4;
  assert(0xf2b20120 == *(u4 *)base);
  base += 4;
  assert(0xf2cacf00 == *(u4 *)base);
  base += 4;
  assert(0xf2e24680 == *(u4 *)base);
}

static void test_mov32()
{
  printf("start test_mov32\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.mov32(X0, X19);
  dump(base, 1);
  assert(0x2a1303e0 == *(u4 *)base);
}

static void test_mov64()
{
  printf("start test_mov64\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.mov64(X0, X19);
  dump(base, 1);
  assert(0xaa1303e0 == *(u4 *)base);
}

static void test_str32()
{
  printf("start test_str32\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.str32(SP, X0, 24);
  assembler.str32(SP, X1, 40);
  assembler.str32(SP, X0, 12);
  dump(base, 3);
  assert(0xb9001be0 == *(u4 *)base);
  base += 4;
  assert(0xb9002be1 == *(u4 *)base);
  base += 4;
  assert(0xb9000fe0 == *(u4 *)base);
}

static void test_str64()
{
  printf("start test_str64\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.str64(SP, X19, 16);
  assembler.str64(SP, X1, 32);
  assembler.str64(SP, X1, 0);
  dump(base, 3);
  assert(0xf9000bf3 == *(u4 *)base);
  base += 4;
  assert(0xf90013e1 == *(u4 *)base);
  base += 4;
  assert(0xf90003e1 == *(u4 *)base);
}

static void test_ldr32()
{
  printf("start test_ldr32\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.ldr32(X1, SP, 28);
  assembler.ldr32(X0, SP, 24);
  assembler.ldr32(X1, SP, 44);
  dump(base, 3);
  assert(0xb9401fe1 == *(u4 *)base);
  base += 4;
  assert(0xb9401be0 == *(u4 *)base);
  base += 4;
  assert(0xb9402fe1 == *(u4 *)base);
}

static void test_ldr64()
{
  printf("start test_ldr64\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.ldr64(X1, SP, 8);
  assembler.ldr64(X0, SP, 32);
  assembler.ldr64(X1, SP, 40);
  dump(base, 3);
  assert(0xf94007e1 == *(u4 *)base);
  base += 4;
  assert(0xf94013e0 == *(u4 *)base);
  base += 4;
  assert(0xf94017e1 == *(u4 *)base);
}

static void test_add32()
{
  printf("start test_add32\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.add32(X19, X1, X0);
  assembler.add32(X0, X19, X0);
  assembler.add32(X0, X0, 0xff);
  dump(base, 3);
  assert(0x0b000033 == *(u4 *)base);
  base += 4;
  assert(0x0b000260 == *(u4 *)base);
  base += 4;
  assert(0x1103fc00 == *(u4 *)base);
}

static void test_add64()
{
  printf("start test_add64\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.add64(X19, X1, X0);
  assembler.add64(X0, X19, X0);
  assembler.add64(SP, SP, 0x10);
  dump(base, 3);
  assert(0x8b000033 == *(u4 *)base);
  base += 4;
  assert(0x8b000260 == *(u4 *)base);
  base += 4;
  assert(0x910043ff == *(u4 *)base);
}

void test_sub()
{
  printf("start test_sub\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.sub32(X0, X0, 0xa);
  assembler.sub32(X0, X1, X0);
  assembler.sub64(X0, X0, 0xa);
  assembler.sub64(X0, X1, X0);
  dump(base, 4);
  assert(0x51002800 == *(u4 *)base);
  base += 4;
  assert(0x4b000020 == *(u4 *)base);
  base += 4;
  assert(0xd1002800 == *(u4 *)base);
  base += 4;
  assert(0xcb000020 == *(u4 *)base);
}

static void test_mul()
{
  printf("test mul\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.mul32(X0, X1, X0);
  assembler.mul64(X0, X1, X0);
  dump(base, 2);
  assert(0x1b007c20 == *(u4 *)base);
  base += 4;
  assert(0x9b007c20 == *(u4 *)base);
}

static void test_div()
{
  printf("test div\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.div32(X0, X1, X0);
  assembler.div64(X0, X1, X0);
  dump(base, 2);
  assert(0x1ac00c20 == *(u4 *)base);
  base += 4;
  assert(0x9ac00c20 == *(u4 *)base);
}

static void test_ldrh_strh()
{
  printf("test test_ldrh_strh\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.ldrh(X0, X0, 0);
  assembler.strh(X0, SP, 6);
  dump(base, 2);
  assert(0x79400000 == *(u4 *)base);
  base += 4;
  assert(0x79000fe0 == *(u4 *)base);
}

static void test_ldp_post_index64()
{
  printf("test test_ldp_pre_index64\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.ldp_post_index64(X29, X30, SP, 32);
  dump(base, 1);
  assert(0xa8c27bfd == *(u4 *)base);
}

static void test_stp_pre_index64()
{
  printf("test test_stp_pre_index64\n");
  Arm64Assembler assembler(1024, nullptr);
  u8 base = (u8)assembler.NewCodeSnippet();
  assembler.stp_pre_index64(X29, X30, SP, -32);
  dump(base, 1);
  assert(0xa9be7bfd == *(u4 *)base);
}

int main()
{
  test_move_imm32();
  test_move_imm64();
  test_ret();
  test_mov32();
  test_mov64();
  test_str32();
  test_str64();
  test_ldr32();
  test_ldr64();
  test_add32();
  test_add64();
  test_sub();
  test_mul();
  test_div();
  test_ldrh_strh();
  test_ldp_post_index64();
  test_stp_pre_index64();
  return 0;
}