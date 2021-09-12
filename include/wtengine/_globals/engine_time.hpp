/*!
 * WTEngine | File:  engine_time.hpp
 * 
 * \author Matthew Evans
 * \version 0.7
 * \copyright See LICENSE.md for copyright information.
 * \date 2019-2021
 */

#ifndef WTE_ENGINE_TIME_HPP
#define WTE_ENGINE_TIME_HPP

#include <cstdint>

namespace wte {

/*!
 * \class engine_time
 * \brief Tracks current time of the engine.
 */
class engine_time final {
    friend class engine;

    public:
        engine_time() = delete;                       //!<  Delete constructor.
        ~engine_time() = delete;                      //!<  Delete destructor.
        engine_time(const engine_time&) = delete;     //!<  Delete copy constructor.
        void operator=(engine_time const&) = delete;  //!<  Delete assignment operator.

        /*!
         * \brief Check the internal engine timer.
         * \return Timer value.
         */
        static const int64_t check_time(void);

    private:
        /*
         * Sets the internal timer.
         * This is called once during the game loop in class wte_main
         * and when starting/stopping the game.
         */
        static void set_time(
            const int64_t& t
        );

        static int64_t current_time;  //  Track game timer
};

}  //  end namespace wte

#endif
