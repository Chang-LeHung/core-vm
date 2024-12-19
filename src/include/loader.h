
#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>

class FileInputStream
{
private:
  std::string _filename;
  std::size_t _size;
  std::size_t _cur;
  const char *_buffer;

public:
  FileInputStream(const std::string &filename);

  FileInputStream(const char *buf, std::size_t size);

  FileInputStream(const FileInputStream &is)
  {
    _filename = is._filename;
    _size = is._size;
    _cur = 0;
    _buffer = new char[is._size];
    std::memcpy((void *)_buffer, is._buffer, _size);
  }

  FileInputStream(FileInputStream &&is)
  {
    _filename = is._filename;
    _size = is._size;
    _cur = 0;
    _buffer = is._buffer;
    is._buffer = (const char *)nullptr;
  }

  virtual ~FileInputStream();

  inline char Next() { return _cur < _size ? _buffer[_cur++] : '\0'; }

  inline char CurrentChar() { return _cur < _size ? _buffer[_cur] : '\0'; }

  inline bool HasNext() { return _cur < _size; }

  inline void Back() { --_cur; }

  inline bool Back(int n)
  {
    if (n > _cur)
      return false;
    _cur -= n;
    return true;
  }

  inline void Skip() { ++_cur; }

  inline void Skip(int n) { _cur += n; }

  inline int Current() { return _cur; }

  inline const char *Buffer() const { return _buffer; }

  bool Reset(std::size_t pos)
  {
    if (pos <= _size)
    {
      _cur = pos;
      return true;
    }
    else
    {
      return false;
    }
  }
};