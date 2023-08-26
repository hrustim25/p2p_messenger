FROM ubuntu:20.04

RUN ln -snf /usr/share/zoneinfo/$CONTAINER_TIMEZONE /etc/localtime && echo $CONTAINER_TIMEZONE > /etc/timezone

RUN apt update && apt install -y --no-install-recommends \
    build-essential \
    autoconf \
    libtool \
    pkg-config \
    ca-certificates \
    cmake \
    git \
    libsqlite3-dev \
    libssl-dev
RUN apt clean

WORKDIR /app
COPY ./src ./src
COPY ./CMakeLists.txt ./CMakeLists.txt

RUN cd /app && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j 4

WORKDIR /app/build

ENTRYPOINT [ "/bin/bash", "-l", "-c" ]
CMD [ "./client" ]
