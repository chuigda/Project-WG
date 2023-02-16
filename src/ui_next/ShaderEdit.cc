#include "ui_next/ShaderEdit.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include "wgc0310/Shader.h"
#include "ui_next/GLWindow.h"
#include "ui_next/ShaderHighlighter.h"
#include "util/FileUtil.h"
#include "ui_next/CodeEdit.h"

ShaderEdit::ShaderEdit(GLWindow *glWindow)
  : m_ShaderText(wgc0310::GetDefaultShaderText()),
    m_GLWindow(glWindow),
    m_ShaderPrevIndex(0),
    m_ShaderSubPrevIndex(0)
{
  this->setWindowTitle("着色器编辑器");

  QFont monospaceFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setSpacing(3);
  layout->setContentsMargins(3, 3, 3, 3);

  QHBoxLayout *hBox = new QHBoxLayout();
  layout->addLayout(hBox);

  QComboBox *shaderComboBox = new QComboBox();
  shaderComboBox->setMinimumWidth(120);
  shaderComboBox->addItem(QStringLiteral("非透明体"));
  shaderComboBox->addItem(QStringLiteral("透明体"));

  QComboBox *shaderSubComboBox = new QComboBox();
  shaderSubComboBox->setMinimumWidth(120);
  shaderSubComboBox->addItem(QStringLiteral("顶点着色器"));
  shaderSubComboBox->addItem(QStringLiteral("片段着色器"));

  QPushButton *loadPresetButton = new QPushButton("预设");
  loadPresetButton->setFixedWidth(48);

  QPushButton *newButton = new QPushButton("新建");
  newButton->setFixedWidth(48);

  QPushButton *loadButton = new QPushButton("读取");
  loadButton->setFixedWidth(48);

  QPushButton *saveButton = new QPushButton("保存");
  saveButton->setFixedWidth(48);

  QPushButton *compileButton = new QPushButton("编译");
  compileButton->setFixedWidth(48);

  hBox->addWidget(shaderComboBox);
  hBox->addWidget(shaderSubComboBox);
  hBox->addStretch();
  hBox->addWidget(loadPresetButton);
  hBox->addWidget(newButton);
  hBox->addWidget(loadButton);
  hBox->addWidget(saveButton);
  hBox->addWidget(compileButton);

  CodeEdit *codeEdit = new CodeEdit();
  codeEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  codeEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  codeEdit->setLineWrapMode(QTextEdit::NoWrap);
  codeEdit->setFont(monospaceFont);
  ShaderHighlighter *highlighter = new ShaderHighlighter(codeEdit->document());
  Q_UNUSED(highlighter)
  layout->addWidget(codeEdit);

  QPlainTextEdit *outputText = new QPlainTextEdit();
  outputText->setReadOnly(true);
  outputText->setVisible(false);
  outputText->setMaximumHeight(160);
  outputText->setStyleSheet("color: #cd0000");
  outputText->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  outputText->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  outputText->setLineWrapMode(QPlainTextEdit::NoWrap);
  outputText->setFont(monospaceFont);
  layout->addWidget(outputText);

  QHBoxLayout *statusLine = new QHBoxLayout();
  layout->addLayout(statusLine);

  QLabel *statusLabel = new QLabel("就绪");
  statusLine->addWidget(statusLabel);
  statusLine->addStretch();

  QLabel *lineDisplay = new QLabel();
  lineDisplay->setFixedWidth(72);
  lineDisplay->setFont(monospaceFont);
  statusLine->addWidget(lineDisplay);

  QLabel *columnDisplay = new QLabel();
  columnDisplay->setFixedWidth(72);
  columnDisplay->setFont(monospaceFont);
  statusLine->addWidget(columnDisplay);

  // update cursor
  connect(codeEdit, &QTextEdit::cursorPositionChanged, this, [codeEdit, lineDisplay, columnDisplay]() {
    QTextCursor cursor = codeEdit->textCursor();
    lineDisplay->setText(QString("LN: %1").arg(cursor.blockNumber() + 1));
    columnDisplay->setText(QString("COL: %1").arg(cursor.columnNumber() + 1));
  });

  auto saveCurrentShaderCode = [=, this] () {
    if (m_ShaderPrevIndex == 0) {
      if (m_ShaderSubPrevIndex == 0) {
        m_ShaderText.opaqueVS = codeEdit->toPlainText();
      } else {
        m_ShaderText.opaqueFS = codeEdit->toPlainText();
      }
    } else {
      if (m_ShaderSubPrevIndex == 0) {
        m_ShaderText.translucentVS = codeEdit->toPlainText();
      } else {
        m_ShaderText.translucentFS = codeEdit->toPlainText();
      }
    }
  };

  auto loadShaderCode = [=, this] {
    m_ShaderPrevIndex = shaderComboBox->currentIndex();
    m_ShaderSubPrevIndex = shaderSubComboBox->currentIndex();

    if (shaderComboBox->currentIndex() == 0) {
      if (shaderSubComboBox->currentIndex() == 0) {
        codeEdit->setPlainText(m_ShaderText.opaqueVS);
      } else {
        codeEdit->setPlainText(m_ShaderText.opaqueFS);
      }
    } else {
      if (shaderSubComboBox->currentIndex() == 0) {
        codeEdit->setPlainText(m_ShaderText.translucentVS);
      } else {
        codeEdit->setPlainText(m_ShaderText.translucentFS);
      }
    }
  };

  connect(shaderComboBox, &QComboBox::currentIndexChanged, this, saveCurrentShaderCode);
  connect(shaderComboBox, &QComboBox::currentIndexChanged, this, loadShaderCode);
  connect(shaderSubComboBox, &QComboBox::currentIndexChanged, this, saveCurrentShaderCode);
  connect(shaderSubComboBox, &QComboBox::currentIndexChanged, this, loadShaderCode);

  QMenu *loadPresetMenu = new QMenu(this);
  {
    QAction *phong = new QAction("标准着色器");
    phong->setData(QVariant(0));
    phong->setToolTip("Project-WG 从 0.4.0 开始默认使用的着色器");
    loadPresetMenu->addAction(phong);
  }

  connect(loadPresetMenu, &QMenu::triggered, this, [=, this] (QAction *action) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    int r = QMessageBox::warning(this,
                                 "警告",
                                 "你将会丢失所有更改！",
                                 "我手滑了",
                                 "JUST DO IT!");
#pragma clang diagnostic pop
    if (r != 1) {
      return;
    }

    switch (action->data().toInt()) {
    case 0: m_ShaderText = wgc0310::GetStandardShaderText(); break;
    default: Q_UNREACHABLE();
    }
    loadShaderCode();
  });

  connect(loadPresetButton, &QPushButton::clicked, this, [this, loadPresetMenu, loadPresetButton] {
    QPoint point = mapToGlobal(loadPresetButton->pos());
    point.setY(point.y() + loadPresetButton->height());
    loadPresetMenu->popup(point);
  });

  connect(newButton, &QPushButton::clicked, this, [=, this] {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    int r = QMessageBox::warning(this,
                                 "警告",
                                 "你将会丢失所有更改！",
                                 "我手滑了",
                                 "JUST DO IT!");
#pragma clang diagnostic pop
    if (r != 1) {
      return;
    }

    if (shaderSubComboBox->currentIndex() == 0) {
      codeEdit->setPlainText(cw::ReadToString(":/shader/empty/empty.vert"));
    } else {
      codeEdit->setPlainText(cw::ReadToString(":/shader/empty/empty.frag"));
    }
  });

  connect(loadButton, &QPushButton::clicked, this, [=, this] {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    int r = QMessageBox::warning(this,
                                 "警告",
                                 "你将会丢失所有更改！",
                                 "我手滑了",
                                 "JUST DO IT!");
#pragma clang diagnostic pop
    if (r != 1) {
      return;
    }

    QString fileName = QFileDialog::getOpenFileName(
      this,
      "打开文件",
      "",
      "GLSL shader (*.glsl *.vert *.vs *.frag *.fs);;All files (*.*)"
    );
    if (fileName.isEmpty()) {
      return;
    }

    QString content = cw::ReadToString(fileName);
    if (content.isEmpty()) {
      statusLabel->setText(QStringLiteral("打开文件 %1 失败").arg(fileName));
    } else {
      statusLabel->setText(QStringLiteral("已打开 %1").arg(fileName));
      codeEdit->setPlainText(content);
    }
  });

  connect(saveButton, &QPushButton::clicked, this, [=, this] {
    QString fileName = QFileDialog::getSaveFileName(
      this,
      "保存文件",
      "",
      "GLSL shader (*.glsl *.vert *.vs *.frag *.fs);;All files (*.*)"
    );
    if (fileName.isEmpty()) {
      return;
    }

    if (!fileName.endsWith(".glsl")
        && !fileName.endsWith(".vert")
        && !fileName.endsWith(".vs")
        && !fileName.endsWith(".frag")
        && !fileName.endsWith(".fs")) {
      fileName += ".glsl";
    }

    if (!cw::WriteToFile(fileName, codeEdit->toPlainText())) {
      statusLabel->setText(QStringLiteral("保存文件 %1 失败").arg(fileName));
    } else {
      statusLabel->setText(QStringLiteral("已保存 %1").arg(fileName));
    }
  });

  connect(compileButton, &QPushButton::clicked, this, [=, this] {
    saveCurrentShaderCode();
    m_GLWindow->RunWithGLContext([=, this] {
      QString errorMessage;
      std::unique_ptr<wgc0310::ShaderCollection> shader =
        wgc0310::CompileShader(m_GLWindow->GL, m_ShaderText, &errorMessage);

      if (!shader) {
        outputText->setPlainText(errorMessage);
        outputText->setVisible(true);
        statusLabel->setText("编译失败");
      } else {
        outputText->setVisible(false);
        statusLabel->setText("编译成功");
        m_GLWindow->SetShader(std::move(shader));
      }
    });
  });

  connect(m_GLWindow, &GLWindow::OpenGLInitialized, this, [=, this] {
    QString errorMessage;
    std::unique_ptr<wgc0310::ShaderCollection> shader =
      wgc0310::CompileShader(m_GLWindow->GL, m_ShaderText, &errorMessage);
    if (!shader) {
      std::abort();
    }

    m_GLWindow->SetShader(std::move(shader));
  });

  loadShaderCode();
  this->resize(800, 600);
}
