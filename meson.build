project('yamdown', 'c', default_options: ['warning_level=3'])

gtk4_dep = dependency('gtk4', fallback : ['gtk4', 'gtk4_dep'])
#webkit2gtk_dep = dependency('webkit2gtk-5.0')

gnome=import('gnome')
resources = gnome.compile_resources('ui-resources','yamdown.gresource.xml')

sourcefiles=files('main.c', 'yamdown.c', 'notebook.c', 'paneview.c', 'notebook_label.c')

executable('yamdown', sourcefiles, resources, dependencies: [gtk4_dep]) #, webkit2gtk_dep])
