
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
nc localhost 4000
```

or 

```
nc <CONTAINER IP> 4000
```

if running in Docker.

To close connection just say `bye`!
