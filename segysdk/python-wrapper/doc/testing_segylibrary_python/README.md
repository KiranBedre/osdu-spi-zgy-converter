# Windows

- prerequisite: 64-bit Python 3.6+ is installed and added to path
- unpack the files in an empty folder
- create a virtual environment
    - you can use `recreate_venv.cmd` - this will create a `venv` subfolder and open a new command prompt
- easy mode: run `test.cmd` in the newly opened command prompt
- explanation:
    - `pip install keyring artifacts-keyring` - required for login and to access Azure artifacts
    - create `pip.ini` with Azure package source
        ```
        set PIP_INI=%VIRTUAL_ENV%\pip.ini
        echo [global]> %PIP_INI%
        echo extra-index-url=https://pkgs.dev.azure.com/slb1-swt/_packaging/SegyLibraryFeed%%40Local/pypi/simple/>> %PIP_INI%
        ```
    - install `segylibrary-python`
        - `pip install segylibrary-python` will install the latest version
        - you can install a fixed version by adding a version number: `pip install segylibrary-python==0.0.2933504`
        - on the first run, you will be asked to log in on a Microsoft web page with a code, follow the instructions

    - `python %~dp0..\test.py %~dp0..\test.sgy` runs the test Python script (creates an index for the included SEG-Y file)

# Ubuntu Focal

- prerequisite: 64-bit Python 3.6+ is installed and added to path (`sudo apt-get install python3 python3-venv`)
  prerequisite: runtime dependencies for SegyLibrary: `sudo apt-get install --yes --no-install-recommends libstdc++6 libgomp1 libcurl4 libcpprest`
- prerequisite: .Net Core SDK
    - `wget https://packages.microsoft.com/config/ubuntu/20.04/packages-microsoft-prod.deb`
    - `sudo dpkg -i packages-microsoft-prod.deb`
    - `sudo apt-get update`
    - `sudo apt-get install dotnet-sdk-5.0`
- unpack the files in an empty folder
- create a virtual environment
    - you can use `recreate_venv.sh` - this will create a `venv` subfolder
    - run the venv activation script: `source venv/bin/activate`
- easy mode: run `test.sh` in the venv command prompt
- explanation:
    - `pip install keyring artifacts-keyring` - required for login and to access Azure artifacts
    - create `pip.conf` with Azure package source
        ```
        set PIP_INI=%VIRTUAL_ENV%/pip.conf
        echo [global]> %PIP_INI%
        echo extra-index-url=https://pkgs.dev.azure.com/slb1-swt/_packaging/SegyLibraryFeed%40Local/pypi/simple/>> %PIP_INI%
        ```
    - install `segylibrary-python`
        - `pip install segylibrary-python` will install the latest version
        - you can install a fixed version by adding a version number: `pip install segylibrary-python==0.0.2933504`
        - on the first run, you will be asked to log in on a Microsoft web page with a code, follow the instructions

    - `python "$SCRIPT_DIR/../test.py" "$SCRIPT_DIR/../test.sgy"` runs the test Python script (creates an index for the included SEG-Y file)

# Expected outcome

An index for test.sgy will be (re)created. For clean results, delete the index file before testing.
