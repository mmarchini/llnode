#ifndef SRC_LLNODE_H_
#define SRC_LLNODE_H_

#include <string>

#include <lldb/API/LLDB.h>

#include "src/llv8.h"
#include "src/node.h"

namespace llnode {

class CommandBase : public lldb::SBCommandPluginInterface {
 public:
  char** ParseInspectOptions(char** cmd, v8::Value::InspectOptions* options);
};

class BacktraceCmd : public CommandBase {
 public:
  BacktraceCmd(v8::LLV8* llv8) : llv8_(llv8) {}
  ~BacktraceCmd() override {}

  bool DoExecute(lldb::SBDebugger d, char** cmd,
                 lldb::SBCommandReturnObject& result) override;

 private:
  v8::LLV8* llv8_;
};

class PrintCmd : public CommandBase {
 public:
  PrintCmd(v8::LLV8* llv8, bool detailed) : llv8_(llv8), detailed_(detailed) {}

  ~PrintCmd() override {}

  bool DoExecute(lldb::SBDebugger d, char** cmd,
                 lldb::SBCommandReturnObject& result) override;

 private:
  v8::LLV8* llv8_;
  bool detailed_;
};

class ListCmd : public CommandBase {
 public:
  ListCmd(v8::LLV8* llv8) : llv8_(llv8) {}
  ~ListCmd() override {}

  bool DoExecute(lldb::SBDebugger d, char** cmd,
                 lldb::SBCommandReturnObject& result) override;

 private:
  v8::LLV8* llv8_;
};

class GetActiveHandlesCmd : public CommandBase {
 public:
  GetActiveHandlesCmd(v8::LLV8* llv8, node::Node* node) : llv8_(llv8),
      node_(node) {}
  ~GetActiveHandlesCmd() override {}

  bool DoExecute(lldb::SBDebugger d, char** cmd,
                 lldb::SBCommandReturnObject& result) override;
 private:
  v8::LLV8* llv8_;
  node::Node* node_;
};

class GetActiveRequestsCmd : public CommandBase {
 public:
  GetActiveRequestsCmd(v8::LLV8* llv8, node::Node* node) : llv8_(llv8),
      node_(node) {}
  ~GetActiveRequestsCmd() override {}

  bool DoExecute(lldb::SBDebugger d, char** cmd,
                 lldb::SBCommandReturnObject& result) override;
 private:
  v8::LLV8* llv8_;
  node::Node* node_;
};


}  // namespace llnode

#endif  // SRC_LLNODE_H_
