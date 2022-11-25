#include "v8-context.h"
#include <node.h>

namespace __entry__ {

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

void RunCallback(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Local<Function> cb = Local<Function>::Cast(args[0]);
  const unsigned argc = 1;
  Local<Value> argv[argc] = {
      String::NewFromUtf8(isolate, "hello world").ToLocalChecked()};
  (void)cb->Call(Context::New(isolate), isolate->GetCurrentContext()->Global(),
                 argc, argv);
}

void init(Local<Object> exports, Local<Object> module) {
  Isolate *isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  NODE_SET_METHOD(exports, "runCallback", RunCallback);

  Local<Object> global = isolate->GetCurrentContext()->Global();
  Local<Object> console =
      global
          ->Get(Context::New(isolate),
                String::NewFromUtf8(isolate, "console").ToLocalChecked())
          .ToLocalChecked()
          ->ToObject(Context::New(isolate))
          .ToLocalChecked();
  Local<Function> log = Local<Function>::Cast(
      console
          ->Get(Context::New(isolate),
                String::NewFromUtf8(isolate, "log").ToLocalChecked())
          .ToLocalChecked());
  Local<Value> argv[3] = {
      module, String::NewFromUtf8(isolate, "---").ToLocalChecked(), exports};

  (void)log->Call(Context::New(isolate), console, 3, argv);
}

NODE_MODULE(addon, init)

} // namespace __entry__