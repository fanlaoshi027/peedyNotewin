#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>
#include <QColor>
#include "../core/ToolType.h"
#include "../core/DocumentViewport.h"

class QPaintEvent;
class QResizeEvent;
class QToolButton;
class QButtonGroup;
class QHBoxLayout;
class QLabel;
class QFrame;
class PenSubToolbar;
class MarkerSubToolbar;
class EraserSubToolbar;
class HighlighterSubToolbar;
class OcrSubToolbar;

/**
 * Toolbar - new lightweight Windows-first toolbar for Mosuan/SpeedyNote.
 *
 * The old expandable/paged toolbar is intentionally removed from the visual
 * layer. Existing subtoolbar objects remain hidden as compatibility backends
 * because MainWindow already uses their settings/signals. They are not part of
 * the new UI and can be removed after the new settings surface is complete.
 */
class Toolbar : public QWidget {
    Q_OBJECT

public:
    explicit Toolbar(QWidget *parent = nullptr);

    void setCurrentTool(ToolType tool);
    void setObjectInsertMode(DocumentViewport::ObjectInsertMode mode);
    void setTouchGestureMode(int mode);
    void updateTheme(bool darkMode);
    void setUndoEnabled(bool enabled);
    void setRedoEnabled(bool enabled);
    void setStraightLineMode(bool enabled);

    void onTabChanged(int newTabId, int oldTabId);
    void clearTabState(int tabId);

    PenSubToolbar* penSubToolbar() const { return m_penSubToolbar; }
    MarkerSubToolbar* markerSubToolbar() const { return m_markerSubToolbar; }
    EraserSubToolbar* eraserSubToolbar() const { return m_eraserSubToolbar; }
    HighlighterSubToolbar* highlighterSubToolbar() const { return m_highlighterSubToolbar; }
    OcrSubToolbar* ocrSubToolbar() const { return m_ocrSubToolbar; }

    void setOcrAvailable(bool available);
    void setOcrConfidenceSupported(bool supported);

    QSize minimumSizeHint() const override;

signals:
    void toolSelected(ToolType tool);
    void objectInsertModeSelected(DocumentViewport::ObjectInsertMode mode);
    void straightLineToggled(bool enabled);
    void undoClicked();
    void redoClicked();
    void touchGestureModeChanged(int mode);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QToolButton* makeToolButton(const QString& text, const QString& tooltip, bool checkable = true);
    void setupUi();
    void connectSignals();
    void refreshButtonState();
    void setTouchButtonText();

    QToolButton *m_penButton = nullptr;
    QToolButton *m_markerButton = nullptr;
    QToolButton *m_eraserButton = nullptr;
    QToolButton *m_highlighterButton = nullptr;
    QToolButton *m_lassoButton = nullptr;
    QToolButton *m_objectButton = nullptr;
    QToolButton *m_panButton = nullptr;
    QToolButton *m_lineButton = nullptr;
    QToolButton *m_undoButton = nullptr;
    QToolButton *m_redoButton = nullptr;
    QToolButton *m_touchButton = nullptr;
    QLabel *m_pageSpacer = nullptr;
    QFrame *m_separator = nullptr;
    QButtonGroup *m_toolGroup = nullptr;
    QHBoxLayout *m_layout = nullptr;

    PenSubToolbar *m_penSubToolbar = nullptr;
    MarkerSubToolbar *m_markerSubToolbar = nullptr;
    EraserSubToolbar *m_eraserSubToolbar = nullptr;
    HighlighterSubToolbar *m_highlighterSubToolbar = nullptr;
    OcrSubToolbar *m_ocrSubToolbar = nullptr;

    bool m_darkMode = true;
    bool m_ocrAvailable = false;
    bool m_ocrConfidenceSupported = false;
    int m_touchMode = 2;
    ToolType m_currentTool = ToolType::Pen;
    DocumentViewport::ObjectInsertMode m_objectInsertMode =
        DocumentViewport::ObjectInsertMode::Image;

    static constexpr int TOOLBAR_HEIGHT = 48;
};

#endif // TOOLBAR_H
