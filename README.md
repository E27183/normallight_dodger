## NORMALLIGHT

In this game, you must navigate within a large boundary box while avoiding contact with the ors flying around you! The game continues until you strike the boundary or an orb.

To move:

 - Press space to accelerate forwards.
 - Press the left, right, up and down keys to steer in that direction.
 - Decelaration happens automatically over time

To compile and play:
 - Ensure you have g++ and libsdl2-dev installed
 - Download the source code
 - Navigate to the root folder and run ```make fullstack```


### Known bugs and missing features

 - Overlap of orbs does not correspond with distance
 - The current ship position is not visible
 - Dying closes the gui instead of rendering a "loss" screen
 - Orbs touching each other or the ship or each other has no effect and does not result in losing