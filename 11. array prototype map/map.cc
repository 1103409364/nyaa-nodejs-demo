#include "v8-context.h"
#include <node.h>

namespace __map__ {

using v8::Array;
using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Int32;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Object;
using v8::ObjectTemplate;
using v8::Persistent;
using v8::String;
using v8::Value;

Persistent<Function> cons;

void Map(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate(); // 隔离实例
  Local<Context> context = isolate->GetCurrentContext();

  Local<Array> array = args[0].As<Array>();
  Local<Function> func = args[1].As<Function>();

  Local<Array> ret = Array::New(isolate, array->Length());

  Local<Value> null = v8::Null(isolate);
  // 声明数组 a 长度 3，初始化三个参数都是 Value 类型
  Local<Value> a[3] = {Local<Object>(), null, array};
  for (uint32_t i = 0; i < array->Length(); i++) {
    a[0] = array->Get(context, i).ToLocalChecked();
    a[1] = Int32::New(isolate, i);

    MaybeLocal<Value> v = func->Call(context, null, 3, a);
    (void)ret->Set(context, i, v.ToLocalChecked());
  }

  args.GetReturnValue().Set(ret);
}

void Init(Local<Object> exports, Local<Object> module) {
  Isolate *isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  Local<Context> context = isolate->GetCurrentContext();

  (void)module->Set(context,
                    String::NewFromUtf8(isolate, "exports").ToLocalChecked(),
                    FunctionTemplate::New(isolate, Map)
                        ->GetFunction(context)
                        .ToLocalChecked());
}

NODE_MODULE(_template, Init)

} // namespace __map__
