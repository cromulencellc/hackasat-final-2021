
FROM python:slim-buster as pio-base

RUN apt-get update && apt-get install -y \
       libusb-dev \
       git \
        # wget \
        && apt-get clean autoclean \ 
        && rm -rf /var/lib/apt/lists/*

# RUN wget https://raw.githubusercontent.com/platformio/platformio-core/master/scripts/99-platformio-udev.rules

COPY requirements.txt requirements.txt

ARG UID=1000
ARG GID=1000
ARG USERNAME=has

RUN if [ ${UID:-0} -ne 0 ] && [ ${GID:-0} -ne 0 ]; then \
    groupadd -g ${GID} $USERNAME &&\
    useradd -l -m -u ${UID} -g $USERNAME $USERNAME; \
    fi 

    # && \
    # apt-get install -y sudo \
    # && echo  ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    # && chmod 0440 /etc/sudoers.d/$USERNAME

# RUN mkdir -p /opt/rpi/tools/arm-bcm2708
# WORKDIR /opt/rpi/tools/arm-bcm2708

# RUN curl https://codeload.github.com/raspberrypi/tools/tar.gz/master | tar -xz --strip=2 tools-master/arm-bcm2708

# RUN cp 99-platformio-udev.rules /etc/udev/

RUN mkdir -p /mount
RUN chown -R ${UID}:${GID} /mnt

USER $USERNAME

ENV VIRTUAL_ENV=/home/$USERNAME/pio_venv

RUN python3 -m venv ${VIRTUAL_ENV}

ENV PATH="$VIRTUAL_ENV/bin:$PATH"

RUN pip install --no-cache-dir -r requirements.txt

WORKDIR /mnt

RUN pio settings set check_platformio_interval 365
RUN pio settings set check_libraries_interval 365
RUN pio settings set check_platforms_interval 365

COPY platformio.ini platformio.ini

RUN pio run -t clean

RUN pio system prune -f

RUN rm platformio.ini

ENTRYPOINT [ "pio" ]
