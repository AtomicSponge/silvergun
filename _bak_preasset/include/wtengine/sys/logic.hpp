/*!
 * WTEngine | File:  logic.hpp
 * 
 * \author Matthew Evans
 * \version 0.5
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2021
 */

#ifndef WTE_SYS_LOGIC_HPP
#define WTE_SYS_LOGIC_HPP

#include "wtengine/sys/system.hpp"

namespace wte
{

namespace sys
{

/*!
 * \class logic
 * \brief Processes entities that have ai components.
 * 
 * Also sends messages to entities with dispatch components.
 */
class logic final : public system {
    public:
        logic();
        ~logic();

        /*!
         * \brief Finds all entities with an ai component and processes their logic.
         */
        void run(void) override;
};

} //  namespace sys

} //  namespace wte

#endif
