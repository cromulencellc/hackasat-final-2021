FROM has2/finals/opensatkit-docker/cfs-builder/rtems:latest

USER root

RUN apt update ;\
    apt install -y default-jre ;\
    ln -s /opt/rtems/5/bin/sparc-rtems5-gcc /opt/rtems/5/bin/sparc-rtems-gcc

ENV PATH=/opt/mkprom2:$PATH
COPY mkprom2 /opt/mkprom2

ENV PATH=/opt/grmon/linux/bin64:$PATH
COPY grmon /opt/grmon

WORKDIR /mnt

ENTRYPOINT [ "/bin/bash", "-c" ]
