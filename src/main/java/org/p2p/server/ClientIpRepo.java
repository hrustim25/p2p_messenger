package org.p2p.server;

import org.springframework.data.repository.CrudRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
interface ClientIpRepo extends CrudRepository<ClientIpEntity, Long> {
    Optional<ClientIpEntity> findByIp(String ip);
}


