

#include "lexer.h"
#include "loader.h"
#include "util.h"
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstring>

template <typename T> CoreVMADT::CoreVMADT(T data, SymbolType tag)
{
  _data.lval = (u8)data;
  _tag = tag;
}

template <> CoreVMADT::CoreVMADT(double data, SymbolType tag)
{
  _data.dval = data;
  _tag = tag;
}

template <> CoreVMADT::CoreVMADT(float data, SymbolType tag)
{
  _data.fval = data;
  _tag = tag;
}

CoreVMADT::CoreVMADT(const CoreVMADT &adt)
{
  _tag = adt._tag;
  if (_tag == SymbolType::kString)
  {
    _data.sval = (u8 *)new char[std::strlen((const char *)adt._data.sval) + 1];
    std::strcpy((char *)_data.sval, (char *)adt._data.sval);
  }
  else
  {
    _data.lval = adt._data.lval;
  }
}

CoreVMADT::CoreVMADT(CoreVMADT &&adt)
{
  _tag = adt._tag;
  _data.lval = adt._data.lval;
  if (_tag == SymbolType::kString)
  {
    adt._data.sval = (u8 *)nullptr;
  }
}

CoreVMADT &CoreVMADT::operator=(const CoreVMADT &adt)
{
  _tag = adt._tag;
  if (_tag == SymbolType::kString)
  {
    _data.sval = (u8 *)new char[std::strlen((const char *)adt._data.sval) + 1];
    std::strcpy((char *)_data.sval, (char *)adt._data.sval);
  }
  else
  {
    _data.lval = adt._data.lval;
  }
  return *this;
}

CoreVMADT &CoreVMADT::operator=(CoreVMADT &&adt)
{
  _tag = adt._tag;
  _data.lval = adt._data.lval;
  if (_tag == SymbolType::kString)
  {
    adt._data.sval = (u8 *)nullptr;
  }
  return *this;
}

double CoreVMADT::GetDoubleADT() const
{
  return _data.dval;
}

char CoreVMADT::GetCharADT() const
{
  return _data.cval;
}

float CoreVMADT::GetFloatADT() const
{
  return _data.fval;
}

int CoreVMADT::GetIntADT() const
{
  return _data.ival;
}

long CoreVMADT::GetLongADT() const
{
  return _data.lval;
}

void CoreVMADT::Serialize(std::ostream &os)
{
  switch (_tag)
  {
  case SymbolType::kDouble:
    os << GetDoubleADT();
    break;
  case SymbolType::kInt:
    os << GetIntADT();
    break;
  case SymbolType::kString:
    os << GetCharPtrADT();
    break;
  case SymbolType::kLParentheses:
    os << '(';
    break;
  case SymbolType::kRParentheses:
    os << ')';
    break;
  case SymbolType::kLBrace:
    os << '{';
    break;
  case SymbolType::kRBrace:
    os << '}';
    break;
  case SymbolType::kChar:
    os << GetCharADT();
    break;
  case SymbolType::kEq:
    os << '=';
    break;
  case SymbolType::kMinus:
    os << '-';
    break;
  case SymbolType::kMul:
    os << '*';
    break;
  case SymbolType::kDiv:
    os << '/';
    break;
  case SymbolType::kPlus:
    os << '+';
    break;
  case SymbolType::kSemicolon:
    os << ';';
    break;
  case SymbolType::kComma:
    os << ',';
    break;
  case SymbolType::kLQuote:
    os << '\"';
    break;
  case SymbolType::kRQuote:
    os << '\"';
    break;
  default:
    panic("Invalid symbol type", __FILE__, __LINE__);
  }
}

void *CoreVMADT::GetPtrADT() const
{
  return _data.sval;
}

const char *CoreVMADT::GetCharPtrADT() const
{
  return (const char *)_data.sval;
}

CoreVMADT::~CoreVMADT()
{
  if (_tag == SymbolType::kString)
  {
    delete[] _data.sval;
  }
}

CoreVMLexer::CoreVMLexer(FileInputStream is) : _is(std::move(is))
{
}

