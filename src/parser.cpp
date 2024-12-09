

#include "parser.h"
#include "ir.h"
#include "lexer.h"
#include "util.h"
#include <fcntl.h>
#include <memory>
#include <unordered_map>
#include <utility>

std::unordered_map<std::string, CVMType> CVMType::_type_set = {};

CVMType CVMType::GetOrCreateType(const std::string &name)
{
  auto it = _type_set.find(name);
  if (it != _type_set.end())
  {
    return it->second;
  }
  else
  {
    CVMType type = GetOrCreateType(name);
    _type_set[name] = type;
    return type;
  }
}

bool CVMType::ContainType(const std::string &name)
{
  return _type_set.find(name) != _type_set.end();
}

bool CVMType::ContainType(const CVMType &t)
{
  return _type_set.find(t.GetName()) != _type_set.end();
}

std::shared_ptr<IR> Parser::Atom()
{
}

std::shared_ptr<IR> Parser::Build()
{
  return Program();
}

std::shared_ptr<IR> Parser::Program()
{
  auto program = std::make_shared<ProgramIR>();
  while (_pos < _tokens.size())
  {
    auto ir = Statement();
    // program.AddIR(std::move(ir));
  }
  return program;
}

std::shared_ptr<IR> Parser::Statement()
{
  auto &token = Next();
  const char *buf;
  std::string name;
  switch (token.GetTag())
  {
  case SymbolType::kString:
    buf = token.GetCharPtrADT();
    name = std::string(buf);
    if (CVMType::ContainType(name) && HashNext() && Next(false).IsString() // look ahead
    )
    {
      auto nxt_s = std::string(Next().GetCharPtrADT());
      if (HashNext())
      {
        if (Next(false).IsSemicolon())
        {
          // a declaration
          Next(); // eat semicolon
          AddSymbol(nxt_s, name);
        }
        else if (Next(false).IsEq())
        {
          Next(); // eat equal
          auto expr = Expression();
        }
      }
      else
      {
        panic("expected a semicolon or expression", __FILE__, __LINE__);
      }
    }
    else
    {
    }
    break;
  default:
    panic("unexpected token in statement", __FILE__, __LINE__);
  }
  return std::make_shared<IR>(OpCode::kNop, void_op);
}

std::shared_ptr<IR> Parser::Expression()
{
  return std::make_shared<IR>(OpCode::kNop, void_op);
}

int Parser::AddSymbol(const std::string &symbol, const std::string &type_name)
{
  int size = _symbol_table.size();
  _symbol_table[symbol] = std::make_pair(size, CVMType::GetOrCreateType(type_name));
  return size;
}

int Parser::SymbolIx(const std::string &symbol) const
{
  auto it = _symbol_table.find(symbol);
  if (it == _symbol_table.end())
    return -1;
  return it->second.first;
}

void CVMType::InitIntrinsicType()
{
  CVMType::GetOrCreateType("int");
  CVMType::GetOrCreateType("float");
  CVMType::GetOrCreateType("double");
  CVMType::GetOrCreateType("long");
}
