
FROM python:slim-buster

ENV VIRTUAL_ENV=/opt/venv

RUN python3 -m venv ${VIRTUAL_ENV}

ENV PATH="${VIRTUAL_ENV}/bin:${PATH}"

RUN pip install --upgrade pip --no-cache

COPY requirements.txt .

RUN pip install -r requirements.txt --no-cache

COPY *.py /

COPY .env .

ENTRYPOINT [ "python", "bridge.py" ]