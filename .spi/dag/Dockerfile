ARG PYTHON_IMAGE=python:3.12-slim-bookworm@sha256:34386ef0cb081344d7ec1c103ba398e6e9f64e9ab3a1509accc92a4e24a07258

FROM ${PYTHON_IMAGE} AS render

WORKDIR /src
COPY .spi/ci/spi_zgy /src/spi_zgy
COPY airflow/workflow-svc-v2/segy_to_zgy_ingestion_dag.py /src/source.py

RUN PYTHONPATH=/src python -m spi_zgy.render_dag \
      --source /src/source.py \
      --output /out/segy_to_zgy_ingestion_dag.py \
    && python -m compileall -q /out/segy_to_zgy_ingestion_dag.py

FROM scratch

ARG build_date
ARG commit_id
ARG release_version

LABEL org.opencontainers.image.created="${build_date}" \
      org.opencontainers.image.revision="${commit_id}" \
      org.opencontainers.image.version="${release_version}" \
      org.opencontainers.image.title="OSDU SEG-Y to ZGY Airflow DAG layer" \
      org.opencontainers.image.licenses="Apache-2.0"

COPY --from=render /out/ /dags/
