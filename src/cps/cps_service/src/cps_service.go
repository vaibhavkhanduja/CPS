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
        "time"
	"os"
        "bufio"
) 

const cps_receiver string = "/vagrant/bin/cps_receiver"

var Usage = func() {
	fmt.Fprintf(os.Stderr, "Usage of cps_service\n")
	flag.PrintDefaults()
	os.Exit(1)
}

func eventCallback(event *dockerclient.Event, args ...interface{}) {
	log.Printf("Received event: %#v\n", *event)
}

func startDockerThread() {
  docker, err := containerconnect.CreateClient()
  if err != nil {
	log.Fatal(err)
  }
  docker.StartMonitorEvents(eventCallback)
  time.Sleep(3600 * time.Second)
}

func startCPSReceiver() {
  cmd := cmdexe.CreateCommand(cps_receiver)
  if cmd == nil {
    log.Printf("Could not locate cps receiver")
  }
  stdout, err := cmd.StdoutPipe()
  if  err != nil {
    log.Fatal(err)
  }
  if err = cmd.Start(); err != nil {
    log.Fatal(err)
  }
  defer cmd.Wait()  // Doesn't block
  go func() {
   scanner := bufio.NewScanner(stdout)
   for scanner.Scan() {
     line := scanner.Text()
     fmt.Printf("%s\n", line)
   }
  }()
}

func main() {
 go startDockerThread();
 startCPSReceiver();
}

