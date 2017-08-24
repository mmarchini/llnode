#include "llnode-module.h"
#include "llnode-constants.h"

namespace llnode {
namespace node {

Environment *Environment::GetCurrent(LLNode *node) {
  addr_t envAddr = node->env()->kCurrentEnvironment;
  // TODO (mmarchini): maybe throw some warning here
  if (envAddr == -1) {
    return nullptr;
  }

  Environment *env = new Environment(envAddr);
  return env;
}

}
}
