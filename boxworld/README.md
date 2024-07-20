# Boxworld

A version of Boxworld or Sokoban.

This version of Boxworld has the same 100 levels that are found in the Windows 3.1 version of Boxworld from 1992.

The aim of the game is to place all of the yellow boxes on the spaces with the blue dots. Place boxes by pushing them using the animated character. The boxes change colour to blue when they are placed on the blue dots. You can only push the boxes, they can't be pulled. The level is complete when all boxes are in place.

# usage
        boxworld <options>

        --device,-d - framebuffer device to use (default is /dev/fb0)
        --help,-h - print usage and exit
        --joystick,-j - joystick device to use (default is /dev/input/js0)
        --kmsdrm,-k - use KMS/DRM dumb buffer

## Controls:-
- Move the character via the D-pad.
- (A) move to the next level.
- (B) move to the previous level.
- (X) Undo the last box move. There is only one level of undo.
- (Y) restart the current level from the beginning.

