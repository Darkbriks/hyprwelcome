#pragma once

#include <QJsonObject>
#include <optional>

struct TodoItem {
    int                   id;
    QString               text;
    bool                  completed{false};
    std::optional<QDate>  dueDate;    // optionnel
    QDateTime             createdAt;

    QJsonObject toJson() const;
    static TodoItem fromJson(const QJsonObject& obj);
    std::optional<int> daysUntilDue() const;
    QString urgencyClass() const;
};