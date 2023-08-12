package org.p2p.server;

import io.grpc.Server;
import io.grpc.ServerBuilder;
import io.grpc.stub.StreamObserver;
import msgr.grpc.ClientToServerCallerGrpc;
import org.lognet.springboot.grpc.GRpcService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

import java.io.IOException;
@GRpcService
class ClientToServer extends ClientToServerCallerGrpc.ClientToServerCallerImplBase {
    @Autowired
    ClientIpService clientIpService;
    @Override
    public void register(msgr.grpc.ClientToServer.RegistrationRequest request, StreamObserver<msgr.grpc.ClientToServer.RegistrationResponse> responseObserver) {
        if (clientIpService.getByIp(request.getAddress()).isEmpty()) {
            ClientIpEntity clientIpEntity = new ClientIpEntity(request.getAddress());
            clientIpService.save(clientIpEntity);
            System.out.println("mem" + clientIpEntity.getId());
            clientIpEntity = clientIpService.getByIp(clientIpEntity.getIp()).get();
            responseObserver.onNext(msgr.grpc.ClientToServer
                    .RegistrationResponse
                    .newBuilder()
                    .setStatus("ok")
                    .setUserId(String.valueOf(clientIpEntity.getId()))
                    .build());
            responseObserver.onCompleted();
        } else {
            responseObserver
                    .onNext(msgr
                            .grpc.ClientToServer
                            .RegistrationResponse
                            .newBuilder()
                            .setStatus("-1")
                            .build());
            responseObserver.onCompleted();
        }
    }

    @Override
    public void updateData(msgr.grpc.ClientToServer.UpdateDataRequest updateDataRequest, StreamObserver<msgr.grpc.ClientToServer.UpdateDataResponse> updateDataResponseStreamObserver) {
        if (clientIpService.getById(Long.parseLong(updateDataRequest.getUserId())).isPresent()){
            ClientIpEntity clientIpEntity = new ClientIpEntity(Long.parseLong(updateDataRequest.getUserId()), updateDataRequest.getAddress());
            clientIpService.update(clientIpEntity);
            updateDataResponseStreamObserver
                    .onNext(msgr.grpc.ClientToServer
                            .UpdateDataResponse
                            .newBuilder()
                            .setStatus("ok")
                            .build());
            updateDataResponseStreamObserver.onCompleted();
        }
        else{
            updateDataResponseStreamObserver
                    .onNext(msgr.grpc.ClientToServer
                            .UpdateDataResponse
                            .newBuilder()
                            .setStatus("-1")
                            .build());
            updateDataResponseStreamObserver.onCompleted();
        }
    }

    @Override
    public void getClientAddress(msgr.grpc.ClientToServer.ClientAddressRequest clientAddressRequest, StreamObserver<msgr.grpc.ClientToServer.ClientAddressResponse> clientAddressResponseStreamObserver) {
        if (clientIpService.getById(Long.parseLong(clientAddressRequest.getReceiverId())).isPresent()){
            ClientIpEntity clientIpEntity = clientIpService.getById(Long.parseLong(clientAddressRequest.getReceiverId())).get();
            clientAddressResponseStreamObserver.onNext(msgr.grpc.ClientToServer
                    .ClientAddressResponse
                    .newBuilder()
                    .setStatus("ok")
                    .setReceiverAddress(clientIpEntity.getIp())
                    .build());
            clientAddressResponseStreamObserver.onCompleted();
        }
        else{
            clientAddressResponseStreamObserver.onNext(msgr.grpc.ClientToServer
                    .ClientAddressResponse
                    .newBuilder()
                    .setStatus("-1")
                    .build());
            clientAddressResponseStreamObserver.onCompleted();
        }
    }
}
@SpringBootApplication
public class Main {


    public static void main(String[] args) throws IOException, InterruptedException {
        SpringApplication.run(Main.class, args);
        System.out.println("hui");
        System.out.println("Server has started!");
        //   System.out.println(ClientIpService.getbyIp("huh").get().getIp());
        //ClientIpService.update(new ClientIpEntity((long)2, "huh"));
    }
}