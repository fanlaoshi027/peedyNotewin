#include "Toolbar.h"
#include "../MainWindow.h"

#include "subtoolbars/PenSubToolbar.h"
#include "subtoolbars/MarkerSubToolbar.h"
#include "subtoolbars/EraserSubToolbar.h"
#include "subtoolbars/HighlighterSubToolbar.h"
#include "subtoolbars/OcrSubToolbar.h"

#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QToolButton>
#include <QSizePolicy>

Toolbar::Toolbar(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    connectSignals();
    updateTheme(true);
}

QToolButton* Toolbar::makeToolButton(const QString& text, const QString& tooltip, bool checkable)
{
    auto* button = new QToolButton(this);
    button->setText(text);
    button->setToolTip(tooltip);
    button->setCheckable(checkable);
    button->setAutoRaise(true);
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedSize(38, 38);
    button->setFocusPolicy(Qt::NoFocus);
    return button;
}

void Toolbar::setupUi()
{
    setFixedHeight(TOOLBAR_HEIGHT);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(8, 5, 8, 5);
    m_layout->setSpacing(3);

    m_toolGroup = new QButtonGroup(this);
    m_toolGroup->setExclusive(true);

    m_penButton = makeToolButton(QStringLiteral("✎"), QStringLiteral("笔"));
    m_markerButton = makeToolButton(QStringLiteral("▰"), QStringLiteral("马克笔"));
    m_eraserButton = makeToolButton(QStringLiteral("⌫"), QStringLiteral("橡皮"));
    m_highlighterButton = makeToolButton(QStringLiteral("▱"), QStringLiteral("荧光笔"));
    m_lassoButton = makeToolButton(QStringLiteral("⌁"), QStringLiteral("选择笔迹"));
    m_objectButton = makeToolButton(QStringLiteral("□"), QStringLiteral("对象选择"));
    m_panButton = makeToolButton(QStringLiteral("✋"), QStringLiteral("移动页面"));
    m_lineButton = makeToolButton(QStringLiteral("／"), QStringLiteral("直线（两个控制点）"), true);
    m_pdfInvertButton = makeToolButton(QStringLiteral("◐"), QStringLiteral("PDF反色"), true);

    for (QToolButton* b : {m_penButton, m_markerButton, m_eraserButton,
                           m_highlighterButton, m_lassoButton, m_objectButton,
                           m_panButton}) {
        m_toolGroup->addButton(b);
        m_layout->addWidget(b);
    }
    m_penButton->setChecked(true);

    m_separator = new QFrame(this);
    m_separator->setFrameShape(QFrame::VLine);
    m_separator->setFixedHeight(26);
    m_layout->addWidget(m_separator);

    m_layout->addWidget(m_lineButton);
    m_layout->addWidget(m_pdfInvertButton);

    m_undoButton = makeToolButton(QStringLiteral("↶"), QStringLiteral("撤销"), false);
    m_redoButton = makeToolButton(QStringLiteral("↷"), QStringLiteral("重做"), false);
    m_layout->addWidget(m_undoButton);
    m_layout->addWidget(m_redoButton);

    m_pageSpacer = new QLabel(this);
    m_pageSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_layout->addWidget(m_pageSpacer, 1);

    m_touchButton = makeToolButton(QStringLiteral("☝"), QStringLiteral("触控模式"), false);
    m_layout->addWidget(m_touchButton);

    // Compatibility backends remain available to MainWindow, but their visual
    // widgets are completely hidden. The new toolbar owns the presentation.
    m_penSubToolbar = new PenSubToolbar(this);
    m_markerSubToolbar = new MarkerSubToolbar(this);
    m_eraserSubToolbar = new EraserSubToolbar(this);
    m_highlighterSubToolbar = new HighlighterSubToolbar(this);
    m_ocrSubToolbar = new OcrSubToolbar(this);
    for (QWidget* backend : {static_cast<QWidget*>(m_penSubToolbar),
                             static_cast<QWidget*>(m_markerSubToolbar),
                             static_cast<QWidget*>(m_eraserSubToolbar),
                             static_cast<QWidget*>(m_highlighterSubToolbar),
                             static_cast<QWidget*>(m_ocrSubToolbar)}) {
        backend->hide();
        backend->setFixedSize(0, 0);
    }
    setTouchButtonText();
}

