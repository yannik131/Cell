# Cell

Physics simulation for colliding discs. Idea:

- Define different disc types (mass, radius, color)
- Define reactions:
  - unimolecular: transformation (A -> B), decomposition (A -> B + C)
  - bimolecular: combination (A + B -> C), exchange (A + B -> C + D)
  - with time-based (unimolecular) or collision-based (bimolecular) probabilities
- Define compartments with semi-permeable membranes
  - Define membrane types (radius, disc count)
  - Set permeabilities: Inward, outward, bidirectional or no permeability based on disc type
  - Set distribution: Frequency for each disc type
  - Add membranes at specified positions (currently only manually with a table)
  - If use distribution is disabled in Setup, add discs manually as well
- Set time step, time scale and average velocity in Setup
- Track state of the simulation with plots and a real-time view
- Shortcuts: Press `F` to toggle fullscreen, `Space` to toggle simulation start/stop

![](doc/screenshot.png)

Uses an `sf::RenderWindow` for rendering in the OpenGL context of Qt for performance reasons (I haven't tested performance of `QtGraphicsView` but I don't think it can display thousands of circles with decent FPS so I'm sticking with SFML). Fullscreen can be toggled by pressing `F`.

## Build

You can find the necessary build steps for linux/mac in the `scripts/unix/setup.sh` script and for windows in `scripts/windows/setup.ps1`. If you run them, they might even compile everything for you, but I wouldn't count on it since there are some dependencies vcpkg won't install (especially on linux/mac, see `setup.sh`).
I'm pinning SFML to version 2.6.2 with vcpkg overlays because 3.0.x currently does not work on windows when creating a `sf::RenderWindow` from an existing OpenGL context (fix comes with [this PR](https://github.com/SFML/SFML/pull/3469) in 3.1).

## VS Code

There is a `CMakePresetsWindows.json` located in `scripts/` that currently on
ly works on windows that I personally use with vs code. I wanted to use `clang-tidy` on windows and needed to use `ninja` for that, but I had trouble convincing `ninja` to use the `msvc` compiler over `gcc`, so I manually set the compiler to `cl` in the presets. It of course selected the x86 version over the x64 one for whatever reason. So I told `cmake` where to find `cl` and all the dependencies by adding `"cmake.environment": { "PATH": ..., "LIB": ..., "INCLUDE": "... }` to my `.vscode/settings.json`. I obtained the environmental variables by starting the "x64 Native Tools Command Prompt for VS 2022" and echoed `%PATH%`, `%LIB%` and `%INCLUDE%`. Took me a few days to get this to work.

To not get any include errors you need to add the respective folder to the `.vscode/c_cpp_properties.json`, here's mine as an example (currently working on windows):

<details>
    <summary>c_cpp_properties.json</summary>

```json
{
    "configurations": [
        {
            "includePath": [
                "${workspaceFolder}/contrib/qcustomplot/",
                "${workspaceFolder}/src/lib/",
                "${workspaceFolder}/src/lib/cell/",
                "${workspaceFolder}/src/lib/gui/core",
                "${workspaceFolder}/src/lib/gui/delegates/",
                "${workspaceFolder}/src/lib/gui/dialogs/",
                "${workspaceFolder}/src/lib/gui/models/",
                "${workspaceFolder}/src/lib/gui/widgets/",
                "${workspaceFolder}/build_debug/cell-gui_autogen/include",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtConcurrent",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtCore",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtDBus",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtDeviceDiscoverySupport",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtExampleIcons",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtExamplesAssetDownloader",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtFbSupport",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtGui",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtNetwork",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtOpenGL",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtOpenGLWidgets",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtPrintSupport",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtSql",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtTest",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtWidgets",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/Qt6/QtXml",
                "${workspaceFolder}/build_debug/vcpkg_installed/x64-windows/include/"
            ],
            "defines": [
                "GLOG_USE_GLOG_EXPORT"
            ],
            "cStandard": "c17"
        }
    ],
    "version": 4
}
```

</details>

## Tests

Code coverage (currently only libcell): [cellcoverage.myactivities.net](https://cellcoverage.myactivities.net)
