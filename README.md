# Cell

Physics simulation for colliding discs. Idea:

- Define different disc types (mass, radius, color, etc.)
- Define reactions: decomposition (A -> B + C), combination (A + B -> C), exchange (A + B -> C + D)
- Define compartments with semi-permeable membranes (pending)
- Track state of the simulation with plots and a real-time view

![](doc/screenshot.png)

Uses an `sf::RenderWindow` for rendering in the OpenGL context of Qt for performance reasons (I haven't tested performance of `QtGraphicsView` but I don't think it can display thousands of circles with 120 FPS so I'm sticking with SFML).

## Build

You can find the necessary build steps for linux/mac in the `scripts/unix/setup.sh` script and for windows in `scripts/windows/setup.ps1`. If you run them, they might even compile everything for you, but I wouldn't count on it since there are some dependencies vcpkg won't install (especially on linux/mac, see `setup.sh`).
Also `git submodule update` sometimes only creates a shallow clone of the repository, in which case a `git fetch --unshallow` might be in order. This is necessary because I'm pinning SFML to version 2.6.2 because 3.0.x currently does not work on windows when creating a `sf::RenderWindow` from an existing OpenGL context (fix comes with [this PR](https://github.com/SFML/SFML/pull/3469) in 3.1).

## VS Code

There is a `CMakePresets.json` that currently only works on windows that I personally use with vs code. I wanted to use `clang-tidy` on windows and needed to use `ninja` for that, but I had trouble convincing `ninja` to use the `msvc` compiler over `gcc`, so I manually set the compiler to `cl` in the presets. It of course selected the x86 version over the x64 one for whatever reason. So I told `cmake` where to find `cl` and all the dependencies by adding `"cmake.environment": { "PATH": ..., "LIB": ..., "INCLUDE": "... }` to my `.vscode/settings.json`. I obtained the environmental variables by starting the "x64 Native Tools Command Prompt for VS 2022" and echoed `%PATH%`, `%LIB%` and `%INCLUDE%`. Took me a few days to get this to work.

To not get any include errors you need to add the respective folder to the `.vscode/c_cpp_properties.json`, here's mine as an example (currently working on windows):

<details>
    <summary>c_cpp_properties.json</summary>

```json
{
    "configurations": [
        {
            "includePath": [
                "${workspaceFolder}/contrib/qcustomplot/",
                "${workspaceFolder}/src/libcell/",
                "${workspaceFolder}/src/libcellgui/core",
                "${workspaceFolder}/src/libcellgui/delegates/",
                "${workspaceFolder}/src/libcellgui/dialogs/",
                "${workspaceFolder}/src/libcellgui/models/",
                "${workspaceFolder}/src/libcellgui/widgets/",
                "${workspaceFolder}/build_debug/libcellgui_autogen/include",
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
