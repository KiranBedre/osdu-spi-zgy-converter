# Copyright 2022 Schlumberger
# Schlumberger Private

param (
        [switch]$solution,
	[switch]$testUtils,
        [switch]$sauth2,
	[switch]$licenseComments,
	[switch]$sauthCSharp,
        [switch]$folder = $true,
        [switch]$useVS2019 = $false,
	[switch]$useVS2022 = $true
 )
 

..\SetEnvironment.ps1

[string]$vspath 

if ($useVS2019) {
	$useVS2022 = $false
	$vspath = 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\'
}

if ($useVS2022) {
	$useVS2019 = $false
	$vspath = 'C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\'
}


[string]$vsexe  = $vspath + '\devenv.exe'


if ($solution) {
	$testUtils = $false
        $sauth2 = $false
	$sauthCSharp = $false
	$folder = $false
        $licenseComments = $false
}

if ($testUtils) {
	$solution = $false
        $sauth2 = $false
	$sauthCSharp = $false
	$folder = $false
        $licenseComments = $false
}

if ($sauth2) {
	$solution = $false
	$testUtils = $false
	$folder = $false
	$sauthCSharp = $false
        $licenseComments = $false
}

if ($licenseComments) {
	$solution = $false
	$testUtils = $false
	$folder = $false
	$sauthCSharp = $false
	$sauth2 = $false
        $licenseComments = $true
}

if ($sauthCSharp) {
	$solution = $false
        $sauth2 = $false
	$testUtils = $false
	$folder = $false
	$licenseComments = $false
}

if ($folder) {
	$testUtils = $false
	$solution = $false
        $sauth2 = $false
	$sauthCSharp = $false
	$licenseComments = $false
}

" solution= 	"  + $solution
" testUtils= 	"  + $testUtils
" SAuth2= 	"  + $sauth2
" sauthCSharp=  "  + $sauthCSharp
" licenseComments=  "  + $licenseComments
" folder= 	"  + $folder

[string]$ProjectArea = $PSScriptRoot
[string]$SegySolution = $ProjectArea + '\out\SegySdkProject.sln'
[string]$TestSolution = $ProjectArea + '\internal\TestUtilities\TestUtilities.sln'
[string]$SAuth2Solution = $ProjectArea + '\internal\SAuth2\BuildArea\SAuth2ClientLibrary.sln'
[string]$sauthCSSolution = $ProjectArea + '\internal\TestUtilities\SAuth2\SAuthV2CSharpSample.sln'
[string]$licenseCommentsCSSolution = $ProjectArea + '\internal\tools\UpdateLicenseComment\UpdateLicenseComment.sln'

$env:SEGY_LOCALTESTDATA_PATH= $PSScriptRoot + '\segysdk\testdata'
$env:SEGY_INETRNAL_TESTDATA_PATH= $PSScriptRoot + '\internal\testdata'
$env:VCPKG_HOME="C:/vcpkg"

pushd $PSScriptRoot

if ($useVS2019) {	
	"Using Visual Studio 2019:"
}

if ($useVS2022) {
	"Using Visual Studio 2022:"
}

if ($folder) {
	$Solution = $false
	$testUtils = $false
	"Executing devenv: " + $vsexe + $ProjectArea
	& $vsexe .
}

if ($solution) {
	$folder = $false
	$testUtils = $false
	"Executing devenv: " + $vsexe + " " + $SegySolution 
	& $vsexe $SegySolution 
}
 
if ($testUtils) {

	$folder = $false
	$solution = $false
	
	"Executing devenv: " + $vsexe + " " + $TestSolution
	& $vsexe $TestSolution
}

if ($sauth2) {

	$folder = $false
	$testUtils = $false
	$solution = $false
	
	"Executing devenv: " + $vsexe + " " + $SAuth2Solution
	& $vsexe $SAuth2Solution
}

if ($sauthCSharp) {

	$folder = $false
	$testUtils = $false
	$solution = $false
	
	"Executing devenv: " + $vsexe + " " + $sauthCSSolution
	& $vsexe $sauthCStSolution
}

if ($licenseComments) {

	$folder = $false
	$testUtils = $false
	$solution = $false
	
	"Executing devenv: " + $vsexe + " " + $licenseCommentsCSSolution
	& $vsexe $licenseCommentsCSSolution
}

popd
