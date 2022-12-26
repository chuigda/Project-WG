#include "ui_next/SearchDialog.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QMessageBox>

SearchDialog::SearchDialog(QPlainTextEdit *textEdit, CloseSignallingWidget *parent)
  : QWidget(parent, Qt::Window),
    m_TextEdit(textEdit)
{
  setWindowTitle("搜索");

  QHBoxLayout *layout = new QHBoxLayout(this);
  setLayout(layout);

  QLineEdit *lineEdit = new QLineEdit();
  layout->addWidget(lineEdit);

  QPushButton *button = new QPushButton("搜索");
  layout->addWidget(button);

  connect(button, &QPushButton::clicked, m_TextEdit, [this, lineEdit] {
    QString text = lineEdit->text().trimmed();
    if (text.isEmpty()) {
      return;
    }

    if (!m_TextEdit->find(text)) {
      QTextCursor cursor = m_TextEdit->textCursor();
      cursor.movePosition(QTextCursor::Start);
      m_TextEdit->setTextCursor(cursor);
      if (!m_TextEdit->find(text)) {
        QMessageBox::warning(this, "搜索", "没有找到指定的文本");
      }
    }
  });

  connect(parent, &CloseSignallingWidget::AboutToClose, this, &QWidget::close);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "VirtualCallInCtorOrDtor"
  // This should be safe since base class `QWidget` has been initialised,
  // and we did not reimplement `sizeHint`
  QSize size = sizeHint();
#pragma clang diagnostic pop
  setFixedSize(500, size.height());
}
