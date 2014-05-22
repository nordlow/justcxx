#pragma once

#ifndef GTKMM_EXAMPLEWINDOW_H
#define GTKMM_EXAMPLEWINDOW_H

#include <gtkmm.h>
#include "treeview_withpopup.hpp"

class ExampleWindow : public Gtk::Window
{
public:
    ExampleWindow();
    virtual ~ExampleWindow();

protected:
    //Signal handlers:
    void on_button_quit();



    //Child widgets:
    Gtk::Box m_VBox;

    Gtk::ScrolledWindow m_ScrolledWindow;
    TreeView_WithPopup m_TreeView;

    Gtk::ButtonBox m_ButtonBox;
    Gtk::Button m_Button_Quit;
};

#endif //GTKMM_EXAMPLEWINDOW_H
