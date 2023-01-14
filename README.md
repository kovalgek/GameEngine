![GameEngineLogo][logo]{ width="300" height="300" style="display: block; margin: 0 auto" }

Parelhoen is a guineafowl in Dutch. These birds are beautiful creatures that come from Africa. I remember this one guineafowl that broke out of the farm and hung out in a parking lot. During the crazy times of COVID, this bird always put a smile on my face. So, I decided to name my game engine after it as a nod to its awesomeness. Long live Parelhoen, the brave little bird that brightened up my days!


<!-- ABOUT THE PROJECT -->
## About The Project

This project is a rendering engine that is written in C++ 11 and using DirectX 12 as low-level graphics API. Additionally, I employed the [ImGui](https://github.com/ocornut/imgui) library to create a user interface for configuring various rendering features in real-time. Initially, I integrated the [EnTT](https://github.com/skypjack/entt) header-only library as an Entity-Component-System, but later decided to implement a simpler solution without it."

<!-- INSTALLATION -->
## Installation

I used CMake as a building tool. To run the project, do these steps:

1. Clone the repo
```sh
   git clone https://github.com/kovalgek/GameEngine.git
```
2. Run these scripts in the root directory
3. Open generated project file using Visual Studio 2019

<!-- ROADMAP -->
## Roadmap

- [x] 3d objects in custom format
- [x] Fog
- [x] Mirrors
- [x] Light
- [x] Shadow
- [ ] Support `.obj` 3d file format
- [ ] Network layer
- [ ] Universal layer to support other rendering API like metal

<!-- SCHEMES -->
## Schemes


#### DataFlow
![DataFlow1][dataflow1]{ style="border: 20px solid black" }
#### Resources Scheme
![DataFlow2][dataflow2]{ style="border: 20px solid black" }


<!-- Acknowledgments -->
## Acknowledgments

I found Frank Luna's book [Introduction to 3D Game Programming with DirectX 12](https://www.amazon.com/Introduction-3D-Game-Programming-DirectX/dp/1942270062) to be incredibly helpful in my development of this project. Instead of simply copying his examples, I aimed to create a game engine with a strong and well-designed architecture. Additionally, I found the experience of programming in C++ to be a refreshing change from my primary job, where I primarily work with Swift and Objective-C.


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[logo]: images/parelhoen.png
[dataflow1]: images/ParelhoenDataFlow.png
[dataflow2]: images/ParelhoenDiagram.png