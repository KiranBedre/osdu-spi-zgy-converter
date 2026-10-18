#!/bin/bash -e

# Copyright 2021 Schlumberger
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

SCRIPT_DIR=$(dirname "$0")

pip install keyring artifacts-keyring

PIP_INI="$VIRTUAL_ENV/pip.conf"
echo '[global]'> "$PIP_INI"
echo 'extra-index-url=https://pkgs.dev.azure.com/slb1-swt/_packaging/SegyLibraryFeed%40Local/pypi/simple/'>> "$PIP_INI"

pip install segylibrary-python

python "$SCRIPT_DIR/../test.py" "$SCRIPT_DIR/../test.sgy"