const std::vector<CoreVMADT> &CoreVMLexer::DoParse()
{
  char c;
  char buf[1024];
#define MEMCPY(buf, _is, begin_pos, end_pos)                                   \
  std::memcpy(buf, (char *)_is.Buffer() + begin_pos, end_pos - begin_pos)
  while (_is.HasNext())
  {
    c = _is.Next();
    if (std::isdigit(c))
    {
      int begin_pos = _is.Current() - 1;
      int dotn = 0;
      while (_is.HasNext() && (std::isdigit(c) || (c == '.' && dotn++ == 0)))
      {
        c = _is.Next();
      }
      if (dotn > 1)
      {
        std::sprintf(buf, "Invalid number at %d", begin_pos);
        panic(buf, __FILE__, __LINE__); // program exits here
      }
      int end_pos = _is.Current();
      MEMCPY(buf, _is, begin_pos, end_pos);
      buf[end_pos - begin_pos] = '\0';
      if (dotn == 1)
      {
        _tokens.emplace_back(atof(buf), SymbolType::kDouble);
      }
      else
      {
        assert(dotn == 0);
        if (c != 'L') [[unlikely]]
          _tokens.emplace_back(atoi(buf), SymbolType::kInt);
        else
        {
          _tokens.emplace_back(atol(buf), SymbolType::kLong);
          c = _is.Next();
        }
      }
      _is.Back();
    }
    else if (std::isalpha(c) || c == '_')
    {
      int begin_pos = _is.Current() - 1;
      while (std::isalpha(c) || c == '_' || std::isdigit(c))
      {
        c = _is.Next();
      }
      int end_pos = _is.Current();
      int size = end_pos - begin_pos;
      char *str = new char[size];
      MEMCPY(str, _is, begin_pos, end_pos);
      str[size - 1] = '\0';
      _tokens.emplace_back((u8 *)str, SymbolType::kString);
      _is.Back();
    }
    else if (c == '[')
    {
      _tokens.emplace_back('[', SymbolType::kLBrace);
    }
    else if (c == ']')
    {
      _tokens.emplace_back(']', SymbolType::kRBrace);
    }
    else if (c == '(')
    {
      _tokens.emplace_back('(', SymbolType::kLParentheses);
    }
    else if (c == ')')
    {
      _tokens.emplace_back(')', SymbolType::kRParentheses);
    }
    else if (c == ' ' || c == '\n')
    {
    }
    else if (c == '+')
    {
      _tokens.emplace_back('+', SymbolType::kPlus);
    }
    else if (c == '-')
    {
      _tokens.emplace_back('-', SymbolType::kMinus);
    }
    else if (c == '*')
    {
      _tokens.emplace_back('*', SymbolType::kMul);
    }
    else if (c == '/')
    {
      _tokens.emplace_back('/', SymbolType::kDiv);
    }
    else if (c == '=')
    {
      _tokens.emplace_back('=', SymbolType::kEq);
    }
    else if (c == ';')
    {
      _tokens.emplace_back(';', SymbolType::kSemicolon);
    }
    else if (c == ',')
    {
      _tokens.emplace_back(',', SymbolType::kComma);
    }
    else if (c == '\"')
    {
      int begin_pos = _is.Current();
      while (_is.HasNext() && _is.Next() != '\"')
        ;
      _is.Skip(-1);
      if (_is.CurrentChar() != '\"')
        panic("Invalid string literal", __FILE__, __LINE__);
      _is.Skip();
      // create a new heap buf and copy in it
      int end_pos = _is.Current();
      char *str = new char[end_pos - begin_pos + 1];
      MEMCPY(str, _is, begin_pos, end_pos);
      str[end_pos - begin_pos] = '\0';
      // pass the ownship of str to CoreVMADT
      _tokens.emplace_back((u8 *)str, SymbolType::kStringVar);
    }
    else
    {
      std::sprintf(buf, "Invalid char at %d, char = %c", _is.Current(), c);
      panic(buf, __FILE__, __LINE__);
    }
  }
  return _tokens;
}

void CoreVMLexer::Serialize(std::ostream &os)
{
  os << "Tokens:\n";
  int cnt = 1;
  for (auto &token : _tokens)
  {
    os << cnt++ << ". ";
    token.Serialize(os);
    os << "\n";
  }
}