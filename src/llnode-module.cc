#include "llnode-module.h"

namespace llnode {
namespace node {

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
