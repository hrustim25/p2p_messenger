package org.p2p.client;


import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import msgr.grpc.ClientToServer;
import msgr.grpc.ClientToServerCallerGrpc;

public class Client {

    public static void main(String... args) {
        ManagedChannel managedChannel = ManagedChannelBuilder
                .forTarget("0.0.0.0:9999")
                .usePlaintext()
                .build();
        ClientToServerCallerGrpc.ClientToServerCallerBlockingStub stub =
                ClientToServerCallerGrpc
                .newBlockingStub(managedChannel);
      //  ClientToServer.UpdateDataResponse updateDataResponse = stub.updateData(ClientToServer.UpdateDataRequest.newBuilder().setAddress("newadd").setUserId(String.valueOf(6)).build());
        ClientToServer.RegistrationResponse registrationResponse = stub.register(ClientToServer.RegistrationRequest.newBuilder().setAddress("efwer").build());
      System.out.println(registrationResponse.getUserId());
    //    ClientToServer.ClientAddressResponse clientAddressResponse = stub.getClientAddress(ClientToServer.ClientAddressRequest.newBuilder().setClientId(String.valueOf(2)).setReceiverId(String.valueOf(3)).build());
     //  System.out.println(clientAddressResponse.getReceiverAddress());
    }
}