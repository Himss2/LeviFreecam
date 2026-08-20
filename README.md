# Levi Freecam

A native Freecam project for Levi Launcher Android.

This repository starts from the official Levi native lifecycle and Mod Menu
patterns. The first milestone intentionally **does not hook Minecraft yet**.
It exists to prove that the package builds, loads, registers correctly, and can
be enabled/disabled safely before the gameplay implementation is introduced.

## Current milestone: v0.1.0

Implemented:

- `preload-native` manifest
- `PL_REGISTER_MOD` C++ lifecycle
- Levi native logger
- Freecam module in Levi Mod Menu
- native `FC` HUD button using `ButtonBuilder`
- one internal Freecam state source
- deterministic disable/unload cleanup
- ARM64 build
- 16 KiB ELF page-size linker setting
- GitHub Actions build and `.levipack` packaging

Not implemented yet:

- Minecraft packet hooks
- `PlayerAuthInput` suppression
- local spectator injection/spoof
- game-mode save/restore
- version signatures

So v0.1.0 is expected to show the Freecam UI and logs, but **must not move the
camera yet**.

## Repository layout

```text
LeviFreecam/
├── CMakeLists.txt
├── manifest.json
├── .github/workflows/build.yml
├── scripts/
│   ├── build.ps1
│   └── build.sh
├── docs/
│   └── ARCHITECTURE.md
└── src/
    ├── FreecamMod.cpp
    ├── FreecamMod.hpp
    ├── core/
    │   ├── FreecamController.cpp
    │   └── FreecamController.hpp
    ├── ui/
    │   ├── ModMenu.cpp
    │   └── ModMenu.hpp
    ├── hooks/README.md
    ├── game/README.md
    └── memory/README.md
```

## Dependencies

- Android NDK 28.2.13676358
- CMake 3.22+
- Ninja
- `preloader-android` 0.2.2 (fetched automatically by CMake)

Only the public Levi/preloader SDK is used.

## Local build (Linux/macOS)

Set `ANDROID_HOME` or `ANDROID_NDK_HOME`, then:

```bash
./scripts/build.sh
```

Output:

```text
dist/arm64-v8a/levi-freecam.levipack
```

## Local build (Windows PowerShell)

```powershell
./scripts/build.ps1 -Clean
```

## GitHub Actions

Push the repository to GitHub. The workflow builds `arm64-v8a` and uploads:

```text
levi-freecam.levipack
```

A tag such as `v0.1.0` also publishes the package as a release asset.

## Expected v0.1.0 test

1. Import `levi-freecam.levipack` into Levi Launcher.
2. Launch Minecraft.
3. Confirm `Freecam` appears in Mod Menu.
4. Confirm the `FC` HUD button is available.
5. Toggle from Mod Menu or press the button.
6. Verify Minecraft does not crash.
7. Verify logs report `Freecam state = ON/OFF`.
8. Camera movement should remain vanilla at this milestone.

## Next milestone

v0.2.0 will add only the runtime/signature/hook foundation. Packet cancellation
and spectator spoof will remain disabled until their targets are positively
validated for the selected Minecraft version.
