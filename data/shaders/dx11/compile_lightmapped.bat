echo off
setlocal enableDelayedExpansion 
rem fxc.exe /T vs_4_0 /E main /nologo /Fo lightmapped_generic.vso /Fc lightmapped_generic.vslisting lightmapped_generic.vs
rem fxc.exe /T ps_4_0 /E main /nologo /Fo lightmapped_generic.pso /Fc lightmapped_generic.pslisting lightmapped_generic.ps

fxc.exe /T vs_4_0 /E main /nologo /Fo lightmapped_generic.vso lightmapped_generic.vs
fxc.exe /T ps_4_0 /E main /nologo /Fo lightmapped_generic.pso lightmapped_generic.ps

rem fxc.exe /T vs_4_0 /E main /nologo /DLIGHT_COUNT=1 /Fo lightmapped_light_1.vso lightmapped_generic.vs
rem fxc.exe /T ps_4_0 /E main /nologo /DLIGHT_COUNT=1 /Fo lightmapped_light_1.pso lightmapped_generic.ps
rem 
rem fxc.exe /T vs_4_0 /E main /nologo /DLIGHT_COUNT=2 /Fo lightmapped_light_2.vso lightmapped_generic.vs
rem fxc.exe /T ps_4_0 /E main /nologo /DLIGHT_COUNT=2 /Fo lightmapped_light_2.pso lightmapped_generic.ps
rem 
rem fxc.exe /T vs_4_0 /E main /nologo /DLIGHT_COUNT=3 /Fo lightmapped_light_3.vso lightmapped_generic.vs
rem fxc.exe /T ps_4_0 /E main /nologo /DLIGHT_COUNT=3 /Fo lightmapped_light_3.pso lightmapped_generic.ps
rem 
rem fxc.exe /T vs_4_0 /E main /nologo /DLIGHT_COUNT=4 /Fo lightmapped_light_4.vso lightmapped_generic.vs
rem fxc.exe /T ps_4_0 /E main /nologo /DLIGHT_COUNT=4 /Fo lightmapped_light_4.pso lightmapped_generic.ps

rem echo compile permutations ...
rem 
rem set lightmapped_lights_count=4
rem 
rem for /l %%x in (1, 1, %lightmapped_lights_count%) do ( 
rem 	set lightmapped_shader_define=LIGHT_COUNT+%%x
rem 	echo %lightmapped_shader_define%
rem )
rem echo on