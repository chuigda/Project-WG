#ifndef PROJECT_WG_UINEXT_CODE_EDIT_H
#define PROJECT_WG_UINEXT_CODE_EDIT_H

#include <QTextEdit>

// an editor widget with auto indentation and automatically replaces tabs with FOUR spaces
class CodeEdit final : public QTextEdit {
  Q_OBJECT

public:
  explicit CodeEdit(QWidget *parent = nullptr);
  ~CodeEdit() final = default;

  // overwrite several events
protected:
  void keyPressEvent(QKeyEvent *event) final;
};

#endif // PROJECT_WG_UINEXT_CODE_EDIT_H
