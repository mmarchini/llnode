#ifndef SRC_LLNODE_MODULE_H_
#define SRC_LLNODE_MODULE_H_

#include <list>
#include <lldb/API/SBExpressionOptions.h>

#include "llnode-constants.h"

namespace llnode {
namespace node {

class LLNode;
class HandleWrap;
class ReqWrap;
template<typename T, typename C>
class Queue;

// TODO (mmarchini): give a better name
class BaseNode {
  public:
    BaseNode(LLNode *node) : node_(node) {};
  protected:
    LLNode *node_;
};

class Environment : public BaseNode {
  public:
    Environment (LLNode *node, addr_t raw) : BaseNode(node), raw_(raw) {};
    inline addr_t raw() { return raw_; };

    static Environment GetCurrent(LLNode *node);

    Queue<HandleWrap, constants::HandleWrapQueue> handle_wrap_queue() const;
    Queue<ReqWrap, constants::ReqWrapQueue> req_wrap_queue() const;

  private:
    addr_t raw_;
};

class BaseObject : public BaseNode {
  public:
    BaseObject(LLNode *node, addr_t raw) : BaseNode(node), raw_(raw) {
    };

    addr_t persistent_addr();

    addr_t v8_object_addr();


  private:
    addr_t raw_;
};

class AsyncWrap : public BaseObject {
  public:
    // static std::list<HandleWrap> *GetEnvQueue(Environment *env);
    AsyncWrap(LLNode *node, addr_t raw) : BaseObject(node, raw) {};
};

class HandleWrap : public AsyncWrap {
  public:
    HandleWrap(LLNode *node, addr_t raw) : AsyncWrap(node, raw) {};

    static HandleWrap FromListNode(LLNode *node, addr_t list_node_addr);
};

class ReqWrap : public AsyncWrap {
  public:
    ReqWrap(LLNode *node, addr_t raw) : AsyncWrap(node, raw) {};

    static ReqWrap FromListNode(LLNode *node, addr_t list_node_addr);
};

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

template<typename T, typename C>
class Queue : public BaseNode {
  class Iterator : public BaseNode{
    public:
      inline T operator*() const;
      inline const Iterator operator++();
      inline bool operator!=(const Iterator& that) const;


      inline Iterator(LLNode *node, addr_t current, C *constants) : BaseNode(node), current_(current), constants_(constants) {};

    public:
      addr_t current_;
      C *constants_;
  };

  public:

    inline Queue(LLNode *node, addr_t raw, C *constants) : BaseNode(node), raw_(raw), constants_(constants) {};

    inline Iterator begin() const;
    inline Iterator end() const;

  private:
    addr_t raw_;
    C *constants_;
};

}
}

#endif
