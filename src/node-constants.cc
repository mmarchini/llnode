#include <lldb/API/LLDB.h>

#include "src/node-constants.h"
#include "src/llv8-inl.h"

using lldb::SBProcess;
using lldb::SBThread;
using lldb::SBFrame;
using lldb::SBStream;

namespace llnode {
namespace node {
namespace constants {

void Environment::Load() {
  kReqWrapQueueOffset = LoadConstant(
      "offset_Environment__req_wrap_queue___Environment_ReqWrapQueue");
  kHandleWrapQueueOffset = LoadConstant(
      "offset_Environment__handle_wrap_queue___Environment_HandleWrapQueue");
  kEnvContextEmbedderDataIndex =
      LoadConstant("const_Environment__kContextEmbedderDataIndex__int");
  kCurrentEnvironment = LoadCurrentEnvironment();
}

addr_t Environment::LoadCurrentEnvironment() {
  addr_t currentEnvironment = -1;
  SBProcess process = target_.GetProcess();
  SBThread thread = process.GetSelectedThread();
  if (!thread.IsValid()) {
    // TODO Error("Invalid thread");
    return -1;
  }

  llv8()->Load(target_);

  SBStream desc;
  if (!thread.GetDescription(desc)) {
    // TODO Error("Couldn't get thread description");
    return -1;
  }
  SBFrame selected_frame = thread.GetSelectedFrame();

  uint32_t num_frames = thread.GetNumFrames();
  for (uint32_t i = 0; i < num_frames; i++) {
    SBFrame frame = thread.GetFrameAtIndex(i);

    if (!frame.GetSymbol().IsValid()) {
      Error err;
      v8::JSFrame v8_frame(llv8(), static_cast<int64_t>(frame.GetFP()));
      v8::JSFunction v8_function = v8_frame.GetFunction(err);
      if (err.Fail()) {
        // std::cout << "Failed to get V8 Frame" << std::endl;
        continue;
      }
      v8::Value val;
      val = v8_function.GetContext(err);
      if (err.Fail()) {
        // std::cout << "Failed to get V8 Context" << std::endl;
        continue;
      }
      bool found = false;
      while (!found) {
        v8::Context context(val);
        v8::Value native = context.Native(err);
        if (err.Success()) {
          if (native.raw() == context.raw()) {
            found = true;
            currentEnvironment = CurrentEnvironmentFromContext(native);
            break;
          }
        }

        val = context.Previous(err);
        if (err.Fail()) {
          // std::cout << "Failed to get previous V8 Context" << std::endl;
          break;
        }
      }
      if (found) {
        break;
      }
    }
  }

  return currentEnvironment;
}

addr_t Environment::CurrentEnvironmentFromContext(v8::Value context) {
  llv8()->Load(target_);
  Error err;

  v8::FixedArray contextArray = v8::FixedArray(context);
  v8::FixedArray embed = contextArray.Get<v8::FixedArray>(
      llv8()->context()->kEmbedderDataIndex, err);
  if (err.Fail()) {
    return -1;
  }
  v8::Smi encodedEnv = embed.Get<v8::Smi>(kEnvContextEmbedderDataIndex, err);
  if (err.Fail()) {
    return -1;
  } else {
    return encodedEnv.raw();
  }
}


void ReqWrapQueue::Load() {
  kHeadOffset = LoadConstant(
      "offset_Environment_ReqWrapQueue__head___ListNode_ReqWrapQueue");
  kNextOffset = LoadConstant("offset_ListNode_ReqWrap__next___uintptr_t");
}

void ReqWrap::Load() {
  kListNodeOffset = LoadConstant(
      "offset_ReqWrap__req_wrap_queue___ListNode_ReqWrapQueue");
}

void HandleWrapQueue::Load() {
  kHeadOffset = LoadConstant(
      "offset_Environment_HandleWrapQueue__head___ListNode_HandleWrap");
  kNextOffset = LoadConstant("offset_ListNode_HandleWrap__next___uintptr_t");
}

void HandleWrap::Load() {
  kListNodeOffset = LoadConstant(
      "offset_HandleWrap__handle_wrap_queue___ListNode_HandleWrap");
}

void BaseObject::Load() {
  kPersistentHandleOffset = LoadConstant(
      "offset_BaseObject__persistent_handle___v8_Persistent_v8_Object");
}
}
}
}
