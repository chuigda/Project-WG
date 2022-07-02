#include "include/ui/MessageBoxAlter.h"
#include "ui_MessageBoxAlter.h"

MessageBoxAlter::MessageBoxAlter(const QString &title,
                                 const QString &text,
                                 QWidget *parent) :
  QWidget(parent, Qt::Dialog),
  ui(new Ui::MessageBoxAlter),
  m_Title(title),
  m_Text(text)
{
  ui->setupUi(this);
  setFixedSize(600, 600);
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowModality(Qt::WindowModal);

  setWindowTitle(title);
  ui->contentText->setPlainText(m_Text);

  ui->okButton->setAutoDefault(true);
  ui->okButton->setDefault(true);
  connect(ui->okButton, &QPushButton::clicked,
          this, &MessageBoxAlter::close);
}

MessageBoxAlter::~MessageBoxAlter() {
  delete ui;
}

void MessageBoxAlter::Show(const QString &title,
                           const QString &text,
                           QWidget *parent)
{
  MessageBoxAlter *mb = new MessageBoxAlter(title, text, parent);
  mb->show();
}
