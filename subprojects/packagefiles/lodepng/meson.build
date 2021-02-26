project('liblodepng', 'c', version: '99999')

c_source_file = configure_file(copy: true, input: 'lodepng.cpp', output: '@BASENAME@.c')

liblodepng = library('lodepng', c_source_file)
lodepng_dep = declare_dependency(include_directories: '.', link_with: liblodepng)
