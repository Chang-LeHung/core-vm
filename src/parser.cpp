

#include "parser.h"
#include "ir.h"
#include "lexer.h"
#include "opcode.h"
#include "types.h"
#include "util.h"
#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <linux/limits.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

CVMType CVMType::GetOrCreateType(const std::string &name)
{
  auto it = _type_set.find(name);
  if (it != _type_set.end())
  {
    return it->second;
  }
  else
  {
    CVMType type(name);
    _type_set.insert({name, type});
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

std::shared_ptr<IR> Parser::Term()
{
  auto res = Primary();
  while (HashNext())
  {
    if (Next(false).IsMul())
    {
      Next(); // eat the token
      auto rhs = Primary();
      res = std::make_shared<BinaryIR>(OpCode::kIMul, res, rhs);
    }
    else if (Next(false).IsDiv())
    {
      Next(); // eat the token
      auto rhs = Primary();
      res = std::make_shared<BinaryIR>(OpCode::kIDiv, res, rhs);
    }
    else
    {
      break;
    }
  }

  return res;
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
    program->AddSharedIR(ir);
  }
  return program;
}

std::shared_ptr<AssignStmtIR>
Parser::ExtraceAssignIR(const std::string &type_name, const std::string &symbol,
                        int loc)
{
  auto expr = Expression();
  auto store = std::make_shared<StoreIR>(
      symbol, loc, CVMType::GetOrCreateType(type_name),
      GetStoreOpcodeByType(CVMType::GetOrCreateType(type_name)));
  if (expr->GetResType() != store->GetResType()) [[unlikely]]
  {
    char error_msg[ERROR_MSG_LEN];
    assert(symbol.size() < MAX_VAR_LEN);
    std::sprintf(error_msg, "type mismatch var name is %s", symbol.c_str());
    panic(error_msg, __FILE__, __LINE__);
  }
  return std::make_shared<AssignStmtIR>(store, expr, expr->GetResType());
}

std::shared_ptr<IR> Parser::Statement()
{
  auto &token = Next();
  const char *buf;
  std::string name;
  switch (token.GetTag())
  {
  case SymbolType::kString:
  {
    buf = token.GetCharPtrADT();
    name = std::string(buf);
    if (CVMType::ContainType(name) && HashNext() && Next(false).IsString())
    {
      auto nxt_s = std::string(Next().GetCharPtrADT());
      if (HashNext())
      {
        int loc = AddSymbol(nxt_s, name);
        if (Next(false).IsSemicolon())
        {
          // a declaration
          Next(); // eat semicolon
          return std::make_shared<DefinitionIR>(nxt_s, loc,
                                                CVMType::GetOrCreateType(name));
        }
        else if (Next(false).IsEq())
        {
          Next(); // eat equal
          return ExtraceAssignIR(name, nxt_s, loc);
        }
        else
        {
          panic("illegal statement", __FILE__, __LINE__);
        }
      }
      else
      {
        panic("expected a semicolon or expression", __FILE__, __LINE__);
      }
    }
    else
    {
      if (HashNext() && Next(false).IsEq())
      {
        Next(); // eat assign
        // return ExtraceAssignIR(name, nxt_s, loc);
      }
      panic("expected a eq", __FILE__, __LINE__);
    }
  }
  case SymbolType::kSemicolon:
    return std::make_shared<NopIR>();
  default:
    panic("unexpected token in statement", __FILE__, __LINE__);
  }
}

std::shared_ptr<IR> Parser::Expression()
{
  auto res = Term();
  while (HashNext())
  {
    if (Next(false).IsPlus())
    {
      Next();
      res = std::make_shared<BinaryIR>(OpCode::kIAdd, res, Term());
    }
    else if (Next(false).IsMinus())
    {
      Next();
      res = std::make_shared<BinaryIR>(OpCode::kISub, res, Term());
    }
    else
      break;
  }
  return res;
}

int Parser::AddSymbol(const std::string &symbol, const std::string &type_name)
{
  int size = _symbol_table.size();
  _symbol_table.insert(
      {symbol, std::make_pair(size, CVMType::GetOrCreateType(type_name))});
  return size;
}

int Parser::SymbolIx(const std::string &symbol) const
{
  auto it = _symbol_table.find(symbol);
  if (it == _symbol_table.end())
    return -1;
  return it->second.first;
}

std::shared_ptr<IR> Parser::Primary()
{
  if (!HashNext())
    panic("Primary require a token", __FILE__, __LINE__);
  auto token = Next();
  switch (token.GetTag())
  {
  case SymbolType::kInt:
    return std::make_shared<Constant>(token, OpCode::kIConst,
                                      CVMType::GetOrCreateType("int"));
  case SymbolType::kFloat:
    return std::make_shared<Constant>(token, OpCode::kFConst,
                                      CVMType::GetOrCreateType("float"));
  case SymbolType::kLong:
    return std::make_shared<Constant>(token, OpCode::kLConst,
                                      CVMType::GetOrCreateType("long"));
  case SymbolType::kDouble:
    return std::make_shared<Constant>(token, OpCode::kDConst,
                                      CVMType::GetOrCreateType("double"));
  case SymbolType::kString:
  {
    // load a variable
    auto var_name = std::string(token.GetCharPtrADT());
    auto it = _symbol_table.find(var_name);
    if (it == _symbol_table.end()) [[unlikely]]
    {
      char buf[ERROR_MSG_LEN];
      std::sprintf(buf, "unknown variable: %s", var_name.c_str());
      panic(buf, __FILE__, __LINE__);
    }
    return std::make_shared<VariableIR>(var_name, it->second.first,
                                        GetLoadOpcodeByType(it->second.second),
                                        it->second.second);
  }
  case SymbolType::kLParentheses:
  {
    auto pos = GetPos();
    const auto token = Next();
    if (IsType(token) && HashNext() && Next().IsRParentheses())
    {
      Next(); // eat rparentheses
      auto type = CVMType::GetOrCreateType(token.GetCharPtrADT());
      const auto &ir = Primary();
      return std::make_shared<CastIR>(GetCastCode(ir->GetResType(), type), type,
                                      ir);
    }
    else
    {
      SetPos(pos);
      auto res = Expression();
      auto token = Next();
      if (token.IsRParentheses())
        panic("require )", __FILE__, __LINE__);
      return res;
    }
  }
  default:
    panic("Primary require a number or string", __FILE__, __LINE__);
  }
}

void CVMType::InitIntrinsicType()
{
  CVMType::GetOrCreateType("int");
  CVMType::GetOrCreateType("float");
  CVMType::GetOrCreateType("double");
  CVMType::GetOrCreateType("long");
}

bool Parser::IsType(const CoreVMADT &adt)
{
  return adt.IsString() && CVMType::ContainType(adt.GetCharPtrADT());
}

OpCode Parser::GetCastCode(const CVMType &from, const CVMType &to)
{
  if (from == to)
    return OpCode::kNop;
  if (from.IsInt())
  {
    if (to.IsLong())
      return OpCode::kICastL;
    if (to.IsFloat())
      return OpCode::kICastF;
    if (to.IsDouble())
      return OpCode::kICastD;
  }
  else if (from.IsFloat())
  {
    if (to.IsInt())
      return OpCode::kFCastI;
    if (to.IsDouble())
      return OpCode::kFCastD;
    if (to.IsLong())
      return OpCode::kFCastL;
  }
  else if (from.IsLong())
  {
    if (to.IsInt())
      return OpCode::kLCastI;
    if (to.IsFloat())
      return OpCode::kLCastF;
    if (to.IsDouble())
      return OpCode::kLCastD;
  }
  else if (from.IsDouble())
  {
    if (to.IsInt())
      return OpCode::kDCastI;
    if (to.IsFloat())
      return OpCode::kDCastF;
    if (to.IsLong())
      return OpCode::kDCastL;
  }
  panic("unsupported cast", __FILE__, __LINE__);
}