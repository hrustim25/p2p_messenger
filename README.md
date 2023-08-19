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

## SSL/TLS support

### Manual certificate generation

You can generate certificates using OpenSSL with:
```
openssl genrsa -out rootkey.pem 2048
openssl req -x509 -new -nodes -key rootkey.pem -days 9999 -out rootcrt.pem
openssl genrsa -out serverkey.pem 2048
openssl req -new -key serverkey.pem -out servercsr.pem
openssl x509 -req -in servercsr.pem -CA rootcrt.pem -CAkey rootkey.pem -CAcreateserial -out servercrt.pem -days 9999
```
Fill all required fields.
**Attention**: For second time use actual server domain or `localhost` (to run locally) for Common Name (CN).

### Certificate usage

Place `servercrt.pem` file next to `client`.
Place `servercrt.pem`, `serverkey.pem`, `rootcrt.pem` files next to `test_server`.
Certificates will be loaded automatically.
