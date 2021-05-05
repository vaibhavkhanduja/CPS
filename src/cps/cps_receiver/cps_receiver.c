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

#include "cps_receiver.h"

syscall_intercept_info_node *head, *curr, *tail;


void log_data(syscall_intercept_info_node *in) {
   while (in != head) {
     fprintf(stdout,"{\"timestamp\":%u, \"inode\":%u, \"pid\":%u, \"path\":\"%s\"}\n", (unsigned)time(NULL), in->i->proc_inum, in->i->pid, in->i->path);
     in = in->prev;
   }
}

int main(int argc, char **argv){
  int c, running;
  syscall_intercept_info *info;
  syscall_intercept_info_node *in, *tmp;
  static int verbose_flag;

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

  while (1) {
   static struct option long_options[] =  {
            /* These options set a flag. */
            {"verbose", no_argument,       &verbose_flag, 1},
            {"file",    required_argument, 0, 'f'},
            {0, 0, 0, 0}
   };
   /* getopt_long stores the option index here. */
   int option_index = 0;
   c = getopt_long (argc, argv, "f:",
         long_options, &option_index);
   /* Detect the end of the options. */
   if (c == -1)
       break;

   switch (c) {
     case 0:
     case 'f':
       /* If this option set a flag, do nothing else now. */
       if (long_options[option_index].flag != 0)
          break;
       printf ("option %s", long_options[option_index].name);
       if (optarg)
          printf (" with arg %s", optarg);
       printf ("\n");
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
    if (sock_fd == -1){
      printf("Error starting NetLink.\n");
      return -1;
    }
    stdin_fd = fcntl(STDIN_FILENO,  F_DUPFD, 0);
    set_client_pid(getpid());
    signal(SIGSEGV, do_segfault);
    efd = epoll_create1(0);
    if (efd == -1){
      printf("Error creating epoll\n");
      return -1;
    }
    event.data.fd = sock_fd;
    event.events = EPOLLIN;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &event) == -1){
   	printf("Error adding socket fd to epoll\n");
	return -1;
    }
    #if 0
    event.data.fd = stdin_fd;
    event.events = EPOLLIN;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, stdin_fd, &event) == -1){
      printf("Error adding stdin fd to epoll\n");
      return -1;
    }
   #endif
   head = new(sizeof(syscall_intercept_info_node));
   if (!head){
     printf("Error allocating memory for data list\n");
     return -1;
   }
   head->prev = head->next = NULL;
   head->i = NULL;
   tail = curr = head;
   running = 1;
   while (running){
      int n, i;
      n = epoll_wait(efd, events, MAXEVENTS, -1);
     for (i = 0; i < n; i++){
       if (events[i].events & EPOLLIN){
        if (events[i].data.fd == sock_fd){
          while ((info = read_from_socket(sock_fd, nlh)) != NULL) {
             add_data(info);
          }
         if (curr == tail) {
           log_data(curr);
         }
	} else if (events[i].data.fd == stdin_fd){
	        log_data(curr);
	       }
	} else if (events[i].events & (EPOLLHUP | EPOLLERR)) {
		continue;
		}
     }
  }
 close(sock_fd);
 if (head->i && head != tail){
   in = head;
   while (!in->next){
     free_data(in->i);
     tmp = in;
     in = in->next;
     del(tmp);
   }
  }

  del(nlh);
  set_client_pid(-1);
  return 0;
}

void do_segfault(){
	abort();
}

void add_data(syscall_intercept_info *i) {
    static unsigned long int total_nodes = 0;
    syscall_intercept_info_node *in, *tmp;
    if (!i) {
	return;
    }
    if (head->i == NULL){
	in = head;
	in->i = i;
    } else {
	if (total_nodes >= MEM_LIMIT){
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
    if (curr == tail->prev) {
      curr = tail;
    }
} 

void free_data(syscall_intercept_info *i) {
	del(i->pname);
	del(i->operation);
	del(i->path);
	del(i->result);
	del(i->details);
	del(i);
}
