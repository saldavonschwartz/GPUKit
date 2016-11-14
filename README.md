# OXFEDE::GPUKit

A C++ framework for using OpenGL in an object oriented way.

The framework exposes OpenGL functionality thru an object oriented interface. It's still work in progress but supports the following constructs so far:

. vertex, geometry and fragment shaders (including the ability to parse text files into these objects.)
. programs, which expose vertex inputs / outputs thru a parameterized interface.
. buffers, which expose texture target binding for shader output.
. 2D and 3D (Cubemap) textures.
. geometry.

The framework also has a couple higher level constructs:
. Passes: a way to organize the rendering code associated with a program / buffer combination.
. Material: a way to proxy a program's attributes (shader uniforms) so the program can be shared among different geometry.

## Dependencies: 

## How to use it:
1. Include the whole GPUKit source folder in your project, or build it into a library and link against that.
2. Include `GPUKit.h` in your code. 
3. Make sure to call `GPUKit::init` before using anything else from the API.
4. Make sure to call `GPUKit::shutdown` when you are done and want to tear down the OpenGL context and window.




[More information and examples on this project](http://0xfede.io/2016/11/06/GPUKit.html)

<br>
Copyright (c) 2016 Federico Saldarini

[LinkedIn][l1] | [Blog][l2] | [GitHub][l3]

[l1]: https://www.linkedin.com/in/federicosaldarini
[l2]: http://0xfede.io
[l3]: https://github.com/saldavonschwartz
