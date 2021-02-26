project('lv_lib_png', 'c')

sources = ['lodepng.c', 'lv_png.c']
lvgl = dependency(
    'lvgl',
    fallback : ['lvgl', 'lvgl_dep'],
    static: true,
    default_options: ['default_library=static'],
)

add_project_arguments(
    '-DLV_LVGL_H_INCLUDE_SIMPLE',
    language: 'c'
)
lv_lib_png = library('lv_lib_png', sources, dependencies: lvgl)

if meson.is_subproject()
    lv_lib_png_dep = declare_dependency(include_directories: '.', link_with: lv_lib_png)
endif
