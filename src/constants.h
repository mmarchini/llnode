#ifndef SRC_CONSTANTS_H_
#define SRC_CONSTANTS_H_

#include <lldb/API/LLDB.h>
using lldb::SBTarget;

namespace llnode {
namespace constants {

class ConstantsWrapper {
 public:
  ConstantsWrapper() : loaded_(false) {}

  inline bool is_loaded() const { return loaded_; }

  void Assign(lldb::SBTarget target);

  inline virtual std::string kConstantPrefix() { return ""; };

 protected:
  int64_t LoadRawConstant(const char* name, int64_t def = -1);
  int64_t LoadConstant(const char* name, int64_t def = -1);
  int64_t LoadConstant(const char* name, const char* fallback,
                       int64_t def = -1);

  lldb::SBTarget target_;
  bool loaded_;
};
}
}

#endif
