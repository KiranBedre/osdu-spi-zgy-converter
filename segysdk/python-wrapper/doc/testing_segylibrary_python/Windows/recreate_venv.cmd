@echo off
set VENV_DIR=%~dp0venv
echo %VENV_DIR% will be deleted after key press.
pause
rmdir /s /q %VENV_DIR%
python -m venv %VENV_DIR%
pushd %VENV_DIR%
cmd /k scripts\activate