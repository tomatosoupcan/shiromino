# Shiromino - Playstation Vita port
All known features are working perfectly. Needs some aesthetic refreshes and code cleanup.

## Building
Requires a [Vita SDK](https://vitasdk.org) environment. Inside, install `SDL2`, `SDL2_image` and `SDL2_mixer`. `debugnet` is currently required for debugging, but will be made an optional dependency. Additionally requires [VitaSmith/libsqlite](https://github.com/VitaSmith/libsqlite), install it into the SDK directory overwriting existing (crippled) version of sqlite.

To build, run `cmake CMakeLists.txt` and then `make`.

## Known issues
 * Debugging permanently enabled, hardcoded IP and port
 * Stretched backgrounds
 * Bad usage of screen estate
 * PC specific dead code
 * Pointless config

## Development notes
 * SDL-Vita requires texture dimensions that divide by 8
 * SDL-Vita doesn't support render-to-texture
 * All paths need to be absolute, `chdir()` does nothing
 * `ux0:data` is a good location for writeable files
 * `ux0:app/TEAR0001` is bind mounted at `app0:` (might be read-only?)