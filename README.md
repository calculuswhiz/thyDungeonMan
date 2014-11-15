Thy Dungeonman! Thy Hunger!
=============

YOU ARE THY DUNGEONMAN!

Thy Dungeonman implemented with ncurses. (Therefore, Linux only.)  Play the original at http://www.homestarrunner.com/dungeonman.html.

This is a command-line game. **To play the full version, you need the library csfml**, a multimedia library with C bindings. To play without sound, all you need is ncurses, which your system probably has installed already.

  -```make``` for full version with sound.
  
  -```make nosound``` if you just have ncurses.

Makes an executable in the **parent directory**. To run, just type ```../dungeonGame``` after installing. No parameters.

Plays just like the original game. Looks a little different (e.g. It doesn't do the character by character display), but I did fix one bug, so there is that.

Oh, you can also type 'exit'. Returns -1 on exit. Returns 0 on win. (New .bashrc/.zshrc anyone?) I forget what happens when you press ```ctrl+'c'``` though...
