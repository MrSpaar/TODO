#include "includes/gui.h"
#include <gtkmm/application.h>


int main(int argc, char **argv) {
    auto app = Gtk::Application::create("org.todo");
    return app->make_window_and_run<GUI>(argc, argv);
}
