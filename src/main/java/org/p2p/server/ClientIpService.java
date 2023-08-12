package org.p2p.server;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.Optional;

@Service
public class ClientIpService {
    @Autowired
    ClientIpRepo clientIpRepo;
    public void save(ClientIpEntity clientIpEntity){
        clientIpRepo.save(clientIpEntity);
    }

    public void update(ClientIpEntity clientIpEntity){
        clientIpRepo.save(clientIpEntity);
    }

    public Optional<ClientIpEntity> getByIp(String ip){
        return clientIpRepo.findByIp(ip);
    }

    public Optional<ClientIpEntity> getById(long id){
        return clientIpRepo.findById(id);
    }


}
