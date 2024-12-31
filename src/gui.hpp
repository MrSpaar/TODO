#pragma once

#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/scrolledwindow.h>
#include <nlohmann/json_fwd.hpp>


class GUI: public Gtk::Window {
public:
    GUI() {
        set_child(container);
        set_title("TODO");
        set_resizable(false);
        set_default_size(300, 400);

        container.attach(taskWindow, 0, 0, 2, 1);
        container.attach(taskEntry, 0, 1);
        container.attach(addButton, 1, 1);
        container.set_margin(10);
        container.set_row_spacing(10);
        container.set_column_spacing(5);

        taskContainer.set_vexpand(true);
        taskContainer.set_row_spacing(5);
        taskContainer.set_column_spacing(5);
        taskContainer.set_orientation(Gtk::Orientation::VERTICAL);
        taskWindow.set_child(taskContainer);

        taskEntry.set_hexpand(true);
        taskEntry.set_placeholder_text("Task or category description");
        taskEntry.signal_activate().connect(
            sigc::mem_fun(*this, &GUI::addRow)
        );

        addButton.set_image_from_icon_name("plus-symbolic");
        addButton.signal_clicked().connect(
            sigc::mem_fun(*this, &GUI::addRow)
        );

        if (!std::filesystem::exists("todo.json"))
            return;

        std::ifstream file("todo.json");
        tasks = nlohmann::json::parse(file);

        for (const nlohmann::json &task: tasks)
            addTask(
                task[0].template get<std::string>(),
                task[1].template get<bool>()
            );

    }

    ~GUI() {
        std::ofstream file("todo.json");
        file << tasks;
    }
private:
    void addRow() {
        std::string text = taskEntry.get_text();
        if (text.empty()) return;

        addTask(text, false);
        taskEntry.set_text("");
        tasks.push_back({text, false});
    }

    void addTask(const std::string &text, bool checked) {
        int row = ++rows;

        auto checkButton = Gtk::make_managed<Gtk::CheckButton>();
        auto label = Gtk::make_managed<Gtk::Label>();
        auto deleteButton = Gtk::make_managed<Gtk::Button>();

        taskContainer.attach(*checkButton, 0, row);
        taskContainer.attach(*label, 1, row);
        taskContainer.attach(*deleteButton, 2, row);

        label->set_hexpand(true);
        label->set_halign(Gtk::Align::START);
        label->set_markup(checked ? "<s>" + text + "</s>" : text);

        checkButton->set_active(checked);
        checkButton->signal_toggled().connect([this, label, checkButton, row] {
            label->set_markup(checkButton->get_active() ?
                "<s>" + label->get_text() + "</s>" : label->get_text()
            );

            tasks[row][1] = checkButton->get_active();
        });

        deleteButton->set_image_from_icon_name("minus-symbolic");
        deleteButton->signal_clicked().connect([this, label, checkButton, deleteButton, row] {
            tasks.erase(row);
            taskContainer.remove(*label);
            taskContainer.remove(*checkButton);
            taskContainer.remove(*deleteButton);
        });
    }
private:
    int rows = -1;
    nlohmann::json tasks;

    Gtk::Grid container;
    Gtk::Grid taskContainer;
    Gtk::ScrolledWindow taskWindow;
    Gtk::Entry taskEntry;
    Gtk::Button addButton;
};
