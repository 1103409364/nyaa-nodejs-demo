#include "v8-context.h"
#include <node.h>

namespace __try_catch__ {

using v8::Array;
using v8::Context;
using v8::Exception;
using v8::External;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Int32;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Number;
using v8::Object;
using v8::ObjectTemplate;
using v8::Persistent;
using v8::String;
using v8::TryCatch;
using v8::Value;

Persistent<Function> cons;

void Get(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  TryCatch trycatch(isolate);

  MaybeLocal<Object> maybe_obj = args[0]->ToObject(context);
  MaybeLocal<String> maybe_key = args[1]->ToString(context);

  if (maybe_obj.IsEmpty() || maybe_key.IsEmpty()) {
    if (trycatch.HasCaught())
      trycatch.ReThrow();
    else
      args.GetReturnValue().Set(v8::Undefined(isolate));

    return;
  }

  Local<Object> obj = maybe_obj.ToLocalChecked();
  Local<String> key = maybe_key.ToLocalChecked();

  Local<Value> ret = obj->Get(context, key).ToLocalChecked();
  args.GetReturnValue().Set(ret);
}

void Div(const FunctionCallbackInfo<Value> &args) {
  // 篇幅所限，使用旧版 API
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<Int32> a = args[0]->ToInt32(context).ToLocalChecked();
  Local<Int32> b = args[1]->ToInt32(context).ToLocalChecked();

  int32_t av = a->Value();
  int32_t bv = b->Value();

  if (bv == 0) {
    isolate->ThrowException(Exception::Error(
        String::NewFromUtf8(isolate, "除数不能为 0。").ToLocalChecked()));
    return;
  }

  args.GetReturnValue().Set(av / bv);
}

void Init(Local<Object> exports, Local<Object> module) {
  Isolate *isolate = Isolate::GetCurrent();
  Local<Context> context = isolate->GetCurrentContext();
  HandleScope scope(isolate);

  (void)module->Set(context,
                    String::NewFromUtf8(isolate, "exports").ToLocalChecked(),
                    FunctionTemplate::New(isolate, Get)
                        ->GetFunction(context)
                        .ToLocalChecked());

  Local<Object> exp =
      module
          ->Get(context,
                String::NewFromUtf8(isolate, "exports").ToLocalChecked())
          .ToLocalChecked()
          .As<Object>();
  (void)exp->Set(context, String::NewFromUtf8(isolate, "div").ToLocalChecked(),
                 FunctionTemplate::New(isolate, Div)
                     ->GetFunction(context)
                     .ToLocalChecked());
}

NODE_MODULE(_template, Init)

} // namespace __try_catch__
