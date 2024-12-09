

#pragma once
#include "assembler.h"
#include "lexer.h"
#include "opcode.h"
#include "types.h"
#include <fcntl.h>
#include <memory>
#include <string>
#include <vector>

class IR
{

private:
  OpCode _opcode;
  CVMType _res_type;

public:
  IR(OpCode opcode, CVMType type) : _opcode(opcode), _res_type(type) {}

  IR(const IR &ir) : _res_type(ir._res_type)
  {
    _opcode = ir._opcode;
    _res_type = ir._res_type;
  }

  inline OpCode GetType() const { return _opcode; }

  inline OpCode GetOpCode() const { return _opcode; }

  inline const char *GetName() { return _res_type.GetName().c_str(); }

  virtual void emit(CVMAssembler &assembler) const {};

  virtual CVMType GetResType() const { return _res_type; }

  inline bool IsIntRes() const
  {
    return CVMType::GetOrCreateType("int") == _res_type;
  }

  inline bool IsLongRes() const
  {
    return CVMType::GetOrCreateType("long") == _res_type;
  }

  inline bool IsFloatRes() const
  {
    return CVMType::GetOrCreateType("float") == _res_type;
  }

  inline bool IsDoubleRes() const
  {
    return CVMType::GetOrCreateType("double") == _res_type;
  }

  virtual ~IR() = default;
};

class BinaryIR : public IR
{

private:
  std::shared_ptr<IR> _lhs;
  std::shared_ptr<IR> _rhs;
  OpCode _op;

public:
  BinaryIR(OpCode opcode, std::shared_ptr<IR> lhs, std::shared_ptr<IR> rhs)
      : IR(opcode, lhs->GetResType()), _lhs(lhs), _rhs(rhs)
  {
  }

  virtual void emit(CVMAssembler &assembler) const override;
};

class VariableIR : public IR
{
private:
  std::string _name;
  int _ix; // index in constant pool

public:
  VariableIR(std::string name, int ix, OpCode code, CVMType type)
      : IR(code, type), _name(name), _ix(ix)
  {
  }

  inline const std::string &GetName() const { return _name; }

  virtual void emit(CVMAssembler &assembler) const override;

  inline int GetIx() const { return _ix; }
};

class Constant : public IR
{
  CoreVMADT _val;
  OpCode _op;

public:
  Constant(CoreVMADT val, OpCode op, CVMType type)
      : IR(op, type), _val(val), _op(op)
  {
  }

  inline CoreVMADT GetVal() const { return _val; }

  virtual void emit(CVMAssembler &assembler) const override;
};

class StoreIR : public VariableIR
{
public:
  StoreIR(std::string name, int ix, CVMType type, OpCode code)
      : VariableIR(name, ix, code, type)
  {
  }

  virtual void emit(CVMAssembler &assembler) const override;
};

class ProgramIR : public IR
{
private:
  std::vector<std::shared_ptr<IR>> _irs;

public:
  ProgramIR() : IR(OpCode::kProgram, void_op) {} // a special case

  ProgramIR(const ProgramIR &program) : IR(OpCode::kProgram, void_op)
  {
    _irs = program._irs;
  }

  void AddIR(const IR &ir) { _irs.emplace_back(std::make_shared<IR>(ir)); }

  std::shared_ptr<IR> &GetByIndex(int index) { return _irs[index]; }

  void emit(CVMAssembler &assembler) const override;
  virtual ~ProgramIR() = default;
};

class AssignStmtIR : public IR
{
private:
  std::shared_ptr<StoreIR> _lhs;
  std::shared_ptr<IR> _rhs;

public:
  AssignStmtIR(std::shared_ptr<StoreIR> lhs, std::shared_ptr<IR> rhs,
               CVMType type)
      : IR(OpCode::kStmt, type), _lhs(lhs), _rhs(rhs)
  {
  }

  virtual void emit(CVMAssembler &assembler) const override;

  virtual ~AssignStmtIR() = default;
};

class DefinitionIR : public IR
{
private:
  std::string _var_name;
  int _idx; // index of the variable in the symbol table

public:
  DefinitionIR(std::string name, int idx, CVMType type)
      : IR(OpCode::kDef, type), _var_name(name), _idx(idx)
  {
  }

  virtual void emit(CVMAssembler &assembler) const override;

  std::string GetVarName() const { return _var_name; }

  virtual ~DefinitionIR() = default;
};

class NopIR : public IR
{
public:
  NopIR() : IR(OpCode::kNop, void_op) {}

  virtual void emit(CVMAssembler &assembler) const override;

  virtual ~NopIR() = default;
};