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

VENV_DIR="$(dirname "$0")/venv"
echo $VENV_DIR will be deleted after key press.
read -rsn1
if [ -d "$VENV_DIR" ] ; then rm -rf "$VENV_DIR" ; fi
python3 -m venv "$VENV_DIR"
pushd "$VENV_DIR"
echo Please run this command: source \"$VENV_DIR/bin/activate\"
