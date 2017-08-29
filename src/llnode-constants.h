#ifndef SRC_LLNODE_CONSTANTS_H_
#define SRC_LLNODE_CONSTANTS_H_

#include <lldb/API/LLDB.h>
#include "src/constants.h"
#include "src/llv8.h"

using lldb::addr_t;

namespace llnode {
using constants::ConstantsWrapper;
namespace node {
namespace constants {
#define MODULE_DEFAULT_METHODS(NAME) \
  NAME() {}                          \
  inline NAME* operator()() {        \
    if (loaded_) return this;        \
    loaded_ = true;                  \
    Load();                          \
    return this;                     \
  }


class Module : public ConstantsWrapper {
 public:
  inline std::string kConstantPrefix() override { return "nodedbg_"; };
};

class Environment : public Module {
 public:
  MODULE_DEFAULT_METHODS(Environment);

  int64_t kIsolate;
  int64_t kReqWrapQueueOffset;
  int64_t kHandleWrapQueueOffset;
  int64_t kEnvContextEmbedderDataIndex;
  addr_t kCurrentEnvironment;

 protected:
  void Load();

 private:
  addr_t LoadCurrentEnvironment();
  addr_t DefaultLoadCurrentEnvironment();
  addr_t FallbackLoadCurrentEnvironment();
  addr_t CurrentEnvironmentFromContext(v8::Value context);
};

class ReqWrapQueue : public Module {
 public:
  MODULE_DEFAULT_METHODS(ReqWrapQueue);

  int64_t kHeadOffset;
  int64_t kNextOffset;

 protected:
  void Load();
};

class ReqWrap : public Module {
 public:
  MODULE_DEFAULT_METHODS(ReqWrap);

  int64_t kListNodeOffset;

 protected:
  void Load();
};

class HandleWrapQueue : public Module {
 public:
  MODULE_DEFAULT_METHODS(HandleWrapQueue);

  int64_t kHeadOffset;
  int64_t kNextOffset;

 protected:
  void Load();
};

class HandleWrap : public Module {
 public:
  MODULE_DEFAULT_METHODS(HandleWrap);

  int64_t kListNodeOffset;

 protected:
  void Load();
};

class BaseObject : public Module {
 public:
  MODULE_DEFAULT_METHODS(BaseObject);

  int64_t kPersistentHandleOffset;

 protected:
  void Load();
};
}
}
}

#endif
