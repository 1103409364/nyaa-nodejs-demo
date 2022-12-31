#include "myobject.h"
#include "v8-context.h"
#include <node.h>

namespace __addon5__ {

using v8::Context;
using v8::Local;
using v8::Number;
using v8::Object;

void Add(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Context> context = isolate->GetCurrentContext();
  MyObject *obj1 =
      ObjectWrap::Unwrap<MyObject>(args[0]->ToObject(context).ToLocalChecked());
  MyObject *obj2 =
      ObjectWrap::Unwrap<MyObject>(args[1]->ToObject(context).ToLocalChecked());

  double sum = obj1->value() + obj2->value();
  args.GetReturnValue().Set(Number::New(isolate, sum));
}

void InitAll(Local<Object> exports, Local<Object> module) {
  MyObject::Init(exports->GetIsolate());

  NODE_SET_METHOD(exports, "createObject", MyObject::NewInstance);
  NODE_SET_METHOD(exports, "add", Add);
}

NODE_MODULE(addon1, InitAll)

} // namespace __addon5__
