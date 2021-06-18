# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
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
