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

/**
 * @file utils.h
 * @brief Contains utility functions available to users of the api.
 *
 * @author Brian Geffon
 * @author Manjesh Nilange
 */

#pragma once
#ifndef ATSCPPAPI_UTILS_H_
#define ATSCPPAPI_UTILS_H_

#include <string>
#include <arpa/inet.h>
#include <stdint.h>

namespace atscppapi {
namespace utils {

/**
 * @brief Returns a pretty printed string of a sockaddr *
 *
 * @param sockaddr* A pointer to a sockaddr *
 * @return a string which is the pretty printed address
 */
std::string getIpString(const sockaddr *);

/**
 * @brief Returns just the port portion of the sockaddr *
 *
 * @param sockaddr* A pointer to a sockaddr *
 * @return a uint16_t which is the port from the sockaddr *
 */
uint16_t getPort(const sockaddr *);

/**
 * @brief Returns a pretty printed string of a sockaddr * including port
 *
 * @param sockaddr* A pointer to a sockaddr *
 * @return a string which is the pretty printed address including port
 */
std::string getIpPortString(const sockaddr *);

/**
 * @brief This is the environment variable that disables caching in all
 * types including InitializableValue.
 */
extern const std::string DISABLE_DATA_CACHING_ENV_FLAG;

}
}

#endif /* ATSCPPAPI_UTILS_H_ */
