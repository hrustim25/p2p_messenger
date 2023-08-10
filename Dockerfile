
FROM openjdk AS kek
COPY . /app
WORKDIR /app
ENTRYPOINT ["java", "-jar", "target/grpc-example-1.0-SNAPSHOT-jar-with-dependencies.jar"]
