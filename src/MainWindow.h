#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "App.h"
#include "LabData.h"
#include "RecordList.h"

#include <giomm/simpleactiongroup.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/dialog.h>

class CDManager;   // Forward declarations
class CDViewer;

/// MainWindow DEFINITION ///

class MainWindow : public Gtk::Window 
{ 
public:
    MainWindow(App* app, LabData* labData);
    ~MainWindow();

private:
    CDManager* cdm = nullptr;  // CDManager window reference
    CDViewer* cdv = nullptr; // CDViewer window reference

    Glib::RefPtr<Gio::SimpleActionGroup> fileGroup;   // Action group for the "File" and "Edit" menus

    Gtk::Box vBox;

    LabData* labData;   // Data about the current lab

    // Loads a different lab
    void LoadNew();

    // Writes changes to lab and saves graph
    void Save();

    // Calls Save() and exits
    void SaveAndExit();

    // Allows user to make a new connection
    void OpenCDManager();

    // Opens a view of collision domains
    void OpenCDViewer();
};

/// ToolWindow DEFINITION ///

class ToolWindow : public Gtk::Window    // Provides styling for tool windows
{
public:
    ToolWindow(Gtk::Window* parent, Glib::ustring title, int width, int height);
};

/// CDManager DEFINITION ///

class CDManager : public ToolWindow
{
public:
    CDManager(Gtk::Window* parent, LabData* labData);

private:
    LabData* labData;

    Gtk::Box vBox;
    RecordList cdView;
    Gtk::Button okBtn;
    Gtk::Button addBtn;

    class CDRecord : public Gtk::Box {   // A record in the list of collision domains
    public:
        CDRecord(CDManager& parent, Glib::ustring cd);

        Glib::ustring cd;   // Collision domain name

    private:
        LabData* ld;

        Gtk::Button delBtn;    // To delete record
        Gtk::Entry cdEntry;

        void onFinishEditing();   // Called when user goes out of focus of the entry or when pressing enter
        bool onFocusOut(GdkEventFocus* gef);
    };

    void onOkPress();
    void onDelPress(CDRecord* record);
    void onAddPress();
};

/// CDViewer DEFINITION ///

class CDViewer : public ToolWindow 
{
public:
    CDViewer(Gtk::Window* parent, LabData* labData);
    ~CDViewer();

private:
    void onRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

    void onCreateClick();
    
    // Refreshes treeStore to show any modifications
    void onShow();

    // Fills the treeStore structure
    void PopulateTreeStore();

    LabData* ld;

    Gtk::Box vBox;
    Gtk::ScrolledWindow treeScroll;
    Gtk::Button createBtn;
    Gtk::TreeView treeView;

    Glib::RefPtr<Gtk::TreeStore> treeStore;

    Gtk::TreeModel::iterator* iterList;  // Contains each iterator associated with each collision domain

    struct TreeColumns : Gtk::TreeModel::ColumnRecord 
    {
        TreeColumns();

        Gtk::TreeModelColumn<Glib::ustring> name;
        Gtk::TreeModelColumn<LabMachine*> lm;  // Pointer the lab machine a row represents
    } treeColumns;
};

/// MachineDialog DEFINITION ///

class MachineDialog : public Gtk::Dialog 
{
public:
    MachineDialog(LabMachine* lm, Gtk::Window& parent);

    // Wrapper around run() that also commits user changes
    void AwaitChanges();

private:
    Gtk::Label nameLbl;    // Machine name label
    Gtk::Entry nameEntry;  // Name can be modified here

    Gtk::Label intLbl;   // Interfaces label
    RecordList intList;  // List of interfaces

    Gtk::Button addBtn;   // To add an interface
};

#endif