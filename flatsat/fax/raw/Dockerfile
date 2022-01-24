FROM python:slim-buster

RUN pip3 install ballcosmos

ARG UID=1000
ARG GID=1000

ENV SERVER_PORT=1337
ENV COSMOS_CTS_HOSTNAME=fax_cosmos-digest_1

WORKDIR /raw
COPY server_raw.py /raw/

RUN groupadd -r --gid=${GID} builder ;\
    useradd -m -r --uid=${UID} -g builder builder ;\
    chown -R builder:builder /raw

USER builder

EXPOSE 1337/tcp

ENTRYPOINT [ "python3", "server_raw.py"]








