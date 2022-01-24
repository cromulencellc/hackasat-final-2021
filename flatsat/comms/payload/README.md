### Payload

Software for flatsat "payload" aka comm board with CM4 and B200mini.

#### Payload Dispatcher Docker

Initial proof-of-concept for payload dispatcher is run in Docker on the RPI amm64 host. This container can be built on the your development box usign commands below and the Makefile wrapper.

```shell
cd payload/
make build
```

Additionally since arm64 docker containters cannot be built using CI/CD there is a Makefile command to push an updated Docker container to the Gitlab container repository

```shell
cd payload/
make build && make push
```

The payload flowgraphs and and payload-dispatcher Docker image are nominally deployed to the FlatSat COMM payload using the `flatsat.sh` tool in the flatsat repository with the `-c` option.

```shell
./flatsat.sh -t <team> -i <ethernet> -c
```

#### Radio
Contains the flowgraphs deployed on the comm board CM4 which also runs the B200mini SDR. The payload_flatsat_rx flowgraph is provided a team number which it uses to select the appropriate RF channel to receive. The channel is demodulated, unscrambled, and error-corrected before being routed to the rest of the flatsat. The flowgraphs were made with Gnuradio 3.9.

The payload_flatsat_rx flowgraph requires hier flowgraphs under comms/shared/flowgraphs/dependencies.