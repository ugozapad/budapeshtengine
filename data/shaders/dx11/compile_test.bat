echo off
setlocal enableDelayedExpansion 
fxc.exe /T vs_4_0 /E main /nologo /Fo test.vso /Fc test.vslisting test.vs
fxc.exe /T ps_4_0 /E main /nologo /Fo test.pso /Fc test.pslisting test.ps

rem fxc.exe /T vs_4_0 /E main /nologo /Fo test.vso test.vs
rem fxc.exe /T ps_4_0 /E main /nologo /Fo test.pso test.ps
