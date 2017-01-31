#!/usr/bin/env bash

cat << EOF
Hi! Seems that you successfully built the echo Docker image!

You might want to try out one of the examples:

EOF

echo $(find . -maxdepth 1 -perm -111 -type f)

cat << EOF

Run one of the above examples with
    docker run concurrency/echo <EXAMPLE>

Note that if you start a server and you want to
connect with a client outside your container, you need to find the
IP address. Find out with
    docker inspect --format '{{ .NetworkSettings.IPAddress }}' $(docker ps | awk '$2 ~ /concurrency\/echo/ { print $1 }')

Example:
    docker run concurrency/echo ./single-uv

Then if it is bound to e.g. 172.17.0.2, connect with
    nc 172.17.0.2 3000

Enjoy!

EOF
