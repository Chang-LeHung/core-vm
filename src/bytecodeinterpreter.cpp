

#include "bytecodeinterpreter.h"
#include "opcode.h"
#include "types.h"
#include "util.h"

#define TOP_AS_TYPE(type) (*(type *)((u8 *)_stack + _top))
#define POP_INT()                                                              \
  TOP_AS_TYPE(int);                                                            \
  _top--

#define POP_DOUBLE()                                                           \
  TOP_AS_TYPE(double);                                                         \
  _top--

#define POP_FLOAT()                                                            \
  TOP_AS_TYPE(float);                                                          \
  _top--

#define POP_LONG()                                                             \
  TOP_AS_TYPE(long);                                                           \
  _top--

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
  type lhs = macro();                                                          \
  type rhs = macro()

void ByteCodeInterpreter::Run()
{
  for (; _pc < _code_size;)
    switch (static_cast<OpCode>(_code_buf[_pc]))
    {
    case OpCode::kIAdd:
    {
      POP_TWO(int, POP_INT);
      PUSH_INT(lhs + rhs);
      _pc++;
      break;
    }
    case OpCode::kISub:
    {
      POP_TWO(int, POP_INT);
      PUSH_INT(lhs - rhs);
      _pc++;
      break;
    }
    case OpCode::kIMul:
    {
      POP_TWO(int, POP_INT);
      PUSH_INT(lhs * rhs);
      _pc++;
      break;
    }
    case OpCode::kIDiv:
    {
      POP_TWO(int, POP_INT);
      PUSH_INT(lhs / rhs);
      _pc++;
      break;
    }
    case OpCode::kIMod:
    {
      POP_TWO(int, POP_INT);
      PUSH_INT(lhs % rhs);
      _pc++;
      break;
    }
    case OpCode::kDAdd:
    {
      POP_TWO(double, POP_DOUBLE);
      PUSH_DOUBLE(lhs + rhs);
      _pc++;
      break;
    }
    case OpCode::kDSub:
    {
      POP_TWO(double, POP_DOUBLE);
      PUSH_DOUBLE(lhs - rhs);
      _pc++;
      break;
    }
    case OpCode::kDMul:
    {
      POP_TWO(double, POP_DOUBLE);
      PUSH_DOUBLE(lhs * rhs);
      _pc++;
      break;
    }
    case OpCode::kDDiv:
    {
      POP_TWO(double, POP_DOUBLE);
      PUSH_DOUBLE(lhs / rhs);
      _pc++;
      break;
    }
    case OpCode::kFAdd:
    {
      POP_TWO(float, POP_FLOAT);
      PUSH_FLOAT(lhs + rhs);
      _pc++;
      break;
    }
    case OpCode::kFSub:
    {
      POP_TWO(float, POP_FLOAT);
      PUSH_FLOAT(lhs - rhs);
      _pc++;
      break;
    }
    case OpCode::kFMul:
    {
      POP_TWO(float, POP_FLOAT);
      PUSH_FLOAT(lhs * rhs);
      _pc++;
    }
    case OpCode::kFDiv:
    {
      POP_TWO(float, POP_FLOAT);
      PUSH_FLOAT(lhs / rhs);
      _pc++;
      break;
    }
    case OpCode::kIConst:
    {
      int val = *(int *)(_code_buf + _pc + 1);
      PUSH_INT(val);
      _pc += 5;
      break;
    }
    case OpCode::kDConst:
    {
      double val = *(double *)(_code_buf + _pc + 1);
      PUSH_DOUBLE(val);
      _pc += 9;
      break;
    }
    case OpCode::kFConst:
    {
      float val = *(float *)(_code_buf + _pc + 1);
      PUSH_FLOAT(val);
      _pc += 5;
      break;
    }
    case OpCode::kLConst:
    {
      long val = *(long *)(_code_buf + _pc + 1);
      PUSH_LONG(val);
      _pc += 9;
      break;
    }
    case OpCode::kICastD:
    {
      int val = POP_INT();
      PUSH_DOUBLE(static_cast<double>(val));
      _pc++;
      break;
    }
    case OpCode::kICastF:
    {
      int val = POP_INT();
      PUSH_FLOAT(static_cast<float>(val));
      _pc++;
      break;
    }
    case OpCode::kICastL:
    {
      int val = POP_INT();
      PUSH_LONG(static_cast<long>(val));
      _pc++;
      break;
    }
    case OpCode::kLCastD:
    {
      long val = POP_LONG();
      PUSH_DOUBLE(static_cast<double>(val));
      _pc++;
      break;
    }
    case OpCode::kLCastF:
    {
      long val = POP_LONG();
      PUSH_FLOAT(static_cast<float>(val));
      _pc++;
      break;
    }
    case OpCode::kLCastI:
    {
      long val = POP_LONG();
      PUSH_INT(static_cast<int>(val));
      _pc++;
    }
    case OpCode::kFCastI:
    {
      float val = POP_FLOAT();
      PUSH_INT(static_cast<int>(val));
      _pc++;
    }
    case OpCode::kFCastL:
    {
      float val = POP_FLOAT();
      PUSH_LONG(static_cast<long>(val));
      _pc++;
    }
    case OpCode::kFCastD:
    {
      float val = POP_FLOAT();
      PUSH_DOUBLE(static_cast<double>(val));
      _pc++;
    }
    case OpCode::kDCastI:
    {
      double val = POP_DOUBLE();
      PUSH_INT(static_cast<int>(val));
      _pc++;
    }
    case OpCode::kDCastF:
    {
      double val = POP_DOUBLE();
      PUSH_FLOAT(static_cast<float>(val));
      _pc++;
    }
    case OpCode::kDCastL:
    {
      double val = POP_DOUBLE();
      PUSH_LONG(static_cast<long>(val));
      _pc++;
    }
    case OpCode::kLoadI:
    {
      int ix = *(u2 *)(_code_buf + _pc + 1);
      PUSH_INT(*(int *)(_stack + ix));
      _pc += 3;
    }
    case OpCode::kLoadD:
    {
      int ix = *(u2 *)(_code_buf + _pc + 1);
      PUSH_DOUBLE(*(double *)(_stack + ix));
      _pc += 3;
    }
    case OpCode::kLoadF:
    {
      int ix = *(u2 *)(_code_buf + _pc + 1);
      PUSH_FLOAT(*(float *)(_stack + ix));
      _pc += 3;
    }
    case OpCode::kLoadL:
    {
      int ix = *(u2 *)(_code_buf + _pc + 1);
      PUSH_LONG(*(long *)(_stack + ix));
      _pc += 3;
    }
    case OpCode::kStoreI:
    {
      int ix = *(u2 *)(_code_buf + _pc + 1);
      *(int *)(_stack + ix) = POP_INT();
      _pc += 3;
    }
    case OpCode::kStoreD:
    {
      int ix = *(u2 *)(_code_buf + _pc + 1);
      *(double *)(_stack + ix) = POP_DOUBLE();
      _pc += 3;
    }
    case OpCode::kStoreF:
    {
      int ix = *(u2 *)(_code_buf + _pc + 1);
      *(float *)(_stack + ix) = POP_FLOAT();
      _pc += 3;
    }
    case OpCode::kStoreL:
    {
      int ix = *(u2 *)(_code_buf + _pc + 1);
      *(long *)(_stack + ix) = POP_LONG();
      _pc += 3;
    }
    default:
      panic("unimplemented bytecode", __FILE__, __LINE__);
    }
}