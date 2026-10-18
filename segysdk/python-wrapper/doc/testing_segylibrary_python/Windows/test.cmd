pip install keyring artifacts-keyring

set PIP_INI=%VIRTUAL_ENV%\pip.ini
echo [global]> %PIP_INI%
echo extra-index-url=https://pkgs.dev.azure.com/slb1-swt/_packaging/SegyLibraryFeed%%40Local/pypi/simple/>> %PIP_INI%

pip install segylibrary-python

python %~dp0..\test.py %~dp0..\test.sgy
