#include "MainWindow.h"
#include "NkUtils.h"
#include <gtkmm/builder.h>
#include <beahm/dirutils.h>
#include <gtkmm/menubar.h>

/// MainWindow IMPLEMENTATION ///

MainWindow::MainWindow(App* app, LabData* labData) : 
    vBox(Gtk::ORIENTATION_VERTICAL, 0),
    labData(labData)
{
    set_default_geometry(800, 700);
    set_title(beahm::getFileName(labData->getLabDir()));
    set_position(Gtk::WIN_POS_CENTER);

    /// CREATING MENUS ///

    fileGroup = Gio::SimpleActionGroup::create();

    fileGroup->add_action("open", sigc::mem_fun(*this, &MainWindow::LoadNew));
    fileGroup->add_action("save", sigc::mem_fun(*this, &MainWindow::Save));
    fileGroup->add_action("exit", sigc::mem_fun(*this, &MainWindow::SaveAndExit));
    fileGroup->add_action("cds", sigc::mem_fun(*this, &MainWindow::OpenCDManager));
    fileGroup->add_action("viewcd", sigc::mem_fun(*this, &MainWindow::OpenCDViewer));

    insert_action_group("file", fileGroup);

    app->set_accel_for_action("file.save", "<Primary>s");   // Set Ctrl+S as hotkey for Save() function
    app->set_accel_for_action("file.cds", "<Primary>m");   // Set Ctrl+M as hotkey for OpenCDManager()
    app->set_accel_for_action("file.viewcd", "<Primary>d");  // Set Ctrl+D for OpenCDViewer()

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
    "    <submenu>"
    "      <attribute name='label'>_Edit</attribute>"
    "      <section>"
    "        <item>"
    "          <attribute name='label'>_Collision Domains</attribute>"
    "          <attribute name='action'>file.cds</attribute>"
    "          <attribute name='accel'>&lt;Primary&gt;m</attribute>"
    "        </item>"
    "      </section>"
    "    </submenu>"
    "    <submenu>"
    "      <attribute name='label'>_View</attribute>"
    "      <section>"
    "        <item>"
    "          <attribute name='label'>_Connections</attribute>"
    "          <attribute name='action'>file.viewcd</attribute>"
    "          <attribute name='accel'>&lt;Primary&gt;d</attribute>"
    "        </item>"
    "      </section>"
    "    </submenu>"
    "  </menu>"
    "</interface>";

    Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();   // Builder to extract widgets from markup

    builder->add_from_string(menuMarkup);

    Glib::RefPtr<Gio::Menu> menuObj = Glib::RefPtr<Gio::Menu>::cast_dynamic(builder->get_object("menubar"));
    Gtk::MenuBar* mBar = Gtk::make_managed<Gtk::MenuBar>(menuObj);

    // FINISHED CREATING MENUS //

    add(vBox);

    vBox.pack_start(*mBar, Gtk::PACK_SHRINK, 0);

    show_all_children();

    labData->EnumerateLabDir();   // Initialise labData structures
}

MainWindow::~MainWindow() 
{
    delete labData;  // Free labData
    delete cdm;  // Free CDManager window
    delete cdv;  // Free CDViewer window
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

void MainWindow::OpenCDManager() 
{
    if (cdm == nullptr) 
    {
        cdm = new CDManager(this, labData);
    }
    cdm->show();    
}

void MainWindow::OpenCDViewer() 
{
    if (cdv == nullptr) 
    {
        cdv = new CDViewer(this, labData);
    }
    cdv->show();    
}

/// ToolWindow IMPLEMENTATION ///

ToolWindow::ToolWindow(Gtk::Window* parent, Glib::ustring title, int width, int height)
{
    set_title(title);
    set_default_geometry(width, height);
    set_transient_for(*parent);
}

/// CDManager IMPLEMENTATION ///

CDManager::CDManager(Gtk::Window* parent, LabData* labData) :
    ToolWindow(parent, "Manage Collision Domains", 500, 600),
    vBox(Gtk::ORIENTATION_VERTICAL, 10),
    okBtn("OK"),
    labData(labData),
    addBtn("+")
{
    okBtn.signal_clicked().connect(sigc::mem_fun(*this, &CDManager::onOkPress));
    addBtn.signal_clicked().connect(sigc::mem_fun(*this, &CDManager::onAddPress));

    cdView.set_margin_top(10);
    NkUtils::set_margin_both(cdView, 10);
    NkUtils::set_margin_both(okBtn, 10);
    okBtn.set_margin_bottom(10);
    
    for (const std::string s : labData->cds) // Populate cdBox
    {
        CDRecord* record = Gtk::make_managed<CDRecord>(*this, s);
        cdView.Append(record);
    }

    cdView.GetBox().pack_start(addBtn, Gtk::PACK_SHRINK, 0);

    add(vBox);

    vBox.pack_start(cdView, Gtk::PACK_EXPAND_WIDGET, 0);
    vBox.pack_start(okBtn, Gtk::PACK_SHRINK, 0);

    show_all_children();
}

void CDManager::onOkPress() 
{
    hide();
}

void CDManager::onAddPress() 
{
    CDRecord* record = Gtk::make_managed<CDRecord>(*this, "New Collision Domain");
    cdView.Append(record);
    cdView.GetBox().reorder_child(addBtn, -1);  // Move addBtn to end

    cdView.RefreshView();
}

void CDManager::onDelPress(CDRecord* record) 
{
    for (int i = 0; i < labData->cds.size(); i++)  
    {
        if (!record->cd.compare(labData->cds[i]))  // Find collision domain and remove it from cds
        {
            labData->cds.erase(labData->cds.begin() + i);
            break;
        }
    }

    cdView.Remove(record);   // Remove from view
}

CDManager::CDRecord::CDRecord(CDManager& parent, Glib::ustring cd) :
    Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5),
    delBtn("Delete"),
    ld(parent.labData),
    cd(cd)
{
    cdEntry.set_text(cd);
    cdEntry.signal_activate().connect(sigc::mem_fun(*this, &CDManager::CDRecord::onFinishEditing));
    cdEntry.signal_focus_out_event().connect(sigc::mem_fun(*this, &CDManager::CDRecord::onFocusOut));

    delBtn.signal_clicked().connect(sigc::bind(sigc::mem_fun(parent, &CDManager::onDelPress), this));

    pack_start(cdEntry, Gtk::PACK_EXPAND_WIDGET, 0);
    pack_start(delBtn, Gtk::PACK_SHRINK, 0);
}

