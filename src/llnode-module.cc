#include "llnode-module.h"

namespace llnode {
namespace node {

Environment::Environment(addr_t raw) : raw_(raw) {

}

std::list<HandleWrap> *HandleWrap::GetQueue(Environment *env) {
  std::list<HandleWrap> *queue = new std::list<HandleWrap>();
  uint64_t currentNode = env->raw();
  SBError sberr;

  currentNode += node_->env()->kHandleWrapQueueOffset;  // XXX env.handle_wrap_queue_
  currentNode += node_->handle_wrap_queue()->kHeadOffset;   // XXX env.handle_wrap_queue_.head_
  currentNode += node_->handle_wrap_queue()->kNextOffset;   // XXX env.handle_wrap_queue_.head_.next_
  currentNode = node_->process().ReadPointerFromMemory(currentNode, sberr);

  // TODO (mmarchini): add better stop condition
  while (go) {
    addr_t wrap = currentNode - node_->handle_wrap()->kListNodeOffset;

    addr_t persistentHandlePtr = wrap + node_->base_object()->kPersistentHandleOffset;

    // XXX w->persistent().IsEmpty()
    if (persistentHandlePtr == 0) {
      continue;
    }

    addr_t persistentHandle = node_->process().ReadPointerFromMemory(persistentHandlePtr, sberr);
    addr_t obj = node_->process().ReadPointerFromMemory(persistentHandle, sberr);
    // TODO needs a better check
    if (sberr.Fail()) {
      break;
    }

    v8::JSObject v8_object(&llv8, obj);
    v8::Error err;
    std::string res = v8_object.Inspect(&inspect_options, err);
    if (err.Fail()) {
      // result.SetError("Failed to evaluate expression");
      break;
    }

    // XXX env.handle_wrap_queue_.head_.next_->next_->(...)->next_
    currentNode += node_.handle_wrap_queue()->kNextOffset;
    currentNode = process.ReadPointerFromMemory(currentNode, sberr);
  }

  return ;
}

void LLNode::Load(SBTarget target) {
  // Reload process anyway
  process_ = target.GetProcess();

  // No need to reload
  if (target_ == target) return;

  target_ = target;

  env.Assign(target);
  req_wrap_queue.Assign(target);
  req_wrap.Assign(target);
  handle_wrap_queue.Assign(target);
  handle_wrap.Assign(target);
  base_object.Assign(target);
}

}
}
