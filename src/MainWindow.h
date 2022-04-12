#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "App.h"
#include "LabData.h"
#include "RecordList.h"

#include <giomm/simpleactiongroup.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/combobox.h>
#include <gtkmm/treestore.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/button.h>
#include <gtkmm/dialog.h>

class CDManager;   // Forward declarations
class CDViewer;
class MachineViewer;

/// MainWindow DEFINITION ///

class MainWindow : public Gtk::Window 
{ 
public:
    MainWindow(App* app, LabData* labData);
    ~MainWindow();

    // Calls PopulateTreeStore on CDViewer
    void UpdateCDViewer(); 

private:
    CDManager* cdm = nullptr;  // CDManager window reference
    CDViewer* cdv = nullptr; // CDViewer window reference
    MachineViewer* mcv = nullptr;  // MachineViewer window reference

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

    // Opens a view of machines
    void OpenMachineViewer();
};

/// ToolWindow DEFINITION ///

class ToolWindow : public Gtk::Window    // Provides styling for tool windows
{
public:
    ToolWindow(Gtk::Window* parent, Glib::ustring title, int width, int height);

protected:
    Gtk::Box vBox;
};

/// CDManager DEFINITION ///

class CDManager : public ToolWindow
{
public:
    CDManager(MainWindow* parent, LabData* labData);

private:
    LabData* labData;
    MainWindow* parent;

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

    // Fills the treeStore structure
    void PopulateTreeStore();

private:
    void onRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

    void onOKClick();
    
    // Refreshes treeStore to show any modifications
    void onShow();

    LabData* ld;

    Gtk::ScrolledWindow treeScroll;
    Gtk::Button okBtn;
    Gtk::TreeView treeView;

    Glib::RefPtr<Gtk::TreeStore> treeStore;

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
    MachineDialog(LabMachine* lm, std::vector<std::string>& cds, Gtk::Window& parent);

    // Wrapper around run() that waits for user to finish changing values
    void AwaitChanges(LabData* ld);

private:
    LabMachine* lm;   // The lab machine being modified

    Gtk::Box nameBox;   // Box to arrange nameLbl and nameEntry
    Gtk::Label nameLbl;    // Machine name label
    Gtk::Entry nameEntry;  // Name can be modified here

    struct ListColumns : Gtk::TreeModel::ColumnRecord   // For cdChoice options
    {
        ListColumns();

        Gtk::TreeModelColumn<Glib::ustring> name;  // Name of collision domain
    } comboColumns;

    Glib::RefPtr<Gtk::ListStore> cdChoiceModel;   // List of collision domain options

    class InterfaceRecord : public Gtk::Box {   // A record in the list of machine interfaces
    public:
        InterfaceRecord(MachineDialog& parent, MachineInterface* mi);
        
        MachineInterface* mi;

    private:
        MachineDialog& parent;

        Gtk::Button delBtn;    // To delete record
        Gtk::Label intName;   // Interface name
        Gtk::ComboBox cdChoice;    // Collision domain selection

        // Called when cdChoice selection changes 
        void onComboChange();
    };

    Gtk::Label intLbl;   // Interfaces label
    RecordList intList;  // List of interfaces

    Gtk::Button addBtn;   // To add an interface

    // Fills intList with interface records
    void PopulateInterfaces();

    // Called when an interface is deleted
    void onDelPress(InterfaceRecord* record);

    // Called when an interface is added
    void onAddPress();
};

/// MachineViewer DEFINITION ///

class MachineViewer : public ToolWindow
{
public:
    MachineViewer(MainWindow* parent, LabData* labData);

private:
    LabData* labData;
    MainWindow* parent;

    RecordList machineList;
    Gtk::Button okBtn;
    Gtk::Button addBtn;

    class MachineRecord : public Gtk::Box {   // A record in the list of machines
    public:
        MachineRecord(MachineViewer& parent, LabMachine* lm);
        
        LabMachine* lm;

    private:
        MachineViewer& parent;

        Gtk::Button delBtn;    // To delete record
        Gtk::Button editBtn;   // To edit a machine
        Gtk::Label machineLbl;   // Contains machine name

        void onEditPress();
    };

    void onDelPress(MachineRecord* record);

    void onAddPress();

    void onOKPress();
};

#endif