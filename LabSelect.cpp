#include "LabSelect.h"
#include "NkUtils.h"

/// LABSELECTWIN IMPLEMENTATION ///

LabSelectWin::LabSelectWin() :
    vBox(Gtk::ORIENTATION_VERTICAL, 10),
    chooseBtn("New Lab")
{
    set_default_geometry(500, 600);
    set_title("Select a Lab");

    titleLbl.set_markup("<span font='15'><b>Select a Lab</b></span>");
    recentLbl.set_markup("<span font='12'>Recent Labs</span>");
    chooseLbl.set_markup("<span font='11'>Or Select a New One</span>");
    recentLbl.set_halign(Gtk::ALIGN_START);
    recentLbl.set_margin_start(10);
    chooseLbl.set_margin_bottom(10);

    NkUtils::set_margin_both(chooseBtn, 10);

    projectsView.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);
    NkUtils::set_margin_both(projectsView, 10);

    add(vBox);

    vBox.pack_start(titleLbl, Gtk::PACK_SHRINK, 20);
    vBox.pack_start(recentLbl, Gtk::PACK_SHRINK, 0);
    vBox.pack_start(projectsView, Gtk::PACK_EXPAND_WIDGET, 0);
    vBox.pack_start(chooseBtn, Gtk::PACK_SHRINK, 0);
    vBox.pack_start(chooseLbl, Gtk::PACK_SHRINK, 0);

    show_all_children();
}

Glib::ustring LabSelectWin::AwaitSelection() 
{
    gtk_main();     // Blocks until gtk_main_quit()
    return labDir;
}

void LabSelectWin::on_hide() 
{
    gtk_main_quit();  // Unblock gtk_main() call

    Gtk::Window::on_hide();  // Call base class method
}

void LabSelectWin::onSelectLab(Glib::ustring labDir) 
{
    this->labDir = labDir;

    gtk_main_quit();   // Unblock gtk_main() call
}

/// LABENTRY IMPLEMENTATION ///

LabEntry::LabEntry(LabSelectWin* parent, Glib::ustring labDir) :
    Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5),
    labDirLbl(labDir),
    chooseBtn("Select"),
    labDir(labDir)
{
    chooseBtn.signal_clicked().connect(sigc::bind(sigc::mem_fun(*parent, &LabSelectWin::onSelectLab), this->labDir));

    pack_start(labDirLbl, Gtk::PACK_SHRINK, 0);
    pack_start(chooseBtn, Gtk::PACK_EXPAND_WIDGET, 0);
}