package org.p2p.server;

import io.grpc.Server;
import io.grpc.ServerBuilder;
import io.grpc.stub.StreamObserver;
import jakarta.persistence.Entity;
import jakarta.persistence.EntityManager;
import jakarta.persistence.EntityManagerFactory;
import jakarta.persistence.Persistence;
import lombok.Getter;
import lombok.RequiredArgsConstructor;
import msgr.grpc.ClientToServerCallerGrpc;
import org.hibernate.Query;
import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.hibernate.Transaction;
import org.hibernate.boot.MetadataSources;
import org.hibernate.boot.registry.StandardServiceRegistry;
import org.hibernate.boot.registry.StandardServiceRegistryBuilder;
import org.hibernate.impl.QueryImpl;

import java.io.IOException;

class ClientToServer extends ClientToServerCallerGrpc.ClientToServerCallerImplBase {
    @Override
    public void register(msgr.grpc.ClientToServer.RegistrationRequest request, StreamObserver<msgr.grpc.ClientToServer.RegistrationResponse> responseObserver) {
        if (ClientIpService.getbyIp(request.getAddress()).isEmpty()) {
            ClientIpEntity clientIpEntity = new ClientIpEntity(request.getAddress());
            ClientIpService.save(clientIpEntity);
            System.out.println("mem" + clientIpEntity.getId());
            clientIpEntity = ClientIpService.getbyIp(clientIpEntity.getIp()).get();
            responseObserver.onNext(msgr.grpc.ClientToServer
                    .RegistrationResponse
                    .newBuilder()
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
        if (ClientIpService.getById(Long.parseLong(updateDataRequest.getUserId())).isPresent()){
            ClientIpEntity clientIpEntity = new ClientIpEntity(Long.parseLong(updateDataRequest.getUserId()), updateDataRequest.getAddress());
            ClientIpService.update(clientIpEntity);
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
        if (ClientIpService.getById(Long.parseLong(clientAddressRequest.getReceiverId())).isPresent()){
            ClientIpEntity clientIpEntity = ClientIpService.getById(Long.parseLong(clientAddressRequest.getReceiverId())).get();
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

public class Main {


    public static void main(String[] args) throws IOException, InterruptedException {
       Server server = ServerBuilder.forPort(9999)
                .addService(new ClientToServer())
                .build();
        server.start();
        System.out.println(Thread.currentThread().threadId());
        System.out.println("Server has started!");

        server.awaitTermination();
     //   System.out.println(ClientIpService.getbyIp("huh").get().getIp());
        //ClientIpService.update(new ClientIpEntity((long)2, "huh"));
    }
}