# GridFluidSimulation

## Overview
This project realises a simple, grid based version of a fluid simulation using SGE, based on POWIDL.

## Current status
At the moment, the simulation works fine so far with just a fluid being simulated and a dye, that can be dropped in to visualize current fluid movement.
More dye can be dropped in by clicking into the grid and velocity + dye can be added at a point by clicking and dragging inside the grid. When clicking outside the grid, velocity + dye get applied at the nearest edge.

## Future ideas
- integrate pressure handling
- integrate gravity (relies on pressure handling)
- integrate a GUI for different parameters

## Sidenotes
Some things to watch out for:

### Known issues
- the line renderer only seems to render a maximum of 2^16 lines, which means, that only the first portion of velocities is drawn if enabled (see controls how to do that)
- the way the velocities influence the field causes axies aligned artefacts at the border of manipulated area. Thus when having only a point source of e.g. velocity input, the velocity seems to spread out further at the according axies
- corners are not handled properly at the moment - it may be, that dye is getting stuck there

### Controls

#### general shortcuts
- P: pause/resume simulation
- R: reset simulation
- A: single step through simulation

#### rendering shortcuts
- CTRL + C: toggle drawing of a coordinate system
- CTRL + G: toggle drawing of the grid lines
- CTRL + V: toggle drawing of the real stored velocity vectors
- CTRL + I: toggle drawing of the velocites at grid cell centers
- CTRL + O: toggle drawing of the grid outline (if grid lines are not drawn anyway)
- F: change the fog color to a random new one
