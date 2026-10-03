from pathlib import Path


def replace_once(path: str, old: str, new: str):
    p = Path(path)
    text = p.read_text(encoding="utf-8")
    if old not in text:
        raise SystemExit(f"Patch pattern not found: {path}: {old[:80]!r}")
    p.write_text(text.replace(old, new, 1), encoding="utf-8")


# Toolbar.h
replace_once(
    "source/ui/Toolbar.h",
    "    void touchGestureModeChanged(int mode);\n",
    "    void touchGestureModeChanged(int mode);\n    void pdfDarkModeToggled(bool enabled);\n",
)
replace_once(
    "source/ui/Toolbar.h",
    "    ThreeStateButton *m_touchGestureButton;\n",
    "    ThreeStateButton *m_touchGestureButton;\n    ToggleButton *m_pdfDarkModeButton;\n",
)
replace_once(
    "source/ui/Toolbar.h",
    "    void setTouchGestureMode(int mode);\n",
    "    void setTouchGestureMode(int mode);\n    void setPdfDarkMode(bool enabled);\n",
)

# Toolbar.cpp
replace_once(
    "source/ui/Toolbar.cpp",
    "#include <QResizeEvent>\n",
    "#include <QResizeEvent>\n#include <QSignalBlocker>\n",
)
replace_once(
    "source/ui/Toolbar.cpp",
    "    m_touchGestureButton->setThemedIcon(\"hand\");\n    m_touchGestureButton->setToolTip(tr(\"Touch Gesture Mode\\n0: Off\\n1: Y-axis scroll only\\n2: Full gestures\"));\n    mainLayout->addWidget(m_touchGestureButton);\n\n    m_page2Widgets = {\n        m_ocrExpandable, m_panButton, undoGap,\n        m_undoButton, m_redoButton, touchGap, m_touchGestureButton\n    };\n",
    "    m_touchGestureButton->setThemedIcon(\"hand\");\n    m_touchGestureButton->setToolTip(tr(\"Touch Gesture Mode\\n0: Off\\n1: Y-axis scroll only\\n2: Full gestures\"));\n    mainLayout->addWidget(m_touchGestureButton);\n\n    // --- PDF Dark Theme Toggle ---\n    m_pdfDarkModeButton = new ToggleButton(this);\n    m_pdfDarkModeButton->setText(QStringLiteral(\"☾\"));\n    m_pdfDarkModeButton->setToolTip(tr(\"Dark Theme\"));\n    mainLayout->addWidget(m_pdfDarkModeButton);\n\n    m_page2Widgets = {\n        m_ocrExpandable, m_panButton, undoGap,\n        m_undoButton, m_redoButton, touchGap, m_touchGestureButton,\n        m_pdfDarkModeButton\n    };\n",
)
replace_once(
    "source/ui/Toolbar.cpp",
    "    connect(m_touchGestureButton, &ThreeStateButton::stateChanged,\n            this, &Toolbar::touchGestureModeChanged);\n",
    "    connect(m_touchGestureButton, &ThreeStateButton::stateChanged,\n            this, &Toolbar::touchGestureModeChanged);\n    connect(m_pdfDarkModeButton, &ToggleButton::toggled,\n            this, &Toolbar::pdfDarkModeToggled);\n",
)
replace_once(
    "source/ui/Toolbar.cpp",
    "void Toolbar::setTouchGestureMode(int mode)\n{\n    m_touchGestureButton->setState(mode);\n}\n",
    "void Toolbar::setTouchGestureMode(int mode)\n{\n    m_touchGestureButton->setState(mode);\n}\n\nvoid Toolbar::setPdfDarkMode(bool enabled)\n{\n    if (!m_pdfDarkModeButton) return;\n    QSignalBlocker blocker(m_pdfDarkModeButton);\n    m_pdfDarkModeButton->setChecked(enabled);\n}\n",
)
replace_once(
    "source/ui/Toolbar.cpp",
    "    m_touchGestureButton->setDarkMode(darkMode);\n    m_pagerBackButton->setDarkMode(darkMode);\n",
    "    m_touchGestureButton->setDarkMode(darkMode);\n    m_pdfDarkModeButton->setDarkMode(darkMode);\n    m_pagerBackButton->setDarkMode(darkMode);\n",
)

# MainWindow.cpp: use the existing PDF dark-mode renderer and its QSettings key.
replace_once(
    "source/MainWindow.cpp",
    "    connect(m_toolbar, &Toolbar::straightLineToggled, this, [this](bool enabled) {\n",
    "    connect(m_toolbar, &Toolbar::pdfDarkModeToggled, this, [this](bool enabled) {\n        QSettings settings(\"SpeedyNote\", \"App\");\n        settings.setValue(\"display/pdfDarkMode\", enabled);\n        setPdfDarkModeEnabled(enabled);\n        if (DocumentViewport* vp = currentViewport()) {\n            vp->setPdfDarkModeEnabled(resolvePdfDarkMode(vp->document()));\n            vp->update();\n        }\n    });\n    m_toolbar->setPdfDarkMode(resolvePdfDarkMode(currentViewport() ? currentViewport()->document() : nullptr));\n\n    connect(m_toolbar, &Toolbar::straightLineToggled, this, [this](bool enabled) {\n",
)
