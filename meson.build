project('doom', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++11'])

doom = executable('doom',
                  ['src/parser.cpp', 'main.cpp'],
                  install : true)

test('test', doom)