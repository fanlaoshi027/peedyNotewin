#pragma once

#include <QWidget>

class MosuanTopToolbar;
class FavoritesSidebar;
class QSplitter;
class QToolButton;
class QWidget;

class MosuanShell final : public QWidget {
    Q_OBJECT
public:
    explicit MosuanShell(QWidget* parent = nullptr);

    MosuanTopToolbar* topToolbar() const { return m_toolbar; }
    FavoritesSidebar* favoritesSidebar() const { return m_favorites; }
    QWidget* documentHost() const { return m_documentHost; }

public slots:
    void toggleFavorites();
    void setDocumentWidget(QWidget* widget);

private:
    MosuanTopToolbar* m_toolbar = nullptr;
    FavoritesSidebar* m_favorites = nullptr;
    QWidget* m_documentHost = nullptr;
    QToolButton* m_showFavorites = nullptr;
};
