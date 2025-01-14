#include "v8-context.h"
#include <node.h>

namespace __internal_field_wrong__ {

using v8::Context;
using v8::External;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Object;
using v8::ObjectTemplate;
using v8::String;
using v8::Value;

enum Gender { MALE, FEMALE };

struct Person {
  char name[256];
  int age;
  Gender gender;
};

void GetSummary(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<Object> self = args.Holder();
  Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));

  Person *person = static_cast<Person *>(wrap->Value());

  char ret[512];
  //   已废弃
  //   sprintf(ret, "%s，%s，%d 岁。", person->name,
  //           person->gender == MALE ? "男" : "女", person->age);

  snprintf(ret, 6, "%s，%s，%d 岁。", person->name,
           person->gender == MALE ? "男" : "女", person->age);
  MaybeLocal<String> summary = String::NewFromUtf8(isolate, ret);
  // 拿到这个对象的内置对象并做一些计算，将结果返回给JavaScript代码
  args.GetReturnValue().Set(summary.ToLocalChecked());
}

void Init(Local<Object> exports, Local<Object> module) {
  Isolate *isolate = Isolate::GetCurrent();
  Local<Context> context = isolate->GetCurrentContext();
  HandleScope scope(isolate);

  Local<ObjectTemplate> templ = ObjectTemplate::New(isolate);
  templ->SetInternalFieldCount(1);

  Person *person = new Person();

  strcpy(person->name, "芙兰朵露·丝卡蕾特");
  person->gender = Gender::FEMALE;
  person->age = 495;

  MaybeLocal<Object> dummy_obj = templ->NewInstance(context);
  Local<Object> obj = dummy_obj.ToLocalChecked();

  // 假设 obj 肯定不为空
  obj->SetInternalField(0, External::New(isolate, person));
  (void)obj->Set(context,
                 String::NewFromUtf8(isolate, "getSummary").ToLocalChecked(),
                 FunctionTemplate::New(isolate, GetSummary)
                     ->GetFunction(context)
                     .ToLocalChecked());

  (void)module->Set(
      context, String::NewFromUtf8(isolate, "exports").ToLocalChecked(), obj);
}

NODE_MODULE(_template, Init)

} // namespace __internal_field_wrong__
