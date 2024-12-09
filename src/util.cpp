

#include "util.h"
#include "types.h"
#include <cstdio>
#include <cstdlib>
#include <error.h>
#include <unistd.h>

[[noreturn]] void panic(const char *msg, const char *file, int line)
{
  fprintf(stderr, "%s:%d: %s\n", file, line, msg);
  exit(EXIT_FAILURE);
}

OpCode GetStoreOpcodeByType(CVMType type)
{
  if (type == CVMType::GetOrCreateType("double"))
  {
    return OpCode::kStoreD;
  }
  else if (type == CVMType::GetOrCreateType("float"))
  {
    return OpCode::kStoreF;
  }
  else if (type == CVMType::GetOrCreateType("int"))
  {
    return OpCode::kStoreI;
  }
  else if (type == CVMType::GetOrCreateType("long"))
  {
    return OpCode::kStoreL;
  }
  char error_msg[256];
  std::sprintf(error_msg, "unknown type %s", type.GetName().c_str());
  panic(error_msg, __FILE__, __LINE__);
}

OpCode GetLoadOpcodeByType(CVMType type)
{
  if (type == CVMType::GetOrCreateType("double"))
  {
    return OpCode::kLoadD;
  }
  else if (type == CVMType::GetOrCreateType("float"))
  {
    return OpCode::kLoadF;
  }
  else if (type == CVMType::GetOrCreateType("int"))
  {
    return OpCode::kLoadI;
  }
  else if (type == CVMType::GetOrCreateType("long"))
  {
    return OpCode::kLoadL;
  }
  panic("Unknown type", __FILE__, __LINE__);
}