#ifndef NK_UTILS_H
#define NK_UTILS_H

#include <gtkmm.h>

// Class containing some useful static functions
class NkUtils 
{
public:
    /* Sets the margin on the left and right sides of a widget to val, or the top and bottom sides if orientation is set 
    to Gtk::ORIENTATION_VERTICAL */
    static void set_margin_both(Gtk::Widget& widget, int val, Gtk::Orientation orientation = Gtk::ORIENTATION_HORIZONTAL);
};

#endif