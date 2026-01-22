# Doom Remake

This project was built for a bachelor thesis. It is not yet finished, but i hope to finish it soon. Here's some small description, of the classes and their purpose.

## Classes

- __Game__ - main class, handles the game loop.
- __Player__ - player class, handles the player movement.
- __Camera__ - class responsible for handling camera movement, as well as providing the view vector.
- __HudRender__ - class responsible for rendering the HUD.
- __Parser__ - class responsible for parsing the wad file.
- __TexBinder__ - class responsible for binding textures to OpenGL, and unbinding them.
- __OriginalTextureRenderer__ - class responsible for loading and generating bitmaps of the original textures.
- __Weapon__ - class responsible for handling weapons.
- __Entity__ - base class for all entities, including static objects.
- __AnimatedEntity__ - class responsible for handling animated entities.
- __Collectibles__ - class responsible for handling collectibles, base interface for ammunition, health packs etc.. 
- __Enemy__ - class responsible for handling enemies.
- __Projectile__ - class responsible for handling projectiles (every attack is a projectile).
- __RayCaser__ - class responsible for providing the raycast, used for enemies, player and projectiles.
- __Map__ - class responsible for loading and rendering the map.
- __Sector__ - class responsible for handling sectors of the map (Holds entities inside, as well as the sector's floors/ceiling).
- __Wall__ - class responsible for handling walls of the map.
- __ActionPerfromer__ - class responsible for moving map parts, like doors, floors etc..


## Dependencies

- OpenGL
- GLFW
- GLM
- SDL2
- [CDT](https://github.com/artem-ogre/CDT.git)