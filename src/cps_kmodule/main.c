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

#include "main.h"

/*****************************************************************************\
| Main methods: _init and _exit                                               |
\*****************************************************************************/

int cps_state = 0, client_pid = -1;

static int __init hook_init(void){
	static int netlink_id;

	cps_info("================\n");
	cps_info("Starting cps\n");

	netlink_id = nl_init();
	if(netlink_id == -2){
		cps_error("Can't create netlink thread\n");
	}else if(netlink_id == -1){
		cps_info("Error creating socket.\n");
	}else{
		cps_info("Acquired NETLINK socket (%d)\n", netlink_id);
	}

	set_cps_control_netlink_id(&netlink_id);

	hook_calls();

	if(!register_cps_controls()){
		cps_error("Error creating fs control interface in /proc/sys/cps/\n");
		return -ENOMEM;
	}

	cps_info("Successfully loaded\n");

	return 0;
}

static void __exit hook_exit(void){
	cps_state = 0;

	cps_info("Stopping cps\n");

	unregister_cps_controls();

	unhook_calls();
	
	nl_halt();

	cps_info("Successfully unloaded\n");
	cps_info("================\n");
}

/*****************************************************************************\
|                                      END                                    |
\*****************************************************************************/

module_init(hook_init);
module_exit(hook_exit);
MODULE_LICENSE("APACHE");
MODULE_AUTHOR("Vaibhav Khanduja <vaibhavkhanduja@gmail.com>");
MODULE_DESCRIPTION("Container Protection Service");
