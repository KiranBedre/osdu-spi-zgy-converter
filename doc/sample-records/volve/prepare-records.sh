#!/bin/bash

# Settings
DATA_PARTITION_ID=suzafardftest26-opendes
ACL_OWNER=data.default.owners@suzafardftest26-opendes.contoso.com
ACL_VIEWER=data.default.viewers@suzafardftest26-opendes.contoso.com
LEGAL_TAG=suzafardftest26-opendes-Seismic-Legal-Tag-Test999544286722

# Code
set -e

TMP_DIR=$(mktemp -d)

function gen_id() {
    ID=$(tr -dc a-f0-9 < /dev/urandom | head -c 32)
    echo "$DATA_PARTITION_ID:$1:$ID"
}

TMP_FILES=""

function patch_common() {
    IN_FILE="$1/$2.json"
    OUT_FILE="$TMP_DIR/$1_$2.json"
    TMP_FILES="$TMP_FILES $OUT_FILE"
    jq --arg ID "$3" --arg OWNER "$ACL_OWNER" --arg VIEWER "$ACL_VIEWER" --arg LTAG "$LEGAL_TAG"  '. + {"id":$ID, "acl":{"owners": [$OWNER],"viewers": [$VIEWER]}, "legal": {"legaltags":[$LTAG],"otherRelevantDataCountries":["NO"],"status":"compliant"}}' < "$IN_FILE" > "$OUT_FILE"
}

function patch_trace_data() {
    TYPE=SeismicTraceData
    patch_common "$1" "$TYPE" "$TRACE_DATA_ID"

    IN_FILE="$TMP_DIR/$1_$TYPE.json"
    TMP_FILE=$(mktemp)
    jq --arg BIN_GRID_ID "$BIN_GRID_ID" --arg FC_SEGY_ID "$FC_SEGY_ID" '. * {"data":{"Datasets":[$FC_SEGY_ID],"BinGridID":$BIN_GRID_ID}}' --arg "$FC_SEGY_ID" "$3" < "$IN_FILE" > "$TMP_FILE"
    mv -f "$TMP_FILE" "$IN_FILE"
}

function patch_work_product() {
    TYPE=WorkProduct
    patch_common "$1" "$TYPE" "$WORK_PRODUCT_ID"

    IN_FILE="$TMP_DIR/$1_$TYPE.json"
    TMP_FILE=$(mktemp)
    jq --arg BIN_GRID_ID "$BIN_GRID_ID" --arg TRACE_DATA_ID "$TRACE_DATA_ID" '. * {"data":{"Components":[$TRACE_DATA_ID,$BIN_GRID_ID]}}' --arg "$FC_SEGY_ID" "$3" < "$IN_FILE" > "$TMP_FILE"
    mv -f "$TMP_FILE" "$IN_FILE"
}

BIN_GRID_ID=$(gen_id work-product-component--SeismicBinGrid)

patch_common shared SeismicBinGrid "$BIN_GRID_ID"

for TARGET in time depth
do
    FC_SEGY_ID=$(gen_id dataset--FileCollection.SEGY)
    TRACE_DATA_ID=$(gen_id work-product-component--SeismicTraceData)
    WORK_PRODUCT_ID=$(gen_id work-product--WorkProduct)

    patch_common "$TARGET" FileCollection.SEGY "$FC_SEGY_ID"
    patch_trace_data "$TARGET"
    patch_work_product "$TARGET"

    jq -n --arg DATA_PARTITION_ID "$DATA_PARTITION_ID" --arg FC_SEGY_ID "$FC_SEGY_ID" --arg WORK_PRODUCT_ID "$WORK_PRODUCT_ID" '. + {"executionContext": {"data_partition_id": $DATA_PARTITION_ID,"sd_svc_api_key": "no","storage_svc_api_key": "no","filecollection_segy_id": $FC_SEGY_ID,"work_product_id": $WORK_PRODUCT_ID}}'
done

ALL_RECORDS=$(mktemp)
echo '[]' > "$ALL_RECORDS"
for f in $TMP_FILES
do
    NEXT_TMP=$(mktemp)
    cat "$ALL_RECORDS" | jq --argfile F $f '. + [$F]' > "$NEXT_TMP"
    rm "$ALL_RECORDS"
    ALL_RECORDS="$NEXT_TMP"
done

mv "$ALL_RECORDS" all_records.json

rm -rf "$TMP_DIR"
