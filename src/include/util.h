

#pragma once

#include <ostream>

void panic(const char *msg, const char *file, int line);

class Seriable
{
public:
  virtual void Serialize(std::ostream &os) = 0;
};