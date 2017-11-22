#include "src/constants.h"
#include "src/llv8.h"

namespace llnode {
using v8::Error;
using v8::constants::IsDebugMode;
using v8::constants::LookupConstant;
namespace constants {

void ConstantsWrapper::Assign(SBTarget target) {
  loaded_ = false;
  target_ = target;
}

int64_t ConstantsWrapper::LoadRawConstant(const char* name, int64_t def) {
  Error err;
  int64_t v = LookupConstant(target_, name, def, err);
  if (err.Fail() && IsDebugMode()) fprintf(stderr, "Failed to load %s\n", name);

  return v;
}


int64_t ConstantsWrapper::LoadConstant(const char* name, int64_t def) {
  Error err;
  int64_t v =
      LookupConstant(target_, (kConstantPrefix() + name).c_str(), def, err);
  if (err.Fail() && IsDebugMode()) fprintf(stderr, "Failed to load %s\n", name);

  return v;
}


int64_t ConstantsWrapper::LoadConstant(const char* name, const char* fallback,
                                       int64_t def) {
  Error err;
  int64_t v =
      LookupConstant(target_, (kConstantPrefix() + name).c_str(), def, err);
  if (err.Fail())
    v = LookupConstant(target_, (kConstantPrefix() + fallback).c_str(), def,
                       err);
  if (err.Fail() && IsDebugMode()) fprintf(stderr, "Failed to load %s\n", name);

  return v;
}
}
}
