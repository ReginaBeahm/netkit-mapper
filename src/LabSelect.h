#ifndef LAB_SELECT_H
#define LAB_SELECT_H

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>

// Window for selecting a lab to graph
class LabSelectWin : public Gtk::Window 
{
public:
    LabSelectWin();

    // Blocks and returns with the user's selected lab directory
    Glib::ustring AwaitSelection();

    // Sets labDir and unblocks AwaitSelection()
    void selectLab(Glib::ustring labDir);

protected:
    void on_hide() override;

private:
    Gtk::Box vBox;

    Gtk::Label titleLbl;
    Gtk::Label recentLbl;
    Gtk::ScrolledWindow projectsView;
    Gtk::Button chooseBtn;
    Gtk::Label chooseLbl;

    Glib::ustring labDir = "";

    // Click handler for chooseBtn
    void onNewLabClick();
};

// A single recent lab entry
class LabEntry : public Gtk::Box 
{
public:
    LabEntry(LabSelectWin* parent, Glib::ustring labDir);

private:
    Gtk::Label labDirLbl;
    Gtk::Button chooseBtn;

    Glib::ustring labDir;
};

#endif