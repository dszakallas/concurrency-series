# On your machine

Generate build files and build:
```
mkdir build && cd build
cmake ..
make
```

Run server and listen on port 4000
```
./single 4000 # single threaded
./multi 5000  # multi-threaded
./single-uv 6000 # single threaded non-blocking with libuv
```

Connect with

```
nc localhost <PORT>
```

To close connection just say `bye`!

# Docker

You can all above examples on Docker if you have Windows or don't fancy
installing the deps.

```
docker build -t concurrency/echo .
docker run concurrency/echo # ./index.sh help
```
