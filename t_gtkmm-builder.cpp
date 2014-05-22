#include <iomanip>
#include <sstream>
#include <iostream>

#include <locale.h>
//#include <sigc++/retype_return.h>
#include <gtkmm.h>

using std::cout;
using std::endl;

class hello : public Gtk::Window
{
public:
    hello();
    virtual ~hello();

protected:
    // The widgets that are manipulated.
    Gtk::Button* quit_button;

    // Glade interface description.
    //Glib::RefPtr<Gnome::Glade::Xml> xml_interface;
    Glib::RefPtr<Gtk::Builder>  xml_interface;

};

using namespace std;

hello::hello()
{
    int k;
    double heading;

    cout << "hello()" << endl;
    set_title("gtk-builder demo");
    set_resizable(false);

    // Get the Glade user interface and add it to this window.
    //xml_interface = Gnome::Glade::Xml::create("hello.glade", "layout1");
    xml_interface = Gtk::Builder::create_from_file("gtkmm-builder-hello.xml");

    Gtk::Layout *main_vbox;
    xml_interface->get_widget("layout1", main_vbox);
    //add(*main_vbox);
// if this line is uncommented I get a runtime warning about
// layout1 already being inside a window

    xml_interface->get_widget("button2", quit_button);

    // Set up signal handers for buttons.
    quit_button->signal_clicked().connect ( sigc::mem_fun(*this, &hello::hide) );

}

hello::~hello()
{
}

int main (int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);
    hello window;
    kit.run(window);
    return 0;
}
