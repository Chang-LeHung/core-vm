

#include "bytecodeinterpreter.h"
#include "opcode.h"
#include "types.h"
#include "util.h"
#include <iostream>

#define TOP_AS_TYPE(type) (*(type *)((u8 *)_stack + _top))
#define POP_INT(ref)                                                           \
  _top--;                                                                      \
  ref = TOP_AS_TYPE(int)

#define POP_DOUBLE(ref)                                                        \
  _top--;                                                                      \
  ref = TOP_AS_TYPE(double)

#define POP_FLOAT(ref)                                                         \
  _top--;                                                                      \
  ref = TOP_AS_TYPE(float)

#define POP_LONG(ref)                                                          \
  _top--;                                                                      \
  ref = TOP_AS_TYPE(long)

#define STORE_TOP(type, value) (*(type *)((u8 *)_stack + _top) = (value))

#define PUSH_INT(value)                                                        \
  STORE_TOP(int, value);                                                       \
  _top++

#define PUSH_FLOAT(value)                                                      \
  STORE_TOP(float, value);                                                     \
  _top++

#define PUSH_LONG(value)                                                       \
  STORE_TOP(long, value);                                                      \
  _top++

#define PUSH_DOUBLE(value)                                                     \
  STORE_TOP(double, value);                                                    \
  _top++

#define POP_TWO(type, macro)                                                   \
  macro(type lhs);                                                             \
  macro(type rhs)

