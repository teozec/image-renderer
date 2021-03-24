# image-renderer

image-renderer is a C++ tool to generate photo-realistic images.
It is based on the lectures of the _Numerical tecniques for photorealistic image generation_ course ([EN](https://www.unimi.it/en/education/degree-programme-courses/2022/numerical-tecniques-photorealistic-image-generation), [IT](https://www.unimi.it/it/corsi/insegnamenti-dei-corsi-di-laurea/2022/calcolo-numerico-la-generazione-di-immagini-fotorealistiche)), held by [Prof. Maurizio Tomasi](http://cosmo.fisica.unimi.it/persone/maurizio-tomasi/) in 2021.

## Installation

After obtaining the source code (e.g. by cloning from [our GitHub repository](https://github.com/teozec/image-renderer)), compile it using the following procedure in the code directory:

```bash
mkdir build
cd build
cmake ..
make
```

The executable files are generated in the `build` directory.

Please notice that [`cmake`](https://cmake.org/) (version >= 3.12) and a C++17 compiler are required.

## Contributing

If you find any problem or wish to contribute, please open an issue or a pull request on [our GitHub repository](https://github.com/teozec/image-renderer). Thank you!

## License

image-renderer is free software, and it is released under version 3 of the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.html).
