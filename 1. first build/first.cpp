#include <node.h>

namespace __first__ {

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

void Method(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  args.GetReturnValue().Set(
      // ToLocalChecked 将 MaybeLocal<> 转为 Local<> 本地句柄，
      // 如果 MaybeLocal<>是空指针句柄
      String::NewFromUtf8(isolate, "first build").ToLocalChecked());
}

void init(Local<Object> exports) { NODE_SET_METHOD(exports, "first", Method); }

NODE_MODULE(NODE_GYP_MODULE_NAME, init) // NODE_GYP_MODULE_NAME addon

} // namespace __first__