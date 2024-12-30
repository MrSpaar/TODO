project(
  'todo',
  'cpp',
  version: '0.1.0',
)

gnome = import('gnome')
gresource = gnome.compile_resources('gresource', 'resources/gresource.xml', source_dir: 'resources')

executable(
  'bin',
  gresource,
  [
    'src/main.cpp',
    'src/gui.cpp',
    'src/sqlite.cpp',
  ],
  dependencies: [dependency('gtkmm-4.0'), dependency('sqlite3')],
)
