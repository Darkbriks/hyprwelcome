#pragma once

#include "ConfigLoader.hpp"
#include "ThemeManager.hpp"

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include <QProcess>

class ActionButton : public QPushButton {
    Q_OBJECT

public:
    explicit ActionButton(const ActionConfig& action,
                          const ThemeColors&  colors,
                          QWidget* parent = nullptr,
                          int subtitleRefreshIntervalMs = 5000,
                          int subtitleCmdTimeoutMs = 4000);

    void applyTheme(const ThemeColors& colors);

private slots:
    void onClicked();
    void refreshSubtitle();

private:
    ActionConfig m_action;
    QLabel*      m_iconLabel{nullptr};
    QLabel*      m_textLabel{nullptr};
    QLabel*      m_subtitleLabel{nullptr};
    QTimer*      m_refreshTimer{nullptr};

    QString accentColor(const ThemeColors& colors) const;
    QString resolveIcon(const QString& icon) const;

    bool isSubtitleCmd() const;

    QString subtitleCmd() const;
};