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

NewModels = ['src/NewModels/Map.cpp', 'src/NewModels/RayCaster.cpp','src/NewModels/Sector.cpp','src/NewModels/Wall.cpp']


doom = executable('doom',
                  ['src/parser.cpp','src/OriginalTextureRenderer.cpp', 'src/Enemy.cpp',
                   'src/Entity.cpp','src/AnimatedEntity.cpp', 'src/headers/stb_image.h',
                   'src/Game.cpp', 'src/Camera.cpp', 'src/Player.cpp',
                   'src/CircuitFinder.cpp', 'src/TexBinder.cpp', 'main.cpp',
                   'src/RawLumpKeeper.cpp', 'src/textures/playpal.cpp', 'src/Collectibles.cpp',
                   'src/HudRender.cpp', 'src/Projectile.cpp', 'src/Weapon.cpp',
                   'src/SpecialBinder.cpp', 'src/ActionPerformer.cpp', NewModels],
                  dependencies : [regex, sdl2, sdl2_main, gl, cdt_lib, glew],
                  cpp_args : ['-funroll-loops', '-march=native','-O3'],
                  #link_args : ['-static-libgcc', '-static-libstdc++','-static'],
                  install : true)

test('test', doom)