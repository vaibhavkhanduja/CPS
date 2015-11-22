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

package containerconnect

import (
	"fmt"
	"github.com/Pallinder/go-randomdata"
	"github.com/vaibhavkhanduja/dockerclient"
	"log"
	"strings"
	"time"
)

func eventCallback(event *dockerclient.Event, args ...interface{}) {
	log.Printf("Received event: %#v\n", *event)
}

func RunContainer(docker *dockerclient.DockerClient, containerId string, mt string, monitor bool) (err error) {

	var hostConfig dockerclient.HostConfig
	bindmt := mt + ":/data:rw"
	hostConfig.Binds = []string{bindmt}
	//hostConfig.NetworkMode = "host"

	if monitor {
		go func() {
			err = docker.StartContainer(containerId, &hostConfig)
			if err != nil {
				log.Print(err)
			}
		}()
	} else {
		err = docker.StartContainer(containerId, &hostConfig)
		if err != nil {
			log.Print("Cannot start container")
			return err
		}
	}
	if monitor {
		docker.StartMonitorEvents(eventCallback)
		time.Sleep(3600 * time.Second)
	}

	return err
}

func StartContainer(docker *dockerclient.DockerClient, config *dockerclient.ContainerConfig) (containerId string, err error) {

	name := randomdata.City()
	name = strings.Replace(name, " ", "", -1)
	_, err = docker.CreateContainer(config, name)
	if err != nil {
		log.Print("Cannot create container")
		return "", err
	}

	fmt.Println(name)
	return name, err
}

func CreateClient() (docker *dockerclient.DockerClient, err error) {

	docker, err = dockerclient.NewDockerClient("unix:///var/run/docker.sock", nil)
	if err != nil {
		log.Print("Cannot init the docker client")
		return nil, err
	}

	return docker, err
}
