/*!
 * WTEngine | File:  menu_manager.hpp
 * 
 * \author Matthew Evans
 * \version 0.1a
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2020
 */

#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <stdexcept>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#include "wtengine/mgr/menu_manager.hpp"

namespace wte
{

namespace mgr
{

menu_manager::menu_manager() :
menu_width(500), menu_height(400), menu_padding(32), font_size(8), is_button_left(true) {
    menus.clear();
    opened_menus = {};
};

menu_manager::~menu_manager() {
    opened_menus = {};
    menus.clear();
};

void menu_manager::initialize(void) {
    if(menu_font_file.empty()) menu_font = al_create_builtin_font();
    else {
        menu_font = al_load_font(menu_font_file.c_str(), menu_font_size, menu_font_flags);
        if(!menu_font) menu_font = al_create_builtin_font();
    }
    if(!menu_font) throw std::runtime_error("Unable to set font for menus!");

    //  Create the main menu.
    mnu::menu temp_main_menu = mnu::menu("main_menu", "Main Menu");
    if(!new_menu(temp_main_menu)) throw std::runtime_error("Unable to create main menu!");

    //  Create the in-game menu.
    mnu::menu temp_game_menu = mnu::menu("game_menu", "Game Menu");
    if(!new_menu(temp_game_menu)) throw std::runtime_error("Unable to create game menu!");

    //  Set font size.
    font_size = al_get_font_line_height(menu_font);

    //  Create the menu cursor.
    cursor_bitmap = al_create_bitmap(font_size, font_size);

    //  Create the the menu bitmap for rendering.
    al_set_new_bitmap_flags(ALLEGRO_NO_PRESERVE_TEXTURE);
    menu_bitmap = al_create_bitmap(menu_width, menu_height);
    al_set_new_bitmap_flags(ALLEGRO_CONVERT_BITMAP);

    //  Create timer & its queue.
    menu_timer = al_create_timer(1.0f / 30.0f);
    if(!menu_timer) throw std::runtime_error("Failed to create menu timer!");
    menu_event_queue = al_create_event_queue();
    if(!menu_event_queue) throw std::runtime_error("Failed to create menu event queue!");
    al_register_event_source(menu_event_queue, al_get_timer_event_source(menu_timer));
};

void menu_manager::de_init(void) {
    al_destroy_bitmap(menu_bitmap);
    al_destroy_bitmap(cursor_bitmap);
    al_destroy_font(menu_font);
    al_destroy_event_queue(menu_event_queue);
    al_destroy_timer(menu_timer);
};

void menu_manager::set_menu_size(const float& mw, const float& mh, const float& mp) {
    menu_width = mw;
    menu_height = mh;
    menu_padding = mp;

    al_destroy_bitmap(menu_bitmap);
    al_set_new_bitmap_flags(ALLEGRO_NO_PRESERVE_TEXTURE);
    menu_bitmap = al_create_bitmap(menu_width, menu_height);
    al_set_new_bitmap_flags(ALLEGRO_CONVERT_BITMAP);
};

void menu_manager::reload_menu_bitmap(void) {
    al_destroy_bitmap(menu_bitmap);
    al_set_new_bitmap_flags(ALLEGRO_NO_PRESERVE_TEXTURE);
    menu_bitmap = al_create_bitmap(menu_width, menu_height);
    al_set_new_bitmap_flags(ALLEGRO_CONVERT_BITMAP);
};

void menu_manager::set_menu_color(ALLEGRO_COLOR fcolor, ALLEGRO_COLOR bgcolor) {
    menu_font_color = fcolor;
    menu_bg_color = bgcolor;

    al_set_target_bitmap(cursor_bitmap);
    al_clear_to_color(menu_font_color);
};

void menu_manager::set_font_file(const std::string& fname, const int& size, const int& flags) {
    menu_font_file = fname;
    menu_font_size = size;
    menu_font_flags = flags;
};

const bool menu_manager::new_menu(const mnu::menu& new_menu) {
    for(menu_citerator it = menus.begin(); it != menus.end(); it++) {
        if(new_menu.get_id() == (*it)->get_id()) return false;
    }
    menus.push_back(std::make_shared<mnu::menu>(new_menu));
    return true;
};

const mnu::menu_csptr menu_manager::get_menu(const std::string& name) const {
    if(menus.empty()) throw std::runtime_error("No menus have been loaded!");

    for(menu_citerator it = menus.begin(); it != menus.end(); it++) {
        if(name == (*it)->get_id()) return *it;
    }

    //  Menu not found, return main menu or game menu if the game is running
    if(engine_flags::is_set(GAME_STARTED)) {
        for(menu_citerator it = menus.begin(); it != menus.end(); it++) {
            if("game_menu" == (*it)->get_id()) return *it;
        }
    } else {
        for(menu_citerator it = menus.begin(); it != menus.end(); it++) {
            if("main_menu" == (*it)->get_id()) return *it;
        }
    }

    //  Menu still not found - just return the first one in the vector
    return *menus.begin();
};

const mnu::menu_sptr menu_manager::set_menu(const std::string& name) {
    if(menus.empty()) throw std::runtime_error("No menus have been loaded!");

    for(menu_iterator it = menus.begin(); it != menus.end(); it++) {
        if(name == (*it)->get_id()) return *it;
    }

    //  Menu not found - return null pointer
    return nullptr;
};

void menu_manager::reset(void) {
    opened_menus = {};
    engine_flags::unset(GAME_MENU_OPENED);
};

void menu_manager::open_menu(const std::string& menu_id) {
    opened_menus.push(get_menu(menu_id));
    engine_flags::set(GAME_MENU_OPENED);
    menu_position = opened_menus.top()->items_cbegin();

    //  Set default values for any menu settings objects.
    for(auto it = opened_menus.top()->items_cbegin(); it != opened_menus.top()->items_cend(); it++) {
        (*it)->set_default();
    }
};

void menu_manager::close_menu(void) {
    opened_menus.pop();
    if(opened_menus.empty()) engine_flags::unset(GAME_MENU_OPENED);
    else menu_position = opened_menus.top()->items_cbegin();
};

void menu_manager::run(message_manager& messages) {
    if(opened_menus.empty()) {
        //  No menus currently opened, add one to the stack
        if(engine_flags::is_set(GAME_STARTED)) open_menu("game_menu"); //  Add the in-game menu to the stack.
        else open_menu("main_menu"); //  Add the main menu to the stack.
    }

    /* ************************************************************ */
    //  Check for any incoming menu messages and process.
    message_container temp_messages = messages.get_messages("menu");
    for(auto & m_it : temp_messages) {
        //  Reload menu, setting new defaults.
        if(m_it.get_cmd() == "reload") {
            for(auto it = opened_menus.top()->items_cbegin();
                it != opened_menus.top()->items_cend(); it++) {
                (*it)->set_default();
                (*it)->reset_to_default();
            }
        }
    }
    /* ************************************************************ */

    /* ************************************************************ */
    //  Iterate through the menu items depending on key press.
    if(input_flags::check_button_event(WTE_INPUT_BUTTON_UP, WTE_BUTTON_EVENT_DOWN) &&
       menu_position != opened_menus.top()->items_cbegin()) menu_position--;
    if(input_flags::check_button_event(WTE_INPUT_BUTTON_DOWN, WTE_BUTTON_EVENT_DOWN) &&
       menu_position != --opened_menus.top()->items_cend()) menu_position++;
    /* ************************************************************ */

    /* ************************************************************ */
    //  Switch through a menu item's options depending on key press.
    //  The longer the button is held down, the faster it will scroll through options.
    if(input_flags::check_button_event(WTE_INPUT_BUTTON_LEFT, WTE_BUTTON_EVENT_DOWN) &&
       menu_position != opened_menus.top()->items_cend())
    {
        al_start_timer(menu_timer);
        is_button_left = true;
    }
    if(input_flags::check_button_event(WTE_INPUT_BUTTON_RIGHT, WTE_BUTTON_EVENT_DOWN) &&
       menu_position != opened_menus.top()->items_cend())
    {
        al_start_timer(menu_timer);
        is_button_left = false;
    }

    if(input_flags::check_button_event(WTE_INPUT_BUTTON_LEFT, WTE_BUTTON_EVENT_UP)) {
        al_stop_timer(menu_timer);
        al_set_timer_count(menu_timer, 0);
    }
    if(input_flags::check_button_event(WTE_INPUT_BUTTON_RIGHT, WTE_BUTTON_EVENT_UP)) {
        al_stop_timer(menu_timer);
        al_set_timer_count(menu_timer, 0);
    }

    bool toggle_menu_item = false;
    ALLEGRO_EVENT event;
    const bool queue_not_empty = al_get_next_event(menu_event_queue, &event);
    if(queue_not_empty && event.type == ALLEGRO_EVENT_TIMER) {
        if(al_get_timer_count(menu_timer) == 1) toggle_menu_item = true;
        if(al_get_timer_count(menu_timer) >= 20) {
            if(al_get_timer_count(menu_timer) >= 70) toggle_menu_item = true;
            else if(al_get_timer_count(menu_timer) % 5 == 0) toggle_menu_item = true;
        }
    }

    if(toggle_menu_item) {
        if(is_button_left) (*menu_position)->on_left();
        else (*menu_position)->on_right();
    }
    //  End left/right input.
    /* ************************************************************ */

    /* ************************************************************ */
    //  Menu item was selected, process what happens.
    if(input_flags::check_button_event(WTE_INPUT_MENU_SELECT, WTE_BUTTON_EVENT_DOWN) &&
       menu_position != opened_menus.top()->items_cend())
    {
        message temp_msg = (*menu_position)->on_select();
        /* *** Check if the message is for the menu system and process. ********* */
        if(temp_msg.get_sys() == "menu") {

            /* *** MENU APPLY ACTION ********* */
            if(temp_msg.get_cmd() == "apply") {
                //  Do apply.  Go through menu items, find all actionable menu objects and process.
                std::string eng_settings_string = "";
                std::string game_settings_string = "";

                for(auto it = opened_menus.top()->items_cbegin(); it != opened_menus.top()->items_cend(); it++) {
                    //  Process menu selection objects.
                    if(std::dynamic_pointer_cast<mnu::menu_item_selection>(*it)) {
                        //  See if the selection is a game or engine setting, add to correct string.
                        if((*it)->is_engine_setting()) {
                            if(!eng_settings_string.empty()) eng_settings_string += ";";
                            eng_settings_string += std::static_pointer_cast<mnu::menu_item_selection>(*it)->get_setting();
                        } else {
                            if(!game_settings_string.empty()) game_settings_string += ";";
                            game_settings_string += std::static_pointer_cast<mnu::menu_item_selection>(*it)->get_setting();
                        }
                    }

                    //  Process menu toggle objects.
                    if(std::dynamic_pointer_cast<mnu::menu_item_toggle>(*it)) {
                        messages.add_message(message("system",
                                             std::static_pointer_cast<mnu::menu_item_toggle>(*it)->get_active_cmd(),
                                             std::static_pointer_cast<mnu::menu_item_toggle>(*it)->get_active_args()));
                    }

                    //  Add additional item processing here.
                }

                //  Send messages in reverse order for system to process correctly.
                messages.add_message(message("system", "alert", "Settings applied."));

                //  Apply engine settings if any.
                if(!eng_settings_string.empty()) {
                    //  Check various apply events.
                    for(auto it = opened_menus.top()->items_cbegin(); it != opened_menus.top()->items_cend(); it++) {
                        //  Check to see if the display needs to be reconfigured.
                        if((*it)->setting_changed() && (*it)->get_setting_type() == mnu::ENGINE_SETTING_RECONF)
                            messages.add_message(message("system", "reconf_display", ""));
                        //  Check to see if the audio levels need to be set.
                        if((*it)->setting_changed() && (*it)->get_setting_type() == mnu::AUDIO_SETTING)
                            messages.add_message(message("audio", "set_volume", ""));
                    }
                    messages.add_message(message("system", "set_engcfg", eng_settings_string));
                }

                //  Apply game settings if any.
                if(!game_settings_string.empty()) {
                    messages.add_message(message("system", "set_gamecfg", game_settings_string));
                }

                //  Send a reload command to the menus.
                messages.add_message(message("menu", "reload", ""));
            }
            /* *** END MENU APPLY ACTION ********* */

            /* *** MENU CANCEL ACTION ********* */
            if(temp_msg.get_cmd() == "cancel") {
                //  Do cancel.  Go through menu items, reset menu settings objects to their defaults.
                for(auto it = opened_menus.top()->items_cbegin(); it != opened_menus.top()->items_cend(); it++) {
                    (*it)->reset_to_default();
                }
            }
            /* *** END MENU CANCEL ACTION ********* */
        }
        /* *** END MENU MESSAGE PROCESSING ********* */

        //  If the message is not for the menu system, add to the message queue.
        else if(temp_msg.get_cmd() != "null") messages.add_message(temp_msg);
    }
    //  End menu item processing.
    /* ************************************************************ */

    //  Check for close menu input.
    if(input_flags::check_button_event(WTE_INPUT_MENU_CLOSE, WTE_BUTTON_EVENT_DOWN)) close_menu();
}

ALLEGRO_BITMAP& menu_manager::render_menu(void) const {
    //  Set drawing to the menu bitmap.
    al_set_target_bitmap(menu_bitmap);
    al_clear_to_color(menu_bg_color);

    //  If the menu stack is empty then the run member hasn't been called yet.
    //  Return a blank bitmap for now.
    if(opened_menus.empty()) return *menu_bitmap;

    //  Render menu title.
    al_draw_text(menu_font, menu_font_color, menu_width / 2, menu_padding,
                 ALLEGRO_ALIGN_CENTER, opened_menus.top()->get_title().c_str());

    //  Render menu items.
    float cursor_pos = 10.0;
    float vpart = 0.0, hpart = 0.0, offset = 0.0;
    std::size_t vcounter = 0;

    offset = menu_padding + font_size + menu_padding;
    vpart = (menu_height - offset) / (opened_menus.top()->num_items() + 1);
    for(auto it = opened_menus.top()->items_cbegin(); it != opened_menus.top()->items_cend(); it++) {
        vcounter++;
        hpart = menu_width / ((*it)->get_text().size() + 1);
        for(std::size_t i = 0; i < (*it)->get_text().size(); i++)
            al_draw_text(menu_font, menu_font_color, hpart * (i + 1), (offset / 2) + (vpart * vcounter),
                         ALLEGRO_ALIGN_CENTER, (*it)->get_text()[i].c_str());
        if(it == menu_position) cursor_pos = (offset / 2) + (vpart * vcounter);
    }

    //  Render menu cursor.
    if(opened_menus.top()->num_items() != 0) al_draw_bitmap(cursor_bitmap, menu_padding, cursor_pos, 0);

    return *menu_bitmap;
}

}  // end namespace mgr

}  // end namespace wte