#ifndef SRC_LLNODE_MODULE_INL_H_
#define SRC_LLNODE_MODULE_INL_H_

#include "llnode-module.h"
#include "llnode-constants.h"

namespace llnode {
namespace node {

Environment Environment::GetCurrent(LLNode *node) {
  // TODO (mmarchini): maybe throw some warning here if env is not valid
  addr_t envAddr = node->env()->kCurrentEnvironment;

  return Environment(node, envAddr);
}

}
}

#endif