__attribute__((no_sanitize("address", "undefined", "leak"))) void
ByteCodeInterpreter::Run()
{
  for (; _pc < _code_size;)
    switch (static_cast<OpCode>(*(u2 *)(_code_buf + _pc)))
    {
    case OpCode::kIAdd:
    {
      POP_TWO(int, POP_INT);
      PUSH_INT(lhs + rhs);
      _pc += 2;
      break;
    }
    case OpCode::kISub:
    {
      POP_TWO(int, POP_INT);
      PUSH_INT(lhs - rhs);
      _pc += 2;
      break;
    }
    case OpCode::kIMul:
    {
      POP_TWO(int, POP_INT);
      PUSH_INT(lhs * rhs);
      _pc += 2;
      break;
    }
    case OpCode::kIDiv:
    {
      POP_TWO(int, POP_INT);
      PUSH_INT(lhs / rhs);
      _pc += 2;
      break;
    }
    case OpCode::kIMod:
    {
      POP_TWO(int, POP_INT);
      PUSH_INT(lhs % rhs);
      _pc += 2;
      break;
    }
    case OpCode::kDAdd:
    {
      POP_TWO(double, POP_DOUBLE);
      PUSH_DOUBLE(lhs + rhs);
      _pc += 2;
      break;
    }
    case OpCode::kDSub:
    {
      POP_TWO(double, POP_DOUBLE);
      PUSH_DOUBLE(lhs - rhs);
      _pc += 2;
      break;
    }
    case OpCode::kDMul:
    {
      POP_TWO(double, POP_DOUBLE);
      PUSH_DOUBLE(lhs * rhs);
      _pc += 2;
      break;
    }
    case OpCode::kDDiv:
    {
      POP_TWO(double, POP_DOUBLE);
      PUSH_DOUBLE(lhs / rhs);
      _pc += 2;
      break;
    }
    case OpCode::kFAdd:
    {
      POP_TWO(float, POP_FLOAT);
      PUSH_FLOAT(lhs + rhs);
      _pc += 2;
      break;
    }
    case OpCode::kFSub:
    {
      POP_TWO(float, POP_FLOAT);
      PUSH_FLOAT(lhs - rhs);
      _pc += 2;
      break;
    }
    case OpCode::kFMul:
    {
      POP_TWO(float, POP_FLOAT);
      PUSH_FLOAT(lhs * rhs);
      _pc += 2;
    }
    case OpCode::kFDiv:
    {
      POP_TWO(float, POP_FLOAT);
      PUSH_FLOAT(lhs / rhs);
      _pc += 2;
      break;
    }
    case OpCode::kIConst:
    {
      int val = *(int *)(_code_buf + _pc + 2);
      PUSH_INT(val);
      _pc += 6;
      break;
    }
    case OpCode::kDConst:
    {
      double val = *(double *)(_code_buf + _pc + 2);
      PUSH_DOUBLE(val);
      _pc += 10;
      break;
    }
    case OpCode::kFConst:
    {
      float val = *(float *)(_code_buf + _pc + 2);
      PUSH_FLOAT(val);
      _pc += 6;
      break;
    }
    case OpCode::kLConst:
    {
      long val = *(long *)(_code_buf + _pc + 2);
      PUSH_LONG(val);
      _pc += 10;
      break;
    }
    case OpCode::kICastD:
    {
      int val;
      POP_INT(val);
      PUSH_DOUBLE(static_cast<double>(val));
      _pc += 2;
      break;
    }
    case OpCode::kICastF:
    {
      int val;
      POP_INT(val);
      PUSH_FLOAT(static_cast<float>(val));
      _pc += 2;
      break;
    }
    case OpCode::kICastL:
    {
      int val;
      POP_INT(val);
      PUSH_LONG(static_cast<long>(val));
      _pc += 2;
      break;
    }
    case OpCode::kLCastD:
    {
      long val;
      POP_LONG(val);
      PUSH_DOUBLE(static_cast<double>(val));
      _pc += 2;
      break;
    }
    case OpCode::kLCastF:
    {
      long val;
      POP_LONG(val);
      PUSH_FLOAT(static_cast<float>(val));
      _pc += 2;
      break;
    }
    case OpCode::kLCastI:
    {
      long val;
      POP_LONG(val);
      PUSH_INT(static_cast<int>(val));
      _pc += 2;
      break;
    }
    case OpCode::kFCastI:
    {
      float val;
      POP_FLOAT(val);
      PUSH_INT(static_cast<int>(val));
      _pc += 2;
      break;
    }
    case OpCode::kFCastL:
    {
      float val;
      POP_FLOAT(val);
      PUSH_LONG(static_cast<long>(val));
      _pc += 2;
      break;
    }
    case OpCode::kFCastD:
    {
      float val;
      POP_FLOAT(val);
      PUSH_DOUBLE(static_cast<double>(val));
      _pc += 2;
      break;
    }
    case OpCode::kDCastI:
    {
      double val;
      POP_DOUBLE(val);
      PUSH_INT(static_cast<int>(val));
      _pc += 2;
      break;
    }
    case OpCode::kDCastF:
    {
      double val;
      POP_DOUBLE(val);
      PUSH_FLOAT(static_cast<float>(val));
      _pc += 2;
      break;
    }
    case OpCode::kDCastL:
    {
      double val;
      POP_DOUBLE(val);
      PUSH_LONG(static_cast<long>(val));
      _pc += 2;
      break;
    }
    case OpCode::kLoadI:
    {
      int ix = *(u2 *)(_code_buf + _pc + 2);
      PUSH_INT(*(int *)(_local_var_table + ix));
      _pc += 4;
      break;
    }
    case OpCode::kLoadD:
    {
      int ix = *(u2 *)(_code_buf + _pc + 2);
      PUSH_DOUBLE(*(double *)(_local_var_table + ix));
      _pc += 4;
      break;
    }
    case OpCode::kLoadF:
    {
      int ix = *(u2 *)(_code_buf + _pc + 2);
      PUSH_FLOAT(*(float *)(_local_var_table + ix));
      _pc += 4;
      break;
    }
    case OpCode::kLoadL:
    {
      int ix = *(u2 *)(_code_buf + _pc + 2);
      PUSH_LONG(*(long *)(_local_var_table + ix));
      _pc += 4;
      break;
    }
    case OpCode::kStoreI:
    {
      int ix = *(u2 *)(_code_buf + _pc + 2);
      POP_INT(*(int *)(_local_var_table + ix));
      _pc += 4;
      break;
    }
    case OpCode::kStoreD:
    {
      int ix = *(u2 *)(_code_buf + _pc + 2);
      POP_DOUBLE(*(double *)(_local_var_table + ix));
      _pc += 4;
      break;
    }
    case OpCode::kStoreF:
    {
      int ix = *(u2 *)(_code_buf + _pc + 2);
      POP_FLOAT(*(float *)(_local_var_table + ix));
      _pc += 4;
      break;
    }
    case OpCode::kStoreL:
    {
      int ix = *(u2 *)(_code_buf + _pc + 2);
      POP_LONG(*(long *)(_local_var_table + ix));
      _pc += 4;
      break;
    }
    case OpCode::kPrintI:
    {
      int val;
      POP_INT(val);
      std::cout << val << "\n";
      _pc += 4;
      break;
    }
    case OpCode::kPrintF:
    {
      float val;
      POP_FLOAT(val);
      std::cout << val << "\n";
      _pc += 4;
      break;
    }
    case OpCode::kPrintD:
    {
      double val;
      POP_DOUBLE(val);
      std::cout << val << "\n";
      _pc += 4;
      break;
    }
    case OpCode::kPrintL:
    {
      long val;
      POP_LONG(val);
      std::cout << val << "\n";
      _pc += 4;
      break;
    }
    case OpCode::kDone:
      // program ends here
      return;
    default:
      char buf[ERROR_MSG_LEN];
      std::snprintf(buf, ERROR_MSG_LEN, "unimplemented bytecode %d",
                    _code_buf[_pc]);
      panic(buf, __FILE__, __LINE__);
    }
}