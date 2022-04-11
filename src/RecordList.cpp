#include "RecordList.h"

void RecordList::Append(Gtk::Box* record) 
{    
    vBox.pack_start(*record, Gtk::PACK_SHRINK, 0);
}

RecordList::RecordList() : vBox(Gtk::ORIENTATION_VERTICAL, 5)
{
    set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);  // Set vertical scrollbar to always visible

    add(vBox);
}

void RecordList::RefreshView() 
{
    vBox.show_all_children();
}

void RecordList::Remove(Gtk::Box* record) 
{
    vBox.remove(*record);
    delete record;    // Since no longer managed by container
}

Gtk::Box& RecordList::GetBox() 
{
    return vBox;
}