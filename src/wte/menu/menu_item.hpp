/*
  WTEngine
  By:  Matthew Evans
  File:  menu_item.hpp

  See LICENSE.txt for copyright information

  A menu item
*/

#ifndef WTE_MENU_MENU_ITEM_HPP
#define WTE_MENU_MENU_ITEM_HPP

#include <string>
#include <vector>
#include <memory>

namespace wte
{

namespace mnu
{

//!
/*!
*/
class menu_item {
    public:
        inline virtual ~menu_item() {};

        virtual std::vector<std::string> get_text(void);
        virtual void do_item(void);

    private:

    protected:
        inline menu_item() {};
};

//! Menu Item shared pointer
typedef std::shared_ptr<menu_item> menu_item_sptr;

}  // end namespace mnu

}  // end namespace wte

#endif
