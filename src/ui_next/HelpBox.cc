#include "ui_next/HelpBox.h"
#include "util/FileUtil.h"

#include <QAbstractTextDocumentLayout>
#include <QScrollBar>
#include <QApplication>
#include <QPaintEngine>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QPaintEvent>

class TextBrowser : public QTextBrowser {
public:
  explicit TextBrowser(QWidget *parent = nullptr)
    : QTextBrowser(parent)
  {
    setContextMenuPolicy(Qt::NoContextMenu);
  }

protected:
  void paintEvent(QPaintEvent *e) override {
    QPainter painter(viewport());
    painter.setRenderHints(QPainter::Antialiasing
                           | QPainter::TextAntialiasing
                           | QPainter::SmoothPixmapTransform);

    const int xOffset = horizontalScrollBar()->value();
    const int yOffset = verticalScrollBar()->value();

    QRect r = e->rect();
    painter.translate(-xOffset, -yOffset);
    r.translate(xOffset, yOffset);

    document()->drawContents(&painter, r);
  }
};

HelpBox::HelpBox(QWidget *parent)
  : CloseSignallingWidget(parent)
{
  this->setWindowTitle("帮助查看器");
  this->resize(600, 600);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->setContentsMargins(3, 3, 3, 3);
  layout->setSpacing(3);
  this->setLayout(layout);

  QHBoxLayout *navigationLine = new QHBoxLayout();
  layout->addLayout(navigationLine);

  QPushButton *homePage = new QPushButton("主页");
  homePage->setFixedWidth(48);
  QPushButton *backward = new QPushButton("◀");
  backward->setEnabled(false);
  backward->setFixedWidth(24);
  QPushButton *forward = new QPushButton("▶");
  forward->setEnabled(false);
  forward->setFixedWidth(24);

  navigationLine->addWidget(homePage);
  navigationLine->addStretch();
  navigationLine->addWidget(backward);
  navigationLine->addWidget(forward);

  TextBrowser *browser = new TextBrowser();
  browser->setOpenExternalLinks(true);

  layout->addWidget(browser);

  connect(homePage, &QPushButton::clicked, browser, [browser] {
    browser->setSource(QUrl("qrc:/help/index.html"));
  });

  connect(backward, &QPushButton::clicked, browser, [browser] {
    browser->backward();
  });

  connect(forward, &QPushButton::clicked, browser, [browser] {
    browser->forward();
  });

  connect(browser, &QTextBrowser::backwardAvailable, backward, &QPushButton::setEnabled);
  connect(browser, &QTextBrowser::forwardAvailable, forward, &QPushButton::setEnabled);

  connect(browser, &QTextBrowser::sourceChanged, this, [this, browser] {
    this->setWindowTitle("帮助查看器 - " + browser->documentTitle());
  });

  browser->setSource(QUrl("qrc:/help/index.html"), QTextDocument::HtmlResource);
}
