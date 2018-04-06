#!/bin/sh
go build --ldflags '-extldflags -static' -o ../bin/discover .
