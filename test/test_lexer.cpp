
#include "lexer.h"
#include "loader.h"
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
  std::vector<CoreVMADT> result = lexer.DoParse();
  std::ostringstream os;
  lexer.Serialize(os);
  std::cout << os.str() << std::endl;
  return 0;
}