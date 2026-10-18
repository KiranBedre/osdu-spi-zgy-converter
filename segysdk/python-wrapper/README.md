# Current status

Work in progress.
- SegySession C API is exported - see `test-console.py` on how to use
    - the exported JSON APIs are nice for testing the basic integration but are horribly ineffective for real development
- bulk reader API is incomplete

# Testing

`test-console.py` or the `Dockerfile` can be used as a primitive smoke test.
    ```
    python test-console.py input_file.sgy(only local files)
    ```
    or
    ```
    cd ../..
    docker build --rm -f .\SegyLibrary\python-wrapper\DockerFile .
    ```
    If the Docker build succeeds then the libraries could be loaded and the called function signatures are correct.

# How to use

## Windows

Add the directory containing the compiled DLLs to `PATH`

Example:
```
set PATH=d:\petrel\segytozgy_build\SegyLibrary\src\out\build\x64-Debug (default)\bin;%PATH%
python my_python_script.py args...
```

## Linux

Add the directory containing the compiled .so files to `LD_LIBRARY_PATH`

Example:
```
LD_LIBRARY_PATH=/home/user/SegyLibrary/src/out/bin/ python3 my_python_script.py args...
```

## Linux (Docker)

Prerequisite: the Docker image for the SEG-Y to ZGY converter is built with tag `segytozgy` - build process is documented in the SegyToZgy README.
