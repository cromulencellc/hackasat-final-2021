FROM ubuntu:20.04

ARG UID=1000
ARG GID=1000

RUN groupadd -r --gid=1000 comm && \
    useradd -m -r -s /bin/bash -g comm --uid=1000 comm

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y --no-install-recommends \
    socat iputils-ping netcat

USER comm

WORKDIR /comm
COPY build /comm
COPY start.sh /comm

ENTRYPOINT [ "/bin/bash", "/comm/start.sh"]
# Old entrypoint: socat", "-v", "udp-l:4531,fork", "exec:/comm/comm