
#pragma once
#include "ir.h"
#include <memory>
#include <sstream>
#include <string>

#define K *(1024)
#define M (1024 * K)

void InitCompiler();

class Compiler
{
private:
  std::string _filename;
  std::shared_ptr<char> _code;
  std::shared_ptr<IR> _ir;
  std::ostringstream _os;
  int _code_size;

public:
  Compiler(const std::string &filename) : _filename(filename), _code(nullptr) {}

  void Compile();

  std::string GetByteCode() const { return _os.str(); }

  virtual ~Compiler() {}

  inline std::shared_ptr<char> GetBinaryCode() const { return _code; }

  inline int GetCodeSize() const { return _code_size; }
};