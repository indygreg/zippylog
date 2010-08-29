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

#pragma once

#include <pblog/pblog.h>
#include <pblog/broker.hpp>
#include <pblog/store.hpp>

#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include <apr.h>
#include <apr_general.h>
#include <apr_getopt.h>
#include <apr_strings.h>
#include <zmq.hpp>

using namespace ::pblog;
using namespace ::pblog::server;

#define MEMORY_ERROR "out of memory" APR_EOL_STR

void _exit()
{
    apr_terminate2();
}

int _abort(int code)
{
    write(STDERR_FILENO, MEMORY_ERROR, strlen(MEMORY_ERROR));
    abort();
    return code;
}

int main(int argc, const char * const argv[])
{
    apr_status_t st;
    apr_pool_t *p, *p_opts;
    apr_getopt_t *opt;
    char option;
    const char *arg;
    apr_int64_t max_threads = 10;
    apr_int64_t listen_port = 52483;
    char *listen_address = "0.0.0.0";
    char *store_path;

    st = apr_app_initialize(&argc, &argv, NULL);
    if (st != APR_SUCCESS) {
        printf("APR failed to initialize\n");
        exit(1);
    }
    atexit(_exit);

    st = apr_pool_create(&p, NULL);
    if (st != APR_SUCCESS) {
        printf("failed to create root APR pool\n");
        exit(1);
    }

    apr_pool_tag(p, "pblogd-root");
    apr_pool_abort_set(_abort, p);

    st = apr_pool_create(&p_opts, p);
    if (st != APR_SUCCESS) {
        printf("unable to create option parsing pool\n");
        exit(1);
    }

    st = apr_getopt_init(&opt, p_opts, argc, argv);
    if (st != APR_SUCCESS) {
        printf("unable to initialize argument parsing\n");
        exit(1);
    }

    while ((st = apr_getopt(opt, "c:s:", &option, &arg)) == APR_SUCCESS) {
        switch (option) {
            case 'c':
                max_threads = apr_atoi64(arg);
                if (errno != 0) {
                    printf("argument to -c must be integer\n");
                    exit(1);
                }
                if (max_threads < 1) {
                    printf("argument to -c must be positive\n");
                    exit(1);
                }
                break;

            case 's':
                store_path = apr_pstrdup(p, arg);
                break;
        }
    }
    apr_pool_destroy(p_opts);

    Store store = Store(store_path);

    Broker broker = Broker(&store, p);
    broker.add_listen_endpoint(apr_psprintf(p, "tcp://%s:%d", listen_address, listen_port));
    broker.run();

    return 0;
}
