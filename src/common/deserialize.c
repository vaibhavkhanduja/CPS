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

#include "deserialize.h"

syscall_intercept_info *deserialize_syscall_info(membuffer *buffer){
	void *tmp;
	syscall_intercept_info *i = new(sizeof(struct syscall_intercept_info));

	if(!i){
		return NULL;
	}

	i->pname = (char *)get_chunk(buffer);

	tmp = get_chunk(buffer);
	i->pid = *(pid_t *)tmp;
	del(tmp);

	i->operation = (char *)get_chunk(buffer);
	i->path = (char *)get_chunk(buffer);
	i->result = (char *)get_chunk(buffer);
	i->details = (char *)get_chunk(buffer);

	tmp = get_chunk(buffer);
	i->proc_inum = *(unsigned int *)tmp;
	del(tmp);

	tmp = get_chunk(buffer);
	i->devid = *(dev_t *)tmp;
	del(tmp);
	
	return i;
}

void *get_chunk(membuffer *buffer){
	size_t size;
	void *chunk;
	membuffer *tmp;

	memcpy(&size, buffer->data, sizeof(size_t));
	chunk = new(size);
	memcpy(chunk, buffer->data + sizeof(size_t), size);

	tmp = new(sizeof(membuffer));
	tmp->len = buffer->len - sizeof(size_t) - size;
	tmp->data = new(tmp->len);

	memcpy(tmp->data, buffer->data + sizeof(size_t) + size, tmp->len);

	del(buffer->data);
	buffer->data = tmp->data;
	buffer->len = tmp->len;
	del(tmp);

	return chunk;
}
