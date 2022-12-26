#ifndef PROJECT_WG_UINEXT_SEARCH_DIALOG_H
#define PROJECT_WG_UINEXT_SEARCH_DIALOG_H

#include <QWidget>
#include "CloseSignallingWidget.h"

class QPlainTextEdit;

class SearchDialog : public QWidget {
  Q_OBJECT
public:
  explicit SearchDialog(QPlainTextEdit *plainTextEdit,
                        CloseSignallingWidget *parent = nullptr);

private:
  QPlainTextEdit *m_TextEdit;
};

#endif // PROJECT_WG_UINEXT_SEARCH_DIALOG_H
