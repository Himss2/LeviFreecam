param(
    [string]$BuildType = "Release",
    [string]$Ndk = "",
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

$Root = Resolve-Path (Join-Path $PSScriptRoot "..")
$Abi = "arm64-v8a"
$NdkVersion = "28.2.13676358"

function Resolve-NdkPath {
    param([string]$ExplicitNdk)

    $Candidates = @()
    if (-not [string]::IsNullOrWhiteSpace($ExplicitNdk)) { $Candidates += $ExplicitNdk }
    if (-not [string]::IsNullOrWhiteSpace($env:ANDROID_NDK_HOME)) { $Candidates += $env:ANDROID_NDK_HOME }
    if (-not [string]::IsNullOrWhiteSpace($env:ANDROID_NDK_ROOT)) { $Candidates += $env:ANDROID_NDK_ROOT }

    foreach ($SdkRoot in @($env:ANDROID_HOME, $env:ANDROID_SDK_ROOT)) {
        if ([string]::IsNullOrWhiteSpace($SdkRoot)) { continue }
        $Pinned = Join-Path $SdkRoot "ndk\$NdkVersion"
        if (Test-Path $Pinned) { $Candidates += $Pinned }
    }

    foreach ($Candidate in ($Candidates | Select-Object -Unique)) {
        if ([string]::IsNullOrWhiteSpace($Candidate)) { continue }
        $Toolchain = Join-Path $Candidate "build\cmake\android.toolchain.cmake"
        if (Test-Path $Toolchain) { return (Resolve-Path $Candidate).Path }
    }

    throw "Android NDK $NdkVersion not found. Pass -Ndk or set ANDROID_HOME/ANDROID_NDK_HOME."
}

$Ndk = Resolve-NdkPath $Ndk
$Toolchain = Join-Path $Ndk "build\cmake\android.toolchain.cmake"
$BuildDir = Join-Path $Root "build\android-$Abi-$BuildType"
$DistDir = Join-Path $Root "dist\$Abi"
$PackageDir = Join-Path $DistDir "levi-freecam"

if ($Clean -and (Test-Path $BuildDir)) {
    Remove-Item $BuildDir -Recurse -Force
}

cmake -S $Root -B $BuildDir -G Ninja `
    "-DCMAKE_TOOLCHAIN_FILE=$Toolchain" `
    "-DANDROID_ABI=$Abi" `
    "-DANDROID_PLATFORM=android-24" `
    "-DANDROID_STL=c++_shared" `
    "-DCMAKE_BUILD_TYPE=$BuildType"
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed" }

cmake --build $BuildDir --target levi_freecam
if ($LASTEXITCODE -ne 0) { throw "CMake build failed" }

if (Test-Path $DistDir) { Remove-Item $DistDir -Recurse -Force }
New-Item -ItemType Directory -Force -Path $PackageDir | Out-Null

$Library = Join-Path $BuildDir "out\$Abi\liblevi_freecam.so"
if (-not (Test-Path $Library)) { throw "Built library not found: $Library" }

Copy-Item (Join-Path $Root "manifest.json") (Join-Path $PackageDir "manifest.json")
Copy-Item $Library (Join-Path $PackageDir "liblevi_freecam.so")

$Zip = Join-Path $DistDir "levi-freecam.zip"
$LeviPack = Join-Path $DistDir "levi-freecam.levipack"
Compress-Archive -Path (Join-Path $PackageDir "*") -DestinationPath $Zip -Force
if (Test-Path $LeviPack) { Remove-Item $LeviPack -Force }
Move-Item $Zip $LeviPack -Force

Write-Host "Built: $LeviPack"
