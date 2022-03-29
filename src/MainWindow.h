#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtkmm.h>
#include "App.h"

class MainWindow : public Gtk::Window 
{ 
public:
    MainWindow(App* app);

private:
    Glib::RefPtr<Gio::SimpleActionGroup> fileGroup;   // Action group for the "File" menu

    Gtk::Box vBox;

    // Loads a different lab
    void LoadNew();

    // Writes changes to lab and saves graph
    void Save();

    // Calls Save() and exits
    void SaveAndExit();
};

#endif