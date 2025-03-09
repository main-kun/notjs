#include <cstdlib>
#include <fstream>
#include <iterator>
#include <stdio.h>
#include <filesystem>
#include <format>
#include <optional>
#include <string>

#include "libplatform/libplatform.h"
#include "v8-context.h"
#include "v8-initialization.h"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-script.h"


std::optional<std::string> ResolvePath(const std::string& path) {
  std::error_code ec;
  const std::filesystem::path canonical_path = std::filesystem::canonical(path, ec);
  if (ec) {
    fprintf(stderr, "Failed to read the file: %s", ec.message().c_str());
    return std::nullopt;
  }
  return canonical_path.generic_string();
}

std::optional<std::string> ReadFile(const std::string& filepath) {
  const std::optional<std::string> resolved_path = ResolvePath(filepath);
  if (!resolved_path) {
    return std::nullopt;
  }
  std::string path_value = resolved_path.value();
  std::ifstream ifs(path_value);
  if (!ifs.is_open()) {
    fprintf(stderr,"Could not open the file %s\n", path_value.c_str());
    return std::nullopt;
  }

  return std::string(
    std::istreambuf_iterator(ifs),
    std::istreambuf_iterator<char>());

}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: simple-runner <script>\n");
    return 1;
  }
  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();


  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
      v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate* isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);
    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate);
    // Create a new context.
    v8::Local<v8::Context> context = v8::Context::New(isolate);
    // Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context);
    {
      const std::optional<std::string> js_code = ReadFile(argv[1]);
      if (!js_code) {
        return 1;
      }
      // Create a string containing the JavaScript source code.
      v8::Local<v8::String> source =
          v8::String::NewFromUtf8(
            isolate,
            js_code.value().c_str(),
            v8::NewStringType::kNormal).ToLocalChecked();
      // Compile the source code.
      v8::Local<v8::Script> script =
          v8::Script::Compile(context, source).ToLocalChecked();
      // Run the script to get the result.
      v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
      // Convert the result to an UTF8 string and print it.
      v8::String::Utf8Value utf8(isolate, result);
      printf("%s\n", *utf8);
    }
  }
  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::DisposePlatform();
  delete create_params.array_buffer_allocator;
  return 0;
}