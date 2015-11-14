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

#include "netlink.h"

struct sock *nl_sk = NULL;

/* kernel thread id */
static struct task_struct *nl_thread = NULL;
/* kernel thread waitqueue */
static DECLARE_WAIT_QUEUE_HEAD(nl_wait);
/* kernel thread data queue */
static struct sk_buff_head nl_queue;

static int nl_send_skb(struct sk_buff *skb){
	if(cps_state && client_pid > 0){
		nlmsg_unicast(nl_sk, skb, client_pid);
	}else{
		kfree_skb(skb);
	}

	return 0;
}

static int nl_kernel_thread(void *arg){
	DECLARE_WAITQUEUE(wait, current);

	add_wait_queue(&nl_wait, &wait);

	while(!kthread_should_stop()){
		struct sk_buff *skb;

		current->state = TASK_RUNNING;

		while((skb = skb_dequeue(&nl_queue)) != NULL){
			nl_send_skb(skb);
		}

		current->state = TASK_INTERRUPTIBLE;

		schedule();
	}

	remove_wait_queue(&nl_wait, &wait);

	skb_queue_purge(&nl_queue);

	return 0;
}

static struct sock *nl_init_sock(int netlink_id){
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0)
	return netlink_kernel_create(&init_net, netlink_id, NULL);
#else
	return netlink_kernel_create(&init_net, netlink_id, 0, NULL, NULL, THIS_MODULE);
#endif
}

int nl_init(void){
	int netlink_id = MAX_LINKS - 1;

	skb_queue_head_init(&nl_queue);

	nl_thread = kthread_run(nl_kernel_thread, NULL, "kpmnld");
	if(IS_ERR_OR_NULL(nl_thread)){
		return -2;
	}

	while(netlink_id >= 0){
		if((nl_sk = nl_init_sock(netlink_id))){
			return netlink_id;
		}
		netlink_id--;
	}

	kthread_stop(nl_thread), nl_thread = NULL;

	return -1;
}

void nl_halt(void){
	netlink_kernel_release(nl_sk);

	kthread_stop(nl_thread), nl_thread = NULL;
}

void nl_send(syscall_intercept_info *i){
	membuffer *x;
	struct nlmsghdr *nlh;
	struct sk_buff *skb_out;

	x = serialize_syscall_info(i);
	if(!x)
		return;

	skb_out = nlmsg_new(x->len, 0);
	if(!skb_out)
		goto end;

	nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, x->len, 0);  
	NETLINK_CB(skb_out).dst_group = 0;
	memcpy(nlmsg_data(nlh), x->data, x->len);

	skb_queue_tail(&nl_queue, skb_out);
	wake_up_interruptible(&nl_wait);

end:
	del(x->data);
	del(x);
}
