ARG UBUNTU_IMAGE=ubuntu:22.04@sha256:0a9887931b0e38af2e3487ed39e29436b0535c4f3f52e57745baaf79d1898956

FROM ${UBUNTU_IMAGE} AS spi-build

ARG AZURE_SDK_COMMIT=4b913e073e50466e8b2d674e65234e7b66d7db49
ARG AZURE_SDK_SHA256=5b273a6fe3ea7342624d4567c1804fae146808cde92f112155b72676fa0d2d74
ARG OPENZGY_VERSION=0.2.1253
ARG OPENZGY_SHA256=445982e6f3da2aa43c5ab3ce39608facd63f052e2d25d4e97e4d33e82b03d0c9

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install --yes --no-install-recommends \
        build-essential \
        ca-certificates \
        cmake \
        curl \
        libboost-program-options-dev \
        libcpprest-dev \
        libcurl4-openssl-dev \
        libgtest-dev \
        libjsoncpp-dev \
        libpng-dev \
        libssl-dev \
        ninja-build \
        python3 \
        tar \
        uuid-dev \
    && rm -rf /var/lib/apt/lists/*

RUN curl --fail --location --silent --show-error \
      --output /tmp/azure-sdk.tar.gz \
      "https://github.com/Azure/azure-sdk-for-cpp/archive/${AZURE_SDK_COMMIT}.tar.gz" \
    && echo "${AZURE_SDK_SHA256}  /tmp/azure-sdk.tar.gz" | sha256sum --check --strict \
    && mkdir /tmp/azure-sdk \
    && tar --extract --gzip --file /tmp/azure-sdk.tar.gz \
      --directory /tmp/azure-sdk --strip-components=1 \
    && AZURE_SDK_DISABLE_AUTO_VCPKG=ON \
       cmake -S /tmp/azure-sdk/sdk/core/azure-core -B /tmp/azure-core-build \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=ON \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      -DFETCH_CONTENT_QUIET=ON \
    && cmake --build /tmp/azure-core-build --parallel 4 \
    && cmake --install /tmp/azure-core-build \
    && AZURE_SDK_DISABLE_AUTO_VCPKG=ON \
       cmake -S /tmp/azure-sdk/sdk/identity/azure-identity -B /tmp/azure-identity-build \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=ON \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      -DFETCH_CONTENT_QUIET=ON \
    && cmake --build /tmp/azure-identity-build --parallel 4 \
    && cmake --install /tmp/azure-identity-build \
    && rm -rf /tmp/azure-sdk.tar.gz /tmp/azure-sdk \
      /tmp/azure-core-build /tmp/azure-identity-build

RUN curl --fail --location --silent --show-error \
      --output /tmp/openzgy.tar.gz \
      "https://community.opengroup.org/api/v4/projects/183/packages/generic/OpenZGY/${OPENZGY_VERSION}/OpenZGY.jammy.sdms-bundle-azure-curl.${OPENZGY_VERSION}.tar.gz" \
    && echo "${OPENZGY_SHA256}  /tmp/openzgy.tar.gz" | sha256sum --check --strict \
    && mkdir /tmp/openzgy \
    && tar --extract --gzip --file /tmp/openzgy.tar.gz --directory /tmp/openzgy \
    && tar --extract --gzip \
      --file /tmp/openzgy/sdapi/Lin64_gcc1140/sdapi_linux64.tar.gz \
      --directory /tmp/openzgy/sdapi/Lin64_gcc1140 \
    && rm /tmp/openzgy.tar.gz

WORKDIR /src
COPY segysdk/src /src
COPY segysdk/testdata /testdata
COPY .spi/native/fuzz_cli.py /spi/fuzz_cli.py

RUN mkdir -p /src/include/SDAPI /src/lib \
    && cp -a /tmp/openzgy/native/include/openzgy /src/include/ \
    && cp -a /tmp/openzgy/sdapi/Lin64_gcc1140/include/. /src/include/SDAPI/ \
    && cp -a /tmp/openzgy/native/jammy-gcc114/lib*.so* /src/lib/

RUN cmake -S /src -B /build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_AZURE_AUTH_PLUGIN=ON \
      -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=/usr/src/googletest \
    && cmake --build /build --parallel 4 \
      --target SegyToZgy UnitTests.SegyToZgyModules segysdk.auth_plugin.Azure \
    && cp -a /src/lib/lib*.so* /build/bin/Release/

RUN mkdir -p /spi-test /runtime/segy /runtime/azure \
    && SEGY_LOCALTESTDATA_PATH=/testdata \
       SD_SVC_TOKEN=dummy-token \
       SD_SVC_URL=https://example.invalid/seistore-svc/api/v3 \
       LD_LIBRARY_PATH=/build/bin/Release:/usr/local/lib \
       /build/bin/Release/UnitTests.SegyToZgyModules \
         --gtest_output=xml:/spi-test/ctest.xml \
    && test -x /build/bin/Release/SegyToZgy \
    && test -f /build/bin/Release/libsegysdk.auth_plugin.Azure.so \
    && LD_LIBRARY_PATH=/build/bin/Release:/usr/local/lib \
       python3 /spi/fuzz_cli.py --binary /build/bin/Release/SegyToZgy --cases 64 \
    && cp /build/bin/Release/SegyToZgy /runtime/segy/ \
    && cp -a /build/bin/Release/lib*.so* /runtime/segy/ \
    && cp -a /usr/local/lib/libazure*.so* /runtime/azure/ \
    && LD_LIBRARY_PATH=/runtime/segy:/runtime/azure \
       sh -c 'ldd /runtime/segy/SegyToZgy; ldd /runtime/segy/libsegysdk.auth_plugin.Azure.so' \
      | tee /tmp/ldd.txt \
    && ! grep -F "not found" /tmp/ldd.txt

FROM scratch AS spi-test
COPY --from=spi-build /spi-test/ctest.xml /ctest.xml

FROM ${UBUNTU_IMAGE} AS final

ARG build_date
ARG commit_id
ARG release_version

LABEL org.opencontainers.image.created="${build_date}" \
      org.opencontainers.image.revision="${commit_id}" \
      org.opencontainers.image.version="${release_version}" \
      org.opencontainers.image.title="OSDU SEG-Y to ZGY converter" \
      org.opencontainers.image.licenses="Apache-2.0"

ENV ENABLE_AZURE_CURL_BASED=true \
    DISABLE_SDAPI_401_RETRY=true \
    LD_LIBRARY_PATH=/usr/local/bin/segy:/usr/local/lib

RUN apt-get update \
    && apt-get install --yes --no-install-recommends \
        ca-certificates \
        libboost-program-options1.74.0 \
        libcpprest2.10 \
        libcurl4 \
        libgomp1 \
        libjsoncpp25 \
        libpng16-16 \
        libssl3 \
        libstdc++6 \
        libuuid1 \
    && rm -rf /var/lib/apt/lists/* \
    && groupadd --gid 10001 spi \
    && useradd --uid 10001 --gid 10001 --no-create-home \
      --home-dir /nonexistent --shell /usr/sbin/nologin spi \
    && mkdir -p /work \
    && chown 10001:10001 /work

COPY --from=spi-build /runtime/segy/ /usr/local/bin/segy/
COPY --from=spi-build /runtime/azure/ /usr/local/lib/

WORKDIR /work
USER 10001:10001
ENTRYPOINT ["/usr/local/bin/segy/SegyToZgy"]
