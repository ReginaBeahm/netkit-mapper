#include "MainWindow.h"
#include <gtkmm/builder.h>
#include <gtkmm/menubar.h>

MainWindow::MainWindow(App* app) : 
    vBox(Gtk::ORIENTATION_VERTICAL, 0)
{
    set_default_geometry(800, 700);
    set_title("[Lab Name]");  // To be set

    /// CREATING "File" MENU ///

    fileGroup = Gio::SimpleActionGroup::create();

    fileGroup->add_action("open", sigc::mem_fun(*this, &MainWindow::LoadNew));
    fileGroup->add_action("save", sigc::mem_fun(*this, &MainWindow::Save));
    fileGroup->add_action("exit", sigc::mem_fun(*this, &MainWindow::SaveAndExit));

    insert_action_group("file", fileGroup);

    app->set_accel_for_action("file.save", "<Primary>s");   // Set Ctrl+S as hotkey for Save() function

    const char* menuMarkup =     // Menu definition
    "<interface>"
    "  <menu id='menubar'>"
    "    <submenu>"
    "      <attribute name='label'>_File</attribute>"
    "      <section>"
    "        <item>"
    "          <attribute name='label'>_Open</attribute>"
    "          <attribute name='action'>file.open</attribute>"
    "        </item>"
    "      </section>"
    "      <section>"
    "        <item>"
    "          <attribute name='label'>_Save</attribute>"
    "          <attribute name='action'>file.save</attribute>"
    "          <attribute name='accel'>&lt;Primary&gt;s</attribute>"
    "        </item>"
    "      </section>"
    "      <section>"
    "        <item>"
    "          <attribute name='label'>_Save &amp; Exit</attribute>"
    "          <attribute name='action'>file.exit</attribute>"
    "        </item>"
    "      </section>"
    "    </submenu>"
    "  </menu>"
    "</interface>";

    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();   // Builder to extract widgets from markup

    builder->add_from_string(menuMarkup);

    Glib::RefPtr<Gio::Menu> menuObj = Glib::RefPtr<Gio::Menu>::cast_dynamic(builder->get_object("menubar"));
    Gtk::MenuBar* mBar = Gtk::make_managed<Gtk::MenuBar>(menuObj);

    // FINISHED CREATING "File" MENU //

    add(vBox);

    vBox.pack_start(*mBar, Gtk::PACK_SHRINK, 0);

    show_all_children();
}

void MainWindow::LoadNew()
{

}

void MainWindow::Save()
{

}

void MainWindow::SaveAndExit() 
{

}