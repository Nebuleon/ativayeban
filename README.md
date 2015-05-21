# Falling Time

Like the fall down games, you are a ball and you must fall into the holes to avoid being crushed by the top of the screen!

[![Gameplay video](http://img.youtube.com/vi/3MVMJkOYHSg/0.jpg)](https://www.youtube.com/watch?v=3MVMJkOYHSg)

![gameplay](https://github.com/cxong/fallingtime/blob/master/screenshot.png)

This game requires SDL 2, CMake and a C compiler. It is optimised for low-resolution screens.

Code is under the GPL version 2; for asset licenses see the `COPYRIGHT` file.

### Building

Use **CMake**, and the libraries **SDL 2**, **SDL_image**, **SDL_mixer** and **SDL_ttf**.

To compile this for PC, use `cmake . && make`. You'll then get a windowed SDL game.

To compile this for GCW-Zero, run `pkg/make_opk.sh` after installing the toolchain as specified in the developer docs.

### Notes

The game uses a custom version of Chipmunk 2D physics; it cannot be replaced with standard libraries.
