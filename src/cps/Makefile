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

CC		=	gcc
CFLAGS	=	-g -Wall -Wextra -Werror -std=gnu89

LDLIBS	=	$(shell pkg-config --silence-errors --libs libkmod)
LDLIBS	+=	$(shell pkg-config --silence-errors --libs ncurses)
OBJS	=	../common/mem_ops.o ../common/deserialize.o ../common/string_utils.o netlink.o utils.o watchdog.o
SOURCES	=	watchdog.c
DESTS	=	../../bin/watchdog

KMOD	=	$(shell pkg-config --exists libkmod && echo "1" || echo "0")

new: clean all

clean:
	@rm -rf *.o */*.o ../common/*.o $(DESTS)

all: _check_kmod $(DESTS)

_check_kmod:
ifeq ($(KMOD), 0)
CFLAGS += -D__NO_KMOD__
else
OBJS += lkm.o
endif

../../bin/watchdog: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
