@echo off
set DOXYGEN_OUTPUT=%~dp0generated\apidoc-python

if exist %DOXYGEN_OUTPUT%\* rmdir /s /q %DOXYGEN_OUTPUT%
mkdir %DOXYGEN_OUTPUT% && pushd %~dp0..\..\python-wrapper\src && (
    type Doxyfile && echo OUTPUT_DIRECTORY = %DOXYGEN_OUTPUT%
) | "c:\Program Files\doxygen\bin\doxygen.exe" - && start %DOXYGEN_OUTPUT%\html\annotated.html
popd