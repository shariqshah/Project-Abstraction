# Project Abstraction #

An abstraction layer for creating realtime rendered applications. All the work done here is being redone from scratch by using as few libraries as possible [here](https://github.com/shariqshah/symmetry).

## Features ##
- Realtime rendering in 3d using OpenGL 3.0.
- Loading and rendering meshes from blender3d using my own addon [BlendExp](https://github.com/shariqshah/blendexp)
- Embedded [AngelScript](http://www.angelcode.com/angelscript/) for writing scripts that define behaviour. Scripts can be added/edited and reloaded in realtime without restarting the application.
- Fully dynamic per-pixel lights (spot/point/directional).
- Shadow mapping for spot/direcional lights with PCF and CSSM(only direcitonal lights).
- Loading and Recreating application state and settings using JSON.

## Libraries/Dependencies ##
- [SDL2](https://libsdl.org/)
- [AngelScript](http://www.angelcode.com/angelscript/)
- [Glew](http://glew.sourceforge.net/)
- [GLM](http://glm.g-truc.net/0.9.7/index.html)
- [Bullet Physics](http://bulletphysics.org/wordpress/)
- [Cmake](https://cmake.org/)
- [ImGui](https://github.com/ocornut/imgui)

## Gallery ##
![2015-05-08-125923_2806x900_scrot.png](https://bitbucket.org/repo/doMRzn/images/4124583816-2015-05-08-125923_2806x900_scrot.png)
![2015-05-03-200303_2806x900_scrot.png](https://bitbucket.org/repo/doMRzn/images/2235684618-2015-05-03-200303_2806x900_scrot.png)
![2015-04-10-211847_2806x900_scrot.png](https://bitbucket.org/repo/doMRzn/images/2781071521-2015-04-10-211847_2806x900_scrot.png)
![2015-05-04-192719_2806x900_scrot.png](https://bitbucket.org/repo/doMRzn/images/1414654832-2015-05-04-192719_2806x900_scrot.png)
![2015-05-07-003015_1366x768_scrot.png](https://bitbucket.org/repo/doMRzn/images/3285117517-2015-05-07-003015_1366x768_scrot.png)
