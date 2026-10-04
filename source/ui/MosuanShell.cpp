#include "MosuanShell.h"
#include "MosuanTopToolbar.h"
#include "FavoritesSidebar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QToolButton>
#include <QVBoxLayout>

MosuanShell::MosuanShell(QWidget* parent) : QWidget(parent) {
    setObjectName("MosuanShell");
    setAttribute(Qt::WA_StyledBackground, true);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    m_toolbar = new MosuanTopToolbar(this);
    root->addWidget(m_toolbar);

    auto* body = new QWidget(this);
    auto* bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);

    m_documentHost = new QWidget(body);
    m_documentHost->setObjectName("MosuanDocumentHost");
    auto* hostLayout = new QVBoxLayout(m_documentHost);
    hostLayout->setContentsMargins(0, 0, 0, 0);
    hostLayout->setSpacing(0);

    auto* empty = new QLabel(QStringLiteral("打开 PDF 开始书写"), m_documentHost);
    empty->setObjectName("MosuanEmptyState");
    empty->setAlignment(Qt::AlignCenter);
    hostLayout->addWidget(empty);
    bodyLayout->addWidget(m_documentHost, 1);

    m_favorites = new FavoritesSidebar(body);
    bodyLayout->addWidget(m_favorites);

    m_showFavorites = new QToolButton(body);
    m_showFavorites->setObjectName("ShowFavoritesButton");
    m_showFavorites->setText(QStringLiteral("⭐"));
    m_showFavorites->setToolTip(QStringLiteral("显示收藏栏"));
    m_showFavorites->setFixedSize(38, 38);
    m_showFavorites->hide();
    bodyLayout->addWidget(m_showFavorites);

    root->addWidget(body, 1);

    connect(m_showFavorites, &QToolButton::clicked, this, &MosuanShell::toggleFavorites);

    setStyleSheet(R"(
        #MosuanShell { background: #0A1018; }
        #MosuanDocumentHost { background: #111923; }
        #MosuanEmptyState { color: #64748B; font-size: 18px; }
        #ShowFavoritesButton { color: #E8EEF5; background: #182434; border: 1px solid #2A3A4F; border-radius: 19px; }
        #ShowFavoritesButton:hover { background: #223149; }
    )");
}

void MosuanShell::toggleFavorites() {
    if (!m_favorites) return;
    const bool visible = m_favorites->isVisible();
    m_favorites->setVisible(!visible);
    if (m_showFavorites) m_showFavorites->setVisible(visible);
}

void MosuanShell::setDocumentWidget(QWidget* widget) {
    if (!m_documentHost || !widget || widget == m_documentHost) return;

    auto* layout = qobject_cast<QVBoxLayout*>(m_documentHost->layout());
    if (!layout) return;

    while (layout->count() > 0) {
        QLayoutItem* item = layout->takeAt(0);
        if (item->widget()) item->widget()->setParent(nullptr);
        delete item;
    }

    widget->setParent(m_documentHost);
    layout->addWidget(widget);
    widget->show();
}