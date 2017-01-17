FROM ubuntu:trusty

RUN apt-get update -yu
RUN apt-get install -yu build-essential cmake

ADD *.c /
ADD *.h /
ADD *.cmake /
ADD CMakeLists.txt /

RUN mkdir build && cd build && cmake .. && make

WORKDIR /build

ADD index.sh ./

CMD ./index.sh
