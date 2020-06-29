/*!
 * \brief WTEngine | File:  enabled.hpp
 * \author Matthew Evans
 *
 * \version 0.1a
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2020
 *
 * \details Enabled component.
 */

#ifndef WTE_CMP_ENABLED_HPP
#define WTE_CMP_ENABLED_HPP

#include "component.hpp"

namespace wte
{

namespace cmp
{

/*!
 * \class Enabled component
 * Toggle an entity to enabled or disabled state
 */
class enabled final : public component {
    public:
        /*!
         * \brief Enabled constructor.
         * If passed nothing, enabled by default.
         * \param void
         * \return void
         */
        inline enabled() : is_enabled(true) {};

        /*!
         * \brief Enabled constructor.
         * Pass boolean value to the component to set if it is enabled.
         * \param void
         * \return void
         */
        inline enabled(const bool e) : is_enabled(e) {};

        /*!
         * Check if the entity is enabled.
         * \param void
         * \return True if enabled, false if not.
         */
        inline const bool check(void) const {
            return is_enabled;
        };

        /*!
         * \brief Set enabled.
         * Sets internal flag to true.
         * \param void
         * \return void
         */
        inline void enable(void) {
            is_enabled = true;
        };

        /*!
         * \brief Set disabled.
         * Sets internal flag to false.
         * \param void
         * \return void
         */
        inline void disable(void) {
            is_enabled = false;
        };

    private:
        bool is_enabled;
};

} //  namespace cmp

} //  namespace wte

#endif
