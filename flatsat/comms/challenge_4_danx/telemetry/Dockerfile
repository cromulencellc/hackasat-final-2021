FROM python:slim-buster

RUN pip3 install paho-mqtt

ARG UID=1000
ARG GID=1000

WORKDIR /telem
COPY send_payload_telem.py /telem

RUN groupadd -r --gid=${GID} telem ;\
    useradd -m -r --uid=${UID} -g telem telem ;\
    chown -R telem:telem /telem

USER telem

ENTRYPOINT [ "python3", "-u", "send_payload_telem.py"]