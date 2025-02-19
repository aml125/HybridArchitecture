<h1 aligh="center">Hybrid architecture for AI-based RTS games</h1>
<p align="center">Hybrid architecture for AI-based RTS games</p>

<p align="center">
  <a href="https://doi.org/10.1109/TG.2025.3533949">
    <img src="https://img.shields.io/badge/Transactions%20on%20Games-2025-blue" alt="Feature Requests">
  </a>
</p>

This repository contains a custom-built game engine using OpenGL for graphics rendering and OpenCL for general-purpose GPU computations. This work proposes a hybrid architecture to optimize the performance of real-time strategy (RTS) games by simultaneously utilizing both a dedicated GPU (dGPU) and an integrated GPU (iGPU). The core idea is to offload AI and physics simulation tasks from the dGPU to the iGPU, freeing up the dGPU exclusively for rendering. This is the system used in the experiments of the article.

## Contents
This repository contains the code of the game engine and game prototype used in the research. Inside you can find a Visual Studio 2019 project, containing the application.

The project is structured into two main folders:
- ecs: Files concerning the game engine
- game: Game prototype implementation

## Requirements
- Visual Studio 2019
- Cmake
- Standart ISO C++ 17
- OpenCL platform (Intel, Nvidia or AMD)

## Installation and use
This project is prepared to be run inside the Visual Studio environment. We do not provide a standalone deployment of the prototype.

Once all the requirements are installed, clone the repository and open it with Visual Studio 2019. In case libraries are missing, go to "Project properties"/"C++"/"Additional include directories" and add the directories "glm", "cmp", "sys", "util", "man". The are located inside the "windowgl" folder.

## Data
This project contains the code, libraries, and game models of the article.

## Acknowledgements
This work was supported by the Spanish Research Agency (AEI) (DOI: 10.13039/501100011033) under project Serverless4HPC PID2023-152804OB-I00.

This research has been performed inside the research group Federated Serverless Architectures for Heterogeneous High Performance Computing in Smart Manufacturing, on the Department of Computer Science and Technology, University of Alicante. <a href=https://aia.ua.es/en/proyectos/federated-serverless-architectures-for-heterogeneous-high-performance-computing-in-smart-manufacturing.html>link</a>

## Citation
```bibtex
@article{macia-lillo_hybrid_2025,
	title = {Hybrid {Architecture} for {AI}-{Based} {RTS} {Games}},
	issn = {2475-1510},
	doi = {10.1109/TG.2025.3533949},
	journal = {IEEE Transactions on Games},
	author = {Maciá-Lillo, Antonio and Jimeno-Morenilla, Antonio and Mora, Higinio and Duta, Eduard},
	year = {2025},
	pages = {1--14},
}
```

## License Information
This project is licensed under the <a href="LICENSE.txt">GPL-3 license</a>.
