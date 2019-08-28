FROM debian:bullseye-slim

RUN apt-get update \
    && apt-get install -y -q \
        locales \
        git \
        cmake \
        gcc \
        g++

# Snoot is locale aware, we must set this
RUN echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen
RUN locale-gen

WORKDIR /snoot
COPY ./ /snoot/
RUN ["git", "submodule", "update", "--init"]
RUN ["/snoot/build.sh"]
ENTRYPOINT ["/snoot/snoot"]

