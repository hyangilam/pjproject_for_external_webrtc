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

#include <pj/addr_resolv_darwin.h>

#define THIS_FILE               "addr_resolv_darwin.m"

#if defined(PJ_GETADDRINFO_USE_CFHOST) && PJ_GETADDRINFO_USE_CFHOST != 0
// https://developer.apple.com/library/archive/samplecode/CFHostSample/Introduction/Intro.html#//apple_ref/doc/uid/DTS10003222
// https://github.com/fruitsamples/CFHostSample/blob/6169d7e03f009ec3e5844b2df84dcb720a5cd6a0/CFHostSample.c#L191
bool pj_getCFHostAddresses(CFHostRef hostRef, int timeOutSec)
{
    PJ_LOG(4, (THIS_FILE, "pj_getCFHostAddresses"));

// Set up a semaphore to signal when resolution is complete
    dispatch_semaphore_t sema = dispatch_semaphore_create(0);

// Start the resolution on a separate thread
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        if (CFHostStartInfoResolution(hostRef, kCFHostAddresses, nil)) {
            // Resolution succeeded, signal the semaphore
            dispatch_semaphore_signal(sema);
        }
    });

// Wait for up to timeOutSec seconds for resolution to complete
    if (dispatch_semaphore_wait(sema, dispatch_time(DISPATCH_TIME_NOW, timeOutSec * NSEC_PER_SEC)) != 0) {
        // Resolution did not complete within 5 seconds, cancel it
        CFHostCancelInfoResolution(hostRef, kCFHostAddresses);
        PJ_LOG(4, (THIS_FILE, "pj_getCFHostAddresses return false"));
        return false;
    } else {
        PJ_LOG(4, (THIS_FILE, "pj_getCFHostAddresses return true"));
        return true;
    }
}
#endif
