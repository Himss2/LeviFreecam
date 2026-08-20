# Levi Freecam Architecture

## v0.1.0

Only Levi lifecycle, Mod Menu registration and internal state are implemented.
No Minecraft function is hooked yet.

```text
Levi Launcher
   |
   +-- preload-native -> liblevi_freecam.so
                         |
                         +-- FreecamMod lifecycle
                         |     load/enable/disable/unload
                         |
                         +-- ModMenu
                         |     Freecam module
                         |     FC HUD button
                         |
                         +-- FreecamController
                               enabled state only
```

## Planned v0.2+

```text
FreecamController
   |
   +-- GameModeController
   |     save original local game type
   |     apply local spectator state
   |     restore original state
   |
   +-- PacketHook
         intercept outgoing packets
         suppress PlayerAuthInput while Freecam is active
```

The implementation will prefer public `pl::memory` APIs and signatures. Hard-
coded Minecraft offsets are a last resort and, if ever required, must live in a
version-specific profile rather than feature code.

## Safety invariant

Before hooks are removed, Freecam must always run a forced cleanup sequence:

1. disable feature state;
2. restore original local game state (once implemented);
3. stop packet suppression (once implemented);
4. uninstall hooks;
5. unregister UI.
