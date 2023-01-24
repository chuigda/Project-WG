#include "ui_next/CodeEdit.h"

#include <QKeyEvent>
#include <QTextBlock>

CodeEdit::CodeEdit(QWidget *parent)
  : QTextEdit(parent)
{
  setAcceptRichText(false);
}

void CodeEdit::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_Tab: {
      insertPlainText("    ");
      break;
    }
    case Qt::Key_Return:
    case Qt::Key_Enter: {
      QTextCursor cursor = textCursor();
      QString text = cursor.block().text();
      qsizetype pos = cursor.positionInBlock();
      qsizetype count = 0;
      while (count < text.size() && count < pos && text.at(count).isSpace()) {
        count++;
      }

      cursor.insertText("\n");
      for (int j = 0; j < count; j++) {
        cursor.insertText(" ");
      }
      break;
    }
    default: {
      QTextEdit::keyPressEvent(event);
    }
  }
}
