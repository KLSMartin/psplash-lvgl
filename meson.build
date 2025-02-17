project('psplash-lvgl',
    'c',
    version: '1.3.9999',
    meson_version: '>=0.55.0', # required to allow patches for wrap-git mode.
    default_options: [
        'warning_level=3',
        'werror=true',
    ],
)

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
    dependency(
        'libconfig',
        fallback : ['libconfig', 'libconfig_dep'],
        static: true,
        default_options: ['default_library=static'],
    ),
]

sources = [
    'psplash-lvgl.c',
    'config.c',
]

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
    sources: sources,
    install: true
)
configure_file(
    input : 'data/config.ini',
    output : 'config.ini',
    copy: true,
    install: true,
    install_dir: get_option('datadir') / meson.project_name(),
    install_mode: 'r--r--r--',
)
install_data (
    'data/logo.png',
    install_mode: 'r--r--r--',
)
