#include "../deps/minihttp/minihttp.h"
#include "v8-context.h"
#include <node.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace __interceptor__ {

using v8::Array;
using v8::EscapableHandleScope;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Integer;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Name;
using v8::Number;
using v8::Object;
using v8::ObjectTemplate;
using v8::Persistent;
using v8::PropertyCallbackInfo;
using v8::PropertyHandlerFlags;
using v8::String;
using v8::Value;

Local<Array> GetList(Isolate *isolate) {
  EscapableHandleScope scope(isolate);

  // 访问 CNode.js API
  char *content = minihttp::Download("https://cnodejs.org/api/v1/topics");
  MaybeLocal<Value> maybe =
      v8::JSON::Parse(v8::Context::New(isolate),
                      String::NewFromUtf8(isolate, content).ToLocalChecked());
  Local<Value> ret = maybe.ToLocalChecked();
  free(content);

  // 得到 `data` 数组
  Local<Array> data = Local<Array>::Cast(
      ret->ToObject(v8::Context::New(isolate))
          .ToLocalChecked()
          ->Get(v8::Context::New(isolate),
                String::NewFromUtf8(isolate, "data").ToLocalChecked())
          .ToLocalChecked()
          ->ToObject(v8::Context::New(isolate))
          .ToLocalChecked());

  printf("fetching %s for %s...ok\n", "https://cnodejs.org/api/v1/topics",
         "list");

  return scope.Escape(data);
}

Local<Object> GetTopic(Isolate *isolate, const char *id, const char *usage) {
  EscapableHandleScope scope(isolate);

  int url_length = strlen("https://cnodejs.org/api/v1/topic/") + strlen(id);
  char url[url_length + 1];
  url[0] = 0;
  strcat(url, "https://cnodejs.org/api/v1/topic/");
  strcat(url, id);

  printf("fetching %s for %s...", url, usage);

  // 访问 CNode.js API
  char *content = minihttp::Download(url);
  if (0 == content) {
    printf("failed\n");
    return Local<Object>();
  }
  MaybeLocal<Value> maybe =
      v8::JSON::Parse(v8::Context::New(isolate),
                      String::NewFromUtf8(isolate, content).ToLocalChecked());
  Local<Value> ret = maybe.ToLocalChecked();

  free(content);

  if (ret->ToObject(v8::Context::New(isolate))
          .ToLocalChecked()
          ->Get(v8::Context::New(isolate),
                String::NewFromUtf8(isolate, "success").ToLocalChecked())
          .ToLocalChecked()
          ->ToBoolean(isolate)
          ->IsFalse()) {
    printf("failed\n");
    return Local<Object>();
  }

  // 得到 `data` 数据
  Local<Object> data =
      ret->ToObject(v8::Context::New(isolate))
          .ToLocalChecked()
          ->Get(v8::Context::New(isolate),
                String::NewFromUtf8(isolate, "data").ToLocalChecked())
          .ToLocalChecked()
          ->ToObject(v8::Context::New(isolate))
          .ToLocalChecked();

  printf("ok\n");
  return scope.Escape(data);
}

void __Sleep() {
#ifdef _WIN32
  Sleep(10);
#else
  usleep(10);
#endif
}

void Getter(Local<Name> property, const PropertyCallbackInfo<Value> &info) {
  Isolate *isolate = info.GetIsolate();
  HandleScope scope(isolate);

  String::Utf8Value key(isolate, property);

  Local<Object> data = GetTopic(isolate, *key, "getter");
  __Sleep();

  if (!data.IsEmpty()) {
    info.GetReturnValue().Set(
        data->Get(v8::Context::New(isolate),
                  String::NewFromUtf8(isolate, "title").ToLocalChecked())
            .ToLocalChecked()
            ->ToString(v8::Context::New(isolate))
            .ToLocalChecked());
  }
}

void Query(Local<Name> property, const PropertyCallbackInfo<Integer> &info) {
  Isolate *isolate = info.GetIsolate();
  HandleScope scope(isolate);

  String::Utf8Value key(isolate, property);

  Local<Object> data = GetTopic(isolate, *key, "query");
  __Sleep();

  if (!data.IsEmpty()) {
    info.GetReturnValue().Set(static_cast<int32_t>(v8::ReadOnly) |
                              static_cast<int32_t>(v8::DontEnum));
    return;
  }
}

void Enumerator(const PropertyCallbackInfo<Array> &info) {
  Isolate *isolate = info.GetIsolate();
  HandleScope scope(isolate);

  Local<Array> data = GetList(isolate);
  __Sleep();

  for (unsigned int i = 0; i < data->Length(); i++) {
    Local<Object> element =
        data->Get(v8::Context::New(isolate), Number::New(isolate, i))
            .ToLocalChecked()
            ->ToObject(v8::Context::New(isolate))
            .ToLocalChecked();
    Local<String> id =
        element
            ->Get(v8::Context::New(isolate),
                  String::NewFromUtf8(isolate, "id").ToLocalChecked())
            .ToLocalChecked()
            ->ToString(v8::Context::New(isolate))
            .ToLocalChecked();
    (void)data->Set(v8::Context::New(isolate), Number::New(isolate, i), id);
  }

  info.GetReturnValue().Set(data);
}

void Init(Local<Object> exports, Local<Object> module) {
  minihttp::InitNetwork();
  atexit(minihttp::StopNetwork);

  Isolate *isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  Local<ObjectTemplate> tpl = ObjectTemplate::New(isolate);
  tpl->SetHandler(v8::NamedPropertyHandlerConfiguration(
      Getter, 0, Query, 0, Enumerator, Local<Value>(),
      PropertyHandlerFlags::kOnlyInterceptStrings));

  (void)module->Set(
      v8::Context::New(isolate),
      String::NewFromUtf8(isolate, "exports").ToLocalChecked(),
      ((v8::MaybeLocal<Object>)tpl->NewInstance(v8::Context::New(isolate)))
          .ToLocalChecked());
}

NODE_MODULE(mapped_property_interceptor, Init)

} // namespace __interceptor__
