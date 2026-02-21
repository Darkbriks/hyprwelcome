#include "TodoItem.hpp"

QJsonObject TodoItem::toJson() const {
    QJsonObject obj;
    obj["id"]        = id;
    obj["text"]      = text;
    obj["completed"] = completed;
    obj["createdAt"] = createdAt.toString(Qt::ISODate);

    if (dueDate.has_value())
        obj["dueDate"] = dueDate->toString(Qt::ISODate);

    return obj;
}

TodoItem TodoItem::fromJson(const QJsonObject& obj) {
    TodoItem item;
    item.id        = obj["id"].toInt();
    item.text      = obj["text"].toString();
    item.completed = obj["completed"].toBool();
    item.createdAt = QDateTime::fromString(obj["createdAt"].toString(), Qt::ISODate);

    if (obj.contains("dueDate") && !obj["dueDate"].toString().isEmpty())
        item.dueDate = QDate::fromString(obj["dueDate"].toString(), Qt::ISODate);

    return item;
}

std::optional<int> TodoItem::daysUntilDue() const {
    if (!dueDate.has_value()) return std::nullopt;
    return QDate::currentDate().daysTo(*dueDate);
}

QString TodoItem::urgencyClass() const {
    const auto days = daysUntilDue();
    if (!days.has_value()) return "none";
    if (*days < 0)  return "overdue";
    if (*days <= 1) return "urgent";
    if (*days <= 3) return "soon";
    return "normal";
}