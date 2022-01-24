
#######################
# Challenge container
#######################

FROM python:alpine



### User Segment Server Files
RUN addgroup -g 1000 userseg && \
    adduser -u 1000 --shell /bin/bash -G userseg -D userseg

RUN mkdir /ping_controller && chown -R userseg:userseg /ping_controller

USER userseg

WORKDIR /ping_controller

COPY requirements.txt /tmp/

ENV VIRTUAL_ENV="/ping_controller/venv"
RUN python -m venv ${VIRTUAL_ENV}
ENV PATH="${VIRTUAL_ENV}/bin:${PATH}"

RUN pip install -r /tmp/requirements.txt

COPY controller.py \
     /ping_controller/

RUN python -m compileall .



ENTRYPOINT ["python", "/ping_controller/controller.py"]