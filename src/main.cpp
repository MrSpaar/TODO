#include "gui.h"


GUI& GUI::init() {
    add(mainBox);
    show_all_children();
    taskWindow.show_all_children();
    return *this;
}


int main(int argc, char** argv) {
    auto app = Gtk::Application::create(argc, argv, "com.github.mrspaar.todo");

    GUI gui;
    app->run(gui.init());

    return 0;
}
