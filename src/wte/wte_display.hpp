/*!
 * \brief WTEngine | File:  wte_display.hpp
 * \author Matthew Evans
 *
 * \version 0.1a
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2020
 *
 * \details Display
 */

#ifndef WTE_DISPLAY_HPP
#define WTE_DISPLAY_HPP

#include <allegro5/allegro.h>

#include <string>

#include "_globals/engine_cfg_map.hpp"
#include "mgr/render_manager.hpp"

namespace wte
{

/*!
 * \class Display class
 */
class wte_display {
    public:
        /*!
         * Display destructor.
         * \param void
         * \return void
         */
        inline virtual ~wte_display() {};

        /*!
         * Get the window title.
         * \param void
         * \return Window title as string.
         */
        inline const std::string get_window_title(void) const {
            return window_title;
        };

    protected:
        /*!
         * Display constructor.
         * \param title Window title.
         * \return void
         */
        inline wte_display(const std::string title) : window_title(title) {};

        /*!
         * Configure the display.
         * \param void
         * \return void
         */
        inline void create_display(void) {
            al_reset_new_display_options();

            //  Configure vsync options.  Gfx driver may override this.
            if(!engine_cfg::is_reg("vsync")) engine_cfg::reg("vsync=0");
            if(engine_cfg::get<int>("vsync") >= 0 || engine_cfg::get<int>("vsync") <= 2) {
                al_set_new_display_option(ALLEGRO_VSYNC, engine_cfg::get<int>("vsync"), ALLEGRO_SUGGEST);
            }
            else {
                al_set_new_display_option(ALLEGRO_VSYNC, 0, ALLEGRO_SUGGEST);
                engine_cfg::set("vsync=0");
            }

            //  Set screen resolution.
            if(!engine_cfg::is_reg("resolution")) throw std::runtime_error("Screen resolution not defined!");
            int screen_w = std::stoi(engine_cfg::get("resolution").substr(0, engine_cfg::get("resolution").find("x")));
            int screen_h = std::stoi(engine_cfg::get("resolution").substr(engine_cfg::get("resolution").find("x") + 1,
                                                                        engine_cfg::get("resolution").length()));

            //  Check if a display mode is set.
            if(engine_cfg::is_reg("display_mode")) {
                if(engine_cfg::get("display_mode") == "windowed") al_set_new_display_flags(ALLEGRO_WINDOWED);
                else if(engine_cfg::get("display_mode") == "windowed_full_screen") al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
                else if(engine_cfg::get("display_mode") == "full_screen") al_set_new_display_flags(ALLEGRO_FULLSCREEN);
                else al_set_new_display_flags(ALLEGRO_WINDOWED);
            } else {
                engine_cfg::reg("display_mode=windowed");
                al_set_new_display_flags(ALLEGRO_WINDOWED);
            }

            //  Set the display.
            display = al_create_display(screen_w, screen_h);

            //  Display failed to load, try a fallback.
            if(!display) {
                al_set_new_display_flags(ALLEGRO_WINDOWED);
                display = al_create_display(mgr::render_manager::get_arena_width(),
                                            mgr::render_manager::get_arena_height());
                if(!display) throw std::runtime_error("Failed to configure display!");
                engine_cfg::set("display_mode=windowed");
                std::string new_res = std::to_string(mgr::render_manager::get_arena_width()) +
                                    "x" + std::to_string(mgr::render_manager::get_arena_height());
                engine_cfg::set("resolution", new_res);
                screen_w = mgr::render_manager::get_arena_width();
                screen_h = mgr::render_manager::get_arena_height();
            }

            al_set_window_title(display, window_title.c_str());
            screen.update_resolution(screen_w, screen_h);

            //  Set the scale factor.
            if(engine_cfg::is_reg("scale_factor")) {
                if(engine_cfg::get("scale_factor") == "1") screen.set_scale_factor(1.0f);
                else if(engine_cfg::get("scale_factor") == "1.25") screen.set_scale_factor(1.25f);
                else if(engine_cfg::get("scale_factor") == "1.5") screen.set_scale_factor(1.5f);
                else if(engine_cfg::get("scale_factor") == "1.75") screen.set_scale_factor(1.75f);
                else if(engine_cfg::get("scale_factor") == "2") screen.set_scale_factor(2.0f);
                else {
                    screen.set_scale_factor(1.0f);
                    engine_cfg::set("scale_factor=1");
                }
            } else {
                screen.set_scale_factor(1.0f);
                engine_cfg::reg("scale_factor=1");
            }
        };

        /*!
         * Destroy the display.
         * \param void
         * \return void
         */
        inline void destroy_display(void) {
            al_destroy_display(display);
        };

        /*!
         * Reconfigure the display.
         * \param void
         * \return void
         */
        inline void reconf_display(void) {
            destroy_display();
            create_display();
            //al_convert_memory_bitmaps();
        };

        //  Declare the render manager.
        mgr::render_manager screen;

        //  Allegro object for the display.
        inline static ALLEGRO_DISPLAY* display = NULL;

    private:
        std::string window_title;  //  Title for application window.
};

} //  end namespace wte

#endif