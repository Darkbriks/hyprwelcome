#include "TodoStore.hpp"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QFile>
#include <cstdlib>

static constexpr int kNotifCheckIntervalMs = 60 * 1000;

TodoStore::TodoStore(QObject* parent) : QObject{parent} {
    load();

    m_notifTimer = new QTimer{this};
    m_notifTimer->setInterval(kNotifCheckIntervalMs);
    connect(m_notifTimer, &QTimer::timeout, this, &TodoStore::checkDueNotifications);
    m_notifTimer->start();

    checkDueNotifications();
}

fs::path TodoStore::dataPath() {
    if (const char* xdg = std::getenv("XDG_DATA_HOME"); xdg && *xdg)
        return fs::path{xdg} / "hyprwelcome" / "todo.json";

    if (const char* home = std::getenv("HOME"); home && *home)
        return fs::path{home} / ".local" / "share" / "hyprwelcome" / "todo.json";

    throw std::runtime_error{"HOME non défini"};
}

void TodoStore::load() {
    const auto path = dataPath();
    if (!fs::exists(path)) return;

    QFile file{QString::fromStdString(path.string())};
    if (!file.open(QIODevice::ReadOnly)) return;

    const auto doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) return;

    const auto root  = doc.object();
    m_nextId         = root["nextId"].toInt(1);

    for (const auto& val : root["items"].toArray())
        m_items.push_back(TodoItem::fromJson(val.toObject()));
}

void TodoStore::save() const {
    const auto path = dataPath();
    fs::create_directories(path.parent_path());

    QJsonArray arr;
    for (const auto& item : m_items)
        arr.append(item.toJson());

    QJsonObject root;
    root["nextId"] = m_nextId;
    root["items"]  = arr;

    QFile file{QString::fromStdString(path.string())};
    if (file.open(QIODevice::WriteOnly))
        file.write(QJsonDocument{root}.toJson());
}

void TodoStore::addItem(const QString& text, std::optional<QDate> dueDate) {
    TodoItem item;
    item.id        = m_nextId++;
    item.text      = text;
    item.completed = false;
    item.dueDate   = dueDate;
    item.createdAt = QDateTime::currentDateTime();

    m_items.prepend(item);
    save();
    emit dataChanged();
}

void TodoStore::setCompleted(int id, bool completed) {
    for (auto& item : m_items) {
        if (item.id == id) {
            item.completed = completed;
            save();
            emit dataChanged();
            return;
        }
    }
}

void TodoStore::removeItem(int id) {
    m_items.removeIf([id](const TodoItem& item) { return item.id == id; });
    save();
    emit dataChanged();
}

void TodoStore::checkDueNotifications() {
    for (const auto& item : m_items) {
        if (item.completed) continue;

        const auto days = item.daysUntilDue();
        if (!days.has_value()) continue;

        QString msg;
        QString urgency = "normal";

        if (*days < 0) {
            msg     = QString{"󰄸 \"%1\" — échéance dépassée !"}.arg(item.text);
            urgency = "critical";
        } else if (*days == 0) {
            msg     = QString{"󰄸 \"%1\" — à faire aujourd'hui !"}.arg(item.text);
            urgency = "critical";
        } else if (*days == 1) {
            msg     = QString{"󰄸 \"%1\" — à faire demain."}.arg(item.text);
            urgency = "normal";
        } else {
            continue;
        }

        QProcess::startDetached("notify-send",
            {"HyprTodo", msg,
             "--icon=checkbox-checked-symbolic",
             "--urgency=" + urgency,
             "--expire-time=8000"});
    }
}