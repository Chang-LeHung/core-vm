
#include "lexer.h"
#include "loader.h"
#include "util.h"
#include <cassert>
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
  auto tokens = os.str();
  assert(tokens.find("1. int") != std::string::npos);
  assert(tokens.find("2. a") != std::string::npos);
  assert(tokens.find("3. =") != std::string::npos);
  assert(tokens.find("4. 1") != std::string::npos);
  assert(tokens.find("5. ;") != std::string::npos);
  assert(tokens.find("6. int") != std::string::npos);
  assert(tokens.find("7. b") != std::string::npos);
  assert(tokens.find("8. =") != std::string::npos);
  assert(tokens.find("9. 1") != std::string::npos);
  assert(tokens.find("10. ;") != std::string::npos);
  assert(tokens.find("11. double") != std::string::npos);
  assert(tokens.find("12. c") != std::string::npos);
  assert(tokens.find("13. =") != std::string::npos);
  assert(tokens.find("14. 213.123") != std::string::npos);
  assert(tokens.find("15. ;") != std::string::npos);
  return 0;
}