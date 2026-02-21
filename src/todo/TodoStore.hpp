#pragma once

#include "TodoItem.hpp"

#include <QObject>
#include <QList>
#include <QTimer>
#include <filesystem>

namespace fs = std::filesystem;

class TodoStore : public QObject {
    Q_OBJECT

public:
    explicit TodoStore(QObject* parent = nullptr);

    const QList<TodoItem>& items() const { return m_items; }

    void addItem(const QString& text, std::optional<QDate> dueDate = std::nullopt);
    void setCompleted(int id, bool completed);
    void removeItem(int id);
    void save() const;

    signals:
        void dataChanged();

private:
    QList<TodoItem> m_items;
    QTimer*         m_notifTimer{nullptr};
    int             m_nextId{1};

    static fs::path dataPath();
    void load();
    void checkDueNotifications();
};