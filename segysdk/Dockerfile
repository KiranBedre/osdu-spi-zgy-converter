ARG TARGET=segytozgy
ARG UBUNTU_NAME=focal


# OpenZgy Bundle packages available on https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/open-zgy/-/packages
# Based on OpenZGY.focal.sdms-bundle-azure-curl.aws.ibm.anthos.0.2.970.tar.gz (Azure, Google Cloud, AWS and IBM backends)
ARG OPENZGY_BUNDLE_URL="https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/open-zgy/-/package_files/4075436/download"

# The version of gcc used for OpenZGY compilation
ARG GCCVERSION=940
ARG GCCPACKAGE=gcc
ARG OPENZGY_OUTPUT=focal-gcc94

# OS image
FROM ubuntu:${UBUNTU_NAME} AS os

# Runtime dependencies for C++
FROM os AS segysdk-c-dependencies

RUN apt-get update && apt-get upgrade --yes && apt-get install --yes \ 
    libstdc++6 libgomp1 libcurl4 libcpprest libjsoncpp-dev

# C++ build
FROM os AS prepare-segysdk-c-build

ARG UBUNTU_NAME
ARG GCCPACKAGE

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get upgrade --yes && apt-get install --yes ca-certificates && apt-get install --yes --no-install-recommends \ 
    autoconf binutils cmake curl file ${GCCPACKAGE} g++ git libtool make libc6-dev ninja-build tar unzip wget dos2unix libcurl4-openssl-dev libssl-dev doxygen libcpprest-dev uuid-dev libjsoncpp-dev


# Build segysdk with prebuilt OpenZGY bundle
FROM prepare-segysdk-c-build AS segysdk-c-build

ARG GCCVERSION
ARG GCCPACKAGE
ARG OPENZGY_OUTPUT
ARG OPENZGY_BUNDLE_URL

ENV DEBIAN_FRONTEND=noninteractive
LABEL description="Build container - segytozgy"

#Get openzgy bundle 
RUN mkdir -p /tmp/openzgy-bundle
WORKDIR /tmp/openzgy-bundle
ADD ${OPENZGY_BUNDLE_URL} /tmp/openzgy-bundle/openzgy.bundle.tar.gz
RUN tar zxvf openzgy.bundle.tar.gz
WORKDIR /tmp/openzgy-bundle/sdapi/Lin64_${GCCPACKAGE}${GCCVERSION}
RUN tar zxvf sdapi_linux64.tar.gz

#Set build area
COPY ./segysdk/src /src/segysdk/src

RUN mkdir -p /src/segysdk/src/out
RUN mkdir -p /src/segysdk/src/include/openzgy
RUN cp -r /tmp/openzgy-bundle/native/include /src/segysdk/src
RUN mkdir -p /src/segysdk/src/include/SDAPI
RUN cp /tmp/openzgy-bundle/sdapi/Lin64_${GCCPACKAGE}${GCCVERSION}/include/*.* /src/segysdk/src/include/SDAPI
RUN mkdir -p /src/segysdk/src/lib
RUN cp -r /tmp/openzgy-bundle/native/${OPENZGY_OUTPUT}/*.* /src/segysdk/src/lib
WORKDIR /src/segysdk/src/out

# Build
RUN mkdir -p /src/segysdk/src/out
WORKDIR /src/segysdk/src/out
RUN cmake -DCMAKE_BUILD_TYPE=Release -GNinja ..
RUN ninja

# Copying the binaries to the Release folder
WORKDIR /src/segysdk/src/out/bin/Release
RUN cp /src/segysdk/src/lib/*.so ./ &&\
     mv libopenzgy.so libopenzgy.so.0.2 && \
     mv libzfp.so libzfp.so.0 && \
     cp /src/segysdk/src/lib/libsdapi.so.3 ./


# C++ test runner
FROM segysdk-c-build AS segysdk-c-test

COPY ./segysdk/testdata /src/segysdk/testdata

WORKDIR /src/segysdk/src/out

ARG AppName
ARG ConsumerKey

ENV CTEST_OUTPUT_ON_FAILURE=1
ENV SEGY_LOCALTESTDATA_PATH=/src/segysdk/testdata
ENV SD_SVC_TOKEN='Dummy-token value'
ENV SD_SVC_URL='https://www.dummy.com/seistore-svc/api/v3'
RUN ninja test

# Runtime dependencies for Python (superset of C++ runtime)
FROM segysdk-c-dependencies AS segysdk-python-dependencies
RUN apt-get install --yes --no-install-recommends python3 python3-pytest python3-numpy python3-distro

# Python test runner
FROM segysdk-python-dependencies AS segysdk-python-test

LABEL description="Run container - python wrapper test"

COPY ./segysdk/testdata /work/testdata
COPY ./segysdk/python-wrapper /work/python-wrapper-test

COPY --from=segysdk-c-build /src/segysdk/src/out/bin/Release /work/python-wrapper-test/src/segysdk/dll/ubuntu/20.04

WORKDIR /usr/local/bin/segy
ENV SD_SVC_TOKEN='Dummy-token value'
ENV SD_SVC_URL='https://www.dummy.com/seistore-svc/api/v3'
RUN python3 /work/python-wrapper-test/tests/test-console.py /work/testdata/tiny.sgy > /dev/null

WORKDIR /work/python-wrapper-test/tests/
ENV SD_SVC_TOKEN='Dummy-token value'
ENV SD_SVC_URL='https://www.dummy.com/seistore-svc/api/v3'
RUN pytest-3

# SEG-Y to ZGY converter image
FROM segysdk-c-dependencies AS segytozgy

LABEL description="Run container - segytozgy"

RUN mkdir /usr/local/segytozgy

COPY --from=segysdk-c-build /src/segysdk/src/out/bin/Release/ /usr/local/bin/segy/

ENV LD_LIBRARY_PATH /usr/local/bin/segy:$LD_LIBRARY_PATH
ENTRYPOINT ["/usr/local/bin/segy/SegyToZgy"]

# Node.js wrapper
FROM segysdk-c-build AS node-js-wrapper

WORKDIR /src/segysdk/src/out
ENV NODE_VERSION 14.17.4

RUN curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.34.0/install.sh | bash
ENV NVM_DIR=/root/.nvm
RUN . "$NVM_DIR/nvm.sh" && nvm install ${NODE_VERSION}
RUN . "$NVM_DIR/nvm.sh" && nvm use v${NODE_VERSION}
RUN . "$NVM_DIR/nvm.sh" && nvm alias default v${NODE_VERSION}
ENV PATH="/root/.nvm/versions/node/v${NODE_VERSION}/bin/:${PATH}"

COPY ./segysdk/node-wrapper /src/segysdk/node-wrapper
WORKDIR /src/segysdk/node-wrapper
COPY ./segysdk/testdata /work/testdata

RUN npm install
RUN npm run rebuild

ENV SD_SVC_TOKEN='Dummy-token value'
ENV SD_SVC_URL='https://www.dummy.com/seistore-svc/api/v3'
RUN npm run test

# Switch to final image (the one to be tagged)
FROM ${TARGET}
