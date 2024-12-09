
#pragma once

#include "ir.h"
#include <vector>

template <typename Container = std::vector<IR>> class IRBuilder
{
private:
  Container _tokens;

public:
  IRBuilder(const Container &tokens) : _tokens(tokens) {}
};