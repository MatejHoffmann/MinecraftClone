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
- Building/Destroing blocks

The mesh is generated using Unreals `UProceduralMeshComponent`. Noise textures are generated using [FastNoiseLite](https://github.com/Auburn/FastNoiseLite).

![Screenshot from the game.](/Images/Minecraft.png)

This is by no means a finished game and there are a lot of things that would be nice to implement, like: 
- Add NPCs
- Add full inventory
- Ability to pickup destroyed blocks
- Add multiplayer support
- Trees
- Biomes

And the list can go on and on...

Additionally, there are several areas I would like to revisit, such as leveraging Curve Tables for terrain generation settings, improving the connection between game logic and UI, and generally applying best UE practices I’ve learned since I stopped working on this project. That said, I’m still quite happy with what I achieved overall.
