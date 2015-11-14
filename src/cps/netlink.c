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

#include "netlink.h"

int net_init(struct nlmsghdr **nlh, struct iovec *iov){
	int sock_fd, ret;
	struct sockaddr_nl src_addr;

	sock_fd = socket(AF_NETLINK, SOCK_DGRAM, get_netlink_id());
	if(sock_fd < 0){
		return -1;
	}

	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();

	ret = bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
	if(ret != 0){
		return -1;
	}

	*nlh = (struct nlmsghdr *)new(NLMSG_SPACE(MAX_PAYLOAD));
	memset(*nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	(*nlh)->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);

	iov->iov_base = (void *)*nlh;
	iov->iov_len = (*nlh)->nlmsg_len;

	return sock_fd;
}

syscall_intercept_info *read_from_socket(int sock_fd, struct nlmsghdr *nlh){
	membuffer *x;
	syscall_intercept_info *i;
	struct iovec iov;
	struct msghdr msg;

	iov.iov_base = nlh;
	iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;

	if (recvmsg(sock_fd, &msg, MSG_DONTWAIT) <= 0)
		return NULL;

	x = new(sizeof(membuffer));
	if(!x){
		return NULL;
	}
	x->len = nlh->nlmsg_len - NLMSG_HDRLEN;
	x->data = new(x->len);
	if(!x->data){
		return NULL;
	}
	memcpy(x->data, NLMSG_DATA(nlh), x->len);

	i = deserialize_syscall_info(x);
	del(x->data);
	del(x);
	if(!i){
		return NULL;
	}

	return i;
}