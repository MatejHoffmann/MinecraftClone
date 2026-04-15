# MinecraftClone

This repository contains a Minecraft clone built in Unreal Engine 5. It was originally created as a learning exercise for UE5, but since I had fun making it, I continued working on it and added more features than I had initially planned.

List of features:

- Procedural world generation based on a random seed
- Advanced terrain generation using multiple noise textures, including 3D noise to create overhangs
- Multithreaded terrain generation and lighting calculations
- Custom ambient occlusion lighting calculated from block neighborhoods
- Basic water system with environmental effects (post-processing shader and swimming mechanics)
- Debugging tools
- Save system

The mesh is generated using Unreals `UProceduralMeshComponent`. Noise textures are generated using [FastNoiseLite](https://github.com/Auburn/FastNoiseLite).
