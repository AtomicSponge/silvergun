/*
  WTEngine Demo
  By:  Matthew Evans
  File:  main.cpp

  See LICENSE.txt for copyright information

  Main file for WTEngine demo
*/

#include "include/wte/wte.hpp"

int main(int argc, char **argv) {
    //  Register engine settings
    wte::engine_cfg::reg("TICKS_PER_SECOND=60.0");
    wte::engine_cfg::reg("screen_width=768");
    wte::engine_cfg::reg("screen_height=1024");

    //  Initialize game object
    wte::wte_main my_game;
    my_game.wte_load();

    //  Run the game loop
    my_game.do_game();

    //  Clean up
    my_game.wte_unload();

    return 0; //  Exit program
}
