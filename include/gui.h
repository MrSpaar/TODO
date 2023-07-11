//
// Created by mrspaar on 7/10/23.
//

#ifndef TODO_GUI_H
#define TODO_GUI_H

#include "sqlite.h"

#include "gtkmm/grid.h"
#include "gtkmm/label.h"
#include "gtkmm/button.h"
#include "gtkmm/entry.h"
#include "gtkmm/window.h"
#include "gtkmm/checkbutton.h"
#include "gtkmm/cssprovider.h"
#include "gtkmm/scrolledwindow.h"


class Task {
public:
    Gtk::CheckButton checkButton;
    Gtk::Label label;
    Gtk::Button deleteButton;
};


class GUI : public Gtk::Window {
public:
    GUI();
    GUI& init();
    static void run();
private:
    SQLite db;

    Gtk::Grid mainGrid;
    Gtk::Entry taskEntry{};
    Gtk::Button addButton;

    Gtk::ScrolledWindow taskWindow;
    Gtk::Grid taskGrid;
    Glib::RefPtr<Gtk::CssProvider> styleProvider;

    std::vector<std::unique_ptr<Task>> tasks;
    void addTask(const std::string &description = "", int id = 0, bool checked = false);
};


#endif //TODO_GUI_H
