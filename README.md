# cro lib
These librares are inspired by the [stb libraries]( https://github.com/nothings/stb ) which are single purpose C / C++ files. They are easy to implement and have minimal dependencies. They will be written with the intention of being used in video games (as this is where my interests lie). All files in the cro lib are public domain.

## cro_mipmap.h -- Mipmap Generation
This header file has functions to generate mipmaps. Reasons you might want this:
* Doesn't use any graphics API's or GPU calls
* Can see what your textures' mipmaps might look like as basis for custom ones
* Generated mipmaps outside of your application and put them into asset packs to avoid GPU overhead of generating them real time
* Generate mipmaps on CPU in case the GPU is busy doing other things

The library currently support the following pixel types:
* 32-bit pixel images (order doesn't matter so RGBA, ARGB, etc) for traditional mipmap generation
* floating point pixel images for use with depth textures

The functions currently used to generate the mipmaps are:
* Average of 4 pixels
* Minumum of 4 pixels
* Maximum of 4 pixels
* Minumum and Maximum of 4 pixels. This was is preferred if you need both min and max mipmaps as it does both at the same time, avoiding redundant work.

These functions will not allocate any memory, so the function cro_GetMipMapSize is provided to inform the user how big a resulting mipmap will be. The functions can also be run in parallel, breaking the work up based on rows. A number of the functions are created via a macro which uses a basic template of how the code will run. If a function down sampling function is needed, it can be very easy to extend the code.