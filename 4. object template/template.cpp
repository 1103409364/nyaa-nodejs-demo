#include "v8-context.h"
#include <node.h>

namespace __template__ {

using v8::Array;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::ObjectTemplate;
using v8::String;
using v8::Value;

void Constructor(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  (void)args.This()->Set(v8::Context::New(isolate),
                         String::NewFromUtf8(isolate, "value").ToLocalChecked(),
                         Number::New(isolate, 233));
  return args.GetReturnValue().Set(args.This());
}

void ClassGet(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  return args.GetReturnValue().Set(
      args.This()
          ->Get(v8::Context::New(isolate),
                String::NewFromUtf8(isolate, "value").ToLocalChecked())
          .ToLocalChecked());
}

void Func(const FunctionCallbackInfo<Value> &args) {
  args.GetReturnValue().Set(2333);
}

void Init(Local<Object> exports) {
  Isolate *isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  // 作为函数模板的原型
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, Constructor);
  tpl->SetClassName(String::NewFromUtf8(isolate, "TestClass").ToLocalChecked());
  Local<ObjectTemplate> proto = tpl->PrototypeTemplate();
  proto->Set(String::NewFromUtf8(isolate, "get").ToLocalChecked(),
             FunctionTemplate::New(isolate, ClassGet));

  (void)exports->Set(
      v8::Context::New(isolate),
      String::NewFromUtf8(isolate, "TestClass").ToLocalChecked(),
      tpl->GetFunction(v8::Context::New(isolate)).ToLocalChecked());

  // 作为对象模板创建对象
  Local<FunctionTemplate> fun = FunctionTemplate::New(isolate);
  fun->SetClassName(
      String::NewFromUtf8(isolate, "TestClass2").ToLocalChecked());
  Local<ObjectTemplate> obj_tpl = ObjectTemplate::New(isolate, fun);
  obj_tpl->Set(String::NewFromUtf8(isolate, "num").ToLocalChecked(),
               Number::New(isolate, 233));
  Local<Array> array = Array::New(isolate, 10);
  for (int i = 0; i < 10; i++) {
    (void)array->Set(
        v8::Context::New(isolate), Number::New(isolate, i),
        obj_tpl->NewInstance(v8::Context::New(isolate)).ToLocalChecked());
  }
  (void)exports->Set(v8::Context::New(isolate),
                     String::NewFromUtf8(isolate, "array").ToLocalChecked(),
                     array);

  // 设置函数体
  Local<ObjectTemplate> obj_with_func_tpl = ObjectTemplate::New(isolate);
  obj_with_func_tpl->Set(
      String::NewFromUtf8(isolate, "cat").ToLocalChecked(),
      String::NewFromUtf8(isolate, "南瓜饼").ToLocalChecked());
  obj_with_func_tpl->Set(
      String::NewFromUtf8(isolate, "dog").ToLocalChecked(),
      String::NewFromUtf8(isolate, "蛋花汤").ToLocalChecked());
  obj_with_func_tpl->SetCallAsFunctionHandler(Func);
  (void)exports->Set(v8::Context::New(isolate),
                     String::NewFromUtf8(isolate, "func").ToLocalChecked(),
                     obj_with_func_tpl->NewInstance(v8::Context::New(isolate))
                         .ToLocalChecked());
}

NODE_MODULE(_template, Init)

} // namespace __template__
