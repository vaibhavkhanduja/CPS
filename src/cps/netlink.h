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

#ifndef NETLINK_H_INCLUDED
#define NETLINK_H_INCLUDED

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#include "utils.h"
#include "../common/mem_ops.h"
#include "../common/structures.h"
#include "../common/deserialize.h"

#define MAX_PAYLOAD 1024

int net_init(struct nlmsghdr **nlh, struct iovec *iov);
syscall_intercept_info *read_from_socket(int sock_fd, struct nlmsghdr *nlh);

#endif