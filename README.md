# image-renderer
[![Operating Systems](https://img.shields.io/badge/OS-Linux%20%7C%20MacOS-lightgrey)](https://img.shields.io/badge/OS-Linux%20%7C%20MacOS-lightgrey)
[![Top Language](https://img.shields.io/github/languages/top/teozec/image-renderer)](https://img.shields.io/github/languages/top/teozec/image-renderer)
[![License](https://img.shields.io/github/license/teozec/image-renderer)](https://img.shields.io/github/license/teozec/image-renderer)

image-renderer is a C++ tool to generate photo-realistic images.
It is based on the lectures of the _Numerical tecniques for photorealistic image generation_ course ([EN](https://www.unimi.it/en/education/degree-programme-courses/2021/numerical-tecniques-photorealistic-image-generation), [IT](https://www.unimi.it/it/corsi/insegnamenti-dei-corsi-di-laurea/2021/calcolo-numerico-la-generazione-di-immagini-fotorealistiche)), held by [Prof. Maurizio Tomasi](http://cosmo.fisica.unimi.it/persone/maurizio-tomasi/) in 2021.

## Table of contents
- [Requirements](#requirements)
- [Building](#building)
    - [Standard Unix](#standard-unix)
    - [MacOS (Xcode)](#macos-(xcode))
	- [Windows](#windows)
- [Usage examples](#usage-examples)
    - [Converting a PFM file to a LDR image](#converting-a-pfm-file-to-a-ldr-image)
- [Contributing](#contributing)
- [License](#license)
## Requirements

The following software is required to build the project:
* [`cmake`](https://cmake.org/) (version >= 3.12)
* A `C++17` compiler.
* [`libgd`](https://libgd.github.io/), compiled with support for:
	* [`libpng`](http://www.libpng.org/pub/png/libpng.html)
	* [`libjpeg`](https://ijg.org/) or [`libjpeg-turbo`](http://www.libpng.org/pub/png/libpng.html)
	* [`libwebp`](https://developers.google.com/speed/webp/)
	* [`libtiff`](http://www.libtiff.org/)
This project also uses the header-only [`Argh!`](https://github.com/adishavit/argh) library, which is already included in this repository in `include/argh.h`.
Therefore, you do not need to install it on your system to compile `image-renderer`.


## Building

After obtaining the source code (e.g. by cloning from [our GitHub repository](https://github.com/teozec/image-renderer)) you can build the project with `cmake`.

### Standard Unix and Windows

Run the following shell commands from the project directory:

```bash
mkdir build
cd build
cmake ..
make
```

The executable files are generated in the `build` directory.

### macOS (Xcode)

First of all you need to add few lines in the `CMakeLists.txt` file:

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENTIONS OFF)
``` 
After saving it, you can build the project using:

```bash
mkdir build
cd build
cmake -G "Xcode" ..
```
Now that it is all set you can open up the Xcode project built in the `build` directory and compile it from within the IDE.


## Usage examples

### Converting a PFM file to a LDR image

After rendering a PFM file, you can use the executable to convert it to a LDR image format.

For example, to save it as a 24-bit png file you can run:
```bash
image-renderer png input.pfm output.png
```

If you wish to convert your PFM to a 8-bit colormap PNG (using a palette), with a compression factor of 5, run:
```bash
image-renderer png -p -c 5 input.pfm output.png 
```

To generate a webp with a normalization factor a=2.5 and a gamma of 1.3, run:
```bash
image-renderer webp -a 2.5 -g 1.3 input.pfm output.webp
```

To generate a jpeg with a quality of 60 (max is 95) run:
```bash
image-renderer jpeg -c 60 input.pfm output.jpeg
```

Similarly all other formats. Here a list of all supported formats:
*	bmp
*	gif
*	jpeg
*	png
*	tiff
*	webp

## Contributing

If you find any problem or wish to contribute, please open an issue or a pull request on [our GitHub repository](https://github.com/teozec/image-renderer). Thank you!


## License

image-renderer is free software, and it is released under version 3 of the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.html).
