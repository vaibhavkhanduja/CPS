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

#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#ifndef __KERNEL__
#include <unistd.h>
#include <sys/types.h>
#endif

typedef unsigned char byte;

typedef struct membuffer{
	size_t len;
	byte *data;
} membuffer;

typedef struct syscall_intercept_info{
	char *pname;
	pid_t pid;
	char *operation;
	char *path;
	char *result;
	char *details;
	unsigned int proc_inum;
	dev_t devid;
} syscall_intercept_info;

typedef struct syscall_intercept_info_node {
	syscall_intercept_info *i;
	struct syscall_intercept_info_node *prev, *next;
} syscall_intercept_info_node;

#endif
