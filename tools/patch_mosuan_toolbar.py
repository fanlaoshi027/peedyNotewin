from pathlib import Path
import re


def replace_once(path: str, old: str, new: str):
    p = Path(path)
    text = p.read_text(encoding="utf-8")
    if old not in text:
        raise SystemExit(f"Patch pattern not found: {path}: {old[:80]!r}")
    p.write_text(text.replace(old, new, 1), encoding="utf-8")


def replace_regex_once(path: str, pattern: str, replacement: str):
    p = Path(path)
    text = p.read_text(encoding="utf-8")
    updated, count = re.subn(pattern, replacement, text, count=1, flags=re.S)
    if count != 1:
        raise SystemExit(f"Regex patch pattern not found: {path}: {pattern[:120]!r}")
    p.write_text(updated, encoding="utf-8")


# Toolbar.h
replace_once(
    "source/ui/Toolbar.h",
    "    void touchGestureModeChanged(int mode);\n",
    "    void touchGestureModeChanged(int mode);\n    void darkThemeToggled(bool enabled);\n",
)
replace_once(
    "source/ui/Toolbar.h",
    "    ThreeStateButton *m_touchGestureButton;\n",
    "    ThreeStateButton *m_touchGestureButton;\n    ToggleButton *m_darkThemeButton;\n",
)
replace_once(
    "source/ui/Toolbar.h",
    "    void setTouchGestureMode(int mode);\n",
    "    void setTouchGestureMode(int mode);\n    void setDarkTheme(bool enabled);\n",
)

# Toolbar.cpp
replace_once(
    "source/ui/Toolbar.cpp",
    "#include <QResizeEvent>\n",
    "#include <QResizeEvent>\n#include <QSignalBlocker>\n",
)

# Remove OCR from the visible toolbar completely. Keep the underlying OCR
# implementation in the project for now so we don't destabilize unrelated code.
replace_regex_once(
    "source/ui/Toolbar.cpp",
    r'\n    // --- OCR \(not in tool group, hover-to-expand\) ---\n.*?\n    // --- Pan \(no subtoolbar\) ---',
    "\n    // --- Pan (no subtoolbar) ---",
)
replace_once(
    "source/ui/Toolbar.cpp",
    "    m_page2Widgets = {\n        m_ocrExpandable, m_panButton, undoGap,\n        m_undoButton, m_redoButton, touchGap, m_touchGestureButton\n    };\n",
    "    // --- Dark Theme Toggle ---\n    m_darkThemeButton = new ToggleButton(this);\n    m_darkThemeButton->setText(QStringLiteral(\"☾\"));\n    m_darkThemeButton->setToolTip(tr(\"Dark Theme\"));\n    mainLayout->addWidget(m_darkThemeButton);\n\n    m_page2Widgets = {\n        m_panButton, undoGap,\n        m_undoButton, m_redoButton, touchGap, m_touchGestureButton,\n        m_darkThemeButton\n    };\n",
)
replace_once(
    "source/ui/Toolbar.cpp",
    "    connect(m_touchGestureButton, &ThreeStateButton::stateChanged,\n            this, &Toolbar::touchGestureModeChanged);\n",
    "    connect(m_touchGestureButton, &ThreeStateButton::stateChanged,\n            this, &Toolbar::touchGestureModeChanged);\n    connect(m_darkThemeButton, &ToggleButton::toggled,\n            this, &Toolbar::darkThemeToggled);\n",
)
replace_once(
    "source/ui/Toolbar.cpp",
    "void Toolbar::setTouchGestureMode(int mode)\n{\n    m_touchGestureButton->setState(mode);\n}\n",
    "void Toolbar::setTouchGestureMode(int mode)\n{\n    m_touchGestureButton->setState(mode);\n}\n\nvoid Toolbar::setDarkTheme(bool enabled)\n{\n    if (!m_darkThemeButton) return;\n    QSignalBlocker blocker(m_darkThemeButton);\n    m_darkThemeButton->setChecked(enabled);\n    m_darkThemeButton->setText(enabled ? QStringLiteral(\"☀\") : QStringLiteral(\"☾\"));\n}\n",
)
replace_once(
    "source/ui/Toolbar.cpp",
    "    m_touchGestureButton->setDarkMode(darkMode);\n    m_pagerBackButton->setDarkMode(darkMode);\n",
    "    m_touchGestureButton->setDarkMode(darkMode);\n    m_darkThemeButton->setDarkMode(darkMode);\n    m_darkThemeButton->setText(darkMode ? QStringLiteral(\"☀\") : QStringLiteral(\"☾\"));\n    m_pagerBackButton->setDarkMode(darkMode);\n",
)

# MainWindow.cpp: the original SpeedyNote theme follows Windows AppsUseLightTheme.
# Expose that existing theme mechanism as a local override, rather than trying
# to use the PDF-only dark-mode flag as a theme switch.
replace_once(
    "source/MainWindow.cpp",
    "bool MainWindow::isDarkMode() {\n#ifdef Q_OS_WIN\n",
    "bool MainWindow::isDarkMode() {\n    QSettings appThemeSettings(\"SpeedyNote\", \"App\");\n    if (appThemeSettings.contains(\"display/darkThemeOverride\")) {\n        return appThemeSettings.value(\"display/darkThemeOverride\").toBool();\n    }\n#ifdef Q_OS_WIN\n",
)

# Wire the toolbar switch to the existing MainWindow::updateTheme() pipeline.
replace_once(
    "source/MainWindow.cpp",
    "    connect(m_toolbar, &Toolbar::straightLineToggled, this, [this](bool enabled) {\n",
    "    connect(m_toolbar, &Toolbar::darkThemeToggled, this, [this](bool enabled) {\n        QSettings settings(\"SpeedyNote\", \"App\");\n        settings.setValue(\"display/darkThemeOverride\", enabled);\n        updateTheme();\n        if (m_toolbar) m_toolbar->setDarkTheme(isDarkMode());\n    });\n    m_toolbar->setDarkTheme(isDarkMode());\n\n    connect(m_toolbar, &Toolbar::straightLineToggled, this, [this](bool enabled) {\n",
)
