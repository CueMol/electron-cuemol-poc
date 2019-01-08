
#include <nan.h>
#include <stdio.h>
#include "my_extension.hpp"

using namespace Nan;
using namespace v8;

NAN_METHOD(hello)
{
  printf("===============\n");
  printf(" hello called\n");

  unsigned char* buffer = (unsigned char*) node::Buffer::Data(info[0]->ToObject());
  unsigned int size = info[1]->Uint32Value();

  printWndInfo(buffer, size);

  info.GetReturnValue().Set(Nan::New("hello, world").ToLocalChecked());
}

NAN_METHOD(createNatWin)
{
  printf("===============\n");
  printf(" createNatWin called\n");

  unsigned char* buffer = (unsigned char*) node::Buffer::Data(info[0]->ToObject());
  unsigned int size = info[1]->Uint32Value();

  createNatWinImpl(buffer, size);
}

NAN_METHOD(setWinPos)
{
  printf("===============\n");
  printf(" setWinPos called\n");
  
  // expect exactly 3 arguments
  if(info.Length() != 4) {
    return Nan::ThrowError(Nan::New("Vector::New - expected arguments x, y, w, h").ToLocalChecked());
  }

  // expect arguments to be numbers
  if(!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber() || !info[3]->IsNumber()) {
    return Nan::ThrowError(Nan::New("Vector::New - expected arguments to be numbers").ToLocalChecked());
  }

  int x = (int) info[0]->NumberValue();
  int y = (int) info[1]->NumberValue();
  int w = (int) info[2]->NumberValue();
  int h = (int) info[3]->NumberValue();

  setWindowPos(x, y, w, h);
}

NAN_MODULE_INIT(init)
{
  NAN_EXPORT(target, hello);
  NAN_EXPORT(target, createNatWin);
  NAN_EXPORT(target, setWinPos);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init);


/*
#include <node.h>

void HelloFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    //args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "hello, world"));
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "hello, world", v8::NewStringType::kNormal).ToLocalChecked());
}

void init(v8::Local<v8::Object> exports)
{
    NODE_SET_METHOD(exports, "hello", HelloFunction);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init);

*/
