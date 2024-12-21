

#include "bytecodeinterpreter.h"
#include "compiler.h"
#include <iostream>
#include <sys/wait.h>

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

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    panic("invalid argc", __FILE__, __LINE__);
  }
  InitCompiler();
  const char *filename = argv[1];
  Compiler compiler(filename);
  compiler.Compile();
  std::cout << compiler.GetByteCode() << std::endl;
  hexdump_with_xxd(
      reinterpret_cast<const void *>(compiler.GetBinaryCode().get()),
      compiler.GetCodeSize());
  ByteCodeInterpreter interpreter(compiler.GetBinaryCode().get(),
                                  compiler.GetCodeSize(), 1 KB,
                                  compiler.GetLocalVarTableSize());
  interpreter.Run();
  return 0;
}