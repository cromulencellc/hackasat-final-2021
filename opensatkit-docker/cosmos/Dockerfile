FROM ubuntu:18.04 as builder

RUN apt-get update -y && apt-get install -y \
  cmake \
  freeglut3 \
  freeglut3-dev \
  gcc \
  g++ \
  git \
  iproute2 \
  libffi-dev \
  libgdbm-dev \
  libgdbm5 \
  libgstreamer-plugins-base1.0-dev \
  libgstreamer1.0-dev \
  libncurses5-dev \
  libpq-dev \
  libreadline6-dev \
  libsmokeqt4-dev \
  libssl-dev \
  libyaml-dev \
  net-tools \
  postgresql-client \
  qt4-default \
  qt4-dev-tools \
  ruby2.5 \
  ruby2.5-dev \
  vim \
  zlib1g-dev \
  && apt-get clean autoclean \ 
  && rm -rf /var/lib/apt/lists/*

ARG UID=1000
ARG GID=1000

RUN if [ ${UID:-0} -ne 0 ] && [ ${GID:-0} -ne 0 ]; then \
    groupadd -g ${UID} cosmos &&\
    useradd -l -m -u ${UID} -g cosmos cosmos; \
    fi

ENV COSMOS_VERSION 4.5.1
RUN gem install rake --no-document


# ENV BUNDLE_PATH /home/cosmos/.gems
# WORKDIR /home/cosmos
# COPY Gemfile Gemfile
# RUN bundle install
# RUN gem install json --no-document
RUN gem install cosmos -v ${COSMOS_VERSION} --no-document
RUN gem install pg
RUN gem install mqtt

from ubuntu:18.04 as runner

RUN apt-get update -y && apt-get install -y \
  freeglut3 \
  iproute2 \
  libffi6 \
  libgdbm5 \
  libgstreamer-plugins-base1.0-0 \
  libgstreamer1.0-0\
  libncurses5 \
  libreadline-dev \
  libsmokeqtgui4-3 \
  libyaml-0-2 \
  libssl1.1 \
  net-tools \
  postgresql-client \
  qt4-default \
  ruby2.5 \
  vim \
  zlib1g \
  && apt-get clean autoclean \ 
  && rm -rf /var/lib/apt/lists/*


COPY --from=builder /var/lib/gems /var/lib/gems
COPY --from=builder /usr/local/bin/cosmos /usr/local/bin/cosmos

ARG UID=1000
ARG GID=1000

RUN if [ ${UID:-0} -ne 0 ] && [ ${GID:-0} -ne 0 ]; then \
    groupadd -g ${UID} cosmos &&\
    useradd -l -m -u ${UID} -g cosmos cosmos; \
    fi

USER cosmos
WORKDIR /cosmos
ENTRYPOINT [ "./start.sh"]
# ENTRYPOINT [ "/bin/bash", "-i", "-c" ]
