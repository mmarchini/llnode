#include "llnode-constants.h"

namespace llnode {
namespace node {

class LLNode {
  public:
    LLNode() : target_(lldb::SBTarget()) {}

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
