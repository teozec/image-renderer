# image-renderer
![Operating Systems](https://img.shields.io/badge/OS-Linux%20%7C%20MacOS%20%7C%20Windows-lightgrey)
![License](https://img.shields.io/github/license/teozec/image-renderer)
![Pulls](https://img.shields.io/github/issues-pr/teozec/image-renderer)
![CI](https://img.shields.io/github/workflow/status/teozec/image-renderer/CMake)
![Top Language](https://img.shields.io/github/languages/top/teozec/image-renderer)
![Version](https://img.shields.io/github/v/release/teozec/image-renderer)
![Commits](https://img.shields.io/github/commit-activity/m/teozec/image-renderer)
![Size](https://img.shields.io/github/repo-size/teozec/image-renderer)

image-renderer is a C++ tool to generate photo-realistic images.
It is based on the lectures of the _Numerical tecniques for photorealistic image generation_ course ([EN](https://www.unimi.it/en/education/degree-programme-courses/2021/numerical-tecniques-photorealistic-image-generation), [IT](https://www.unimi.it/it/corsi/insegnamenti-dei-corsi-di-laurea/2021/calcolo-numerico-la-generazione-di-immagini-fotorealistiche)), held by [Prof. Maurizio Tomasi](http://cosmo.fisica.unimi.it/persone/maurizio-tomasi/) in 2021.

## Table of contents
- [Requirements](#requirements)
- [Building](#building)
    - [Standard Unix and Windows](#standard-unix-and-windows)
    - [MacOS (Xcode)](#macOS-(Xcode))
- [Usage examples](#usage-examples)
    - [Converting a PFM file to a LDR image](#converting-a-pfm-file-to-a-ldr-image)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)
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

If you wish to use Xcode on your macOS, you can build the project using:

```bash
mkdir build
cd build
cmake -G "Xcode" ..
```
Now that it is all set you can open up the Xcode project built in the `build` directory and compile it from within the IDE.

## Basic Tutorial
We prepared a fun animation for you to start off: from the `image-renderer/` repo run the following
```bash
chmod +rx tutorial.sh
./tutorial.sh
```
and enjoy the animation saved in the `build/animation_demo/` folder.

![animation](https://media.giphy.com/media/JUbfrBiFQnYfyQ0oM6/giphy.gif)

## Usage examples
You can choose one of two subcommands: `demo` or `pfm2ldr`.

### Run a demo of our program with `demo`

In order to get used to our program you can run our demo:
```bash
image-renderer demo
```

You can also set some parameters like width and height of the final image, e.g.
```bash
image-renderer demo -w 300 -h 200
```
This will output a `demo.pfm` which can be then converted to any LDR image supported ([see next pararaph](#converting-a-pfm-file-to-a-ldr-image-with-`pfm2ldr`)).

To get a list of all options available run
```bash
image-renderer pfm2ldr --help
```

### Converting a PFM file to a LDR image with `pfm2ldr`

After rendering a PFM file, you can use the executable to convert it to a LDR image format.

Usage:
```bash
image-renderer pfm2ldr [options] <format> <input> <output>
```

For example, to save it as a 24-bit png file you can run:
```bash
image-renderer pfm2ldr png input.pfm output.png
```

If you wish to convert your PFM to a 8-bit colormap PNG (using a palette), with a compression factor of 5, run:
```bash
image-renderer pfm2ldr -p -c 5 png input.pfm output.png 
```

To generate a webp with a normalization factor a=2.5 and a gamma of 1.3, run:
```bash
image-renderer pfm2ldr --afactor=2.5 --gamma=1.3 webp input.pfm output.webp
```

To generate a jpeg with a quality of 60 (max is 95) run:
```bash
image-renderer pfm2ldr -c 60 jpeg input.pfm output.jpeg
```

Similarly all other formats. Here a list of all supported formats:
*	bmp
*	gif
*	jpeg
*	png
*	tiff
*	webp

To get a list of supported options for each format, please run
```bash
image-renderer pfm2ldr --help
```

## Contributing

If you find any problem or wish to contribute, please open an issue or a pull request on [our GitHub repository](https://github.com/teozec/image-renderer). Thank you!


## License

image-renderer is free software, and it is released under version 3 of the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.html).

## Acknowledgements


<img src="https://avatars.githubusercontent.com/u/44500371?v=4" width=100> 
<img src="https://avatars.githubusercontent.com/u/79975678?s=400&u=6770b5f0354ed29bf9a54e7f27a8250bb812c279&v=4" width=100> 

...and many more to come!
