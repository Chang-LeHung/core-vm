
#include "assembler.h"
#include "lexer.h"
#include "loader.h"
#include "parser.h"
#include "types.h"
#include "util.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>
#include <sys/wait.h>
#include <vector>

void hexdump_with_xxd(const void *data, size_t size)
{
  int pipefd[2];
  if (pipe(pipefd) == -1)
  {
    perror("pipe");
    return;
  }

  pid_t pid = fork();
  if (pid == -1)
  {
    perror("fork");
    return;
  }

  if (pid == 0)
  {
    close(pipefd[1]);

    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);

    execlp("xxd", "xxd", "-g", "1", NULL);

    perror("execlp");
    _exit(1);
  }
  else
  {
    close(pipefd[0]);

    write(pipefd[1], data, size);
    close(pipefd[1]);
    waitpid(pid, nullptr, 0);
  }
}

bool IsEq(const char *c1, const char *c2, int size)
{
  for (int i = 0; i < size; ++i)
  {
    if (c1[i] != c2[i])
    {
      return false;
    }
  }
  return true;
}

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
  Parser parser{result};
  auto ir = parser.Build();
  std::ostringstream byte_code;
  ir->Dump(byte_code);
  std::cout << byte_code.str() << std::endl;
  CVMAssembler assembler(1024);
  ir->Emit(assembler);
  const char *code_buf = assembler.GetCode();
  int size = assembler.GetPos() - 2; // delete kDone
  hexdump_with_xxd(code_buf, size);
  if (!strcmp(filename, "../test/test_lexer.cvm"))
  {
    char code[] = {0x12, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00,
                   0x00, 0x12, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1a, 0x00,
                   0x01, 0x00, 0x15, 0x00, 0x0e, 0x2d, 0xb2, 0x9d, 0xef,
                   0xa3, 0x6a, 0x40, 0x1b, 0x00, 0x02, 0x00};
    assert(size == 34);
    assert(IsEq((char *)code, code_buf, size));
    std::cout << "test_lexer.cvm passed\n";
  }
  else if (!strcmp(filename, "../test/test_cast.cvm"))
  {
    char code[] = {
        0x15, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xf3, 0x3f,
        0x27, 0x00, 0x15, 0x00, 0x9a, 0x99, 0x99, 0x99, 0x99, 0x99,
        0xf1, 0x3f, 0x27, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00,
    };
    assert(size == 30);
    assert(IsEq((char *)code, code_buf, size));
    std::cout << "test_cast.cvm passed\n";
  }
  else if (!strcmp(filename, "../test/test_parser.cvm"))
  {
    char code[] = {0x12, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00,
                   0x12, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x01, 0x00,
                   0x15, 0x00, 0x0e, 0x2d, 0xb2, 0x9d, 0xef, 0xa3, 0x6a, 0x40,
                   0x1b, 0x00, 0x02, 0x00, 0x16, 0x00, 0x01, 0x00, 0x16, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x03, 0x00, 0x16, 0x00,
                   0x03, 0x00, 0x2a, 0x00, 0x01, 0x00, 0x12, 0x00, 0x04, 0x00,
                   0x00, 0x00, 0x12, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00,
                   0x12, 0x00, 0x02, 0x00, 0x00, 0x00, 0x12, 0x00, 0x01, 0x00,
                   0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x04, 0x00,
                   0x16, 0x00, 0x04, 0x00, 0x2a, 0x00, 0x01, 0x0};
    assert(size == 98);
    assert(IsEq((char *)code, code_buf, size));
    std::cout << "test_parser.cvm passed\n";
  }
  return 0;
}