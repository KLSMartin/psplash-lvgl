# PSplash Compatible LGVL Boot Splash Screen

A boot splash, that uses lvgl to render the boot progress based on an interface, compatible with psplash.

## Execution Time Configuration
The look of the application can be confiured at execution time via a config file (i.e. `config.ini`).
Configurable aspects are:
- background (png) image
- progress bar
  * colors
  * layout (size, offset)

## PSplash Compatibility
This Splash screen uses the same FIFO (i.e. `$PSPLASH_FIFO_DIR/psplash_fifo`), as psplash.
It furthermore supports the same command-set (it currently ignores `MSG` though).

## Systemd Compatibility (optional)
The application might be invoked as a service of `Type=notify`, as it will notify systemd about its ready state.

As this application is meant to be used in combination with `psplash-systemd.service`.
A suitable drop-in (`/etc/systemd/system/psplash.service.d/lvgl.conf`) might look like:
```ini
[Service]
Type=
Type=notify
ExecStartPre=
ExecStart=
ExecStartPost=
ExecStart=/usr/bin/psplash-lvgl
# The following line might be unnecessary
ExecStartPost=/usr/bin/psplash-systemd
```

# Incompatibility
This bootsplash (currently) ignores:
 - screen rotation
 - screen clearance
 - `MSG` commands