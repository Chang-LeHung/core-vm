
#include "lexer.h"
#include "loader.h"
#include "parser.h"
#include "types.h"
#include "util.h"
#include <iostream>
#include <ostream>
#include <sstream>
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
  return 0;
}