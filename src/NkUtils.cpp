
#include "NkUtils.h"

void NkUtils::set_margin_both(Gtk::Widget& widget, int val, Gtk::Orientation orientation /*= Gtk::ORIENTATION_HORIZONTAL*/) 
{
    if (orientation == Gtk::ORIENTATION_VERTICAL) 
    {
        widget.set_margin_bottom(val);
        widget.set_margin_top(val);
    }
    else 
    {
        widget.set_margin_start(val);
        widget.set_margin_end(val);
    }
}