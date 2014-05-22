#include "treeview_withpopup.hpp"
#include <iostream>

TreeView_WithPopup::TreeView_WithPopup()
{
    //Create the Tree model:
    m_refTreeModel = Gtk::ListStore::create(m_Columns);
    set_model(m_refTreeModel);

    //Fill the TreeView's model
    Gtk::TreeModel::Row row = *(m_refTreeModel->append());
    row[m_Columns.m_col_id] = 1;
    row[m_Columns.m_col_name] = "right-click on this";

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_id] = 2;
    row[m_Columns.m_col_name] = "or this";

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_id] = 3;
    row[m_Columns.m_col_name] = "or this, for a popup context menu";

    //Add the TreeView's view columns:
    append_column("ID", m_Columns.m_col_id);
    append_column("Name", m_Columns.m_col_name);

    //Fill popup menu:
    Gtk::MenuItem* item = Gtk::manage(new Gtk::MenuItem("_Edit", true));
    item->signal_activate().connect(
        sigc::mem_fun(*this, &TreeView_WithPopup::on_menu_file_popup_generic) );
    m_Menu_Popup.append(*item);
    
    item = Gtk::manage(new Gtk::MenuItem("_Process", true));
    item->signal_activate().connect(
        sigc::mem_fun(*this, &TreeView_WithPopup::on_menu_file_popup_generic) );
    m_Menu_Popup.append(*item);
    
    item = Gtk::manage(new Gtk::MenuItem("_Remove", true));
    item->signal_activate().connect(
        sigc::mem_fun(*this, &TreeView_WithPopup::on_menu_file_popup_generic) );
    m_Menu_Popup.append(*item);

    m_Menu_Popup.accelerate(*this);
    m_Menu_Popup.show_all(); //Show all menu items when the menu pops up

#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
    signal_button_press_event()
        .connect(sigc::mem_fun(*this, &TreeView_WithPopup::on_button_press_event), false);
#endif
}

TreeView_WithPopup::~TreeView_WithPopup()
{
}

bool TreeView_WithPopup::on_button_press_event(GdkEventButton* event)
{
    bool return_value = false;

    //Call base class, to allow normal handling,
    //such as allowing the row to be selected by the right-click:
    return_value = TreeView::on_button_press_event(event);

    //Then do our custom stuff:
    if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
    {
        m_Menu_Popup.popup(event->button, event->time);
    }

    return return_value;
}

void TreeView_WithPopup::on_menu_file_popup_generic()
{
    std::cout << "A popup menu item was selected." << std::endl;

    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if(refSelection)
    {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if(iter)
        {
            int id = (*iter)[m_Columns.m_col_id];
            std::cout << "  Selected ID=" << id << std::endl;
        }
    }
}
