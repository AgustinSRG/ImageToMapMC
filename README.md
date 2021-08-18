# Image to Map (Minecraft)

Tool to convert images to blocks, in order to build map art in Minecraft

# Build with cmake (Debug)

Generate makefiles:

```sh
cmake -DCMAKE_BUILD_TYPE=Debug -S. -Bbuild
```

Build binaries:

```sh
cmake --build build --config Debug
```

# Build with cmake (Release)

Generate makefiles:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -S. -Brelease
```

Build binaries:

```sh
cmake --build release --config Release
```
