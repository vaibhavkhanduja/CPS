/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fcntl.h>
#include <signal.h>
#include <getopt.h>
#include <sys/epoll.h>
#include <linux/kdev_t.h>
#include <time.h>

#include "../../common/mem_ops.h"
#include "../../common/structures.h"
#include "../../common/deserialize.h"

#ifndef __NO_KMOD__
#include "lkm.h"
#endif

#include "utils.h"
#include "netlink.h"

#define CPS_VERSION 0.1
#define CPS_MODULE_NAME "cps"
#define CPS_MODULE_PATH "./cps.ko"

#define MAXEVENTS 2
#define MEM_LIMIT 30000

void do_segfault();
void add_data(syscall_intercept_info *i);
void free_data(syscall_intercept_info *i);
