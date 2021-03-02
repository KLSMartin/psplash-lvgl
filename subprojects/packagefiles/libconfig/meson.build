project('libconfig', 'c',
  meson_version : '>=0.57.0',
)

mod = import('unstable_external_project')

p = mod.add_project('configure',
  cross_configure_options : [ '--host=@HOST@' ],
)
libconfig_dep = p.dependency('libconfig')
