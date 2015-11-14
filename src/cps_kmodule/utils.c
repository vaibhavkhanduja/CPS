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

#include "utils.h"
#include <linux/stat.h>

/*****************************************************************************\
| map_writable creates a shadow page mapping of the range                     |
| [addr, addr + len) so that we can write to code mapped read-only.           |
|                                                                             |
| It is similar to a generalized version of x86's text_poke.  But because one |
| cannot use vmalloc/vfree() inside stop_machine, we use map_writable to map  |
| the pages before stop_machine, then use the mapping inside stop_machine,    |
| and unmap the pages afterwards.                                             |
|                                                                             |
| STOLEN from: https://github.com/jirislaby/ksplice                           |
\*****************************************************************************/

void *map_writable(void *addr, size_t len){
	void *vaddr;
	int i, nr_pages = DIV_ROUND_UP(offset_in_page(addr) + len, PAGE_SIZE);
	struct page **pages = kmalloc(nr_pages * sizeof(*pages), GFP_KERNEL);
	void *page_addr = (void *)((unsigned long)addr & PAGE_MASK);

	if(pages == NULL)
		return NULL;

	for(i = 0; i < nr_pages; i++){
		if(__module_address((unsigned long)page_addr) == NULL){
			pages[i] = virt_to_page(page_addr);
			WARN_ON(!PageReserved(pages[i]));
		}else{
			pages[i] = vmalloc_to_page(page_addr);
		}
		if(pages[i] == NULL){
			vaddr = NULL;
			goto out;
		}
		page_addr += PAGE_SIZE;
	}
	vaddr = vmap(pages, nr_pages, VM_MAP, PAGE_KERNEL);

out:
	kfree(pages);
	return vaddr == NULL ? NULL : vaddr + offset_in_page(addr);
}

char *path_from_fd(unsigned int fd){
	struct file *file;
	struct path path;
	struct files_struct *files = current->files;
	char *tmp, *pathname = "", *rpathname = new(4);

	strcpy(rpathname, "Err");

	spin_lock(&files->file_lock);
	file = fcheck_files(files, fd);
	if (!file) {
		spin_unlock(&files->file_lock);
		return rpathname;
	}

	path = file->f_path;
	path_get(&path);
	spin_unlock(&files->file_lock);

	tmp = (char *)__get_free_page(GFP_TEMPORARY);
	if(!tmp){
		path_put(&path);
		return rpathname;
	}

	pathname = d_path(&path, tmp, PAGE_SIZE);
	path_put(&path);

	if(IS_ERR(pathname)){
		free_page((unsigned long)tmp);
		return rpathname;
	}

	del(rpathname);
	rpathname = kstrdup(pathname, GFP_KERNEL);

	free_page((unsigned long)tmp);

	return rpathname;
}

dev_t device_id_from_fd(unsigned int fd) {
/*	struct kstat sb;
	int error = 0;
	error = vfs_fstat(fd, &sb);

	if (error) {
		cps_error("Could not get vfs stat %d", error);
		return 0;
	}
   return  MAJOR(sb.dev);
	*/

	struct file *file;
	dev_t device_id = 0;
	struct files_struct *files = current->files;

	spin_lock(&files->file_lock);
	file = fcheck_files(files, fd);
	if (!file) {
		spin_unlock(&files->file_lock);
		return device_id;
	}
	device_id = file->f_inode->i_sb->s_dev;
	spin_unlock(&files->file_lock);
	return device_id;
}
