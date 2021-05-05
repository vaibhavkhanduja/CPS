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

#include "control.h"

/*****************************************************************************\
| /proc/sys state and methods related to the control of cps                 |
\*****************************************************************************/

static int nsyscalls_state_table = 0;
static struct ctl_table *syscalls_state_table;
static struct ctl_table_header *cps_table_header;

static struct ctl_table state_table[] = {
	{
		.procname = "syscalls", .mode = 0555
	},
	{
		.procname = "state", .mode = 0666,
		.proc_handler = &proc_dointvec_minmax,
		.data = &cps_state, .maxlen	= sizeof(int),
		.extra1 = "\x00\x00\x00\x00" /*0*/, .extra2 = "\x01\x00\x00\x00" /*1*/
	},
	{
		.procname = "netlink", .mode = 0444,
		.proc_handler = &proc_dointvec,
		.data = "\xFF\xFF\xFF\xFF" /*-1, assign after nl_init()*/, .maxlen = sizeof(int)
	},
	{
		.procname = "client_pid", .mode = 0666,
		.proc_handler = &proc_dointvec,
		.data = &client_pid, .maxlen = sizeof(pid_t)
	},
//	{ 0 }
	{
		.procname = NULL, .mode = 0,
		.proc_handler = NULL,
		.data = NULL, .maxlen = 0
	}
};

static struct ctl_table cps_table[] = {
	{
		.procname = "cps", .mode = 0555,
		.child = state_table
	},
//	{ 0 }
	{
		.procname = NULL, .mode = 0,
		.child = NULL
	}
};

int register_cps_controls(void){

	syscalls_state_table = kcalloc(nsyscalls_state_table, sizeof(struct ctl_table), GFP_KERNEL);
	if(!syscalls_state_table){
		return 0;
	}

	cps_table_header = register_sysctl_table(cps_table);
	if(!cps_table_header){
		return 0;
	}else{
		return 1;
	}
}

void add_syscalls_state_table_entry(char *procname, int *state){
	if(nsyscalls_state_table == 0){
		nsyscalls_state_table = 2;
		syscalls_state_table = kcalloc(nsyscalls_state_table, sizeof(struct ctl_table), GFP_KERNEL);
	}else{
		nsyscalls_state_table++;
		syscalls_state_table = krealloc(syscalls_state_table, nsyscalls_state_table * sizeof(struct ctl_table), GFP_KERNEL);
		memset(&(syscalls_state_table[nsyscalls_state_table - 1]), 0, sizeof(struct ctl_table));
	}

	syscalls_state_table[nsyscalls_state_table - 2].procname = procname;
	syscalls_state_table[nsyscalls_state_table - 2].mode = 0666;
	syscalls_state_table[nsyscalls_state_table - 2].proc_handler = &proc_dointvec_minmax;
	syscalls_state_table[nsyscalls_state_table - 2].data = state;
	syscalls_state_table[nsyscalls_state_table - 2].maxlen	= sizeof(int);
	syscalls_state_table[nsyscalls_state_table - 2].extra1 = "\x00\x00\x00\x00"; /*0*/
	syscalls_state_table[nsyscalls_state_table - 2].extra2 = "\x01\x00\x00\x00"; /*1*/

	state_table[0].child = syscalls_state_table;
}

void set_cps_control_netlink_id(int *netlink_id){
	cps_table[0].child[2].data = netlink_id;
}

void unregister_cps_controls(void){
	unregister_sysctl_table(cps_table_header);
	kfree(syscalls_state_table);
}

/*****************************************************************************\
|                                      END                                    |
\*****************************************************************************/
