/*
  WTEngine
  By:  Matthew Evans
  File:  input_manager.hpp

  See LICENSE.txt for copyright information

  Handle player input in its own thread
*/

#ifndef WTE_MGR_INPUT_MANAGER_HPP
#define WTE_MGR_INPUT_MANAGER_HPP

#include <iostream>

#include <allegro5/allegro.h>

#include "manager.hpp"
#include "make_thread.hpp"

#include "..\engine_flags.hpp"
#include "..\input_flags.hpp"
#include "..\alert.hpp"

namespace wte
{

namespace mgr
{

//!  Input manager class
/*!
  Handles input in a thread
*/
class input_manager final : public manager<input_manager>, public make_thread {
    public:
        inline input_manager() {};
        inline ~input_manager() {};

    private:
        //!  Run the input manager
        void run(void);
};

template <> inline bool input_manager::manager<input_manager>::initialized = false;

/*!
*/
inline void input_manager::run(void) {
    ALLEGRO_EVENT_QUEUE* input_queue;
    ALLEGRO_EVENT input_event;

    //  NOTE:  Joystick input is not complete
    //  TODO:  Let user define these in a menu
    struct buttons {
        int fire_1_button = 1; //  B
        int fire_2_button = 0; //  A
        int fire_3_button = 2; //  X
        int select_button = 8;
        int start_button = 9;
    } joy;

    input_flags::unset_all();

    input_queue = al_create_event_queue();
    al_register_event_source(input_queue, al_get_keyboard_event_source());
    if(al_is_joystick_installed()) al_register_event_source(input_queue, al_get_joystick_event_source());

    while(is_running() == true) {
        if(al_reconfigure_joysticks()) {}

        al_get_next_event(input_queue, &input_event);

        //  Clear any active alerts on input event
        if(alert::is_set() &&
           (input_event.type == ALLEGRO_EVENT_KEY_DOWN ||
            input_event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)) {
            alert::clear();
        }

        /* *** PROCESS EVENTS WHILE MENU IS OPENED *** */
        //  Menu input events are cleared in the menu manager
        if(engine_flags::is_set(GAME_MENU_OPENED)) {
            /* *** Keyboard events *** */
            if(input_event.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch(input_event.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                    case ALLEGRO_KEY_W:
                        input_flags::set(INPUT_UP);
                        break;
                    case ALLEGRO_KEY_DOWN:
                    case ALLEGRO_KEY_S:
                        input_flags::set(INPUT_DOWN);
                        break;
                    case ALLEGRO_KEY_LEFT:
                    case ALLEGRO_KEY_A:
                        input_flags::set(INPUT_LEFT);
                        break;
                    case ALLEGRO_KEY_RIGHT:
                    case ALLEGRO_KEY_D:
                        input_flags::set(INPUT_RIGHT);
                        break;
                    case ALLEGRO_KEY_SPACE:
                    case ALLEGRO_KEY_ENTER:
                        input_flags::set(INPUT_MENU_SELECT);
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        //  Can only exit menus if the game is running
                        if(engine_flags::is_set(GAME_STARTED)) {
                            engine_flags::unset(GAME_MENU_OPENED);
                            input_flags::unset_all();
                        }
                        break;
                } //  End switch(input_event.keyboard.keycode)
            } //  End if(input_event.type == ALLEGRO_EVENT_KEY_DOWN)

            /* *** Joystick events *** */
            if(input_event.type == ALLEGRO_EVENT_JOYSTICK_AXIS) {
                //std::cout << input_event.joystick.stick << std::endl;
                switch(input_event.joystick.axis) {
                    case 0: //  X axis
                        if((input_event.joystick.pos < 0)) input_flags::set(INPUT_LEFT);
                        if((input_event.joystick.pos > 0)) input_flags::set(INPUT_RIGHT);
                        break;
                    case 1: //  Y axis
                        if((input_event.joystick.pos < 0)) input_flags::set(INPUT_UP);
                        if((input_event.joystick.pos > 0)) input_flags::set(INPUT_DOWN);
                        break;
                } //  End switch(input_event.joystick.axis)
            } //  End if(input_event.type == ALLEGRO_EVENT_JOYSTICK_AXIS)
            if(input_event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) {
                if((input_event.joystick.button == joy.fire_1_button)) {
                    input_flags::set(INPUT_MENU_SELECT);
                }
                if(input_event.joystick.button == joy.start_button) {
                    if(engine_flags::is_set(GAME_STARTED)) {
                        engine_flags::unset(GAME_MENU_OPENED);
                        input_flags::unset_all();
                    }
                }
            } //  End if(input_event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)
        } //  End menu event processing

        /* *** PROCESS EVENTS WHILE GAME IS RUNNING *** */
        else {
            /* *** Keyboard events *** */
            if(input_event.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch(input_event.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                    case ALLEGRO_KEY_W:
                        input_flags::set(INPUT_UP);
                        break;
                    case ALLEGRO_KEY_DOWN:
                    case ALLEGRO_KEY_S:
                        input_flags::set(INPUT_DOWN);
                        break;
                    case ALLEGRO_KEY_LEFT:
                    case ALLEGRO_KEY_A:
                        input_flags::set(INPUT_LEFT);
                        break;
                    case ALLEGRO_KEY_RIGHT:
                    case ALLEGRO_KEY_D:
                        input_flags::set(INPUT_RIGHT);
                        break;
                    case ALLEGRO_KEY_LCTRL:
                    case ALLEGRO_KEY_RCTRL:
                        input_flags::set(INPUT_ACTION_1);
                        break;
                    case ALLEGRO_KEY_ALT:
                    case ALLEGRO_KEY_ALTGR:
                        input_flags::set(INPUT_ACTION_2);
                        break;
                    case ALLEGRO_KEY_LSHIFT:
                    case ALLEGRO_KEY_RSHIFT:
                        input_flags::set(INPUT_ACTION_3);
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        engine_flags::set(GAME_MENU_OPENED);
                        input_flags::unset_all();
                        break;
                } //  End switch(input_event.keyboard.keycode)
            } //  End if(input_event.type == ALLEGRO_EVENT_KEY_DOWN)
            if(input_event.type == ALLEGRO_EVENT_KEY_UP) {
                switch(input_event.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                    case ALLEGRO_KEY_W:
                        input_flags::unset(INPUT_UP);
                        break;
                    case ALLEGRO_KEY_DOWN:
                    case ALLEGRO_KEY_S:
                        input_flags::unset(INPUT_DOWN);
                        break;
                    case ALLEGRO_KEY_LEFT:
                    case ALLEGRO_KEY_A:
                        input_flags::unset(INPUT_LEFT);
                        break;
                    case ALLEGRO_KEY_RIGHT:
                    case ALLEGRO_KEY_D:
                        input_flags::unset(INPUT_RIGHT);
                        break;
                    case ALLEGRO_KEY_LCTRL:
                    case ALLEGRO_KEY_RCTRL:
                        input_flags::unset(INPUT_ACTION_1);
                        break;
                    case ALLEGRO_KEY_ALT:
                    case ALLEGRO_KEY_ALTGR:
                        input_flags::unset(INPUT_ACTION_2);
                        break;
                    case ALLEGRO_KEY_LSHIFT:
                    case ALLEGRO_KEY_RSHIFT:
                        input_flags::unset(INPUT_ACTION_3);
                        break;
                } //  End switch(input_event.keyboard.keycode)
            } //  End if(input_event.type == ALLEGRO_EVENT_KEY_UP)

            /* *** Joystick events *** */
            if(input_event.type == ALLEGRO_EVENT_JOYSTICK_AXIS) {
                //std::cout << input_event.joystick.stick << std::endl;
                switch(input_event.joystick.axis) {
                    case 0: //  X axis
                        if(input_event.joystick.pos < 0) input_flags::set(INPUT_LEFT);
                        else input_flags::unset(INPUT_LEFT);
                        if(input_event.joystick.pos > 0) input_flags::set(INPUT_RIGHT);
                        else input_flags::unset(INPUT_RIGHT);
                        break;
                    case 1: //  Y axis
                        if(input_event.joystick.pos < 0) input_flags::set(INPUT_UP);
                        else input_flags::unset(INPUT_UP);
                        if(input_event.joystick.pos > 0) input_flags::set(INPUT_DOWN);
                        else input_flags::unset(INPUT_DOWN);
                        break;
                } //  End switch(input_event.joystick.axis)
            } //  End if(input_event.type == ALLEGRO_EVENT_JOYSTICK_AXIS)
            if(input_event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) {
                if(input_event.joystick.button == joy.fire_1_button) input_flags::set(INPUT_ACTION_1);
                if(input_event.joystick.button == joy.fire_2_button) input_flags::set(INPUT_ACTION_2);
                if(input_event.joystick.button == joy.fire_3_button) input_flags::set(INPUT_ACTION_3);
                if(input_event.joystick.button == joy.start_button) {
                    engine_flags::set(GAME_MENU_OPENED);
                    input_flags::unset_all();
                }
            } //  End if(input_event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)
            if(input_event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) {
                if(input_event.joystick.button == joy.fire_1_button) input_flags::unset(INPUT_ACTION_1);
                if(input_event.joystick.button == joy.fire_2_button) input_flags::unset(INPUT_ACTION_2);
                if(input_event.joystick.button == joy.fire_3_button) input_flags::unset(INPUT_ACTION_3);
            } //  End if(input_event.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP)
        } //  End game event processing
    } //  End while(is_running == true)

    al_destroy_event_queue(input_queue);
}

} //  namespace mgr

} //  end namespace wte

#endif
