$config_type = 'Debug'

Set-Location $PSScriptRoot\..\  # enter the parent folder
if(-not (Test-Path -Path "test\$config_type")){
    New-Item -Path "test\$config_type" -ItemType "directory"
}
Set-Location .\test\$config_type  # enter the build folder

$vcpkg_root = $env:VCPKG_ROOT
$vcpkg_triplet = "x64-linux"

cmake -DCMAKE_TOOLCHAIN_FILE="$vcpkg_root\scripts\buildsystems\vcpkg.cmake" `
    -DVCPKG_TARGET_TRIPLET="$vcpkg_triplet" `
    -DCMAKE_CONFIGURATION_TYPES="$config_type" `
    -DCMAKE_BUILD_TYPE="$config_type" `
    -DCMAKE_TEST_SIGNATURE="linux64_test" `
    ..\..
