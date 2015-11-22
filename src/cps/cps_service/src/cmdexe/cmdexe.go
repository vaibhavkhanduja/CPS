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

package cmdexe

import (
	"bytes"
	"fmt"
	"log"
	"os/exec"
	"runtime"
	"strings"
)

func CreateCommand(exe string, s ...string) *exec.Cmd {
	if runtime.GOOS == "nacl" {
		fmt.Println("skipping on nacl")
	}
	cmd := exec.Command(exe, s...)
	return cmd
}

func RunCommand(cmd exec.Cmd) (err error) {
	err = cmd.Start()
	if err != nil {
		log.Fatal(err)
	}
	err = cmd.Wait()
	//log.Printf("Command finished with error: %v", err)
	if _, ok := err.(*exec.ExitError); !ok {
		//fmt.Println("expected *exec.ExitError from cat combined; got %T: %v", err, err)
	}
	return err
}

func CheckCommand(cmd string) (err error) {
	_, err = exec.LookPath(cmd)
	if err != nil {
		fmt.Println("%s is not available in path", cmd)
	}
	return err
}

func RunGetOutput(cmd exec.Cmd) (output string, err error) {
	var buff bytes.Buffer
	cmd.Stdout = &buff
	if err = cmd.Start(); err != nil {
		return "", err
	}
	if err = cmd.Wait(); err != nil {
		return "", err
	}
	output = buff.String()
	t := strings.TrimSpace(output)
	return t, err
}
