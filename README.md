# ReTTo Engine

Not so fast game engine that doesn't use rasterization. 
## Features
- Fully generated image with compute and ray tracing pipeline

## Installation and building
### Prerequirements
- Vulkan API with binded path to it

### Installation
1. `git clone https://github.com/KoT3isGood/rettoEngine.git`
2. Open visual studio solution and build rttEngine project
Now you can create new projects using ReTTo

## Creating new project

You can use your own project to create games with ReTTo. Just make sure to include headers and link rttEngine library.

You may use standart int main() but there is built-in CREATE_GAME(AppInfo) macro.
