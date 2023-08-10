package org.p2p.client;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import msgr.grpc.ClientToServer;
import msgr.grpc.ClientToServerCallerGrpc;

public class Client {

    public static void main(String... args) {
        ManagedChannel managedChannel = ManagedChannelBuilder
                .forTarget("0.0.0.0:9998")
                .usePlaintext()
                .build();
        ClientToServerCallerGrpc.ClientToServerCallerBlockingStub stub =
                ClientToServerCallerGrpc
                .newBlockingStub(managedChannel);
      //  ClientToServer.RegistrationResponse registrationResponse = stub.register(ClientToServer.RegistrationRequest.newBuilder().setAddress("llr").build());
     //  System.out.println(registrationResponse.getUserId());
      //  ClientToServer.ClientAddressResponse clientAddressResponse = stub.getClientAddress(ClientToServer.ClientAddressRequest.newBuilder().setClientId(String.valueOf(999999)).setReceiverId(String.valueOf(2)).build());
      //  System.out.println(clientAddressResponse.getReceiverAddress());
    }
}
