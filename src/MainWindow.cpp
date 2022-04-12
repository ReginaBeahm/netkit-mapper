#include "MainWindow.h"
#include "NkUtils.h"
#include <gtkmm/builder.h>
#include <beahm/dirutils.h>
#include <filesystem>
#include <fstream>
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
    fileGroup->add_action("viewmac", sigc::mem_fun(*this, &MainWindow::OpenMachineViewer));

    insert_action_group("file", fileGroup);

    app->set_accel_for_action("file.save", "<Primary>s");   // Set Ctrl+S as hotkey for Save() function
    app->set_accel_for_action("file.cds", "<Primary>m");   // Set Ctrl+M as hotkey for OpenCDManager()
    app->set_accel_for_action("file.viewcd", "<Primary>d");  // Set Ctrl+D for OpenCDViewer()
    app->set_accel_for_action("file.viewmac", "<Primary>i"); // Set Ctrl+I for OpenMachineViewer()

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
    "      <section>"
    "        <item>"
    "          <attribute name='label'>_Machines</attribute>"
    "          <attribute name='action'>file.viewmac</attribute>"
    "          <attribute name='accel'>&lt;Primary&gt;i</attribute>"
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
    delete mcv;
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

void MainWindow::UpdateCDViewer() 
{
    if (cdv != nullptr) 
    {
        cdv->PopulateTreeStore();
    }
}

void MainWindow::OpenCDViewer() 
{
    if (cdv == nullptr) 
    {
        cdv = new CDViewer(this, labData);
        cdv->PopulateTreeStore();  // Initial tree store population
    }
    cdv->show();
}

void MainWindow::OpenMachineViewer() 
{
    if (mcv == nullptr) 
    {
        mcv = new MachineViewer(this, labData);
    }
    mcv->show();
}

/// ToolWindow IMPLEMENTATION ///

ToolWindow::ToolWindow(Gtk::Window* parent, Glib::ustring title, int width, int height) :
    vBox(Gtk::ORIENTATION_VERTICAL, 10)
{
    set_title(title);
    set_default_geometry(width, height);
    set_transient_for(*parent);
    set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
}

/// CDManager IMPLEMENTATION ///

CDManager::CDManager(MainWindow* parent, LabData* labData) :
    ToolWindow(parent, "Manage Collision Domains", 500, 600),
    okBtn("OK"),
    labData(labData),
    addBtn("+"),
    parent(parent)
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
    
    parent->UpdateCDViewer();   // Update CDViewer with changes
}

