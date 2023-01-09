#ifndef PROJECT_WG_EXTRA_TYPEWRITER_CONTROL_H
#define PROJECT_WG_EXTRA_TYPEWRITER_CONTROL_H

#include <QWidget>
#include <QFontMetrics>

class QPlainTextEdit;

class TypeWriterControl : public QWidget {
public:
  explicit TypeWriterControl(bool *contentChanged = nullptr);
  [[nodiscard]] QImage capture() const;

private slots:
  void updateFont();
  void updateText();
  void updateCanvas();

private:
  bool *m_ContentChanged;
  QWidget *m_PreviewWidget;
  QPlainTextEdit *m_TextEdit;

  QString m_DisplayText;
  QColor m_ForeColor;
  QColor m_BackColor;
  QFont m_Font;
  QFontMetrics m_FontMetrics;
};

#endif // PROJECT_WG_EXTRA_TYPEWRITER_CONTROL_H
