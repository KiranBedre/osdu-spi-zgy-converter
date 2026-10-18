# Copyright 2021 Schlumberger
# Schlumberger Private

param (
	[switch]$cleanup,	
	[switch]$continue = $true
 )

[string]$Project = 'SegySDKProject'
[string]$ProjectArea = $PSScriptRoot
[string]$vcpkghome = $env:VCPKG_HOME

[string]$outFolder = 'out/'
[string]$buildArea = $ProjectArea + '/' + $outFolder

if ($null -eq $env:VCPKG_HOME) { $env:VCPKG_HOME='C:\vcpkg' }

cls;

if (-not ($vcpkghome)){
	$vcpkghome = "C:\vcpkg"
}

if ($cleanup) {
  "Cleanup of build area $buildArea requested ... "
  # delete build folder

  if ( Test-Path -LiteralPath $buildArea -PathType Container ){
	# delete build folder

	Remove-Item -LiteralPath $buildArea -Force -Recurse

  }

  "Continue = $continue"
  if ( -not ($continue)){  exit 1 }
}

if ( -not (Test-Path -LiteralPath $buildArea -PathType Container )) 
{
	
	"Setting up CMake:";
    "   - Project: $Project";
	"   - Project area: $ProjectArea";
	"   - Build area: $buildArea";
	" ";
	"Continue: $continue ";
	
     mkdir $buildArea;
     cd $buildArea;
 #    cmake ..\src -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="$vcpkghome/scripts/buildsystems/vcpkg.cmake";
     cmake ..\  -DCMAKE_TOOLCHAIN_FILE="$vcpkghome/scripts/buildsystems/vcpkg.cmake";
 } 
else {
 
	"   - Project: $Project";
	"   - Project area: $ProjectArea";
	"   - Build area: $buildArea";
	" ";
	"Continue: $continue ";
	
 }

 cd $ProjectArea

 if ($continue) 
{ 
        # in $ProjectArea
	cmake --build  $outFolder --config Debug
}
