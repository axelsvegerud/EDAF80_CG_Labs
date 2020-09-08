Revision history for CG_Labs


v2020.0 2020-08-25
==================

New features
------------

* For those with GPUs (or GPU drivers) which do not support OpenGL 4.1: check
  out the `OpenGL_3.3 branch
  <https://github.com/LUGGPublic/CG_Labs/tree/OpenGL_3.3>`_.
* In any of the assignments, you can use *F11* to toggle between windowed and
  fullscreen mode.
* In the first assignment for EDAF80, textures for all planets (plus the Sun and
  the Moon) are loaded by default, and constants for their orbit and spin
  configurations are provided.
* In the second assignment for EDAF80, a set of control points is now provided;
  there are represented as small spheres in the 3-D view (once
  `parametric_shapes::createSphere()` has been implemented).
* When using the `node::render()` function and GL_KHR_debug is available, all
  OpenGL calls will be placed within a `glPushDebugGroup()` and
  `glPopDebugGroup()` pair using the node’s name, to improve debugging.

Improvements
------------

* All assignments have received several improvements and tweaks, for example
  EDAF80’s third assignment will create and render two spheres by default (one
  for the skybox, and one on which Phong shading and normal mapping will be
  performed).
* The API of all `parametric_shapes` methods has been modified to be more
  consistent and clearer.
* `displayTexture()` no longer takes a camera as input to retrieve the near and
  far parameters used to linearise values read from depth textures, but now
  take those directly as argument instead.
* The shader for celestial bodies’ rings no longer uses an opacity texture but
  instead uses the alpha channel of the diffuse texture.
* Removed unused code and replaced other with standard types.
* The vertical inversion of textures is now performed by STB.
* Re-use functionalities from GLAD
* The `set_uniforms` parameter of several methods of `Node`, has been made
  optional.
* Switch Travis to use Ubuntu Focal and add macOS to the tested environment.
* A lot of improvements on the CMake configuration files to modernise them,
  make them clearer, remove unnecessary operations, display additional
  information when dependencies fail to download or build, etc.
* Reorganise CHANGES.rst and add release date for v2019.1

Fixes
-----

* Do not make helper functions load relative to specific directory
* Do not use `abs()` on floating point numbers
* Trigger a copy of the DLLs for all targets
* Fix typo in `TRSTransform.h`
* Fix typo in `GLStateInspection’s ToString()`

Dependencies updates
--------------------

* Bump CMake requirements to version 3.13
* CMake: Fix the version of stb and tinyfiledialogs
* Update the downloaded version of GLFW to 3.3.2
* Update GLM to 0.9.9.5 and require exact version
* Update Dear ImGui to 1.78 and ship with the code
* Re-generate the GLAD files and up to OpenGL 4.6
* Update assimp requirement to 5.0, update the downloaded version to 5.0.1 and
  apply additional fixes
* Update the resources archive


v2019.1 2019-09-06
==================

New features
------------

* Select polygon mode from GUI, and simplify it
* Toggle visualisation of light cones in wireframe mode from the GUI
* Switch between shaders from GUI
* Replace lodepng with stb, to also support JPEG file loading
* Add shader for celestial rings and load it in lab1

Improvements
------------

* Add a “CHANGES.rst” file that will list the different modifications done,
  from now on.
* Unify the TRS interfaces between the node and the `TRSTransform` classes, by
  using and exposing a `TRSTransform` instance inside the node class
* Edit node::render() to take parent transform
* AppVeyor: switch from VS 2019 Preview to VS 2019
* README: Add tinyfiledialogs to dependencies
* README: Sort the list of dependencies

Fixes
-----

* Ensure Log is destroyed before its clients
* Rename `WindowManager::CreateWindow()` to
  `WindowManager::CreateGLFWWindow()`, to avoid conflict with Windows API
  macro.
