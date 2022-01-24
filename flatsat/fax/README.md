# HAS2 Digest Interface

The Digest Interface does two things:

* Lets other teams read telemetry from other satellites using cosmos JSON RPC
* Provides a faxing functionality so teams can create custom telemetry digests

To build containers

```shell
pushd digest
make build
popd
pushd raw
make build
popd
```

Containers will be tagged:
 - digest: `has2/finals/fax/digest:latest`
 - raw: `has2/finals/fax/raw:latest`

To run

First, download the cosmos api tar.gz, and place that in the right spot.

```
docker-compose up -d
```

Kill:

```
docker-compose down
```

## Story

* Initially, teams will have access to the raw telemetry server at port 1337, so they can manually get telemetry from other teams.
* When we enable the digest challenge, we will expose this server on a port for each team, which will allow teams to exploit the pickling
* "Management needed a unified interface so operators can't screw it up as much"
* 134{TEAM} is the digest port
* 136{TEAM} is open as well