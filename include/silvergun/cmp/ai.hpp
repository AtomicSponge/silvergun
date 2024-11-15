/*
 * silvergun
 * --------
 * By Matthew Evans
 * See LICENSE.md for copyright information.
 */

#if !defined(SLV_CMP_AI_HPP)
#define SLV_CMP_AI_HPP

#include <functional>

#include "silvergun/cmp/component.hpp"

#include "silvergun/mgr/messages.hpp"
#include "silvergun/mgr/world.hpp"

namespace slv::sys {
  class logic;
}

namespace slv::cmp {

/*!
 * \class ai
 * \brief Tag components to be processed by the Logic system.
 * 
 * Allows functions to be created to define the enabled or disabled logic.
 */
class ai final : public component {
  friend class sys::logic;

  private:
    const std::function<void(const entity_id&)> enabled_ai;   //  AI to run when enabled.
    const std::function<void(const entity_id&)> disabled_ai;  //  AI to run when disabled.

  public:
    /*!
     * \brief Create an AI component with enabled only AI.
     * \param func Function to define AI process.
     */
    ai(const std::function<void(const entity_id&)>& func) :
      enabled(true), enabled_ai(func), disabled_ai([](const entity_id& e_id){}) {};

    /*!
     * \brief Create an AI component with enabled and disabled AI.
     * \param func_a Function to define enabled AI process.
     * \param func_b Function to define disabled AI process.
     */
    ai(
      const std::function<void(const entity_id&)>& func_a,
      const std::function<void(const entity_id&)>& func_b
    ) : enabled(true), enabled_ai(func_a), disabled_ai(func_b) {};

    ai() = delete;    //  Delete default constructor.
    ~ai() = default;  //  Default destructor.

    bool enabled;     //!<  Flag to enable or disable the entity.
};

}

#endif
