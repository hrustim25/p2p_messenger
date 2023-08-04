package org.p2p.client;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import msgr.grpc.ClientToServer;
import msgr.grpc.ClientToServerCallerGrpc;

public class Client {

    public static void main(String... args) {
        ManagedChannel managedChannel = ManagedChannelBuilder
                .forTarget("localhost:9999")
                .usePlaintext()
                .build();
        ClientToServerCallerGrpc.ClientToServerCallerBlockingStub stub =
                ClientToServerCallerGrpc
                .newBlockingStub(managedChannel);
        //ClientToServer.RegistrationResponse registrationResponse = stub.register(ClientToServer.RegistrationRequest.newBuilder().setAddress("testik").build());
      //  System.out.println(registrationResponse.getUserId());
        ClientToServer.ClientAddressResponse clientAddressResponse = stub.getClientAddress(ClientToServer.ClientAddressRequest.newBuilder().setClientId(String.valueOf(53)).setReceiverId(String.valueOf(999)).build());
        System.out.println(clientAddressResponse.getStatus());
    }
}
