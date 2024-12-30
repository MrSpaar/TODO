#pragma once

#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/scrolledwindow.h>


class Task {
public:
    explicit Task(Gtk::Grid &parent, int id, const std::string &task, bool checked);
private:
    int id, index;

    Gtk::CheckButton checkButton;
    Gtk::Label label;
    Gtk::Button deleteButton;
};


class GUI : public Gtk::Window {
public:
    GUI();
    ~GUI();
private:
    void addFromEntry();
private:
    Gtk::Grid container;
    Gtk::Entry taskEntry;
    Gtk::Button addButton;
    Gtk::Grid taskGrid;
    Gtk::ScrolledWindow taskWindow;
};
