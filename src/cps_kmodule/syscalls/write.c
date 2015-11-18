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

#include "syscall.h"

extern asmlinkage void hooked_sys_pre_write(unsigned int fd, char __user *buf, size_t count);
extern asmlinkage void hooked_sys_post_write(unsigned int fd, char __user *buf, size_t count);
#ifdef CONFIG_IA32_EMULATION
extern asmlinkage void hooked_sys32_pre_write(unsigned int fd, char __user *buf, size_t count);
extern asmlinkage void hooked_sys32_post_write(unsigned int fd, char __user *buf, size_t count);
#endif

__REGISTER_SYSCALL(write);

asmlinkage void hooked_sys_pre_write(unsigned int fd, char __user *buf, size_t count){
	syscall_intercept_info *i;

	i = new(sizeof(struct syscall_intercept_info));

	__SET_SYSCALL_HOOK_INFO(i);

	if(!i)
		return;

	i->pname = current->comm;
	i->pid = current->pid;
	i->operation = "WRITE";
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,0,0)
        i->proc_inum = task_active_pid_ns(current)->ns.inum;
#else
        i->proc_inum = task_active_pid_ns(current)->proc_inum;
#endif
	i->devid = device_id_from_fd(fd);
	i->path = path_from_fd(fd);

	return;
}

asmlinkage void hooked_sys_post_write(unsigned int fd, char __user *buf, size_t count){
	ssize_t r;
	syscall_intercept_info *i;

	__GET_SYSCALL_RESULT(r);
	__GET_SYSCALL_HOOK_INFO(i);

	if(!i)
		return;

	if(IS_ERR((void *)r)){
		i->result = "Error";
		i->details = kasprintf(GFP_KERNEL, "Errno %d", (int)r);
	}else{
		i->result = "Ok";
		i->details = kasprintf(GFP_KERNEL, "Write %d bytes (was requested to write %zd)", (int)r, count);
	}

	nl_send(i);

	del(i->path);
	del(i->details);
	del(i);

	return;
}

#ifdef CONFIG_IA32_EMULATION
__REGISTER_SYSCALL32(write);

asmlinkage void hooked_sys32_pre_write(unsigned int fd, char __user *buf, size_t count){
	syscall_intercept_info *i;

	i = new(sizeof(struct syscall_intercept_info));

	__SET_SYSCALL_HOOK_INFO32(i);

	if(!i)
		return;

	i->pname = current->comm;
	i->pid = current->pid;
	i->operation = "WRITE32";
	i->path = path_from_fd(fd);
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,0,0)
        i->proc_inum = task_active_pid_ns(current)->ns.inum;
#else
        i->proc_inum = task_active_pid_ns(current)->proc_inum;
#endif
	i->devid = device_id_from_fd(fd);

	return;
}

asmlinkage void hooked_sys32_post_write(unsigned int fd, char __user *buf, size_t count){
	ssize_t r;
	syscall_intercept_info *i;

	__GET_SYSCALL_RESULT32(r);
	__GET_SYSCALL_HOOK_INFO32(i);

	if(!i)
		return;

	if(IS_ERR((void *)r)){
		i->result = "Error";
		i->details = kasprintf(GFP_KERNEL, "Errno %d", (int)r);
	}else{
		i->result = "Ok";
		i->details = kasprintf(GFP_KERNEL, "Write %d bytes (was requested to write %zd)", (int)r, count);
	}

	nl_send(i);

	del(i->path);
	del(i->details);
	del(i);

	return;
}

#endif
