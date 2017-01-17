
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
```

Connect with

```
nc localhost 4000
```

To close connection just say `bye`!
