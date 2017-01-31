FROM ubuntu:trusty

RUN apt-get update -yu
RUN apt-get install -yu \
    build-essential automake autoconf libtool \
    cmake curl

ADD get-uv.sh /

RUN ./get-uv.sh

ADD *.c /
ADD *.h /
ADD *.cmake /
ADD CMakeLists.txt /

RUN mkdir build && cd build && cmake .. && make

WORKDIR /build

ADD index.sh ./

EXPOSE 3000 4000 5000

CMD ./index.sh
