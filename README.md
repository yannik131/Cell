# Cell

Physics simulation for colliding discs. Idea:

- Define different disc types (mass, radius, color, etc.)
- Define reactions: decomposition (A -> B + C), combination (A + B -> C), exchange (A + B -> C + D)
- Define compartments with semi-permeable membranes (pending)
- Track state of the simulation with plots and a real-time view

![](doc/screenshot.png)

Uses an `sf::RenderWindow` for rendering in the OpenGL context of Qt for performance reasons (I haven't tested performance of `QtGraphicsView` but I don't think it can display thousands of circles with 120 FPS so I'm sticking with SFML).

## Build

You can find the necessary build steps in the `scripts/unix/setup.sh` and `scripts/windows/setup.ps1` scripts. If you run them, they might even compile everything for you, but I wouldn't count on it since there are some dependencies vcpkg won't install.
Also `git submodule update` sometimes only creates a shallow clone of the repository, in which case a `git fetch --unshallow` might be in order. This is necessary because I'm pinning SFML to version 2.6.2 because 3.0.x currently does not work on windows when creating `sf::RenderWindow` from an existing OpenGL context (fix comes with [this PR](https://github.com/SFML/SFML/pull/3469) in 3.1).

## Tests

Code coverage (currently only libcell): [cellcoverage.myactivities.net](https://cellcoverage.myactivities.net)