void CDManager::onAddPress() 
{
    CDRecord* record = Gtk::make_managed<CDRecord>(*this, "New Collision Domain");
    cdView.Append(record);

    record->show();
    record->show_all_children();
    
    cdView.GetBox().reorder_child(addBtn, -1);  // Move addBtn to end
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

    // Now must update every interface that was attached to the deleted collision domain
    for (const LabMachine* lm : labData->machines)
    {
        for (MachineInterface* mi : lm->interfaces) 
        {
            if (!mi->segment.compare(record->cd)) 
            {
                mi->segment = "None";    // Disconnect interface
            }
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

    if (input.empty() || !input.compare(cd) || !input.compare("New Collision Domain")) 
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
            // Now must update every interface that was attached to the renamed collision domain
            for (const LabMachine* lm : ld->machines)
            {
                for (MachineInterface* mi : lm->interfaces) 
                {
                    if (!mi->segment.compare(cd)) 
                    {
                        mi->segment = input;
                    }
                }
            }
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
    okBtn("OK"),
    ld(labData)
{
    okBtn.signal_clicked().connect(sigc::mem_fun(*this, &CDViewer::onOKClick)); // Attach signal handlers
    treeView.signal_row_activated().connect(sigc::mem_fun(*this, &CDViewer::onRowActivated));

    okBtn.set_margin_bottom(10);   // Setting margins
    NkUtils::set_margin_both(okBtn, 10);
    NkUtils::set_margin_both(treeScroll, 10);    
    treeScroll.set_margin_top(10);

    treeStore = Gtk::TreeStore::create(treeColumns);  // Create treeStore
    treeView.set_model(treeStore);
    treeView.set_headers_visible(false);   // Hide column titles

    treeScroll.add(treeView);
    treeScroll.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);

    add(vBox);

    vBox.pack_start(treeScroll, Gtk::PACK_EXPAND_WIDGET, 0);
    vBox.pack_start(okBtn, Gtk::PACK_SHRINK, 0);

    treeView.append_column("Name", treeColumns.name);

    show_all_children();
}

void CDViewer::PopulateTreeStore() 
{
    treeStore->clear();  // Clear treeStore for repopulation

    int size = ld->cds.size();
    Gtk::TreeStore::iterator iterList[size];  // Indexes across both lists are the same

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

void CDViewer::onOKClick() 
{
    hide();
}

void CDViewer::onRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column) 
{
    Gtk::TreeStore::iterator it = treeStore->get_iter(path);

    if (it)  // Check if the iterator was correctly returned
    {
        auto row = *it;

        if (row[treeColumns.lm] != nullptr)   // If user did not activate a collision domain row ...
        {
            MachineDialog md(row[treeColumns.lm], ld->cds, *this);
            md.AwaitChanges(ld);
            PopulateTreeStore();    // Refresh treeStore
        }
        else     // Expand to see children of collision domain
        {
            if (treeView.row_expanded(path)) 
            {
                treeView.collapse_row(path);
            }
            else    // Only expand if it isn't already expanded
            {
                treeView.expand_row(path, false);
            }
        }
    }
}

/// MachineDialog IMPLEMENTATION ///

MachineDialog::MachineDialog(LabMachine* lm, std::vector<std::string>& cds, Gtk::Window& parent) : 
    Gtk::Dialog(lm->machineName, parent, true),
    nameBox(Gtk::ORIENTATION_HORIZONTAL, 5),
    addBtn("+"),
    lm(lm)
{  
    set_default_geometry(400, 400);

    add_button("OK", Gtk::RESPONSE_OK);

    addBtn.signal_clicked().connect(sigc::mem_fun(*this, &MachineDialog::onAddPress));

    nameEntry.set_text(lm->machineName);   // Initially set to current machine name

    nameLbl.set_markup("<span font='12'>Machine Name:</span>");

    intLbl.set_markup("<span font='12'>Interfaces</span>");
    intLbl.set_halign(Gtk::ALIGN_START);
    intLbl.set_margin_start(10);

    cdChoiceModel = Gtk::ListStore::create(comboColumns);

    auto row = *(cdChoiceModel->append());
    row[comboColumns.name] = "None";   // Add option to attach to no domains

    for (const std::string s : cds)    // Add each collision domain to ListStore
    {
        row = *(cdChoiceModel->append());
        row[comboColumns.name] = s;
    }

    NkUtils::set_margin_both(intList, 10);
    PopulateInterfaces();

    NkUtils::set_margin_both(nameBox, 10);
    nameBox.set_margin_top(10);
    nameBox.pack_start(nameLbl, Gtk::PACK_SHRINK, 0);
    nameBox.pack_start(nameEntry, Gtk::PACK_EXPAND_WIDGET);

    Gtk::Box* box = get_content_area();
    box->set_spacing(10);
    box->pack_start(nameBox, Gtk::PACK_SHRINK, 0);
    box->pack_start(intLbl, Gtk::PACK_SHRINK, 0);
    box->pack_start(intList, Gtk::PACK_EXPAND_WIDGET, 0);

    show_all_children();
}

void MachineDialog::PopulateInterfaces() 
{
    for (MachineInterface* mi : lm->interfaces)  // Populate interfaces list
    {
        InterfaceRecord* record = Gtk::make_managed<InterfaceRecord>(*this, mi);
        intList.Append(record);
    }
    intList.GetBox().pack_start(addBtn, Gtk::PACK_SHRINK, 0);
}

void MachineDialog::AwaitChanges(LabData* ld) 
{
    run();

    std::string input = nameEntry.get_text();

    std::string oldPath = ld->labDir + "/" + lm->machineName;
    std::string newPath = ld->labDir + "/" + input;

    std::string newFile = ld->labDir + "/" + input + ".startup";

    if (!rename(oldPath.c_str(), newPath.c_str()))  // Renaming failed, possibly due to new machine
    {
        std::string oldFile = ld->labDir + "/" + lm->machineName + ".startup";

        rename(oldFile.c_str(), newFile.c_str());   // Rename startup file
    }
    else 
    {
        std::filesystem::create_directory(newPath);  // Create the directory
        
        std::fstream startup;
        startup.open(newFile, std::ios::out); // Create startup file
        startup.close();
    }

    lm->machineName = input;   // Update machine name
}

MachineDialog::InterfaceRecord::InterfaceRecord(MachineDialog& parent, MachineInterface* mi) :
    Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 10),
    mi(mi),
    delBtn("Delete"),
    intName("eth" + std::to_string((int)mi->id)),
    parent(parent)
{
    delBtn.signal_clicked().connect(sigc::bind(sigc::mem_fun(parent, &MachineDialog::onDelPress), this));
    cdChoice.signal_changed().connect(sigc::mem_fun(*this, &MachineDialog::InterfaceRecord::onComboChange));

    cdChoice.set_model(parent.cdChoiceModel);  // Set combo box options
    cdChoice.pack_start(parent.comboColumns.name);

    for (auto it : parent.cdChoiceModel->children())   // Set selection to current collision domain
    {
        Glib::ustring name = (*it)[parent.comboColumns.name];
        if (!name.compare(mi->segment))      // If collision domains match ...
        {
            cdChoice.set_active(it);
            break;
        }
    }

    pack_start(intName, Gtk::PACK_SHRINK, 0);
    pack_start(cdChoice, Gtk::PACK_EXPAND_WIDGET, 0);
    pack_start(delBtn, Gtk::PACK_SHRINK, 0);
}

void MachineDialog::InterfaceRecord::onComboChange() 
{
    auto it = cdChoice.get_active();

    if (it)    // Update collision domain choice
    {
        Glib::ustring newCd = (*it)[parent.comboColumns.name];
        mi->segment = newCd;
    }
}

MachineDialog::ListColumns::ListColumns() 
{
    add(name);
}

void MachineDialog::onDelPress(InterfaceRecord* record) 
{
    for (int i = 0; i < lm->interfaces.size(); i++) 
    {
        if (lm->interfaces[i] == record->mi)  // Check for object in list to erase it
        {
            lm->interfaces.erase(lm->interfaces.begin() + i);
            delete record->mi;
            break;
        }
    }

    intList.Remove(record);
}

void MachineDialog::onAddPress() 
{
    int id = -1;
    for (const MachineInterface* mi : lm->interfaces)   // Increment greatest interface id to get new id
    {
        if (mi->id > id)
        {
            id = mi->id;
        }
    }

    MachineInterface* newInterface = new MachineInterface;
    newInterface->id = id + 1;   // Set ID
    newInterface->segment = "None";  // Set collision domain to "None"
    lm->interfaces.push_back(newInterface);

    InterfaceRecord* record = Gtk::make_managed<InterfaceRecord>(*this, newInterface);
    intList.Append(record);

    record->show();
    record->show_all_children();

    intList.GetBox().reorder_child(addBtn, -1);    // Move addBtn back to bottom
}

/// MachineViewer IMPLEMENTATION ///

MachineViewer::MachineViewer(MainWindow* parent, LabData* labData) :
    ToolWindow(parent, "Machines", 500, 600),
    addBtn("+"),
    okBtn("OK"),
    labData(labData),
    parent(parent)
{
    addBtn.signal_clicked().connect(sigc::mem_fun(*this, &MachineViewer::onAddPress));
    okBtn.signal_clicked().connect(sigc::mem_fun(*this, &MachineViewer::onOKPress));

    machineList.set_margin_top(10);
    okBtn.set_margin_bottom(10);

    NkUtils::set_margin_both(machineList, 10);
    NkUtils::set_margin_both(okBtn, 10);

    for (LabMachine* lm : labData->machines)    // Populate machineList
    {
        MachineRecord* machine = Gtk::make_managed<MachineRecord>(*this, lm);
        machineList.Append(machine);
    }
    machineList.GetBox().pack_start(addBtn, Gtk::PACK_SHRINK, 0);

    add(vBox);

    vBox.pack_start(machineList, Gtk::PACK_EXPAND_WIDGET, 0);
    vBox.pack_start(okBtn, Gtk::PACK_SHRINK, 0);

    show_all_children();
}

void MachineViewer::onOKPress() 
{
    parent->UpdateCDViewer();   // Update CDViewer with changes
    hide();
}

void MachineViewer::onAddPress() 
{
    LabMachine* lm = new LabMachine();
    labData->machines.push_back(lm);   // Push reference to machines list
    lm->machineName = "New Machine";

    MachineRecord* machine = Gtk::make_managed<MachineRecord>(*this, lm);
    machineList.Append(machine);
    machine->show();
    machine->show_all_children();

    machineList.GetBox().reorder_child(addBtn, -1);
}

void MachineViewer::onDelPress(MachineRecord* record) 
{
    for (int i = 0; i < labData->machines.size(); i++)  // Delete the reference to the machine
    {
        if (record->lm == labData->machines[i]) 
        {
            labData->machines.erase(labData->machines.begin() + i);
            break;
        }
    }

    for (MachineInterface* mi : record->lm->interfaces) 
    {
        delete mi;
    }
    delete record->lm;    // Free the machine's allocated memory

    machineList.Remove(record);  // Update visible machine list

    parent->UpdateCDViewer();   // Prevent user from editing deleted machine
}

MachineViewer::MachineRecord::MachineRecord(MachineViewer& parent, LabMachine* lm) :
    Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5),
    delBtn("Delete"),
    editBtn("Edit"),
    machineLbl(lm->machineName),
    lm(lm),
    parent(parent)
{
    editBtn.signal_clicked().connect(sigc::mem_fun(*this, &MachineViewer::MachineRecord::onEditPress));
    delBtn.signal_clicked().connect(sigc::bind(sigc::mem_fun(parent, &MachineViewer::onDelPress), this));

    machineLbl.set_halign(Gtk::ALIGN_START);

    pack_start(machineLbl, Gtk::PACK_EXPAND_WIDGET, 0);
    pack_start(editBtn, Gtk::PACK_SHRINK, 0);
    pack_start(delBtn, Gtk::PACK_SHRINK, 0);
}

void MachineViewer::MachineRecord::onEditPress() 
{
    MachineDialog md(lm, parent.labData->cds, parent);  // Open modification window
    md.AwaitChanges(parent.labData);

    machineLbl.set_text(lm->machineName);  // Update label text
}