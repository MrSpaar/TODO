CREATE TABLE IF NOT EXISTS todo (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    description TEXT NOT NULL,
    checked INTEGER NOT NULL CHECK ( checked IN (0, 1) )
)
