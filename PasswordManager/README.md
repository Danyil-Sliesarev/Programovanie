# PasswordManager

Qt Widgets + CMake training project covering PR16-PR20:

- QMainWindow, QTableView, menu/toolbar/statusbar
- Editable table with custom QAbstractTableModel
- SQLite storage via DatabaseManager and AccountRepository
- Search and category filtering via QSortFilterProxyModel subclass
- Password leak check via QNetworkAccessManager and Pwned Passwords k-anonymity API
- Batch password check in a background task via QtConcurrent + QFutureWatcher

Open this folder in Qt Creator and build with a Qt 6 kit that includes Sql, Network and Concurrent modules.
