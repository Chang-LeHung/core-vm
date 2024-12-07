
#pragma once
#include <string>

class Parser
{
public:
  auto parse(const std::string &filename);
};

enum class CoreVMType
{
  kInt,
  kLong,
  kDouble,
  kFloat,
  kIntArr,
  kLongArr,
  kDoubleArr,
  kFloatArr,
};

auto add(int a, int b) -> int;
