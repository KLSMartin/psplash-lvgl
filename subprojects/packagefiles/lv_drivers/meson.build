project('lv_drivers', 'c',
  version: '7.11.0',
  meson_version : '>=0.58.1',
)

dependencies = []

video_backend = get_option('display_backend')
if video_backend != 'auto'
    message('lv_drivers: using display backend ' + video_backend)
    platform_specific_configuration_file = files('lv_drv_conf_' + video_backend + '.h')
    if video_backend != 'fb'
        dependencies += [dependency('lib' + video_backend, required: true)]
    endif
else
# is_cross_build is inappropriate here. Use drm/libinput on any embedded (i.e. arm or risc) host system
    if host_machine.cpu_family() in ['arm', 'arm64', 'riscv32', 'riscv64']
        platform_specific_configuration_file = files('lv_drv_conf_drm.h')
        dependencies += [dependency('libdrm', required: true)]
    else
        platform_specific_configuration_file = files('lv_drv_conf_sdl2.h')
        dependencies += [dependency('sdl2', required: true)]
    endif
endif

configuration_file = configure_file(
    copy:true,
    input: platform_specific_configuration_file,
    output: 'lv_drv_conf.h',
)

sources = [
    'display/drm.c',
    'display/fbdev.c',
    'display/monitor.c',
    'win_drv.c',
]
add_project_arguments(
    '-DLV_CONF_INCLUDE_SIMPLE',
    '-DLV_LVGL_H_INCLUDE_SIMPLE',
    '-Wno-unused-parameter',
    '-Wno-error=unused-parameter',
    language: 'c'
)
dependencies += dependency(
    'lvgl',
    fallback : ['lvgl', 'lvgl_dep'],
    static: true,
    default_options: ['default_library=static'],
)

inc_dirs = [
    'display',
]
# '.' implies both: the current source dir _and_ the current build dir (to which `configure_file` outputs files).
# The backend header files include lv_drv_conf.h, so we need to provide that.
inc_dirs += include_directories('.')
lv_drivers = library('lv_drivers', sources, dependencies: dependencies)

if meson.is_subproject()
    lv_drivers_dep = declare_dependency(include_directories: inc_dirs, link_with: lv_drivers)
endif
