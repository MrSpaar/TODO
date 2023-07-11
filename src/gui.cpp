//
// Created by mrspaar on 7/10/23.
//

#include <iostream>
#include "gui.h"


GUI::GUI() {
    db.init("../data/todo.db");
    db.run_migration("../data/migrations.sql");

    set_title("TODO");
    set_border_width(10);
    set_default_size(300, 400);
    set_resizable(false);

    taskWindow.set_size_request(-1, 290);
    taskWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    taskWindow.add(taskGrid);

    taskGrid.set_margin_end(10);
    taskGrid.set_column_spacing(10);
    taskGrid.set_row_spacing(5);

    styleProvider = Gtk::CssProvider::create();
    styleProvider->load_from_data(
            "button { padding: 2.5px 5px; }"
            "button:hover { background-image: image(#f44336); }"
            "button:active { background-image: image(#aa2e25); }"
            "checkbutton:checked check { background-image: image(#8bc34a); border-color: #618833; }"
    );

    mainGrid.attach(taskWindow, 0, 0, 1, 10);
    mainGrid.attach(taskEntry, 0, 11, 1, 1);
    mainGrid.attach(addButton, 0, 12, 1, 1);

    taskEntry.set_placeholder_text("Task description");
    addButton.set_label("Add task");

    mainGrid.set_row_spacing(10);
    mainGrid.set_column_homogeneous(true);

    db << "SELECT * FROM todo", sqlite::run;
    for (SQLRow &row : db)
        addTask(
                row.get<std::string>("description"),
                row.get<int>("id"),
                row.get<int>("checked") == 1
        );
}


void GUI::addTask(const std::string &description, int id, bool checked, Task *task) {
    if (!task)
        task = addRow();

    taskGrid.attach(task->checkButton, 0, task->index, 1, 1);
    taskGrid.attach(task->label, 1, task->index, 1, 1);
    taskGrid.attach(task->deleteButton, 2, task->index, 1, 1);

    task->label.set_hexpand(true);
    task->label.set_halign(Gtk::ALIGN_START);
    task->label.set_markup(checked ? "<s>"+description+"</s>" : description);
    task->label.get_style_context()->add_provider(styleProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    task->checkButton.set_active(checked);
    task->checkButton.get_style_context()->add_provider(
            styleProvider, GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    task->checkButton.signal_clicked().connect([this, id, task] {
        task->label.set_markup(task->checkButton.get_active() ?
                "<s>" + task->label.get_text() + "</s>" : task->label.get_text()
        );

        db << "UPDATE todo SET checked = \u0001 WHERE id = \u0001",
                task->checkButton.get_active() ? 1 : 0, id, sqlite::run;
    });

    task->deleteButton.set_label("⨯");
    task->deleteButton.get_style_context()->add_provider(
            styleProvider, GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    task->deleteButton.signal_clicked().connect([this, id, task] {
        taskGrid.remove(task->checkButton);
        taskGrid.remove(task->label);
        taskGrid.remove(task->deleteButton);
        taskGrid.show_all_children();
        db << "DELETE FROM todo WHERE id = \u0001", id, sqlite::run;
    });
}


template<typename T>
std::string GUI::normalize(const T &value) {
    size_t pos = 0;
    std::string normalized = value;

    while ((pos = normalized.find('&', pos)) != std::string::npos) {
        normalized.replace(pos, 1, "&amp;");
        pos += 5;
    }

    while ((pos = normalized.find('\'', pos)) != std::string::npos) {
        normalized.replace(pos, 1, "&apos;");
        pos += 6;
    }

    pos = 0;
    while ((pos = normalized.find('"', pos)) != std::string::npos) {
        normalized.replace(pos, 1, "&quot;");
        pos += 6;
    }

    return normalized;
}

void GUI::addFromEntry() {
    Task *task = addRow();
    std::string normalized = GUI::normalize(taskEntry.get_text());
    taskEntry.set_text("");

    db << "INSERT INTO todo (description, checked) VALUES (\u0001, \u0001)",
            normalized, task->checkButton.get_active(), sqlite::run;

    db << "SELECT * FROM todo WHERE id = last_insert_rowid()", sqlite::run;

    addTask(normalized, db.get<int>("id"), task->checkButton.get_active(), task);
    show_all_children();
}


GUI& GUI::init() {
    add(mainGrid);
    show_all_children();
    taskWindow.show_all_children();

    taskEntry.signal_activate().connect([this] { GUI::addFromEntry(); });
    addButton.signal_clicked().connect([this] { GUI::addFromEntry(); });

    return *this;
}


void GUI::run() {
    auto app = Gtk::Application::create();
    GUI window;
    app->run(window.init());
}
