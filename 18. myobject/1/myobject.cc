#include "myobject.h"
#include "v8-context.h"
#include "v8-local-handle.h"

namespace __addon1__ {

using v8::Context;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Number;
using v8::String;

MyObject::MyObject(double value) : value_(value) {}

MyObject::~MyObject() {}

void MyObject::Init(Local<Object> exports) {
  Isolate *isolate = exports->GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "MyObject").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "plusOne", PlusOne);

  (void)exports->Set(context,
                     String::NewFromUtf8(isolate, "MyObject").ToLocalChecked(),
                     tpl->GetFunction(context).ToLocalChecked());
}

void MyObject::New(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  double value =
      args[0]->IsUndefined() ? 0 : args[0]->NumberValue(context).ToChecked();
  MyObject *obj = new MyObject(value);
  obj->Wrap(args.This());

  args.GetReturnValue().Set(args.This());
}

void MyObject::PlusOne(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();

  MyObject *obj = ObjectWrap::Unwrap<MyObject>(args.Holder());
  obj->value_ += 1;

  args.GetReturnValue().Set(Number::New(isolate, obj->value_));
}

} // namespace __addon1__
