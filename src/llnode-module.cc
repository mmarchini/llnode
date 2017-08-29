#include "llnode-module.h"

namespace llnode {
namespace node {

addr_t BaseObject::persistent_addr() {
  lldb::SBError sberr;

  addr_t persistentHandlePtr =
      raw_ + node_->base_object()->kPersistentHandleOffset;
  addr_t persistentHandle =
      node_->process().ReadPointerFromMemory(persistentHandlePtr, sberr);
  return persistentHandle;
}

addr_t BaseObject::v8_object_addr() {
  lldb::SBError sberr;

  addr_t persistentHandle = persistent_addr();
  addr_t obj = node_->process().ReadPointerFromMemory(persistentHandle, sberr);
  return obj;
}

HandleWrap HandleWrap::FromListNode(LLNode *node, addr_t list_node_addr) {
  return HandleWrap(node,
                    list_node_addr - node->handle_wrap()->kListNodeOffset);
}

ReqWrap ReqWrap::FromListNode(LLNode *node, addr_t list_node_addr) {
  return ReqWrap(node, list_node_addr - node->req_wrap()->kListNodeOffset);
}

Environment Environment::GetCurrent(LLNode *node) {
  // TODO (mmarchini): maybe throw some warning here when env is not valid
  addr_t envAddr = node->env()->kCurrentEnvironment;

  return Environment(node, envAddr);
}

Queue<HandleWrap, constants::HandleWrapQueue> Environment::handle_wrap_queue()
    const {
  return Queue<HandleWrap, constants::HandleWrapQueue>(
      node_, raw_ + node_->env()->kHandleWrapQueueOffset,
      node_->handle_wrap_queue());
}

Queue<ReqWrap, constants::ReqWrapQueue> Environment::req_wrap_queue() const {
  return Queue<ReqWrap, constants::ReqWrapQueue>(
      node_, raw_ + node_->env()->kReqWrapQueueOffset, node_->req_wrap_queue());
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
