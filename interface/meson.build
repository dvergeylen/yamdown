project('yamdown', 'c', default_options: ['warning_level=3'])

gtk4_dep = dependency('gtk4', fallback : ['libgtk4', 'gtk4_dep'])
webkit2gtk_dep = dependency('webkit2gtk-5.0')

gnome=import('gnome')
resources = gnome.compile_resources('ui-resources','gtk4/yamdown.gresource.xml')

sourcefiles=files('main.c', 'gtk4/yamdown.c', 'gtk4/yamdown_notebook.c', 'gtk4/yamdown_webkit_webview.c')

executable('yamdown', sourcefiles, resources, dependencies: [gtk4_dep, webkit2gtk_dep])
