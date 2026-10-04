#pragma once

#include <QWidget>

class QListWidget;
class QToolButton;

class FavoritesSidebar final : public QWidget {
    Q_OBJECT
public:
    explicit FavoritesSidebar(QWidget* parent = nullptr);

signals:
    void favoriteActivated(const QString& path);
    void favoriteRemoved(const QString& path);
    void folderRequested();

public slots:
    void addFavorite(const QString& path);

private:
    QListWidget* m_list = nullptr;
    QToolButton* m_collapse = nullptr;
};
