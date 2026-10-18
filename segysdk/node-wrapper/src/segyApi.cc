#include <napi.h>
#include "segySessionNode.h>

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return SegySessionNode::Init(env, exports);
}

NODE_API_MODULE(addon, InitAll)