
#pragma once

#include "ir.h"
#include "lexer.h"
#include "opcode.h"
#include "types.h"
#include <fcntl.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Parser
{

private:
  const std::vector<CoreVMADT> &_tokens;
  std::unordered_map<std::string, std::pair<int, CVMType>> _symbol_table;
  int _pos;

  static bool IsType(const CoreVMADT &adt);

  static OpCode GetCastCode(const CVMType &from, const CVMType &to);

public:
  Parser(const std::vector<CoreVMADT> &tokens) : _tokens(tokens), _pos(0)
  {
    if (_tokens.empty())
      panic("empty tokens", __FILE__, __LINE__);
  }

  std::shared_ptr<IR> Build();

private:
  std::shared_ptr<IR> Program();

  std::shared_ptr<AssignStmtIR> ExtraceAssignIR(const std::string &type_name,
                                                const std::string &symbol,
                                                int loc);

  std::shared_ptr<IR> Statement();

  std::shared_ptr<IR> Expression();

  std::shared_ptr<IR> Term();

  std::shared_ptr<IR> Primary();

  int AddSymbol(const std::string &symbol, const std::string &type_name);

  int SymbolIx(const std::string &symbol) const;

  inline bool HashNext() const { return _pos < _tokens.size(); }

  const CoreVMADT &Next(bool forward = true)
  {
    if (forward)
    {
      return _tokens[_pos++];
    }
    return _tokens[_pos];
  }

  const CoreVMADT &Prve(bool backward = true)
  {
    if (backward)
    {
      return _tokens[_pos--];
    }
    return _tokens[_pos];
  }

  inline int GetPos() { return _pos; }

  inline void SetPos(int pos) { _pos = pos; }
};