#include "MosuanTopToolbar.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QFrame>
#include <QPalette>

MosuanTopToolbar::MosuanTopToolbar(QWidget* parent) : QWidget(parent) {
    setObjectName("MosuanTopToolbar");
    setFixedHeight(52);
    setAttribute(Qt::WA_StyledBackground, true);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 6, 10, 6);
    layout->setSpacing(4);

    m_pen = addTool(QStringLiteral("✎"), QStringLiteral("笔"));
    m_highlighter = addTool(QStringLiteral("▰"), QStringLiteral("荧光笔"));
    m_eraser = addTool(QStringLiteral("⌫"), QStringLiteral("橡皮"));
    m_line = addTool(QStringLiteral("／"), QStringLiteral("直线（两个控制点）"));
    m_select = addTool(QStringLiteral("□"), QStringLiteral("选择"));
    m_pan = addTool(QStringLiteral("✋"), QStringLiteral("移动页面"));

    layout->addWidget(m_pen);
    layout->addWidget(m_highlighter);
    layout->addWidget(m_eraser);
    layout->addWidget(m_line);
    layout->addWidget(m_select);
    layout->addWidget(m_pan);

    auto* separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Plain);
    layout->addWidget(separator);

    const QList<QColor> colors = {Qt::black, QColor("#E53935"), QColor("#1976D2")};
    for (const QColor& color : colors) {
        auto* button = addTool(QStringLiteral("●"), QStringLiteral("颜色"));
        QPalette p = button->palette();
        p.setColor(QPalette::ButtonText, color);
        button->setPalette(p);
        layout->addWidget(button);
        connect(button, &QToolButton::clicked, this, [this, color] { emit colorChanged(color); });
    }

    separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    layout->addWidget(separator);

    auto* thin = addTool(QStringLiteral("─"), QStringLiteral("细"));
    auto* medium = addTool(QStringLiteral("━"), QStringLiteral("中"));
    auto* thick = addTool(QStringLiteral("▬"), QStringLiteral("粗"));
    layout->addWidget(thin);
    layout->addWidget(medium);
    layout->addWidget(thick);
    connect(thin, &QToolButton::clicked, this, [this] { emit widthChanged(2.0); });
    connect(medium, &QToolButton::clicked, this, [this] { emit widthChanged(4.0); });
    connect(thick, &QToolButton::clicked, this, [this] { emit widthChanged(7.0); });

    auto* solid = addTool(QStringLiteral("━"), QStringLiteral("实线"));
    auto* dashed = addTool(QStringLiteral("┄"), QStringLiteral("虚线"));
    layout->addWidget(solid);
    layout->addWidget(dashed);
    connect(solid, &QToolButton::clicked, this, [this] { emit dashedChanged(false); });
    connect(dashed, &QToolButton::clicked, this, [this] { emit dashedChanged(true); });

    layout->addStretch(1);

    auto* undo = addTool(QStringLiteral("↶"), QStringLiteral("撤销"));
    auto* redo = addTool(QStringLiteral("↷"), QStringLiteral("重做"));
    auto* invert = addTool(QStringLiteral("◐"), QStringLiteral("PDF反色"));
    auto* fullscreen = addTool(QStringLiteral("⛶"), QStringLiteral("全屏"));
    layout->addWidget(undo);
    layout->addWidget(redo);
    layout->addWidget(invert);
    layout->addWidget(fullscreen);

    connect(undo, &QToolButton::clicked, this, &MosuanTopToolbar::undoRequested);
    connect(redo, &QToolButton::clicked, this, &MosuanTopToolbar::redoRequested);
    connect(invert, &QToolButton::clicked, this, &MosuanTopToolbar::pdfInvertRequested);
    connect(fullscreen, &QToolButton::clicked, this, &MosuanTopToolbar::fullscreenRequested);

    setStyleSheet(R"(
        #MosuanTopToolbar { background: #101722; border-bottom: 1px solid #253142; }
        #MosuanTopToolbar QToolButton { color: #D9E2EC; background: transparent; border: 0; border-radius: 7px; min-width: 34px; min-height: 34px; font-size: 18px; }
        #MosuanTopToolbar QToolButton:hover { background: #1D2A3A; }
        #MosuanTopToolbar QToolButton:pressed { background: #2A3B50; }
        #MosuanTopToolbar QFrame { color: #314052; }
    )");
}

QToolButton* MosuanTopToolbar::addTool(const QString& text, const QString& tooltip) {
    auto* button = new QToolButton(this);
    button->setText(text);
    button->setToolTip(tooltip);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

void MosuanTopToolbar::setActive(QToolButton* button) {
    if (!button) return;
    button->setChecked(true);
}
