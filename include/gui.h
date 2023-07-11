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
    int index;
    Gtk::CheckButton checkButton;
    Gtk::Label label;
    Gtk::Button deleteButton;

    explicit Task(int size): index(size-1) {};
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
    Task* addRow() {
            tasks.emplace_back(std::make_unique<Task>(tasks.size()));
            return tasks.back().get();
    };

    void addFromEntry();
    void addTask(const std::string &description, int id, bool checked, Task *task = nullptr);

    template<typename T>
    static std::string normalize(const T &value);
};


#endif //TODO_GUI_H
