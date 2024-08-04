@echo off
%~dp0track_.exe %*
for %%i in (..\png\*.png) do (
	pngcheck "%%i" 1>nul 2>&1 || dos2unix -k -f "%%i" 1>nul 2>&1
)