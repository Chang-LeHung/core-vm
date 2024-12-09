

#pragma once
#include <assert.h>
#include <cstring>

class CVMAssembler
{
private:
  char *_code;
  int _pos;
  int _capacity;

public:
  CVMAssembler(int capacity) : _pos(0)
  {
    assert(capacity > 0);
    _capacity = RoundUp(capacity);
    if (_capacity > 1)
    {
      _code = new char[_capacity];
    }
  }

  CVMAssembler(CVMAssembler &&asm_)
  {
    _code = asm_._code;
    _pos = asm_._pos;
    _capacity = asm_._capacity;
    asm_._code = nullptr;
  }

  CVMAssembler(const CVMAssembler &asm_)
  {
    _code = asm_._code;
    _pos = asm_._pos;
    _capacity = asm_._capacity;
    _code = new char[_capacity];
    std::memcpy(_code, asm_._code, _capacity);
  }

  CVMAssembler &operator=(CVMAssembler &&asm_)
  {
    _code = asm_._code;
    _pos = asm_._pos;
    _capacity = asm_._capacity;
    asm_._code = nullptr;
    return *this;
  }

  CVMAssembler &operator=(const CVMAssembler &asm_)
  {
    _code = asm_._code;
    _pos = asm_._pos;
    _capacity = asm_._capacity;
    _code = new char[_capacity];
    std::memcpy(_code, asm_._code, _capacity);
    return *this;
  }

  static int RoundUp(int val)
  {
    int res = 1;
    while (res < val)
      res <<= 1;
    assert(res > 0);
    return res;
  }

  template <typename T> void WriteStream(T val)
  {
    int end = _pos + sizeof(T);
    // resize if needed
    if (end >= _capacity)
    {
      int new_size = _capacity << 1;
      assert(new_size > 0);
      char *buf = new char[new_size];
      std::memcpy(buf, _code, _pos);
      _capacity = new_size;
      delete[] _code;
      _code = buf;
    }
    assert(end < _capacity);
    char *offset = _code + _pos;
    *(T *)offset = val;
    _pos += sizeof(T); // forward cursor
  }

  template <typename T> T ReadStream()
  {
    assert(_pos + sizeof(T) <= _capacity);
    char *offset = _code + _pos;
    _pos += sizeof(T);
    return *(T *)offset;
  }

  void Reset(int pos) { _pos = pos; }

  virtual ~CVMAssembler()
  {
    if (_code != nullptr)
      delete[] _code;
  }

  inline const char *GetCode() const { return _code; }

  inline int GetPos() const { return _pos; }
};