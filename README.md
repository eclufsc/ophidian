# openeda

### how to open the project on Eclipse

```bash
mkdir ../openeda-build
cd ../openeda-build
mkdir debug
cd debug
cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ../../openeda
```

Same for Release (switch "Debug" to "Release")
