# p2p_messenger
P2P messenger

## Client
### Client build

Nessesary lib installation:
```
sudo apt install -y build-essential autoconf libtool pkg-config cmake libsqlite3-dev
```

Build client with:
```
mkdir build
cd build
cmake ..
make
```

### Client usage

In build dir:
```
./client
```

### Client with docker

Run client in docker container by building image:
```
docker build -t hrustim25/msgr-client
```
Or by downloading existing image:
```
docker pull hrustim25/msgr-client
```
And run with:
```
docker run --rm -ti --network="host" hrustim25/msgr-client
```
