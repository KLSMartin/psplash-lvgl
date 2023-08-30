project('libconfig', 'c',
  version: '1.7.3',
  meson_version : '>=0.57.2',
)

mod = import('unstable_external_project',
)

p = mod.add_project('configure',
    configure_options : ['--prefix=@PREFIX@',
                       '--disable-shared',
                       '--disable-examples',
                       '--disable-cxx',
                       '--disable-tests',
                      ],
)

libconfig_dep = p.dependency('config')
