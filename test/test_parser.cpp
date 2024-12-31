
#include "lexer.h"
#include "loader.h"
#include "parser.h"
#include "types.h"
#include "util.h"
#include <iostream>
#include <ostream>
#include <sstream>
#include <unistd.h>
#include <vector>

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    panic("invalid argc", __FILE__, __LINE__);
  }
  const char *filename = argv[1];
  CoreVMLexer lexer{FileInputStream(filename)};
  const std::vector<CoreVMADT> &result = lexer.DoParse();
  std::ostringstream os;
  lexer.Serialize(os);
  std::cout << os.str() << std::endl;

  CVMType::InitIntrinsicType();
  std::cout << CVMType::ContainType("int") << std::endl;
  std::cout << CVMType::ContainType("long") << std::endl;
  std::cout << CVMType::ContainType("float") << std::endl;
  std::cout << CVMType::ContainType("double") << std::endl;
  Parser parser{result};
  auto ir = parser.Build();
  std::ostringstream byte_code;
  ir->Dump(byte_code);
  std::cout << byte_code.str() << std::endl;
  auto code = byte_code.str();

  assert(code.find("LoadI     0  (a)") != std::string::npos);
  assert(code.find("LoadI     1  (b)") != std::string::npos);
  assert(code.find("IAdd") != std::string::npos);
  assert(code.find("StoreI    3  (d)") != std::string::npos);
  assert(code.find("") != std::string::npos);
  assert(code.find("LoadI     3  (d)") != std::string::npos);
  assert(code.find("PrintI") != std::string::npos);
  assert(code.find("IConst    1") != std::string::npos);
  assert(code.find("IConst    2") != std::string::npos);
  assert(code.find("IMul") != std::string::npos);
  assert(code.find("IConst    3") != std::string::npos);
  assert(code.find("IConst    4") != std::string::npos);
  assert(code.find("IMul") != std::string::npos);
  assert(code.find("IAdd") != std::string::npos);
  assert(code.find("StoreI    4  (ff)") != std::string::npos);
  assert(code.find("") != std::string::npos);
  assert(code.find("LoadI     4  (ff)") != std::string::npos);
  assert(code.find("PrintI") != std::string::npos);
  return 0;
}