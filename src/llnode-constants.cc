#include <lldb/API/LLDB.h>

#include "llnode-constants.h"
#include "llv8-constants.h"
#include "llv8-inl.h"
#include "llv8.h"

using lldb::SBProcess;
using lldb::SBThread;
using lldb::SBError;
using lldb::SBFrame;
using lldb::SBStream;

namespace llnode {
using v8::Error;
using v8::constants::LookupConstant;
namespace node {
namespace constants {
v8::LLV8 llv8;

void Environment::Load() {
  // FIXME (this symbols is missing apparently)
  kIsolate = LoadRawConstant("node::node_isolate");
  kReqWrapQueueOffset = LoadConstant(
      "offset_Environment__req_wrap_queue___Environment_ReqWrapQueue");
  kHandleWrapQueueOffset = LoadConstant(
      "offset_Environment__handle_wrap_queue___Environment_HandleWrapQueue");
  kEnvContextEmbedderDataIndex =
      LoadConstant("const_Environment__kContextEmbedderDataIndex__int");
  kCurrentEnvironment = LoadCurrentEnvironment();
}

addr_t Environment::LoadCurrentEnvironment() {
  addr_t currentEnvironment = DefaultLoadCurrentEnvironment();
  if (currentEnvironment == -1) {
    currentEnvironment = FallbackLoadCurrentEnvironment();
  }

  return currentEnvironment;
}

addr_t Environment::DefaultLoadCurrentEnvironment() {
  llv8.Load(target_);

  SBProcess process = target_.GetProcess();
  SBError sberr;
  uint64_t env = -1;
  uint64_t isolate_thread = 0;
  uint64_t thread_context_ptr = 0;
  uint64_t thread_context = 0;
  v8::Error err;

  if (!(llv8.isolate()->kThreadLocalTopOffset != -1 &&
        llv8.thread_local_top()->kContextOffset != -1)) {
    return env;
  }

  isolate_thread = kIsolate + llv8.isolate()->kThreadLocalTopOffset;

  thread_context_ptr = isolate_thread + llv8.thread_local_top()->kContextOffset;
  thread_context = process.ReadPointerFromMemory(thread_context_ptr, sberr);
  if (sberr.Fail()) {
    return -1;
  }
  v8::Context ctx(&llv8, thread_context);
  v8::Value native = ctx.Native(err);
  if (err.Fail()) {
    return -1;
  }
  env = CurrentEnvironmentFromContext(native);
  return env;
}

addr_t Environment::CurrentEnvironmentFromContext(v8::Value context) {
  llv8.Load(target_);
  v8::Error err;

  v8::FixedArray contextArray = v8::FixedArray(context);
  v8::FixedArray embed =
      contextArray.Get<v8::FixedArray>(llv8.context()->kEmbedderDataIndex, err);
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

addr_t Environment::FallbackLoadCurrentEnvironment() {
  addr_t env = -1;
  SBProcess process = target_.GetProcess();
  SBThread thread = process.GetSelectedThread();
  if (!thread.IsValid()) {
    return -1;
  }

  llv8.Load(target_);

  SBStream desc;
  if (!thread.GetDescription(desc)) {
    return -1;
  }
  SBFrame selected_frame = thread.GetSelectedFrame();

  uint32_t num_frames = thread.GetNumFrames();
  for (uint32_t i = 0; i < num_frames; i++) {
    SBFrame frame = thread.GetFrameAtIndex(i);

    if (!frame.GetSymbol().IsValid()) {
      v8::Error err;
      v8::JSFrame v8_frame(&llv8, static_cast<int64_t>(frame.GetFP()));
      v8::JSFunction v8_function = v8_frame.GetFunction(err);
      if (err.Fail()) {
        continue;
      }
      v8::Value val;
      val = v8_function.GetContext(err);
      if (err.Fail()) {
        continue;
      }
      bool found = false;
      while (!found) {
        v8::Context context(val);
        v8::Value native = context.Native(err);
        if (err.Success()) {
          if (native.raw() == context.raw()) {
            found = true;
            env = CurrentEnvironmentFromContext(native);
            break;
          }
        }

        val = context.Previous(err);
        if (err.Fail()) {
          break;
        }
      }
      if (found) {
        break;
      }
    }
  }

  return env;
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
      "offset_Environment__handle_wrap_queue___Environment_HandleWrapQueue");
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
