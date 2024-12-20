
#include "compiler.h"
#include "assembler.h"
#include "lexer.h"
#include "parser.h"
#include "types.h"
#include <cstring>
#include <memory>

void Compiler::Compile()
{
  CoreVMLexer lexer{FileInputStream(_filename)};
  const std::vector<CoreVMADT> &result = lexer.DoParse();
  Parser parser{result};
  _ir = parser.Build();
  _ir->Dump(_os);
  CVMAssembler assembler{1 K};
  _ir->Emit(assembler);
  _code_size = assembler.GetPos();
  _sym_size = parser.SymTableSize();
  auto code = new char[_code_size];
  std::memcpy(code, assembler.GetCode(), _code_size);
  // using shared_ptr to avoid unnecessary memory copy.
  _code = std::shared_ptr<char>(code, [](char *ptr) { delete[] ptr; });
}

void InitCompiler()
{
  CVMType::InitIntrinsicType();
}