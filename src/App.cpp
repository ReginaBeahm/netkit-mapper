#include "App.h"
#include "LabSelect.h"
#include "MainWindow.h"
#include "LabData.h"
#include <iostream>

App::App() : Gtk::Application("gd.rf.floydsite.netkitmapper")
{

}

Glib::RefPtr<App> App::create() 
{
    return Glib::RefPtr<App>(new App());
}

void App::on_activate() 
{
    LabSelectWin win1;  // Stack because win1 should only exist during this function's lifetime
    add_window(win1);
    win1.show();

    Glib::ustring labDir = win1.AwaitSelection();
    std::cout << "Given Lab Directory: \"" << labDir << "\"" << std::endl;

    if (!labDir.empty())   // Only proceed if user provided a lab directory
    {
        LabData ld(labDir);   // Initialise LabData structure

        MainWindow* win2 = new MainWindow(this);
        registerWindow(win2);
        win2->show();
    }
}

void App::on_hide_window(Gtk::Window* win) 
{
    delete win;   // Free memory
}

void App::registerWindow(Gtk::Window* win) 
{
    add_window(*win);
    win->signal_hide().connect(sigc::bind(sigc::mem_fun(*this, &App::on_hide_window), win));
}