bool CDManager::CDRecord::onFocusOut(GdkEventFocus* gef) 
{
    onFinishEditing();

    return true;
}

void CDManager::CDRecord::onFinishEditing() 
{
    std::string input = cdEntry.get_text();

    if (input.empty() || !input.compare("New Collision Domain")) 
    {
        return;
    }

    int size = ld->cds.size();
    int i;
    for (i = 0; i < size; i++) 
    {
        if (!cd.compare(ld->cds[i])) 
        {
            ld->cds[i] = input;    // Replace old collision domain name
            break;
        }
    }
    if (i == size)   // Old collision domain wasn't found
    {
        ld->cds.push_back(input);
    }

    cd = input;   // Update internal collision domain name
}

/// CDViewer IMPLEMENTATION ///

CDViewer::TreeColumns::TreeColumns() 
{
    add(name);
    add(lm);
}

CDViewer::CDViewer(Gtk::Window* parent, LabData* labData) : 
    ToolWindow(parent, "Connections", 500, 600),
    vBox(Gtk::ORIENTATION_VERTICAL, 10),
    createBtn("Create New Machine"),
    ld(labData)
{
    createBtn.signal_clicked().connect(sigc::mem_fun(*this, &CDViewer::onCreateClick)); // Attach signal handlers
    treeView.signal_row_activated().connect(sigc::mem_fun(*this, &CDViewer::onRowActivated));

    createBtn.set_margin_bottom(10);   // Setting margins
    NkUtils::set_margin_both(createBtn, 10);
    NkUtils::set_margin_both(treeScroll, 10);    
    treeScroll.set_margin_top(10);

    treeStore = Gtk::TreeStore::create(treeColumns);  // Create treeStore
    treeView.set_model(treeStore);
    treeView.set_headers_visible(false);   // Hide column titles

    treeScroll.add(treeView);
    treeScroll.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);

    add(vBox);

    vBox.pack_start(treeScroll, Gtk::PACK_EXPAND_WIDGET, 0);
    vBox.pack_start(createBtn, Gtk::PACK_SHRINK, 0);

    PopulateTreeStore();  // Fill in treeStore structure

    treeView.append_column("Name", treeColumns.name);

    show_all_children();
}

CDViewer::~CDViewer() 
{
    delete[] iterList;   // Free iterList
}

void CDViewer::PopulateTreeStore() 
{
    int size = ld->cds.size();
    iterList = new Gtk::TreeStore::iterator[size];  // Indexes across both lists are the same

    int i;
    for (i = 0; i < size; i++)     // Populate with collision domains
    {
        iterList[i] = treeStore->append();
        
        auto row = *iterList[i];
        row[treeColumns.name] = ld->cds[i];  // Set field values
        row[treeColumns.lm] = nullptr;  // Collision domains are not associated with a lab machine
    }

    for (LabMachine* lm : ld->machines)           // Loop to add each interface into treeView
    {
        for (MachineInterface* mi : lm->interfaces)     // Enumerate interfaces
        {
            if (mi->segment.compare("None"))   // Exclude disconnected interfaces
            {
                for (i = 0; i < size; i++)   // Find collision domain index
                {
                    if (!mi->segment.compare(ld->cds[i])) 
                    {
                        break;
                    }
                }
                auto row = *treeStore->append((*iterList[i]).children());

                int intID = (int)mi->id;
                std::string miRecord = lm->machineName + " (eth" + std::to_string(intID) + ")";  // Format: NAME (INTERFACE)
                
                row[treeColumns.name] = miRecord;  // Append child row to relevant collision domain
                row[treeColumns.lm] = lm;    // Associate lab machine 
            }
        }
    }
}

void CDViewer::onCreateClick() 
{

}

void CDViewer::onRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column) 
{

}

/// MachineDialog IMPLEMENTATION ///

MachineDialog::MachineDialog(LabMachine* lm, Gtk::Window& parent) : 
    Gtk::Dialog(lm->machineName, parent, true)
{  
    add_button("OK", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);

    Gtk::Box* box = get_content_area();
}

void MachineDialog::AwaitChanges() 
{
    run();
}