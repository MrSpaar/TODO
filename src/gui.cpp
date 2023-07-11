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
    set_default_size(400, 400);
    set_resizable(false);

    taskWindow.set_size_request(-1, 390);
    taskWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    taskWindow.add(taskGrid);

    taskGrid.set_margin_top(10);
    taskGrid.set_margin_start(10);
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

    mainGrid.attach(taskWindow, 0, 0, 2, 10);
    mainGrid.attach(taskEntry, 0, 11, 2, 1);
    mainGrid.attach(addTaskButton, 0, 12, 1, 1);
    mainGrid.attach(addCategoryButton, 1, 12, 1, 1);

    taskEntry.set_placeholder_text("Task or category description");
    addTaskButton.set_label("Add task");
    addCategoryButton.set_label("Add category");

    mainGrid.set_row_spacing(10);
    mainGrid.set_column_spacing(10);
    mainGrid.set_column_homogeneous(true);

    db << "SELECT name FROM categories ORDER BY id", sqlite::run;
    for (SQLRow &row : db)
        categories.push_back(row.get<std::string>("name"));

    db << "SELECT description, todo.id, checked, name, categories.id AS cat_id FROM todo LEFT JOIN categories ON todo.category_id = categories.id", sqlite::run;
    for (SQLRow &row : db)
        addTask(
                row.get<std::string>("description"),
                row.get<int>("id"),
                row.get<int>("cat_id"),
                row.get<int>("checked") == 1
        );
}


void GUI::addTask(const std::string &description, int id, int category_id, bool checked, Task *task) {
    if (!task)
        task = addRow();

    taskGrid.attach(task->checkButton, 0, task->index, 1, 1);
    taskGrid.attach(task->label, 1, task->index, 1, 1);
    taskGrid.attach(task->categoryBox, 2, task->index, 1, 1);
    taskGrid.attach(task->deleteButton, 3, task->index, 1, 1);

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
                "<s>" + normalize(task->label.get_text()) + "</s>" : normalize(task->label.get_text())
        );

        db << "UPDATE todo SET checked = \u0001 WHERE id = \u0001",
                task->checkButton.get_active() ? 1 : 0, id, sqlite::run;
    });

    for (const std::string &category : categories)
        task->categoryBox.append(category);

    task->categoryBox.set_active(category_id);
    task->categoryBox.signal_changed().connect([this, id, task] {
        db << "UPDATE todo SET category_id = \u0001 WHERE id = \u0001",
                task->categoryBox.get_active_row_number(), id, sqlite::run;
    });

    task->deleteButton.set_label("⨯");
    task->deleteButton.get_style_context()->add_provider(
            styleProvider, GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    task->deleteButton.signal_clicked().connect([this, id, task] {
        taskGrid.remove(task->checkButton);
        taskGrid.remove(task->label);
        taskGrid.remove(task->categoryBox);
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
    if (taskEntry.get_text().empty())
        return;

    Task *task = addRow();
    std::string normalized = GUI::normalize(taskEntry.get_text());
    taskEntry.set_text("");

    db << "INSERT INTO todo (description) VALUES (\u0001)", normalized, sqlite::run;
    db << "SELECT * FROM todo WHERE id = last_insert_rowid()", sqlite::run;

    addTask(normalized, db.get<int>("id"), 0, false, task);
    show_all_children();
}


GUI& GUI::init() {
    add(mainGrid);
    show_all_children();
    taskWindow.show_all_children();

    taskEntry.signal_activate().connect([this] { GUI::addFromEntry(); });
    addTaskButton.signal_clicked().connect([this] { GUI::addFromEntry(); });

    addCategoryButton.signal_clicked().connect([this] {
        if (taskEntry.get_text().empty())
            return;

        std::string normalized = GUI::normalize(taskEntry.get_text());
        taskEntry.set_text("");

        if (std::find(categories.begin(), categories.end(), normalized) != categories.end())
            return;

        db << "INSERT INTO categories (name) VALUES (\u0001)", normalized, sqlite::run;
        categories.push_back(normalized);

        for (const auto & taskPtr : tasks) {
            Task *task = taskPtr.get();
            task->categoryBox.append(normalized);
        }
    });

    return *this;
}


void GUI::run() {
    auto app = Gtk::Application::create();
    GUI window;
    app->run(window.init());
}
