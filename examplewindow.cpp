#include <iostream>
#include "examplewindow.hpp"

ExampleWindow::ExampleWindow()
    : m_VBox(Gtk::ORIENTATION_VERTICAL),
      m_Button_Quit("Quit")
{
    set_title("Gtk::TreeView (ListStore) example");
    set_border_width(5);
    set_default_size(400, 200);

    add(m_VBox);

    //Add the TreeView, inside a ScrolledWindow, with the button underneath:
    m_ScrolledWindow.add(m_TreeView);

    //Only show the scrollbars when they are necessary:
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    m_VBox.pack_start(m_ScrolledWindow);
    m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

    m_ButtonBox.pack_start(m_Button_Quit, Gtk::PACK_SHRINK);
    m_ButtonBox.set_border_width(5);
    m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
    m_Button_Quit.signal_clicked().connect( sigc::mem_fun(*this,
                                                          &ExampleWindow::on_button_quit) );

    show_all_children();
}

ExampleWindow::~ExampleWindow()
{
}

void ExampleWindow::on_button_quit()
{
    hide();
}
