# PSplash Compatible LGVL Boot Splash Screen

A boot splash, that uses lvgl to render the boot progress based on an interface, compatible with psplash.

## Execution Time Configuration
The look of the application can be confiured at execution time via a config file (i.e. `config.ini`).
Configurable aspects are:
- background (png) image
- progress bar
  * colors
  * layout (size, offset)

The configuration file can be given as the first parameter to the application.

An example configuration can be found under the `data` directory.


## PSplash Compatibility
This Splash screen uses the same FIFO (i.e. `$PSPLASH_FIFO_DIR/psplash_fifo`), as psplash.
It furthermore supports the same command-set (it currently ignores `MSG` though).

## Systemd Compatibility (optional)
The application might be invoked as a service of `Type=notify`, as it will notify systemd about its ready state.

As this application is meant to be used in combination with `psplash-systemd` (part of `https://git.yoctoproject.org/psplash`).
A suitable service might look like the following:
```ini
[Unit]
Description=Show Boot Screen
DefaultDependencies=no
# psplash creates its FIFO in the directory specified
# by environment variable `PSPLASH_FIFO_DIR`, `/run/` by default.
RequiresMountsFor=/run

[Service]
Type=notify
# the configuration file (config.ini) is read from CWD.
WorkingDirectory=/usr/share/psplash-lvgl
ExecStart=/usr/bin/psplash-lvgl
ExecStartPost=/usr/bin/psplash-systemd

[Install]
WantedBy=sysinit.target
```

# Incompatibility
This bootsplash (currently) ignores:
 - screen rotation
 - screen clearance
 - `MSG` commands


# Simulator builds

It is possible to build the program for desktop. It requires the `SDL2` library.

To build:
```bash
meson setup build && ninja -C build
```

For testing, customize the FIFO and `config.ini` locations:
```sh
# prepare run dir
mkdir run

# fifo in run subdir
PSPLASH_FIFO_DIR=$(pwd)/run/psplash_fifo ./build/psplash-lvgl ./data/config.ini &

# send messages to fifo
echo -e 'PROGRESS 20\0' > run/psplash_fifo

echo -e 'QUIT\0' > run/psplash_fifo
```
