#include "examplewindow.hpp"

int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);
    ExampleWindow window;
    Gtk::Main::run(window);
}
