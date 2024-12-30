#include "includes/gui.h"
#include "includes/sqlite.h"

static sqlite3 *conn = nullptr;
static std::unordered_map<int, std::unique_ptr<Task>> tasks;


GUI::GUI() {
    SQLInit(&conn, "resources/todo.db");

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

    taskGrid.set_row_spacing(5);
    taskGrid.set_column_spacing(10);
    taskGrid.set_vexpand(true);
    taskGrid.set_orientation(Gtk::Orientation::VERTICAL);
    taskWindow.set_child(taskGrid);

    taskEntry.set_hexpand(true);
    taskEntry.set_placeholder_text("Task or category description");
    taskEntry.signal_activate().connect(
        sigc::mem_fun(*this, &GUI::addFromEntry)
    );

    addButton.set_image_from_icon_name("plus-symbolic");
    addButton.signal_clicked().connect(
        sigc::mem_fun(*this, &GUI::addFromEntry)
    );

    SQLData data;
    SQLExec(conn, "SELECT * FROM todo ORDER BY id;", SQLFetchAll, &data, true);

    for (SQLRow &row: data) {
        int id = SQLGet<int>(row, "id");

        tasks[id] = std::make_unique<Task>(
            taskGrid, id,
            SQLGet<std::string>(row, "description"),
            SQLGet<bool>(row, "checked")
        );
    }
}


Task::Task(Gtk::Grid &parent, int id, const std::string &task, bool checked) {
    this->id = id;
    this->index = tasks.size();

    parent.attach(checkButton, 0, id);
    parent.attach(label, 1, id);
    parent.attach(deleteButton, 2, id);

    label.set_hexpand(true);
    label.set_halign(Gtk::Align::START);
    label.set_markup(checked ? "<s>"+task+"</s>" : task);

    checkButton.set_active(checked);
    checkButton.signal_toggled().connect([&] {
        label.set_markup(checkButton.get_active() ?
                "<s>" + normalize(label.get_text()) + "</s>" : normalize(label.get_text())
        );

        std::string sql = "UPDATE todo SET checked = " + std::to_string(checkButton.get_active())
                          + " WHERE id = " + std::to_string(this->id);

        SQLExec(conn, sql, nullptr, nullptr);
    });

    deleteButton.set_image_from_icon_name("minus-symbolic");
    deleteButton.signal_clicked().connect([&] {
        std::string sql = "DELETE FROM todo WHERE id = " + std::to_string(this->id);
        SQLExec(conn, sql, nullptr, nullptr);

        parent.remove_row(this->id);
        tasks.erase(this->index);
    });
}


void GUI::addFromEntry() {
    if (taskEntry.get_text().empty())
        return;

    std::string normalized = normalize(taskEntry.get_text());

    SQLExec(conn, "INSERT INTO todo (description) VALUES ('" + normalized + "');", nullptr, nullptr);
    taskEntry.set_text("");

    SQLRow row;
    SQLExec(conn, "SELECT last_insert_rowid() AS id;", SQLFetchOne, &row);

    int id = SQLGet<int>(row, "id");
    tasks[id] = std::make_unique<Task>(taskGrid, id, normalized, false);
}


GUI::~GUI() {
    sqlite3_close(conn);
}
