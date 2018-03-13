#ifndef SRC_NODE_CONSTANTS_H_
#define SRC_NODE_CONSTANTS_H_

#include <lldb/API/LLDB.h>
#include "src/constants.h"
#include "src/llv8.h"

using lldb::addr_t;

namespace llnode {
namespace node {
namespace constants {

class Module : public Constants {
 public:
  Module(v8::LLV8* llv8) : llv8_(llv8) {}
  inline std::string constant_prefix() override { return "nodedbg_"; };

  inline v8::LLV8* llv8() { return llv8_; }

 private:
  v8::LLV8* llv8_;
};

class Environment : public Module {
 public:
  CONSTANTS_DEFAULT_METHODS(Environment);

  int64_t kReqWrapQueueOffset;
  int64_t kHandleWrapQueueOffset;
  int64_t kEnvContextEmbedderDataIndex;
  addr_t kCurrentEnvironment;

 protected:
  void Load();

 private:
  addr_t LoadCurrentEnvironment();
  addr_t CurrentEnvironmentFromContext(v8::Value context);
};

class ReqWrapQueue : public Module {
 public:
  CONSTANTS_DEFAULT_METHODS(ReqWrapQueue);

  int64_t kHeadOffset;
  int64_t kNextOffset;

 protected:
  void Load();
};

class ReqWrap : public Module {
 public:
  CONSTANTS_DEFAULT_METHODS(ReqWrap);

  int64_t kListNodeOffset;

 protected:
  void Load();
};

class HandleWrapQueue : public Module {
 public:
  CONSTANTS_DEFAULT_METHODS(HandleWrapQueue);

  int64_t kHeadOffset;
  int64_t kNextOffset;

 protected:
  void Load();
};

class HandleWrap : public Module {
 public:
  CONSTANTS_DEFAULT_METHODS(HandleWrap);

  int64_t kListNodeOffset;

 protected:
  void Load();
};

class BaseObject : public Module {
 public:
  CONSTANTS_DEFAULT_METHODS(BaseObject);

  int64_t kPersistentHandleOffset;

 protected:
  void Load();
};
}
}
}

#endif
