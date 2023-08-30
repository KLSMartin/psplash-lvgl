# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]


## [1.2.0] - 2023-08-30
### Added
 - build: enable warnings/werror, fix several warnings and ignore some -Werrors on subprojects
 - main: allow setting config file path as 1. argument.

### Changed
 - lvgl/lv_drivers: update to 7.11.0 (wrap)
 - lv_lib_png: update to latest v7 branch revision
 - libconfig: update to 1.7.3 (wrap), fix local wrap build
 - deps: remove libsystemd dependency, add implementation of sd_notify("READY=1")
 - optimization: lvgl/lv_drivers: remove unused features and the c-files from build definitions

### Fixed
 - lv_drivers: fbdev: ignore unblanking failure. Necessary for RaspberryPi early fb driver.
 - libconfig: fix local wrap build
 - main: fix potential race in init, not showing initial screen before exit.


## [1.1.2] - 2022-06-28
### Fixed
 - subprojects:lv_lib_png: pin down specific revision.


## [1.1.1] - 2021-08-10
### Fixed
 - subprojects:lv_drivers: disp/fb: fix initilization routine to make sure the legacy fb device is actually putting out graphics.


## [1.1.0] - 2021-08-04
### Added
 - subprojects:lv_drivers: disp/drm: execution time device configuration. Allow users to specify the DRM device to use via the environment variable `DRM_DEVICE`.
 - config.ini: properties to configure the progress bar's background's:
   * border_width
   * color
   * padding (the distance between the background's and the indicator's rectangle).
 - config.ini: properties to configure the progress bar's indicator's:
   * border_width
   * color

## Changed
 - set the progress idicator's (bar) border size to 0.

### Fixed
 - lv_drivers: fix variables' types, which depend on the configured display backend.
 - accidental initial progress indicator value of 20%.


## [1.0.0] - 2021-06-18
### Added
 - install runtime configuration files.
 - subprojects:lv_drivers: display driver `framebuffer` support.

### Changed
 - subprojects:lvgl: color depth configuration via meson option
 - subprojects:lv_drivers: display driver configuration via meson option

### Fixed
 - handling of color depth < 32
 - lv_lib_png: pin lvgl v7 compatible release branch

### Removed
 - lv_drivers: libinput support
