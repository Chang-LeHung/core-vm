

#pragma once

#include "opcode.h"
#include "types.h"
#include <ostream>

[[noreturn]] void panic(const char *msg, const char *file, int line);

class Seriable
{
public:
  virtual void Serialize(std::ostream &os) = 0;
};

OpCode GetStoreOpcodeByType(CVMType type);

OpCode GetLoadOpcodeByType(CVMType type);