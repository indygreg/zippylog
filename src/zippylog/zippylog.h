//  Copyright 2010 Gregory Szorc
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef ZIPPYLOG_H_
#define ZIPPYLOG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* we must put this little macro before all exportable symbols on Windows */
#ifdef WINDOWS
#if defined(ZIPPYLOG_IMPORT)
#define ZIPPYLOG_EXPORT __declspec(dllimport)
#else
#define ZIPPYLOG_EXPORT __declspec(dllexport)
#endif
#endif

/* define 32 and 64 bit integer types */
#ifdef WINDOWS
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#endif

// TODO need to add "GOOGLE_PROTOBUF_VERIFY_VERSION;" somewhere

void zippylog_shutdown();

#ifdef __cplusplus
}
#endif
#endif