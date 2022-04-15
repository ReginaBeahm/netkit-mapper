#ifndef NK_UTILS_H
#define NK_UTILS_H

#include <gtkmm/widget.h>
#include <gtkmm/messagedialog.h>

// Namespace containing some useful functions
namespace NkUtils 
{
    /* Sets the margin on the left and right sides of a widget to val, or the top and bottom sides if orientation is set 
    to Gtk::ORIENTATION_VERTICAL */
    void set_margin_both(Gtk::Widget& widget, int val, Gtk::Orientation orientation = Gtk::ORIENTATION_HORIZONTAL);

    // Shows an error dialog with bold text title and description text
    int DialogAlert(Gtk::Window& parent, Gtk::ButtonsType buttons, std::string title, std::string text);
};

#endif