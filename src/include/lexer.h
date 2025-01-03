

#pragma once

#include "loader.h"
#include "types.h"
#include "util.h"
#include <vector>

enum class SymbolType
{
  kString,
  kStringVar,
  kInt,
  kLong,
  kDouble,
  kFloat,
  kLParentheses,
  kRParentheses,
  kRBrace,
  kLBrace,
  kChar,
  kPlus,
  kMul,
  kDiv,
  kMinus,
  kEq,
  kComma,
  kLQuote,
  kRQuote,
  kSemicolon,
};

class CoreVMADT : public Seriable
{
private:
  union ADT
  {
    u1 cval;
    u4 ival;    // int
    u8 lval;    // long
    f4 fval;    // float
    f8 dval;    // double
    char *sval; // string, if as sval, CorVMADT owns the string, need free the
                // memory at constructor
  };
  ADT _data;
  SymbolType _tag;

public:
  template <typename T> CoreVMADT(T data, SymbolType tag);

  CoreVMADT(const CoreVMADT &adt);

  CoreVMADT(CoreVMADT &&adt);

  CoreVMADT &operator=(const CoreVMADT &adt);

  CoreVMADT &operator=(CoreVMADT &&adt);

  int GetIntADT() const;

  long GetLongADT() const;

  float GetFloatADT() const;

  double GetDoubleADT() const;

  char GetCharADT() const;

  void *GetPtrADT() const;

  const char *GetCharPtrADT() const;

  inline SymbolType GetTag() const { return _tag; }

  void Serialize(std::ostream &os);

  inline bool IsString() const { return _tag == SymbolType::kString; }

  inline bool IsInt() const { return _tag == SymbolType::kInt; }

  inline bool IsDouble() const { return _tag == SymbolType::kDouble; }

  inline bool IsFloat() const { return _tag == SymbolType::kFloat; }

  inline bool IsSemicolon() const { return _tag == SymbolType::kSemicolon; }

  inline bool IsPlus() const { return _tag == SymbolType::kPlus; }

  inline bool IsMinus() const { return _tag == SymbolType::kMinus; }

  inline bool IsMul() const { return _tag == SymbolType::kMul; }

  inline bool IsDiv() const { return _tag == SymbolType::kDiv; }

  inline bool IsLParentheses() const
  {
    return _tag == SymbolType::kLParentheses;
  }

  inline bool IsComma() const { return _tag == SymbolType::kComma; }

  inline bool IsRParentheses() const
  {
    return _tag == SymbolType::kRParentheses;
  }

  inline bool IsEq() const { return _tag == SymbolType::kEq; }

  virtual ~CoreVMADT();
};

class CoreVMLexer : public Seriable
{
private:
  FileInputStream _is;
  std::vector<CoreVMADT> _tokens;

public:
  CoreVMLexer(FileInputStream is);

  const std::vector<CoreVMADT> &DoParse();

  virtual void Serialize(std::ostream &os);
};