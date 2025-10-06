project('heretic', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++11'])


srcs = files('main.cpp',
        'src/parser/parser.cpp')

heretic = executable('heretic',
                  srcs,
                  install : true)

test('test', heretic)