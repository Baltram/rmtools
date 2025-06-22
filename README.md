# Modding Tools for the Gothic, Risen & Elex series

This repository contains a collection of tools I developed for the modding community of games by Piranha Bytes, including the *Gothic*, *Risen*, and *Elex* series.

These games are built on proprietary engines (Genome Engine, ZenGin) with custom file formats. The tools here were created through reverse-engineering to allow the community to view, convert, and modify game assets: from 3D models and textures to sound, dialogue, and game logic files. The source code for most of these projects is organized in the subfolders of this repository.

---

## Flagship Tools

### Rimy3D
A comprehensive GUI application for viewing, converting, and editing 3D assets and materials for the *Gothic* and *Risen* series.

*   **Features:**
    *   Import/export for game-specific formats (`._xmsh`, `._xmac`, `._xcom`, etc.)
    *   Support for standard formats like `.3ds`, `.obj`, and `.ase`.
    *   Integrated material editor for `.xshmat` and `._xmat` files.
    *   Automatic loading of textures and materials from game files.
    *   Batch conversion for processing multiple files at once.
    *   Plugins for **GMax**, **Blender**, and **3ds Max** to streamline workflows.
    *   Extensive documentation in both English and German.

*   **Website & Documentation:** **[baltr.de/rimy3d/](https://www.baltr.de/rimy3d/)**
*   **Community Threads:** [Gothic 1/2](https://forum.worldofplayers.de/forum/threads/1483556-Tool-Rimy3D), [Gothic 3](https://forum.worldofplayers.de/forum/threads/1483554-Tool-Rimy3D), [Risen](https://forum.worldofplayers.de/forum/threads/1483555-Tool-Rimy3D)
*   **Source Folder:** **[`./Rimy3D/`](./Rimy3D/)**

### Resource Managers (Elex & Risen 3)
Powerful command-line tools for unpacking game archives (`.pak`) and performing bidirectional conversions of many core asset types.

*   **Elex Resource Manager:** Unpacks/packs `.pak` archives and converts textures, sounds, music, speech, dialogue, quests, and world data for *Elex*.
    *   **Community Thread:** [Elex Resource Manager](https://forum.worldofplayers.de/forum/threads/1512504-tool-Elex-Resource-Manager)
    *   **Source Code:** [`elexresman.cpp`](./mimicry/source/MimicryApp/elexresman.cpp)

*   **Risen 3 Resource Manager:** Unpacks/packs `.pak` archives and converts textures, sounds, music, speech, templates, and world data for *Risen 3*.
    *   **Community Thread:** [Risen 3 Resource Manager](https://forum.worldofplayers.de/forum/threads/1381917-tool-Risen-3-Resource-Manager)
    *   **Source Code:** [`r3resourceman.cpp`](./mimicry/source/MimicryApp/r3resourceman.cpp)

---

## Command-Line Converters (Risen 1 & 2)

A collection of focused, bidirectional command-line tools for various *Risen* and *Risen 2* file formats.

| Tool (Source File) | Description | Community Link |
| :--- | :--- | :--- |
| [`r2tab.cpp`](./mimicry/source/MimicryApp/r2tab.cpp) | Converts between Risen 2 `.tab` (string tables) and `.csv` files. | [r2tab Thread](https://forum.worldofplayers.de/forum/threads/1142441-tool-r2tab) |
| [`r2ximg.cpp`](./mimicry/source/MimicryApp/r2ximg.cpp) | Bidirectional converter for Risen 2 `._ximg` (image) and `.dds` files. | [r2ximg Thread](https://forum.worldofplayers.de/forum/threads/1141103-tool-r2ximg) |
| [`r2xsnd.cpp`](./mimicry/source/MimicryApp/r2xsnd.cpp) | Bidirectional converter for Risen 2 `._xsnd` (sound) and `.wav` files. | [r2xsnd Thread](https://forum.worldofplayers.de/forum/threads/1141873-tool-r2xsnd) |
| [`r2xdlg.cpp`](./mimicry/source/MimicryApp/r2xdlg.cpp) | Bidirectional converter for Risen 2 `._xdlg` (dialogue audio) and `.wav` files. | |
| [`r1xsnd.cpp`](./mimicry/source/MimicryApp/r1xsnd.cpp) | Bidirectional converter for Risen 1 `._xsnd` (sound) and `.mp3` files. | [r1xsnd Thread](https://forum.worldofplayers.de/forum/threads/1482584-tool-r1xsnd) |

---

## Standalone & Deprecated Tools

These are older tools, primarily for *Risen 1*. Most of their functionality has been integrated into and improved upon in **Rimy3D**. They are kept here for historical and reference purposes.

| Project Folder | Description | Status | Community Link |
| :--- | :--- | :--- | :--- |
| [`xcomPose`](./xcomPose/) | A GUI tool for importing/exporting Risen collision meshes (`._xcom`). | Superseded by Rimy3D | [xcomPose Thread](https://forum.worldofplayers.de/forum/threads/971037-tool-xcomPose) |
| [`obj2xmac`](./obj2xmac/) | A tool to replace meshes and skinning in `._xmac` files using `.obj` models. | Superseded by Rimy3D | [obj2xmac Thread](https://forum.worldofplayers.de/forum/threads/952889-tool-obj2xmac) |
| [`xmatEdit`](./xmatEdit/) | A GUI tool for inspecting and modifying Risen material files (`._xmat`). | Superseded by Rimy3D | [xmatEdit Thread](https://forum.worldofplayers.de/forum/threads/952888-tool-xmatEdit) |
| [`Risen XMSH Builder`](./Risen%20XMSH%20Builder/) | An early GUI tool to convert 3D models to Risen's `._xmsh` format. | Superseded by Rimy3D | [XMSH Builder Thread](https://forum.worldofplayers.de/forum/threads/751486-RELEASE-Risen-XMSH-Builder) |
| [`xmac2obj`](./xmac2obj/) | A command-line tool to convert Risen `._xmac` files to `.obj`. | Superseded by Rimy3D | [xmac2obj Thread](https://forum.worldofplayers.de/forum/threads/852455-Tool-xmac2obj) |
| [`gmax2ase`](./gmax2ase/) | Converts `.gmax` files (with Extended Saving) to `.ase` format. | Superseded by Rimy3D | |

---

## Core Frameworks & Libraries

*   [`mimicry`](./mimicry/): A C++ framework containing shared code and classes for file format handling (3D, images, etc.) used by all the newer tools like Rimy3D and the Resource Managers.
*   [`QImageIOPlugin`](./QImageIOPlugin/): Custom Qt image plugins for `.dds`, `.tga`, and the game-specific `.ximg` formats, used by Rimy3D.
*   [`rmStuff`](./rmStuff/): An older framework that was superseded by `mimicry`.
