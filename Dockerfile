
FROM amazoncorretto:11-al2-jdk AS kek
COPY . /app
WORKDIR /app
ENTRYPOINT ["java", "-jar", "target/grpc-example-1.0-SNAPSHOT.jar"]
