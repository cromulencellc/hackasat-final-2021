FROM ubuntu:18.04

RUN DEBIAN_FRONTEND="noninteractive" apt-get update && \
    DEBIAN_FRONTEND="noninteractive" apt-get  install  -y \
        git \
        build-essential \
        libssl-dev \
        freeglut3-dev \
        cmake \
        && apt-get clean autoclean \ 
        && rm -rf /var/lib/apt/lists/*


RUN git clone https://github.com/eclipse/paho.mqtt.c.git -b v1.3.9 --depth=1 && \
    cd paho.mqtt.c && \
    make && \
    make install

ARG UID=1000
ARG GID=1000
ARG USERNAME=has
ARG BUILDDIR=/home/${USERNAME}/42

RUN if [ ${UID:-0} -ne 0 ] && [ ${GID:-0} -ne 0 ]; then \
    groupadd -g ${GID} $USERNAME &&\
    useradd -l -m -u ${UID} -g $USERNAME $USERNAME; \
    fi

RUN mkdir -p ${BUILDDIR}

RUN chown -R ${USERNAME}:${USERNAME} ${BUILDDIR}

WORKDIR ${BUILDDIR}

USER ${USERNAME}

COPY --chown=${USERNAME}:${USERNAME} ./Source ${BUILDDIR}/Source
COPY --chown=${USERNAME}:${USERNAME} ./Kit ${BUILDDIR}/Kit
COPY --chown=${USERNAME}:${USERNAME} ./Include ${BUILDDIR}/Include

COPY --chown=${USERNAME}:${USERNAME} ./Makefile ${BUILDDIR}/Makefile
COPY --chown=${USERNAME}:${USERNAME} ./CMakeLists.txt ${BUILDDIR}/CMakeLists.txt

RUN make

RUN rm -rf ${BUILDDIR}/build ${BUILDDIR}/Source ${BUILDDIR}/Kit ${BUILDDIR}/Include ${BUILDDIR}/CMakeLists.txt ${BUILDDIR}/Makefile ${BUILDDIR}/FlatSatFswApp


# COPY --chown=${USERNAME}:${USERNAME} ./HAS2 ${BUILDDIR}/HAS2
COPY --chown=${USERNAME}:${USERNAME} ./Model/Cubesat_6U.obj ${BUILDDIR}/Model/
COPY --chown=${USERNAME}:${USERNAME} ./Model/ ${BUILDDIR}/Model/
# COPY --chown=${USERNAME}:${USERNAME} ./Model/NASA_LOGO.ppm ${BUILDDIR}/Model/
# COPY --chown=${USERNAME}:${USERNAME} ./Model/BlairSolarCell.ppm ${BUILDDIR}/Model/
# COPY --chown=${USERNAME}:${USERNAME} ./Model/GlastLogo.ppm ${BUILDDIR}/Model/
# COPY --chown=${USERNAME}:${USERNAME} ./Model/ErosColor.ppm ${BUILDDIR}/Model/
# COPY --chown=${USERNAME}:${USERNAME} ./Model/ErosBump.ppm ${BUILDDIR}/Model/
# COPY --chown=${USERNAME}:${USERNAME} ./World ${BUILDDIR}/World
COPY --chown=${USERNAME}:${USERNAME} ./run_has2.sh ${BUILDDIR}/
COPY --chown=${USERNAME}:${USERNAME} ./has2_env.sh ${BUILDDIR}/

ENTRYPOINT [ "/bin/bash", "./run_has2.sh" ]
