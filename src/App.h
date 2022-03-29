#ifndef APP_H
#define APP_H

#include <gtkmm.h>

class App : public Gtk::Application 
{
public:
    static Glib::RefPtr<App> create();

protected:
    App();

    // Fires when the application first starts
    void on_activate() override;

private:
    // Fires when a connected window is closed
    void on_hide_window(Gtk::Window* win);

    // Used to connect event handlers and add_window
    void registerWindow(Gtk::Window* win);
};

#endif