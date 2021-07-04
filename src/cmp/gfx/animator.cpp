/*!
 * WTEngine | File:  animator.cpp
 * 
 * \author Matthew Evans
 * \version 0.2
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2021
 */

#include "wtengine/cmp/gfx/animator.hpp"

namespace wte
{

namespace cmp
{

/*
 *
 */
void animator::run(const entity_id& e_id){
    try { animate(e_id); } catch(const wte_exception& e) { alert::set_alert(e.what()); }
}

/*
 *
 */
void animator::set_drawing(void) { al_set_target_bitmap(internal_bitmap); }

/*
 *
 */
ALLEGRO_BITMAP& animator::get_bitmap(void) const { return *internal_bitmap; }

/*
 *
 */
void animator::set_tint(const ALLEGRO_COLOR& c) {
    tint_color = c;
    tint_set = true;
}

/*
 *
 */
const ALLEGRO_COLOR animator::get_tint(void) const { return tint_color; }

/*
 *
 */
void animator::clear_tint(void) { tint_set = false; }

/*
 *
 */
const bool animator::draw_tinted(void) const { return tint_set; }

} //  namespace cmp

} //  namespace wte
