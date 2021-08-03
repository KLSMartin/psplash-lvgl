# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
 - subprojects:lv_drivers: disp/drm: execution time device configuration. Allow users to specify the DRM device to use via the environment variable `DRM_DEVICE`.

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
