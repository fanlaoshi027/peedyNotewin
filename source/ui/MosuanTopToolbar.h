#pragma once

#include <QWidget>
#include <QColor>

class QToolButton;

class MosuanTopToolbar final : public QWidget {
    Q_OBJECT
public:
    explicit MosuanTopToolbar(QWidget* parent = nullptr);

signals:
    void penRequested();
    void highlighterRequested();
    void eraserRequested();
    void straightLineRequested();
    void selectRequested();
    void panRequested();
    void undoRequested();
    void redoRequested();
    void pdfInvertRequested();
    void fullscreenRequested();
    void colorChanged(const QColor& color);
    void widthChanged(qreal width);
    void dashedChanged(bool dashed);

private:
    QToolButton* addTool(const QString& text, const QString& tooltip);
    void setActive(QToolButton* button);

    QToolButton* m_pen = nullptr;
    QToolButton* m_highlighter = nullptr;
    QToolButton* m_eraser = nullptr;
    QToolButton* m_line = nullptr;
    QToolButton* m_select = nullptr;
    QToolButton* m_pan = nullptr;
};
