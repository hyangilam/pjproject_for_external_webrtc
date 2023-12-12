/*
 * Copyright 2023 SK TELECOM CO., LTD.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ADDR_RESOLV_DARWIN_H__
#define __ADDR_RESOLV_DARWIN_H__

#include <pj/log.h>
#include <pj/os.h>
#include <pj/assert.h>
#include <pj/string.h>
#include <pj/errno.h>

#if defined(PJ_GETADDRINFO_USE_CFHOST) && PJ_GETADDRINFO_USE_CFHOST != 0
#include <dispatch/dispatch.h>
#include <CoreFoundation/CFString.h>
#include <CFNetwork/CFHost.h>

bool pj_getCFHostAddresses(CFHostRef hostRef, int timeOutSec);
#endif

#endif //__ADDR_RESOLV_DARWIN_H__