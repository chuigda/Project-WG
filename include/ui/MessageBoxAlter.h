#ifndef PROJECT_WG_MESSAGE_BOX_ALTER_H
#define PROJECT_WG_MESSAGE_BOX_ALTER_H

#include <QWidget>

namespace Ui {
class MessageBoxAlter;
}

class MessageBoxAlter final : public QWidget
{
    Q_OBJECT

public:
  explicit MessageBoxAlter(const QString& title,
                           const QString& text,
                           QWidget *parent = nullptr);
  ~MessageBoxAlter() final;

  static void Show(const QString& title,
                   const QString& text,
                   QWidget *parent = nullptr);

private:
  Ui::MessageBoxAlter *ui;

  QString m_Title;
  QString m_Text;
};

#endif // PROJECT_WG_MESSAGE_BOX_ALTER_H
