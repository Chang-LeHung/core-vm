
#include "bytecodeinterpreter.h"
#include "compiler.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/wait.h>

void Hexdump(const void *data, size_t size)
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

class CVMConfig
{
private:
  std::string _filename;
  bool _dump_bytecode;
  bool _dump_binary;
  bool _use_asm;

public:
  explicit CVMConfig(char **argv) : _dump_binary(false), _dump_bytecode(false)
  {
    std::string arg;
    argv++; // eat program name
    while (*argv != NULL)
    {
      arg = *argv++;
      if (arg.size() == 0)
        panic("Invalid argument", __FILE__, __LINE__);
      if (arg.starts_with("-"))
      {
        if (arg == "-d")
        {
          _dump_bytecode = true;
        }
        else if (arg == "-b")
        {
          _dump_binary = true;
        }
        else if (arg == "-h")
        {
          std::cout << "Usage: cvm(core-vm) [-d] [-b] <filename>" << std::endl;
          // explain usage
          std::cout << "-d: dump bytecode" << std::endl;
          std::cout << "-b: dump binary code" << std::endl;
          exit(EXIT_SUCCESS);
        }
        else if (arg == "-f")
        {
          _use_asm = true;
        }
      }
      else if (arg.size() > 3 && arg.ends_with(".cvm"))
      {
        _filename = arg;
      }
    }
    if (_filename.empty())
      panic("please specify a cvm file", __FILE__, __LINE__);
  }

  inline bool IsDumpBinary() const { return _dump_binary; }

  inline bool IsDumpBytecode() const { return _dump_bytecode; }
};

int main(int argc, char **argv)
{
  CVMConfig config(argv);
  InitCompiler();
  const char *filename = argv[1];
  Compiler compiler(filename);
  compiler.Compile();
  ByteCodeInterpreter interpreter(compiler.GetBinaryCode().get(),
                                  compiler.GetCodeSize(), 1 KB,
                                  compiler.GetLocalVarTableSize());
  if (config.IsDumpBytecode())
  {
    std::cout << "ByteCode:\n";
    std::cout << compiler.GetByteCode() << std::endl;
  }
  if (config.IsDumpBinary())
  {
    std::cout << "BinaryCode:\n";
    Hexdump(compiler.GetBinaryCode().get(), compiler.GetCodeSize());
  }
  interpreter.Run();
  return 0;
}