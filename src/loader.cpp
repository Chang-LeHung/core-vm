

#include "loader.h"
#include <assert.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>

FileInputStream::FileInputStream(const std::string &filename)
{
  auto f = fopen(filename.c_str(), "r");
  fseek(f, 0, SEEK_END);
  _size = ftell(f);
  _buffer = (char *)new char[_size];
  fseek(f, 0, SEEK_SET);
  auto s = fread((void *)_buffer, 1, _size, f);
  assert(s == _size);
}

FileInputStream::FileInputStream(const char *buf, std::size_t size)
{
  _buffer = buf;
  _size = size;
}

FileInputStream::~FileInputStream()
{
  if (_buffer != nullptr)
    delete[] _buffer;
}