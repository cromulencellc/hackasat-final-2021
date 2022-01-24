FROM python:alpine3.9

ARG UID=1000
ARG GID=1000

RUN addgroup -g ${UID} payload && \
    adduser -u ${GID} --shell /bin/bash -G payload -D payload

USER payload

WORKDIR /payload
COPY src /payload

ENV VIRTUAL_ENV="/payload/venv"
RUN python -m venv ${VIRTUAL_ENV}
ENV PATH="${VIRTUAL_ENV}/bin:${PATH}"
ENV TEAM_NUMBER="${TEAM_NUMBER}"

RUN pip install -r /payload/requirements.txt

EXPOSE 2303/udp

ENTRYPOINT [ "python3", "comm_payload_dispatcher.py"]
