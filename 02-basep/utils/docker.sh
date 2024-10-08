#!/bin/bash

volume1="datasets/gallery"
volume2="datasets/query_basep"
volume3="fingerprint/02-basep"

docker run \
    --rm \
    -p 8888:8888 \
    -v "$volume1:/data/gallery" \
    -v "$volume2:/data/query_basep" \
    -v "$volume3:/work" \
    -w /work \
    "jupyter/base-notebook"

# docker exec -it -w /work <container-ID> bash