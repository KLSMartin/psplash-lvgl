project('psplash-lvgl', 'c', meson_version: '>=0.55.0') # required to allow patches for wrap-git mode.

# UI toolkit
dependencies = [
    dependency(
        'lv_lib_png',
        fallback : ['lv_lib_png', 'lv_lib_png_dep'],
        static: true,
        default_options: ['default_library=static'],
    ),
    dependency(
        'lv_drivers',
        fallback : ['lv_drivers', 'lv_drivers_dep'],
        static: true,
        default_options: ['default_library=static'],
    ),
    dependency(
        'lvgl',
        fallback : ['lvgl', 'lvgl_dep'],
        static: true,
        default_options: ['default_library=static'],
    ),
]

# system integration
systemd = dependency('libsystemd', required: false)
if systemd.found()
    add_project_arguments('-DHAVE_SYSTEMD', language : 'c')
    dependencies += [systemd]
endif

pthreads = dependency('threads')
dependencies += pthreads

add_project_arguments('-D_DEFAULT_SOURCE', language : 'c')

add_project_arguments('-DLV_CONF_INCLUDE_SIMPLE', language : 'c')
add_project_arguments('-DLV_LVGL_H_INCLUDE_SIMPLE', language : 'c')
if get_option('debug')
    add_project_arguments('-DDEBUG=1', language : 'c')
endif

executable(
    'psplash-lvgl',
    dependencies: dependencies,
    sources: 'psplash-lvgl.c',
    install: true
)