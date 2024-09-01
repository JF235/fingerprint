#!/bin/bash

volume1="/path/to/gallery"
volume2="/path/to/query_basep"
container_dir="/data"
docker run \
    --rm \
    -p 8888:8888 \
    -v "$volume1:/data/gallery" \
    -v "$volume2:/data/query_basep" \
    -v "$volume3:/work" \
    -w /work \
    "jupyter/base-notebook"

# docker exec -it -w /work <container-ID> bash