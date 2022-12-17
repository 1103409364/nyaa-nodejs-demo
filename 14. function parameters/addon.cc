#include "v8-local-handle.h"
#include <node.h>

namespace demo {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

// 实现 "add" 函数
void Add(const FunctionCallbackInfo<Value> &args) {
  Isolate *isolate = args.GetIsolate();
  Local<v8::Context> context = isolate->GetCurrentContext();

  // 判断参数个数是否合法
  if (args.Length() < 2) {
    // 不合法则跑错
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")
            .ToLocalChecked()));
    return;
  }

  // 判断参数类型是否合法
  if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
    return;
  }

  // 计算第一个参数加第二个参数的 `double` 值
  // 并新生成一个 Local<Number> 句柄，将计算出来的 `double` 传入
  double value = args[0]->NumberValue(context).ToChecked() +
                 args[1]->NumberValue(context).ToChecked();
  Local<Number> num = Number::New(isolate, value);

  // 设置返回值为新生成的 `num`
  args.GetReturnValue().Set(num);
}

void Init(Local<Object> exports) { NODE_SET_METHOD(exports, "add", Add); }

NODE_MODULE(addon, Init)

} // namespace demo
