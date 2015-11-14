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

#include "mem_ops.h"

void *new(size_t sz){
#ifdef __KERNEL__
	return kmalloc(sz, GFP_KERNEL);
#else
	return malloc(sz);
#endif
}

void *renew(void *ptr, size_t sz){
#ifdef __KERNEL__
	return krealloc(ptr, sz, GFP_KERNEL);
#else
	return realloc(ptr, sz);
#endif
}

void *duplicate(void *ptr){
#ifdef __KERNEL__
	return kstrdup(ptr, GFP_KERNEL);
#else
	return strdup(ptr);
#endif	
}

void del(void *ptr){
#ifdef __KERNEL__
	kfree(ptr);
#else
	free(ptr);
#endif
}