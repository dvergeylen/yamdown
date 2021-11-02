# Reference manual: https://mesonbuild.com/Reference-manual_functions.html#dependency
# Wrap reference manual: https://mesonbuild.com/Wrap-dependency-system-manual.html#provide-section
# Required for cmake projects: 'export PKG_CONFIG_PATH="$PWD/testbuild/meson-uninstalled"'

project('yamdown', 'c', default_options: ['warning_level=3'], version: '1.0.0')

# Imports
cmake = import('cmake')
gnome = import('gnome')

# Dependencies
gtk4_dep = dependency('gtk4', version: '>= 4.4.0', fallback : 'gtk4', default_options: 'buildtype=release')

gtksourceview_dep = dependency('gtksourceview-5.0', version: '>= 5.2.0', fallback: ['gtksourceview-5.0', 'gtksource_dep'])

libsoup3_dep = dependency('libsoup3', version: '>= 3.0.1', fallback: ['libsoup3', 'libsoup_dep'])

# cmark_dep = dependency('libcmark', version: '>= 0.30.1', required: false)
# if not cmark_dep.found()
#   cmark_proj = cmake.subproject('libcmark')
#   cmark_dep = cmark_proj.dependency('cmark')
# endif

md4c_dep = dependency('md4c-html', version: '>= 0.4.8', required: true)
# if not md4c_dep.found()
#   md4c_proj = cmake.subproject('md4c')
#   md4c_dep = md4c_proj.dependency('md4c')
# endif

# opt_var = cmake.subproject_options()
# opt_var.add_cmake_defines({
#   'CMAKE_POLICY_WARNING_CMP0126': 'OFF',
#   'USE_GTK4': 'ON',
#   'PORT': 'GTK',
#   'CMAKE_BUILD_TYPE': 'RelWithDebInfo',
#   'ENABLE_BUBBLEWRAP_SANDBOX': 'OFF',
#   'ENABLE_GAMEPAD': 'OFF',
#   'USE_SOUP2': 'OFF'
# })
# webkit2gtk_proj = cmake.subproject('libwebkit2gtk-5.0', options: opt_var)
# message('CMake targets:\n - ' + '\n - '.join(webkit2gtk_proj.target_list()))
webkit2gtk_dep = dependency('webkit2gtk-5.0')

# Resources
resources = gnome.compile_resources('ui-resources','yamdown.gresource.xml')

sourcefiles = [
  'main.c',
  'yamdown.c',
  'notebook.c',
  'paneview.c',
  'notebook_label.c',
  'renderer.c'
]

yamdown = executable('yamdown',
                     sourcefiles,
                     resources,
                     dependencies: [
                      gtk4_dep,
#                      cmark_dep,
                      md4c_dep,
                      gtksourceview_dep,
                      webkit2gtk_dep
                     ],
                     install: true
                    )
