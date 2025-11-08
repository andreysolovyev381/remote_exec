#!/bin/bash
set -e

docker buildx build \
  --progress=plain \
  --platform=linux/amd64 \
  -t remote_exec_image \
  -f Dockerfile.builder \
  --load .

docker create --name temp_container remote_exec_image
docker cp temp_container:/app/build/src/remote_exec_helper ./linux_execs
docker cp temp_container:/app/build/src/remote_hello_world ./linux_execs
docker rm temp_container