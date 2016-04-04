# Ophidian

### how to open the project on Eclipse

```bash
mkdir ../openeda-build
cd ../openeda-build
mkdir debug
cd debug
cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ../../openeda
```

Same for Release (switch "Debug" to "Release")

You must add a preprocessor macro to enable Eclipse to use c++11 features (e.g., like unordered_map, unique_ptr, etc.)

__cplusplus=201103L

Step by Step: Right-Click on Project -> Properties -> C/C++ General -> Preprocessor Include Paths, Macros etc. -> Entries Tab -> Language GNU C++ -> Select CDT User Setting Entries from Setting Entries -> Click Add... -> Type = Preprocessor Macro, Name = __cplusplus, Value = 201103L

