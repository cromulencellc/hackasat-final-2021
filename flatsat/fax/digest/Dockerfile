FROM ubuntu:20.04

RUN DEBIAN_FRONTEND="noninteractive" apt-get update && \
    DEBIAN_FRONTEND="noninteractive" apt-get install --no-install-recommends -y \
    python3-pip iputils-ping netcat

RUN pip3 install ballcosmos

ARG UID=1000
ARG GID=1000

ENV SERVER_PORT=1340

WORKDIR /digest
COPY server_digest.py digest.py /digest/

RUN groupadd -r --gid=${GID} builder ;\
    useradd -m -r --uid=${UID} -g builder builder ;\
    chown -R builder:builder /digest

USER builder

EXPOSE 1341/tcp
EXPOSE 1361/tcp

ENTRYPOINT [ "python3", "server_digest.py"]
