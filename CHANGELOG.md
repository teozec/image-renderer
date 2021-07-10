# HEAD

- Add bash completions ([#PR17](https://github.com/teozec/image-renderer/pull/17)).
- Improve the command line interface ([#PR28](https://github.com/teozec/image-renderer/pull/28)).
- Implement a parser for scene files and a render action ([PR#20](https://github.com/teozec/image-renderer/pull/20), [PR#30](https://github.com/teozec/image-renderer/pull/30), [#29](https://github.com/teozec/image-renderer/issues/29), [#PR32](https://github.com/teozec/image-renderer/pull/32), [#31](https://github.com/teozec/image-renderer/issues/31)).
- Implement `PerlinNoise` and some related textures ([PR#24](https://github.com/teozec/image-renderer/pull/24)).
- Improve `SpecularBRDF` and implement `DielectricBSDF` ([PR#24](https://github.com/teozec/image-renderer/pull/24)).
- Enable multithreading ([PR#25](https://github.com/teozec/image-renderer/pull/25)).
- Bug fix: make Materials work with CSG shapes ([PR#27](https://github.com/teozec/image-renderer/pull/27), [#26](https://github.com/teozec/image-renderer/issues/26)).
- Bug fix: make PathTracer work properly also without Russian roulette ([PR#23](https://github.com/teozec/image-renderer/pull/23), [#21](https://github.com/teozec/image-renderer/issues/21)).
- Implement a DebugRender for better debugging ([PR#22](https://github.com/teozec/image-renderer/pull/22)).

# Version 0.3.0

- Implement a stacking feature, with mean, median and sigma clipping ([PR#15](https://github.com/teozec/image-renderer/pull/15)).
- Add a chair shape
- Implement path-tracing algorithm ([PR#10](https://github.com/teozec/image-renderer/pull/10), [#18](https://github.com/teozec/image-renderer/issues/18), [PR#19](https://github.com/teozec/image-renderer/pull/19)).
- Implement anti-aliasing algorithm ([PR#16](https://github.com/teozec/image-renderer/pull/16)).

# Version 0.2.0

- Implement triangles, boxes and constructive solid geometry ([PR#8](https://github.com/teozec/image-renderer/pull/8)).
- Bug fix: aspect ratio ([PR#14](https://github.com/teozec/image-renderer/pull/14), [#13](https://github.com/teozec/image-renderer/issues/13)).
- Bug fix: shape behaviour ([PR#12](https://github.com/teozec/image-renderer/pull/12), [#11](https://github.com/teozec/image-renderer/issues/11)).
- Implementation of `shape`, containing spheres, planes and world object ([PR#7](https://github.com/teozec/image-renderer/pull/7)).
- Bug fix: image orientation ([PR#6](https://github.com/teozec/image-renderer/pull/6), [#5](https://github.com/teozec/image-renderer/issues/5)).
- Implementation of `camera`, containing cameras, rays and tracer ([PR#4](https://github.com/teozec/image-renderer/pull/4)).
- Implementation of `geometry`, containing geometric objects ([PR#1](https://github.com/teozec/image-renderer/pull/1)).

# Version 0.1.1
- Automated search for libgd in CMakeLists.txt ([PR#3](https://github.com/teozec/image-renderer/pull/3), [#2](https://github.com/teozec/image-renderer/issues/2)).

# Version 0.1.0
- First release of image-renderer.
