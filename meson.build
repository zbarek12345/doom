project('doom', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++11'])

regex = dependency('regex')
gl = dependency('opengl')
sdl2 = dependency('sdl2')
sdl2_main = dependency('sdl2')

doom = executable('doom',
                  ['src/parser.cpp', 'src/Game.cpp','src/Camera.cpp','src/Player.cpp','src/CircuitFinder.cpp','src/SurfaceCalculator.cpp'
                   , 'main.cpp'],
                  dependencies: [sdl2,sdl2_main,gl, regex],
                  link_args:['-mwindows'],
                  install : true)

test('test', doom)