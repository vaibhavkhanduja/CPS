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

#ifndef MEM_OPS_H_INCLUDED
#define MEM_OPS_H_INCLUDED

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/dcache.h>
#include <linux/fs.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

void *new(size_t sz);
void *renew(void *ptr, size_t sz);
void *duplicate(void *ptr);
void del(void *ptr);

#endif
