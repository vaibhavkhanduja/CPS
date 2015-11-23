# Makefile for a go project
 
# Go parameters

GOBINPATH=/usr/local/go/bin
GOCMD=$(GOBINPATH)/go
GOFMT=$(GOBINPATH)/gofmt
GOBUILD=$(GOCMD) build
GOTEST=$(GOCMD) test

FLAG_OBJ = -v -o
FLAGS =  -ldflags "-w"  -gcflags "-N -l"

# Prepend our _vendor directory to the system GOPATH
# so that import path resolution will prioritize
# our third party snapshots.
GOPATH := ${PWD}/_vendor:${GOPATH}
export GOPATH

INSTALL_PATH := /usr/local/bin
export INSTALL_PATH

EXAMPLE_DOCKER_JSON := /var/lib/embers
export EXAMPLE_DOCKER_JSON

CCODE_PATH := ${PWD}
export CCODE_PATH

default: build

build: compile fmt

clean:
	\rm ../../../bin/cps_service
doc:
	godoc -http=:6060 -index

compile:
	$(GOCMD) get -d ./...
	$(GOBUILD) $(FLAG_OBJ) ../../../bin/cps_service $(FLAGS) ./src/

fmt:
	$(GOFMT) ./src/
	$(GOFMT) ./src/types/
	$(GOFMT) ./src/cmdexe/
	$(GOFMT) ./src/containerconnect/
