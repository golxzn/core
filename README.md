<br/>
<!-- <p align="center">
	<img src="assets/CuteCoreLogo.png" />
</p> -->
<h2 align="center">⚙️ just the <b>gxen</b> game engine <b>core</b> ⚙️</h2>

<br/>

<h2>📜 Table of contents </h2>

- [🧱 Project structure](#-project-structure)
  - [Dependencies](#dependencies)
  - [Modules](#modules)
- [⚖️ Licensing](#️-licensing)

## 🧱 Project structure

### Dependencies

- CMake 3.23 or higher;
- [glm::glm](https://github.com/g-truc/glm.git) (The Happy Bunny License (Modified MIT License));
- [spdlog::spdlog](https://github.com/gabime/spdlog) (MIT License);
- [sago::platform_folders](https://github.com/sago007/PlatformFolders) (MIT License);
- [ghcFilesystem::ghc_filesystem](https://github.com/gulrak/filesystem) (MIT License);
- [stb_image](https://github.com/nothings/stb) (MIT License or Public Domain).

### Modules
- __math__: <golxzn/core/math.hpp>;
  - point: <golxzn/core/math/point.hpp>
  - vector: <golxzn/core/math/vector.hpp> (aliases for point)
  - matrix: <golxzn/core/math/matrix.hpp>
- __types__:
  - angle: <golxzn/core/types/angle.hpp>
- __resources__:
  - manager: <golxzn/core/resources/manager.hpp>
- __utils__: <golxzn/core/utils.hpp>;
  - error: <golxzn/core/utils/error.hpp>
  - numeric: <golxzn/core/utils/numeric.hpp>
  - traits: <golxzn/core/utils/traits.hpp>
  - strutils: <golxzn/core/utils/strutils.hpp>
- __geometry__:
  - transform2D: <golxzn/core/geometry/transform2D.hpp>
  - transform3D: <golxzn/core/geometry/transform3D.hpp>
- __type_registry__; [under development]

## ⚖️ Licensing

See the [LICENSE](LICENSE) document.
