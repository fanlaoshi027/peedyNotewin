#include "FavoritesSidebar.h"

#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QToolButton>
#include <QVBoxLayout>

FavoritesSidebar::FavoritesSidebar(QWidget* parent) : QWidget(parent) {
    setObjectName("FavoritesSidebar");
    setMinimumWidth(220);
    setMaximumWidth(300);
    setAttribute(Qt::WA_StyledBackground, true);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(8);

    auto* header = new QHBoxLayout();
    auto* title = new QLabel(QStringLiteral("⭐  收藏"), this);
    title->setObjectName("FavoritesTitle");
    m_collapse = new QToolButton(this);
    m_collapse->setText(QStringLiteral("›"));
    m_collapse->setToolTip(QStringLiteral("收起收藏栏"));
    m_collapse->setFixedSize(30, 30);
    header->addWidget(title);
    header->addStretch(1);
    header->addWidget(m_collapse);
    root->addLayout(header);

    m_list = new QListWidget(this);
    m_list->setObjectName("FavoritesList");
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    m_list->setContextMenuPolicy(Qt::CustomContextMenu);
    root->addWidget(m_list, 1);

    setStyleSheet(R"(
        #FavoritesSidebar { background: #0D141E; border-left: 1px solid #253142; }
        #FavoritesTitle { color: #E8EEF5; font-size: 16px; font-weight: 600; }
        #FavoritesSidebar QToolButton { color: #AEBCCC; background: transparent; border: 0; border-radius: 6px; font-size: 20px; }
        #FavoritesSidebar QToolButton:hover { background: #1B2736; }
        #FavoritesList { background: transparent; border: 0; color: #D9E2EC; outline: none; }
        #FavoritesList::item { padding: 10px 8px; border-radius: 6px; }
        #FavoritesList::item:hover { background: #182434; }
        #FavoritesList::item:selected { background: #213249; }
    )");

    connect(m_list, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        if (item) emit favoriteActivated(item->data(Qt::UserRole).toString());
    });

    connect(m_collapse, &QToolButton::clicked, this, [this] {
        setVisible(false);
    });
}

void FavoritesSidebar::addFavorite(const QString& path) {
    if (path.isEmpty()) return;
    for (int i = 0; i < m_list->count(); ++i) {
        if (m_list->item(i)->data(Qt::UserRole).toString() == path) return;
    }
    auto* item = new QListWidgetItem(QStringLiteral("📄  ") + QFileInfo(path).fileName(), m_list);
    item->setData(Qt::UserRole, path);
}
