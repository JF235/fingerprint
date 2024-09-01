#!/bin/bash

volume1="/path/to/gallery"
volume2="/path/to/query_basep"
container_dir="/data"

docker run \
    --rm \
    -p 8888:8888 \
    -v "$volume1:$container_dir/gallery" \
    -v "$volume2:$container_dir/query_basep" \
    "jupyter/base-notebook"

# docker exec -it -w /work <container-ID> bash