void Toolbar::connectSignals()
{
    auto chooseTool = [this](ToolType tool) {
        setCurrentTool(tool);
        emit toolSelected(tool);
    };
    connect(m_penButton, &QToolButton::clicked, this, [chooseTool]() mutable { chooseTool(ToolType::Pen); });
    connect(m_markerButton, &QToolButton::clicked, this, [chooseTool]() mutable { chooseTool(ToolType::Marker); });
    connect(m_eraserButton, &QToolButton::clicked, this, [chooseTool]() mutable { chooseTool(ToolType::Eraser); });
    connect(m_highlighterButton, &QToolButton::clicked, this, [chooseTool]() mutable { chooseTool(ToolType::Highlighter); });
    connect(m_lassoButton, &QToolButton::clicked, this, [chooseTool]() mutable { chooseTool(ToolType::Lasso); });
    connect(m_objectButton, &QToolButton::clicked, this, [this]() {
        setCurrentTool(ToolType::ObjectSelect);
        emit objectInsertModeSelected(m_objectInsertMode);
        emit toolSelected(ToolType::ObjectSelect);
    });
    connect(m_panButton, &QToolButton::clicked, this, [chooseTool]() mutable { chooseTool(ToolType::Pan); });
    connect(m_lineButton, &QToolButton::toggled, this, &Toolbar::straightLineToggled);
    connect(m_pdfInvertButton, &QToolButton::toggled, this, [this](bool enabled) {
        emit pdfInvertToggled(enabled);
        if (auto* mainWindow = qobject_cast<MainWindow*>(window())) {
            mainWindow->setPdfDarkModeEnabled(enabled);
        }
    });
    connect(m_undoButton, &QToolButton::clicked, this, &Toolbar::undoClicked);
    connect(m_redoButton, &QToolButton::clicked, this, &Toolbar::redoClicked);
    connect(m_touchButton, &QToolButton::clicked, this, [this]() {
        m_touchMode = (m_touchMode + 1) % 3;
        setTouchButtonText();
        emit touchGestureModeChanged(m_touchMode);
    });
}

void Toolbar::setCurrentTool(ToolType tool)
{
    m_currentTool = tool;
    switch (tool) {
    case ToolType::Pen: m_penButton->setChecked(true); break;
    case ToolType::Marker: m_markerButton->setChecked(true); break;
    case ToolType::Eraser: m_eraserButton->setChecked(true); break;
    case ToolType::Highlighter: m_highlighterButton->setChecked(true); break;
    case ToolType::Lasso: m_lassoButton->setChecked(true); break;
    case ToolType::ObjectSelect: m_objectButton->setChecked(true); break;
    case ToolType::Pan: m_panButton->setChecked(true); break;
    }
}

void Toolbar::setObjectInsertMode(DocumentViewport::ObjectInsertMode mode)
{
    m_objectInsertMode = mode;
}

void Toolbar::setTouchGestureMode(int mode)
{
    m_touchMode = qBound(0, mode, 2);
    setTouchButtonText();
}

void Toolbar::setTouchButtonText()
{
    if (!m_touchButton) return;
    if (m_touchMode == 0) {
        m_touchButton->setText(QStringLiteral("◌"));
        m_touchButton->setToolTip(QStringLiteral("触控：关闭"));
    } else if (m_touchMode == 1) {
        m_touchButton->setText(QStringLiteral("↕"));
        m_touchButton->setToolTip(QStringLiteral("触控：仅上下移动"));
    } else {
        m_touchButton->setText(QStringLiteral("☝"));
        m_touchButton->setToolTip(QStringLiteral("触控：完整手势"));
    }
}

