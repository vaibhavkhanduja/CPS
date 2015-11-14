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

void set_client_pid(int pid){
	FILE *file;

	/*Set our client PID in CPS*/
	file = fopen("/proc/sys/cps/client_pid", "w");
	if(file){
		fprintf(file, "%d", pid);
		fclose(file);
	}
}

int get_netlink_id(void){
	FILE *file;
	int netlink_id;

	/*Get the NetLink ID of CPS*/
	file = fopen("/proc/sys/cps/netlink", "r");
	if(file){
		fscanf(file, "%d", &netlink_id);
		fclose(file);
	}else{
		netlink_id = -1;
	}

	return netlink_id;
}