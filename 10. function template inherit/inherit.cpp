#include "v8-context.h"
#include <node.h>

namespace __inherit__ {

using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Object;
using v8::ObjectTemplate;
using v8::Persistent;
using v8::String;
using v8::Value;

Persistent<Function> cons; // 持久函数句柄

void SetName(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  Local<Object> self = args.Holder();
  (void)self->Set(v8::Context::New(isolate),
                  String::NewFromUtf8(isolate, "name").ToLocalChecked(),
                  args[0]);
}

void Summary(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  Local<Object> self = args.Holder();
  char temp[512];

  String::Utf8Value type(
      isolate, self->Get(v8::Context::New(isolate),
                         String::NewFromUtf8(isolate, "type").ToLocalChecked())
                   .ToLocalChecked()
                   ->ToString(v8::Context::New(isolate))
                   .ToLocalChecked());
  String::Utf8Value name(
      isolate, self->Get(v8::Context::New(isolate),
                         String::NewFromUtf8(isolate, "name").ToLocalChecked())
                   .ToLocalChecked()
                   ->ToString(v8::Context::New(isolate))
                   .ToLocalChecked());

  snprintf(temp, 511, "%s is a/an %s.", *name, *type);

  args.GetReturnValue().Set(
      String::NewFromUtf8(isolate, temp).ToLocalChecked());
}

void Pet(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  // 获取 this
  Local<Object> self = args.Holder();
  (void)self->Set(v8::Context::New(isolate),
                  String::NewFromUtf8(isolate, "name").ToLocalChecked(),
                  String::NewFromUtf8(isolate, "Unknown").ToLocalChecked());
  (void)self->Set(v8::Context::New(isolate),
                  String::NewFromUtf8(isolate, "type").ToLocalChecked(),
                  String::NewFromUtf8(isolate, "animal").ToLocalChecked());
  // 构造函数中需要将 this 返回，以供 const foo = new Pet() 的左值接收
  args.GetReturnValue().Set(self);
}

void Dog(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  Local<Object> self = args.Holder();
  //   获取 Pet 构造函数
  Local<Function> super = cons.Get(isolate);
  // 调用实现继承
  (void)super->Call(v8::Context::New(isolate), self, 0, NULL);
  (void)self->Set(v8::Context::New(isolate),
                  // 将 type 设置为 dog
                  String::NewFromUtf8(isolate, "type").ToLocalChecked(),
                  String::NewFromUtf8(isolate, "dog").ToLocalChecked());

  args.GetReturnValue().Set(self);
}

void Init(Local<Object> exports, Local<Object> module) {
  Isolate *isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  // 创建构造函数
  Local<FunctionTemplate> pet = FunctionTemplate::New(isolate, Pet);
  // 在构造函数的原型上添加方法
  pet->PrototypeTemplate()->Set(
      String::NewFromUtf8(isolate, "setName").ToLocalChecked(),
      FunctionTemplate::New(isolate, SetName));
  pet->PrototypeTemplate()->Set(
      String::NewFromUtf8(isolate, "summary").ToLocalChecked(),
      FunctionTemplate::New(isolate, Summary));
  //   获取 pet 构造句柄
  Local<Function> pet_cons =
      pet->GetFunction(v8::Context::New(isolate)).ToLocalChecked();
  // cons 升格成一个持久句柄。目的是为了在其子类 Dog
  // 的构造函数中能使用它来进行一个函数调用
  // 而这个函数类似于前面JavaScript源码中的 Pet.call（this）
  cons.Reset(isolate, pet_cons);

  Local<FunctionTemplate> dog = FunctionTemplate::New(isolate, Dog);
  // 继承自 Pet
  dog->Inherit(pet);

  Local<Function> dog_cons =
      dog->GetFunction(v8::Context::New(isolate)).ToLocalChecked();

  (void)exports->Set(v8::Context::New(isolate),
                     String::NewFromUtf8(isolate, "Pet").ToLocalChecked(),
                     pet_cons);
  (void)exports->Set(v8::Context::New(isolate),
                     String::NewFromUtf8(isolate, "Dog").ToLocalChecked(),
                     dog_cons);
}

NODE_MODULE(_template, Init)

} // namespace __inherit__
