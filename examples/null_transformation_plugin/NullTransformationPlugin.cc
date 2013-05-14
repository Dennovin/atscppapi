/*
 * Copyright (c) 2013 LinkedIn Corp. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of the license at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 *
 */

#include <iostream>
#include <atscppapi/GlobalPlugin.h>
#include <atscppapi/TransactionPlugin.h>
#include <atscppapi/TransformationPlugin.h>
#include <atscppapi/PluginInit.h>
#include <atscppapi/Logger.h>

using namespace atscppapi;
using std::string;

namespace {
atscppapi::Logger *log;
#define TAG "null_transformation"
}

class NullTransformationPlugin : public TransformationPlugin {
public:
  NullTransformationPlugin(Transaction &transaction)
    : TransformationPlugin(transaction, RESPONSE_TRANSFORMATION) {
    registerHook(HOOK_SEND_RESPONSE_HEADERS);
  }

  void handleSendResponseHeaders(Transaction &transaction) {
    transaction.getClientResponse().getHeaders().set("X-Content-Transformed", "1");
    transaction.resume();
  }

  void consume(const string &data) {
    produce(data);
  }

  void handleInputComplete() {
    setOutputComplete();
  }

  virtual ~NullTransformationPlugin() {

  }

private:
};

class GlobalHookPlugin : public GlobalPlugin {
public:
  GlobalHookPlugin() {
    registerHook(HOOK_READ_RESPONSE_HEADERS);
  }

  virtual void handleReadResponseHeaders(Transaction &transaction) {
    transaction.addPlugin(new NullTransformationPlugin(transaction));
    transaction.resume();
  }

};

void TSPluginInit(int argc, const char *argv[]) {
  TS_DEBUG(TAG, "TSPluginInit");
  GlobalPlugin *instance = new GlobalHookPlugin();
}
