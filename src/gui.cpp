//
// Created by mrspaar on 7/10/23.
//

#include <iostream>
#include "gui.h"


GUI::GUI() {
    SQLInit(&conn, "../data/todo.db");

    set_title("TODO");
    set_border_width(10);
    set_default_size(300, 400);
    set_resizable(false);

    mainBox.set_orientation(Gtk::ORIENTATION_VERTICAL);
    mainBox.set_spacing(10);
    mainBox.pack_start(taskWindow, Gtk::PACK_EXPAND_WIDGET);
    mainBox.pack_start(inputBox, Gtk::PACK_SHRINK);

    taskWindow.set_size_request(-1, 350);
    taskWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    taskWindow.add(taskGrid);

    taskGrid.set_margin_start(10);
    taskGrid.set_margin_end(10);
    taskGrid.set_column_spacing(10);
    taskGrid.set_row_spacing(5);

    deleteButtonStyle = Gtk::CssProvider::create();
    deleteButtonStyle->load_from_data(
            "button { padding: 2.5px 5px; }"
            "button:hover { background-image: image(#f44336); }"
            "button:active { background-image: image(#aa2e25); }"
            "checkbutton:checked check { background-image: image(#8bc34a); border-color: #618833; }"
    );

    inputBox.pack_start(taskEntry, Gtk::PACK_EXPAND_WIDGET);
    inputBox.pack_start(addButton, Gtk::PACK_SHRINK);
    inputBox.set_spacing(10);

    addButtonStyle = Gtk::CssProvider::create();
    addButtonStyle->load_from_data(
            "button { padding-left: 10px; padding-right: 10px; }"
            "button:hover { background-image: image(#618833); }"
            "button:active { background-image: image(#357a38); }"
    );

    taskEntry.set_placeholder_text("Task or category description");
    taskEntry.signal_activate().connect([this] { GUI::addFromEntry(); });

    addButton.set_label("+");
    addButton.signal_clicked().connect([this] { GUI::addFromEntry(); });
    addButton.get_style_context()->add_provider(addButtonStyle, GTK_STYLE_PROVIDER_PRIORITY_USER);

    SQLData data;
    sqlite3_exec(conn, "SELECT * FROM todo;", SQLFetchAll, &data, nullptr);

    for (SQLRow &row: data)
        addTask(
                SQLGet<int>(row, "id"),
                SQLGet<std::string>(row, "description"),
                SQLGet<bool>(row, "checked")
        );
}


void GUI::addTask(int id, const std::string &description, bool checked, Task *task) {
    if (!task)
        task = addRow();

    taskGrid.attach(task->checkButton, 0, task->index, 1, 1);
    taskGrid.attach(task->label, 1, task->index, 1, 1);
    taskGrid.attach(task->deleteButton, 2, task->index, 1, 1);

    task->label.set_hexpand(true);
    task->label.set_halign(Gtk::ALIGN_START);
    task->label.set_markup(checked ? "<s>"+description+"</s>" : description);
    task->label.get_style_context()->add_provider(deleteButtonStyle, GTK_STYLE_PROVIDER_PRIORITY_USER);

    task->checkButton.set_active(checked);
    task->checkButton.get_style_context()->add_provider(
            deleteButtonStyle, GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    task->checkButton.signal_clicked().connect([this, id, task] {
        task->label.set_markup(task->checkButton.get_active() ?
                "<s>" + normalize(task->label.get_text()) + "</s>" : normalize(task->label.get_text())
        );

        std::string sql = "UPDATE todo SET checked = " + std::to_string(task->checkButton.get_active())
                          + " WHERE id = " + std::to_string(id);
        sqlite3_exec(conn, sql.c_str(), nullptr, nullptr, nullptr);
    });

    task->deleteButton.set_label("⨯");
    task->deleteButton.get_style_context()->add_provider(
            deleteButtonStyle, GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    task->deleteButton.signal_clicked().connect([this, id, task] {
        std::string sql = "DELETE FROM todo WHERE id = " + std::to_string(id);
        sqlite3_exec(conn, sql.c_str(), nullptr, nullptr, nullptr);

        taskGrid.remove(task->checkButton);
        taskGrid.remove(task->label);
        taskGrid.remove(task->deleteButton);
        taskGrid.show_all_children();
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
    if (taskEntry.get_text().empty())
        return;

    Task *task = addRow();
    std::string normalized = GUI::normalize(taskEntry.get_text());
    taskEntry.set_text("");

    std::string sql = "INSERT INTO todo (description) VALUES ('" + normalized + "');";
    sqlite3_exec(conn, sql.c_str(), nullptr, nullptr, nullptr);

    SQLRow row;
    sqlite3_exec(conn, "SELECT last_insert_rowid() AS id;", SQLFetchOne, &row, nullptr);

    addTask(SQLGet<int>(row, "id"), normalized, false, task);
    show_all_children();
}


GUI::~GUI() {
    sqlite3_close(conn);
}
