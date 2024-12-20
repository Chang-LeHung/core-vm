
#pragma once
#include <stdint.h>
#include <string>
#include <unordered_map>

#define u1 uint8_t
#define u2 uint16_t
#define u4 uint32_t
#define u8 uint64_t
#define f8 double
#define f4 float
#define i1 int8_t
#define i2 int16_t
#define i4 int32_t
#define i8 int64_t
#define MAX_VAR_LEN 64
#define ERROR_MSG_LEN 128

class CVMType
{
public:
  static std::unordered_map<std::string, CVMType> _type_set;

  static void InitIntrinsicType();

private:
  std::string _name;

public:
  bool operator==(const CVMType &other) const { return _name == other._name; }

  bool operator!=(const CVMType &other) const { return _name != other._name; }

  bool IsInt() const { return _name == "int"; }

  bool IsFloat() const { return _name == "float"; }

  bool IsDouble() const { return _name == "double"; }

  bool IsLong() const { return _name == "long"; }

  CVMType(std::string name) : _name(name) {}

  CVMType(const CVMType &type) { _name = type._name; }

  const std::string &GetName() const { return _name; }

  static CVMType GetOrCreateType(const std::string &name);

  [[maybe_unused]] static bool ContainType(const std::string &name);

  static bool ContainType(const CVMType &type);
};

extern CVMType void_op;

namespace std
{
template <> struct hash<CVMType>
{
  size_t operator()(const CVMType &type) const
  {
    return hash<string>()(type.GetName());
  }
};
} // namespace std
