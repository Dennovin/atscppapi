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

#include <atscppapi/GlobalPlugin.h>
#include <atscppapi/PluginInit.h>
#include <atscppapi/Logger.h>

using namespace std;
using namespace atscppapi;

#define LOG_TAG "detachedrequest"

class GlobalHookPlugin : public GlobalPlugin {
public:
  GlobalHookPlugin() {
    registerHook(HOOK_READ_REQUEST_HEADERS_PRE_REMAP);
  }
  void handleReadRequestHeadersPreRemap(Transaction &transaction) {
    string url_str("http://www.linkedin.com/");
    Request detached_request(url_str);
    Url &url = detached_request.getUrl();
    TS_DEBUG(LOG_TAG, "Method is [%s], url is [%s], version is [%s]",
             HTTP_METHOD_STRINGS[detached_request.getMethod()].c_str(),
             detached_request.getUrl().getUrlString().c_str(),
             HTTP_VERSION_STRINGS[detached_request.getVersion()].c_str());

    Headers &detached_request_headers = detached_request.getHeaders();
    TS_DEBUG(LOG_TAG, "Headers before adds");
    printHeaders(detached_request_headers);

    detached_request_headers.set("Header1", "value1");
    detached_request_headers.append("Header1", "value2");
    detached_request_headers.set("Header2", "value1");
    TS_DEBUG(LOG_TAG, "Headers after adds");
    printHeaders(detached_request_headers);

    detached_request_headers.erase("Header1");
    TS_DEBUG(LOG_TAG, "Headers after erase");
    printHeaders(detached_request_headers);

    transaction.resume();
  }
private:
  void printHeaders(const Headers &headers) {
    for (Headers::const_iterator iter = headers.begin(), end = headers.end(); iter != end; ++iter) {
      TS_DEBUG(LOG_TAG, "Header [%s] has values [%s]", iter->first.c_str(),
               Headers::getJoinedValues(iter->second).c_str());
    }
  }
};

void TSPluginInit(int argc, const char *argv[]) {
  GlobalPlugin *instance = new GlobalHookPlugin();
}
