//
// Created by mrspaar on 7/10/23.
//

#ifndef TODO_GUI_H
#define TODO_GUI_H

#include "gtkmm/box.h"
#include "gtkmm/grid.h"
#include "gtkmm/label.h"
#include "gtkmm/button.h"
#include "gtkmm/entry.h"
#include "gtkmm/checkbutton.h"
#include "gtkmm/cssprovider.h"
#include "gtkmm/comboboxtext.h"
#include "gtkmm/scrolledwindow.h"
#include "gtkmm/applicationwindow.h"

#include "sqlite.h"


class Task {
public:
    int index;
    Gtk::CheckButton checkButton;
    Gtk::Label label;
    Gtk::Button deleteButton;

    explicit Task(int size): index(size-1) {};
};


class GUI : public Gtk::ApplicationWindow {
public:
    GUI();
    GUI& init();
    ~GUI() override;
private:
    sqlite3* conn = nullptr;
    Gtk::Box mainBox;

    Gtk::Box inputBox;
    Gtk::Entry taskEntry{};
    Gtk::Button addButton;

    Gtk::ScrolledWindow taskWindow;
    Gtk::Grid taskGrid;
    Glib::RefPtr<Gtk::CssProvider> addButtonStyle;
    Glib::RefPtr<Gtk::CssProvider> deleteButtonStyle;

    std::vector<std::unique_ptr<Task>> tasks;
    Task* addRow() {
            tasks.emplace_back(std::make_unique<Task>(tasks.size()));
            return tasks.back().get();
    };

    void addFromEntry();
    void addTask(int id, const std::string &description, bool checked, Task *task = nullptr);

    template<typename T>
    static std::string normalize(const T &value);
};


#endif //TODO_GUI_H
