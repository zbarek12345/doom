project('doom', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++23'])

regex = dependency('regex')
gl = dependency('opengl')
sdl2 = dependency('sdl2')
sdl2_main = dependency('sdl2')
glew = dependency('glew')


cmake = import('cmake')



cdt = cmake.subproject('CDT')

cdt_lib = cdt.dependency('CDT')

bgfx = cmake.subproject('bgfx.cmake')
bgfx_dep = bgfx.dependency('bgfx')

doom = executable('doom',
                  ['src/headers/vec2.h','src/parser.cpp','src/OriginalTextureRenderer.cpp',
                   'src/Entity.cpp','src/AnimatedEntity.cpp', 'src/headers/stb_image.h',
                   'src/Game.cpp', 'src/Camera.cpp', 'src/Player.cpp',
                   'src/CircuitFinder.cpp', 'src/TexBinder.cpp', 'main.cpp',
                   'src/RawLumpKeeper.cpp', 'src/textures/playpal.cpp', 'src/Collectibles.cpp'],
                  dependencies : [bgfx_dep, sdl2, sdl2_main, gl, regex, cdt_lib, glew],
                  link_args : ['-mwindows'],
                  install : true)

test('test', doom)