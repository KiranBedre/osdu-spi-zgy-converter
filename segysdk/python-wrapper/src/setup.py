# Copyright 2022 Schlumberger
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

import os
import setuptools

import segysdk.segydll

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

build_id = os.getenv('BUILD_BUILDID')
if (build_id is None): raise EnvironmentError('BUILD_BUILDID environment variable is not set')
dlls = [f'dll/{dll}' for dll in
        (segysdk.segydll.platform_dependencies() + [segysdk.segydll.platform_main_library()] + segysdk.segydll.platform_modules())]

setuptools.setup(
    name="segysdk-python",
    version=f"0.0.{build_id}",
    author="Schlumberger",
    author_email="TODO@slb.com",
    description="SEG-Y SDK",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://slb.com/TODO",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3 :: Only",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux",
    ],
    install_requires=[
        'numpy>=1.19.4',
        'distro>=1.5.0',
    ],
    python_requires='>=3.6',
    package_data={
        "": dlls,
    },
    include_package_data=True,
)
