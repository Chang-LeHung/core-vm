

#pragma once
#include "assembler.h"
#include "lexer.h"
#include "types.h"
#include <fcntl.h>
#include <memory>
#include <string>
#include <vector>

enum class OpCode
{
  kIAdd,
  kISub,
  kIMul,
  kIDiv,
  kIMod,
  kDAdd,
  kDSub,
  kDMul,
  kDDiv,
  kDMod,
  kFAdd,
  kFSub,
  kFMul,
  kFDiv,
  kIConst,
  kLConst,
  kFConst,
  kDconst,
  kLoadI,
  kLoadD,
  kLoadF,
  kLoadL,
  kStoreI,
  kStoreD,
  kStoreF,
  kStoreL,
  kNop,
  kProgram, // not bytecode
};

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

  inline bool IsIntRes() const { return CVMType::GetOrCreateType("int") == _res_type; }

  inline bool IsLongRes() const { return CVMType::GetOrCreateType("long") == _res_type; }

  inline bool IsFloatRes() const { return CVMType::GetOrCreateType("float") == _res_type; }

  inline bool IsDoubleRes() const { return CVMType::GetOrCreateType("double") == _res_type; }

  virtual ~IR() = default;
};

class BinaryIR : public IR
{

private:
  IR _lhs;
  IR _rhs;
  OpCode _op;

public:
  BinaryIR(OpCode opcode, IR lhs, IR rhs) : IR(opcode, lhs.GetResType()), _lhs(lhs), _rhs(rhs) {}

  virtual void emit(CVMAssembler &assembler) const override;
};

class Variable : public IR
{
private:
  std::string _name;
  int _ix; // index in constant pool

public:
  Variable(std::string name, int ix, OpCode code, CVMType type) : IR(code, type), _name(name), _ix(ix) {}

  inline const std::string &GetName() const { return _name; }

  virtual void emit(CVMAssembler &assembler) const override;

  inline int GetIx() const { return _ix; }
};

class Constant : public IR
{
  CoreVMADT _val;
  OpCode _op;

public:
  Constant(CoreVMADT val, OpCode op, CVMType type) : IR(op, type), _val(val), _op(op) {}

  inline CoreVMADT GetVal() const { return _val; }

  virtual void emit(CVMAssembler &assembler) const override;
};

class Store : public Variable
{
public:
  Store(std::string name, int ix, CVMType type, OpCode code) : Variable(name, ix, code, type) {}

  virtual void emit(CVMAssembler &assembler) const override;
};

class ProgramIR : public IR
{
private:
  std::vector<std::shared_ptr<IR>> _irs;

public:
  ProgramIR() : IR(OpCode::kProgram, void_op) {} // a special case

  ProgramIR(const ProgramIR &program) : IR(OpCode::kProgram, void_op) { _irs = program._irs; }

  void AddIR(IR &&ir) { _irs.emplace_back(ir); }

  void AddIR(const IR &ir) { _irs.emplace_back(ir); }

  std::shared_ptr<IR> &GetByIndex(int index) { return _irs[index]; }

  void emit(CVMAssembler &assembler) const override;
  virtual ~ProgramIR() = default;
};
