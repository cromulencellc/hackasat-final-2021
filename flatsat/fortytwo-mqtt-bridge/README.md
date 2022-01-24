# Forty Two Bridge for MQTT Messages

Python application to bridge data from 42 to CFS-ADCS and CFS-EPS and combined returned data from ADCS (actuator) and EPS (status/commands) back to 42.
CI/CD builds docker image automatically on commit.

## Run
This bridge is nominally run as an Docker container from the flatsat or digitaltwin docker-compose enviroments

## Build Locally
```shell
docker build -t has2/fortytwo-bridge:latest .
```

## Run Locally
```shell
docker run --rm -it has2/fortytwo-bridge:latest
```
