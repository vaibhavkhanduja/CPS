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

package main

import (
	"containerconnect"
        "cmdexe"
	"flag"
	"fmt"
	"github.com/vaibhavkhanduja/dockerclient"
	"log"
	"os"
) 

const cps_watchdog string = "/usr/local/cps/bin/cps_watchdog"

var Usage = func() {
	fmt.Fprintf(os.Stderr, "Usage of cps_service\n")
	flag.PrintDefaults()
	os.Exit(1)
}

func eventCallback(event *dockerclient.Event, args ...interface{}) {
	log.Printf("Received event: %#v\n", *event)
}

func main() {
  _, err := containerconnect.CreateClient()
  if err != nil {
	log.Fatal(err)
  }
  cmd := cmdexe.CreateCommand(cps_watchdog)
  if cmd == nil {
    log.Printf("Could not locate cps watchdog")
  }

  out, err := cmdexe.RunGetOutput(*cmd)
  if err != nil {
    log.Printf("Could not run cps watchdog")
  }
  log.Printf(out);
}

