#pragma once

#include <cstdint>
#include <ctime>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <common/CapricaAllocator.h>

#include <pex/PexAsmWriter.h>
#include <pex/PexDebugInfo.h>
#include <pex/PexObject.h>
#include <pex/PexReader.h>
#include <pex/PexString.h>
#include <pex/PexWriter.h>

namespace caprica { namespace pex {

struct PexFile final
{
  uint8_t majorVersion{ 3 };
  uint8_t minorVersion{ 9 };
  uint16_t gameID{ 2 }; // Default to Fallout 4
  time_t compilationTime{ };
  std::string sourceFileName{ "" };
  std::string userName{ "" };
  std::string computerName{ "" };
  PexDebugInfo* debugInfo{ nullptr };
  std::vector<PexObject*> objects{ };

  explicit PexFile() = default;
  PexFile(const PexFile&) = delete;
  ~PexFile() {
    if (debugInfo)
      delete debugInfo;
    for (auto o : objects)
      delete o;
  }

  void ensureDebugInfo() {
    if (!debugInfo)
      debugInfo = new PexDebugInfo();
  }

  PexDebugFunctionInfo* tryFindFunctionDebugInfo(const PexObject* object,
                                                 const PexState* state,
                                                 const PexFunction* function,
                                                 const std::string& propertyName,
                                                 PexDebugFunctionType functionType);
  const PexDebugFunctionInfo* tryFindFunctionDebugInfo(const PexObject* object,
                                                       const PexState* state,
                                                       const PexFunction* function,
                                                       const std::string& propertyName,
                                                       PexDebugFunctionType functionType) const;
  PexString getString(const std::string& str);
  std::string getStringValue(const PexString& str) const;
  PexUserFlags getUserFlag(PexString name, uint8_t bitNum);
  size_t getUserFlagCount() const noexcept;

  static PexFile* read(PexReader& rdr);
  void write(PexWriter& wtr) const;
  void writeAsm(PexAsmWriter& wtr) const;

private:
  ReffyStringPool stringTable;

  std::vector<std::pair<PexString, uint8_t>> userFlagTable;
  std::unordered_map<size_t, size_t> userFlagTableLookup;
};

}}
