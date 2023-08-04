package org.p2p.server;


import lombok.RequiredArgsConstructor;
import org.hibernate.Criteria;
import org.hibernate.Session;
import org.hibernate.Transaction;
import org.hibernate.query.Query;

import java.util.List;
import java.util.Objects;
import java.util.Optional;

@RequiredArgsConstructor
public class ClientIpService {
    public static void save(ClientIpEntity clientIpEntity){
        Session session = HibernateUtil.getSessionFactory().openSession();
        Transaction transaction =  session.beginTransaction();
        session.flush();
        session.merge(clientIpEntity);
        session.getTransaction().commit();
        session.close();
    }

    public static Optional<ClientIpEntity> getById(long id){
        Session session = HibernateUtil.getSessionFactory().openSession();
        Transaction transaction =  session.beginTransaction();
        session.flush();
        ClientIpEntity clientIpEntity = session.get(ClientIpEntity.class, id);
        session.getTransaction().commit();
        session.close();
        if (Objects.isNull(clientIpEntity)) return Optional.empty();
        else  return Optional.of(clientIpEntity);
    }

    public static Optional<ClientIpEntity> getbyIp(String ip){
        Session session = HibernateUtil.getSessionFactory().openSession();
        Transaction transaction =  session.beginTransaction();
        session.flush();
        String hql = "SELECT e FROM ClientIp e WHERE e.ip = :valueToSearch ";
        Query query = session.createQuery(hql);
        query.setParameter("valueToSearch", ip);
        List<ClientIpEntity> result =  query.list();
        session.getTransaction().commit();
        session.close();
        if (!result.isEmpty()){
            return Optional.of(result.get(0));
        }
        return Optional.empty();
    }

    public static void deleteById(long id){
        Session session = HibernateUtil.getSessionFactory().openSession();
        Transaction transaction =  session.beginTransaction();
        session.flush();
        ClientIpEntity clientIpEntity = session.get(ClientIpEntity.class, id);
        session.remove(clientIpEntity);
        session.getTransaction().commit();
        session.close();
    }

    public static void update(ClientIpEntity clientIpEntity){
        Session session = HibernateUtil.getSessionFactory().openSession();
        Transaction transaction = session.beginTransaction();
        session.flush();
        session.merge(clientIpEntity);
        session.getTransaction().commit();
        session.close();
    }

}
