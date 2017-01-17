#!/usr/bin/env bash

cat << EOF
Hi! Seems that you successfully built this Docker image!

You might want to try out one of the examples:

EOF

echo $(find . -maxdepth 1 -perm -111 -type f)

cat << EOF

Run one of the above examples with
    docker run <CONTAINER> <EXAMPLE>

Note that if you start a server and you want to
connect with a local client, you need to find the
IP address of your container. Find out with
    docker inspect --format '{{ .NetworkSettings.IPAddress }}' <CONTAINER>

Enjoy!

EOF