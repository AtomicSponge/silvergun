/*!
 * \brief WTEngine | File:  background.hpp
 * \author Matthew Evans
 *
 * \version 0.1a
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2020
 *
 * \details Background component.
 */

#ifndef WTE_CMP_BACKGROUND_HPP
#define WTE_CMP_BACKGROUND_HPP

#include <allegro5/allegro.h>

#include "../../engine_cfg_map.hpp"
#include "animator.hpp"

namespace wte
{

namespace cmp
{

//! Background component
/*!
 * Store the background bitmap
 */
class background final : public animator {
    public:
        /*!
         * Static background.
         * \param void
         * \return void
         */
        inline background(const int w, const int h, const std::size_t l) : animator(l, {}) {
            background_bitmap = al_create_bitmap(w, h);
        };

        /*!
         * Background with custom animation.
         * \param void
         * \return void
         */
        inline background(const int w,
                          const int h,
                          const std::size_t l,
                          void func(entity, mgr::entity_manager&, int64_t)) :
        animator(l, func) {
            background_bitmap = al_create_bitmap(w, h);
        };

        /*!
         * Background destructor.
         * \param void
         * \return void
         */
        inline ~background() {
            al_destroy_bitmap(background_bitmap);
        };

        ALLEGRO_BITMAP *background_bitmap;
};

} //  namespace cmp

} //  namespace wte

#endif
