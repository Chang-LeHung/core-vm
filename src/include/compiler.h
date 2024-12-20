
#pragma once
#include "ir.h"
#include <memory>
#include <sstream>
#include <string>

#define KB *(1024)
#define MB *(1024 KB)

void InitCompiler();

class Compiler
{
private:
  std::string _filename;
  std::shared_ptr<char> _code;
  std::shared_ptr<IR> _ir;
  std::ostringstream _os;
  int _code_size;
  int _sym_size;

public:
  Compiler(const std::string &filename) : _filename(filename), _code(nullptr) {}

  void Compile();

  std::string GetByteCode() const { return _os.str(); }

  virtual ~Compiler() {}

  inline std::shared_ptr<char> GetBinaryCode() const { return _code; }

  inline int GetCodeSize() const { return _code_size; }

  inline int GetLocalVarTableSize() const { return _sym_size; }
};