void Toolbar::setPdfInvert(bool enabled)
{
    if (m_pdfInvertButton) {
        QSignalBlocker blocker(m_pdfInvertButton);
        m_pdfInvertButton->setChecked(enabled);
    }
}

void Toolbar::updateTheme(bool darkMode)
{
    m_darkMode = darkMode;
    const QString fg = darkMode ? QStringLiteral("#e8edf5") : QStringLiteral("#1f2937");
    const QString hover = darkMode ? QStringLiteral("#1d2a3b") : QStringLiteral("#e9edf2");
    const QString checked = darkMode ? QStringLiteral("#284a6b") : QStringLiteral("#dbeafe");
    const QString disabled = darkMode ? QStringLiteral("#5c6878") : QStringLiteral("#a8b0bc");
    const QString border = darkMode ? QStringLiteral("#263449") : QStringLiteral("#d8dee7");
    setObjectName(QStringLiteral("Toolbar"));
    setStyleSheet(QStringLiteral(
        "QToolButton { background:transparent; color:%1; border:0; border-radius:9px; font-size:19px; }"
        "QToolButton:hover { background:%2; }"
        "QToolButton:checked { background:%3; }"
        "QToolButton:disabled { color:%4; }"
        "QFrame { color:%5; }"
    ).arg(fg, hover, checked, disabled, border));
    if (m_ocrSubToolbar) m_ocrSubToolbar->setDarkMode(darkMode);
    update();
}

void Toolbar::setUndoEnabled(bool enabled)
{
    if (m_undoButton) m_undoButton->setEnabled(enabled);
}

void Toolbar::setRedoEnabled(bool enabled)
{
    if (m_redoButton) m_redoButton->setEnabled(enabled);
}

void Toolbar::setStraightLineMode(bool enabled)
{
    if (m_lineButton) m_lineButton->setChecked(enabled);
}

void Toolbar::onTabChanged(int newTabId, int oldTabId)
{
    if (oldTabId >= 0) {
        m_penSubToolbar->saveTabState(oldTabId);
        m_markerSubToolbar->saveTabState(oldTabId);
        m_eraserSubToolbar->saveTabState(oldTabId);
        m_highlighterSubToolbar->saveTabState(oldTabId);
        m_ocrSubToolbar->saveTabState(oldTabId);
    }
    if (newTabId >= 0) {
        m_penSubToolbar->restoreTabState(newTabId);
        m_markerSubToolbar->restoreTabState(newTabId);
        m_eraserSubToolbar->restoreTabState(newTabId);
        m_highlighterSubToolbar->restoreTabState(newTabId);
        m_ocrSubToolbar->restoreTabState(newTabId);
    }
}

void Toolbar::clearTabState(int tabId)
{
    m_penSubToolbar->clearTabState(tabId);
    m_markerSubToolbar->clearTabState(tabId);
    m_eraserSubToolbar->clearTabState(tabId);
    m_highlighterSubToolbar->clearTabState(tabId);
    m_ocrSubToolbar->clearTabState(tabId);
}

void Toolbar::setOcrAvailable(bool available)
{
    m_ocrAvailable = available;
    m_ocrSubToolbar->setOcrAvailable(available);
}

void Toolbar::setOcrConfidenceSupported(bool supported)
{
    m_ocrConfidenceSupported = supported;
    m_ocrSubToolbar->setConfidenceSupported(supported);
}

QSize Toolbar::minimumSizeHint() const
{
    return QSize(420, TOOLBAR_HEIGHT);
}

void Toolbar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), m_darkMode ? QColor("#101722") : QColor("#f7f8fa"));
    painter.setPen(m_darkMode ? QColor("#263449") : QColor("#d8dee7"));
    painter.drawLine(rect().bottomLeft(), rect().bottomRight());
}

void Toolbar::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}
