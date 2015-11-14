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

#ifndef UDIS_UTILS_H_INCLUDED
#define UDIS_UTILS_H_INCLUDED

#include "../udis86/udis86.h"

typedef struct addrs_t {
	int len1, len2;
	void *addr1, *addr2;
} addrs_t;

uint64_t ud_get_stub_size(void *entry);
void ud_patch_addr(void *entry, void *addr);
void ud_patch_cmp(void *entry);
void *ud_find_syscall_table_addr(void *entry);

#endif