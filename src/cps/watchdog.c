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

#include "watchdog.h"

syscall_intercept_info_node *head, *curr, *tail;


unsigned int get_ns_pid_inode(int pid) {
	char cmd[1000];
	char process[100];
	char data[1000];
	unsigned int inode_pid;
	FILE *fp = NULL;
	struct stat sb;
	sprintf(process, "/proc/%d/ns/pid", pid);
	if (stat(process, &sb) == -1) {
		fprintf(stderr, "Could not get namespace id of process %u", pid);
		exit(-2);
	}
	sprintf(cmd, "ls -Li /proc/%d/ns/pid 2>&1 | awk '{ print $1 }' ", pid);
	fp = popen(cmd, "r");
	//if (!fp) {
	//	return -1;
	//}
	fgets(data, 1000, fp);
	sscanf(data, "%u", &inode_pid);
	return inode_pid;
}

void log_data(syscall_intercept_info_node *in, char *file, int pid,
	 					unsigned int inode_pid, unsigned int minor) {
    /*Print each row*/
		static int print_pid = 0;
		while (in != head) {
		if ((strncmp(in->i->path, file, strlen(file)) == 0) &&
			(minor == (unsigned int)MINOR(in->i->devid)) &&
      				(in->i->proc_inum != inode_pid) &&
				(pid != in->i->pid))  {
					if (print_pid != in->i->pid) {
						printf("Alert ... unknown process accessed data for file %s\n", file);
						printf("minor %u ..namespace inode .. %u in namespace inode %u ... path is %s\n",
								(unsigned int) MINOR(in->i->devid), in->i->proc_inum, inode_pid, in->i->path);
					}
					print_pid = in->i->pid;
			}
			in = in->prev;
     		}
}

int main(int argc, char **argv){
	/*Random vars*/
	int c, running;
	syscall_intercept_info *info;
	syscall_intercept_info_node *in, *tmp;
  static int verbose_flag;
	char *file = NULL;
  int pid = 0;
  unsigned int inode_pid = 0;
	unsigned int minor = 0;

	/*Event loop related vars*/
	int sock_fd, stdin_fd, efd;
	struct epoll_event event = {
		.events = 0
	},
	events[2] = {
		{ .events = 0 },
		{ .events = 0 }
	};

	/*NetLink related vars*/
	struct iovec iov;
	struct nlmsghdr *nlh = NULL;

    while (1)
    {
        static struct option long_options[] =
        {
            /* These options set a flag. */
            {"verbose", no_argument,       &verbose_flag, 1},
            {"file",    required_argument, 0, 'f'},
            {"process",    required_argument, 0, 'p'},
						{"minor", required_argument, 0, 'm'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "f:p:m:",
                         long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'f':
                printf ("option -f with value `%s'\n", optarg);
                file  = strdup(optarg);
                break;
							case 'p':
								printf("option -p with value `%s`\n", optarg);
								pid=atoi(optarg);
	   						inode_pid = get_ns_pid_inode(pid);
								break;
						case 'm':
								printf("option -m with value %s\n", optarg);
								minor = atoi(optarg);
								break;
            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort ();
        }
    }


	/*Initialize NetLink msg headers and get socket file descriptor*/
	sock_fd = net_init(&nlh, &iov);
	if(sock_fd == -1){
		printf("Error starting NetLink.\n");
		return -1;
	}

	/*Get STDIN file descriptor*/
	stdin_fd = fcntl(STDIN_FILENO,  F_DUPFD, 0);

	/*Set our client PID in cps*/
	set_client_pid(getpid());

	/*Make SEGFAULTs play nice with NCURSES*/
	signal(SIGSEGV, do_segfault);

	/*Create event loop*/
	efd = epoll_create1(0);
	if(efd == -1){
		printf("Error creating epoll\n");
		return -1;
	}

	event.data.fd = sock_fd;
	event.events = EPOLLIN;
	if(epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &event) == -1){
		printf("Error adding socket fd to epoll\n");
		return -1;
	}

	event.data.fd = stdin_fd;
	event.events = EPOLLIN;
	if(epoll_ctl(efd, EPOLL_CTL_ADD, stdin_fd, &event) == -1){
		printf("Error adding stdin fd to epoll\n");
		return -1;
	}

	/*Keep track on the data we have*/
	head = new(sizeof(syscall_intercept_info_node));
	if(!head){
		printf("Error allocating memory for data list\n");
		return -1;
	}
	head->prev = head->next = NULL;
	head->i = NULL;
	tail = curr = head;

	/*Start even loop*/
	running = 1;
	while(running){
		int n, i;
		n = epoll_wait(efd, events, MAXEVENTS, -1);
		for(i = 0; i < n; i++){
			if(events[i].events & EPOLLIN){
				if(events[i].data.fd == sock_fd){
					while ((info = read_from_socket(sock_fd, nlh)) != NULL) {
						add_data(info);
					}

					/*Don't draw if there's nothing new to draw*/
					if(curr == tail){
						log_data(curr, file, pid, inode_pid, minor);
					}
				}else if(events[i].data.fd == stdin_fd){
						log_data(curr, file, pid, inode_pid, minor);
				}
			}else if(events[i].events & (EPOLLHUP | EPOLLERR)){
				continue;
			}
		}
	}

	close(sock_fd);

	/*Free all the memory we allocated for the data*/
	if(head->i && head != tail){
		in = head;
		while(!in->next){
			free_data(in->i);

			tmp = in;
			in = in->next;

			del(tmp);
		}
	}

	/*Free everything else we might have allocated*/
	del(nlh);

	/*Let the cps module that we're not listening anymore*/
	set_client_pid(-1);

	return 0;
}

void do_segfault(){
	abort();
}

void add_data(syscall_intercept_info *i){
	static unsigned long int total_nodes = 0;
	syscall_intercept_info_node *in, *tmp;

	if(!i){
		return;
	}

	if(head->i == NULL){
		in = head;
		in->i = i;
	}else{

		if(total_nodes >= MEM_LIMIT){
			tmp = head;
			head = head->next;
			free_data(tmp->i);
			del(tmp);
			total_nodes--;
		}

		in = calloc(sizeof(syscall_intercept_info_node), 1);
		in->prev = tail;
		in->i = i;
		tail->next = in;
		tail = in;
		tail->next = NULL;
	}

	total_nodes++;

	/*Auto-scroll on next draw!*/
	if(curr == tail->prev){
		curr = tail;
	}
}

void free_data(syscall_intercept_info *i){
	del(i->pname);
	del(i->operation);
	del(i->path);
	del(i->result);
	del(i->details);
	del(i);
}
