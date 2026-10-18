#include "segySessionNode.h"

Napi::Object SegySessionNode::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func =
      DefineClass(env,
                  "SegySessionNode",
                  {InstanceMethod("getBinaryHeaderAsJson", &SegySessionNode::GetBinaryHeaderAsJson),
                   InstanceMethod("getAsciiHeaders", &SegySessionNode::GetAsciiHeaders),
                   InstanceMethod("getAsciiHeadersAsJson", &SegySessionNode::GetAsciiHeadersAsJson)});

  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);

  exports.Set("SegySessionNode", func);
  return exports;
}

SegySessionNode::SegySessionNode(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<SegySessionNode>(info) {
  Napi::Object config = info[0].As<Napi::Object>();
  Napi::Env env = info.Env();

  Napi::Value sdms_value = config["sdmsUrl"];
  Napi::Value app_key_value = config["appKey"];
  Napi::Value token_value = config["token"];
  if(sdms_value.IsString() && app_key_value.IsString() && token_value.IsString()) {
    std::string sdms_url = sdms_value.As<Napi::String>().Utf8Value();
    std::string app_key = app_key_value.As<Napi::String>().Utf8Value();
    std::string token = token_value.As<Napi::String>().Utf8Value();

    segy_configure_remote_access(&sdms_url[0], &app_key[0], &token[0]);
  }

  Napi::Value path_value = config["sdPath"];
  if(!path_value.IsString() || path_value.IsEmpty()) {
    Napi::TypeError::New(env, "sdPath expected").ThrowAsJavaScriptException();
    return;
  }

  std::string path = path_value.As<Napi::String>().Utf8Value();

  segy_api_error error;
  this->session_ = segy_create_segy_session(&path[0], "{}", &error);

  ThrowJavaScriptExceptionIfIsError(&error, env);
}

Napi::Value SegySessionNode::GetBinaryHeaderAsJson(const Napi::CallbackInfo& info) {
  segy_api_error error;
  const char* result = segy_get_binary_header_as_json(this->session_, &error);

  return GetNapiValue(result, &error, info);
}

Napi::Value SegySessionNode::GetAsciiHeaders(const Napi::CallbackInfo& info) {
  segy_api_error error;
  const char* result = segy_get_ascii_headers(this->session_, &error);

  return GetNapiValue(result, &error, info);
}

Napi::Value SegySessionNode::GetAsciiHeadersAsJson(const Napi::CallbackInfo& info) {
  segy_api_error error;
  const char* result = segy_get_ascii_headers_as_json(this->session_, &error);

  return GetNapiValue(result, &error, info);
}

void SegySessionNode::ThrowJavaScriptExceptionIfIsError(segy_api_error* error, Napi::Env env) {
  if(error->is_error) {
    Napi::TypeError::New(env, error->message).ThrowAsJavaScriptException();
  }
}

Napi::Value SegySessionNode::GetNapiValue(const char* result, segy_api_error* error, const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  ThrowJavaScriptExceptionIfIsError(error, env);

  return Napi::String::New(env, result);
}