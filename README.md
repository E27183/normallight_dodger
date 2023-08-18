## NORMALLIGHT

In this game, you must navigate within a large boundary box while avoiding contact with the ors flying around you! The game continues until you strike the boundary or an orb.

To move:

 - Press space to accelerate forwards.
 - Press the left, right, up and down keys to steer in that direction.
 - Decelaration happens automatically over time
 - To exit, press escape
 - If you lose and want to play again, press primary enter (not keypad enter)

To compile and play (Linux only):
 - Ensure you have g++, libsdl2-dev and libsdl2-ttf-dev installed
 - Download the source code
 - Navigate to the root folder and run ```make fullstack```

Alternatively to just play:
On linux:
 - Download the normallight binary file and the ttf font file and ensure they're placed in the same directory
 - Run ```chmod 700 normallight``` from the directory you placed it into to allow it to be runnable, then run with ```./normallight```
On Windows:
 - Download the ```windows package.zip``` and extract it anywhere. Then run the executable
