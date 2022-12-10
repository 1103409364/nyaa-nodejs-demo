#include "v8-context.h"
#include <node.h>

namespace __template__ {

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

void Method(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  args.GetReturnValue().Set(
      String::NewFromUtf8(isolate, "this is a function").ToLocalChecked());
}

void init(Local<Object> exports) {
  Isolate *isolate = Isolate::GetCurrent();
  Local<Context> context = isolate->GetCurrentContext();

  HandleScope scope(isolate);
  Local<FunctionTemplate> t = FunctionTemplate::New(isolate, Method);
  Local<Function> fn = t->GetFunction(context).ToLocalChecked();
  Local<String> name =
      String::NewFromUtf8(isolate, "funcCreateByTemplate").ToLocalChecked();
  fn->SetName(name);
  // 警告 Ignoring return value of function declared with 'warn_unused_result'
  // attribute。转为 void
  // https://stackoverflow.com/questions/43238446/ignoring-return-value-of-function-declared-with-warn-unused-result-attribute
  (void)exports->Set(context, name, fn);
}

NODE_MODULE(_template, init)

} // namespace __template__
