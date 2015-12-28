#pragma once

#include <string>

#include <papyrus/PapyrusType.h>
#include <papyrus/expressions/PapyrusExpression.h>
#include <papyrus/statements/PapyrusStatement.h>

#include <pex/PexFile.h>
#include <pex/PexFunctionBuilder.h>

namespace caprica { namespace papyrus { namespace statements {

struct PapyrusDeclareStatement final : public PapyrusStatement
{
  PapyrusType type{ };
  std::string name{ "" };
  expressions::PapyrusExpression* initialValue{ nullptr };

  PapyrusDeclareStatement(parser::PapyrusFileLocation loc) : PapyrusStatement(loc) { }
  ~PapyrusDeclareStatement() {
    if (initialValue)
      delete initialValue;
  }

  virtual void buildPex(pex::PexFile* file, pex::PexFunctionBuilder& bldr) const override {
    namespace op = caprica::pex::op;
    auto loc = bldr.allocateLocal(file, name, type);
    if (initialValue) {
      auto val = initialValue->generateLoad(file, bldr);
      bldr << location;
      bldr << op::assign{ loc, val };
      bldr.freeIfTemp(val);
    }
  }

  virtual void semantic(PapyrusResolutionContext* ctx) override {
    type = ctx->resolveType(type);
    if (initialValue) {
      initialValue->semantic(ctx);
      initialValue = expressions::PapyrusExpression::coerceExpression(initialValue, type);
    }
  }
};

}}}
