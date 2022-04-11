#ifndef RECORD_LIST_H
#define RECORD_LIST_H

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>

// A visible list of records
class RecordList : public Gtk::ScrolledWindow 
{
public:
    // Appends record to record list. Should be managed by Gtk::make_managed
    void Append(Gtk::Box* record);

    RecordList();
    
    // show_all_children()
    void RefreshView();

    // Removes a specific record and deallocates it
    void Remove(Gtk::Box* record);

    Gtk::Box& GetBox();

private:
    Gtk::Box vBox;   // vBox to contain records
};

#endif