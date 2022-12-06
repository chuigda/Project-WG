#include "ui_next/AboutBox.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include "ui_next/LicensePresenter.h"

AboutBox::AboutBox(LicensePresenter *presenter)
  : m_LicensePresenter(presenter)
{
  setWindowTitle("关于");
  setFixedWidth(600);
  setFixedHeight(600);

  QVBoxLayout *box = new QVBoxLayout();
  this->setLayout(box);

  QLabel *label = new QLabel(
    "<h1>Project-WG (v0.2.0)</h1>"
    "<hr/>"
    "(C) 2022 Chuigda WhiteGive<br/>"
    "<a href=\"https://github.com/chuigda/Project-WG\">https://github.com/chuigda/Project-WG</a><br/>"
    "使用 OpenGL 3.3 (Compatibility Profile) 绘制的可爱且能动的小机器人<br/>"
    "欢迎来 <a href=\"https://live.bilibili.com/10309712\">我的直播间</a> 凑热闹以及卡牌子<br/>"
    "<br/>"
    "特别鸣谢"
    "<ul>"
    "<li><b><a href=\"https://space.bilibili.com/14980676\">缪缪Mio (幼-菲尼克斯 UPhoenix)</a></b> - 带给我成为 VTuber 的梦想</li>"
    "<li><b>CrLF0710</b> - 提供了 FBO 渲染方面的技术支持</li>"
    "<li><b>小小小飞翔</b> - 提供了点九图方面的技术支持</li>"
    "<li><b>Seija</b> - 提供了相当多的 OpenGL 技术支持</li>"
    "<li><b>emilianavt</b> - OSF 的作者，为 Project-WG 提供了面捕能力</li>"
    "<li>...以及，大家，<b>所有人</b></li>"
    "</ul>"
  );
  label->setOpenExternalLinks(true);
  box->addWidget(label);
  box->addStretch();

  QHBoxLayout *buttonBox = new QHBoxLayout();
  box->addLayout(buttonBox);

  buttonBox->addStretch();
  QPushButton *licenseButton = new QPushButton("查看协议");
  buttonBox->addWidget(licenseButton);
  connect(licenseButton,
          &QPushButton::clicked,
          m_LicensePresenter,
          [this] {
            m_LicensePresenter->exec();
          });
  QPushButton *aboutQtButton = new QPushButton("关于 Qt");
  buttonBox->addWidget(aboutQtButton);
  connect(aboutQtButton, &QPushButton::clicked, aboutQtButton, [this] {
    QMessageBox::aboutQt(this);
  });
}

AboutBox::~AboutBox() noexcept {
  delete m_LicensePresenter;
}
