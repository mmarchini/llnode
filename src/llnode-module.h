#include "llnode-constants.h"
#include <list>

namespace llnode {
namespace node {

class LLNode;
class HandleWrap;
class ReqWrap;

// TODO (mmarchini): give a better name
class BaseNode {
  protected:
    LLNode *node_;
};

class Environment : public BaseNode {
  public:
    Environment (addr_t raw);
    inline addr_t raw() { return raw_; };

    inline static Environment *GetCurrent(LLNode *node);

  private:
    addr_t raw_;
    std::list<HandleWrap> *handle_wrap_queue_;
    std::list<ReqWrap>    *request_wrap_queue_;
};

class BaseObject : public BaseNode {

};

class AsyncWrap : public BaseObject {
  static std::list<HandleWrap> *GetEnvQueue(Environment *env);
}

class HandleWrap : public AsyncWrap {
  static std::list<HandleWrap> *GetEnvQueue(Environment *env);
}

class RequestWrap : public AsyncWrap {
  static std::list<HandleWrap> *GetEnvQueue(Environment *env);
}

class LLNode {
  public:
    LLNode() : target_(lldb::SBTarget()) {}

    inline lldb::SBProcess process() { return process_; };

    void Load(lldb::SBTarget target);

    constants::Environment env;
    constants::ReqWrapQueue req_wrap_queue;
    constants::ReqWrap req_wrap;
    constants::HandleWrapQueue handle_wrap_queue;
    constants::HandleWrap handle_wrap;
    constants::BaseObject base_object;

  private:
    lldb::SBTarget target_;
    lldb::SBProcess process_;
};

}
}
