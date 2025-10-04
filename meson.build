project('doom', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++11'])

regex = dependency('regex')

doom = executable('doom',
                  ['src/parser.cpp', 'main.cpp'],
                  dependencies: [regex],
                  install : true)

test('test', doom)