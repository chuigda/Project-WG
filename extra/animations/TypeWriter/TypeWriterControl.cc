#include "TypeWriterControl.h"

#include <QPainter>
#include <QFontDatabase>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPlainTextEdit>
#include <QFontComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QColorDialog>

class PreviewWidget : public QWidget {
public:
  PreviewWidget(QString const* text,
                QColor const* foreColor,
                QColor const* backColor,
                QFont const* font,
                int const* alignmentFlag,
                QWidget* parent = nullptr)
    : QWidget(parent),
      m_Text(text),
      m_ForeColor(foreColor),
      m_BackColor(backColor),
      m_Font(font),
      m_AlignmentFlag(alignmentFlag)
  {
    setFixedSize(320, 240);
  }

protected:
  void paintEvent(QPaintEvent*) override {
    QPainter painter(this);

    painter.fillRect(rect(), *m_BackColor);
    painter.setPen(*m_ForeColor);
    painter.setFont(*m_Font);

    painter.drawText(8, 0, width() - 16, height(), *m_AlignmentFlag, *m_Text);
  }

private:
  QString const* m_Text;
  QColor const* m_ForeColor;
  QColor const* m_BackColor;
  QFont const* m_Font;
  int const* m_AlignmentFlag;
};

TypeWriterControl::TypeWriterControl(bool *contentChanged)
  : QWidget(nullptr, Qt::Window),
    m_ContentChanged(contentChanged),
    m_DisplayText("Hello, world!"),
    m_ForeColor(Qt::yellow),
    m_BackColor(Qt::black),
    m_Font(QFontDatabase::systemFont(QFontDatabase::FixedFont)),
    m_FontMetrics(m_Font),
    m_AlignmentFlag(Qt::AlignCenter)
{
  setWindowTitle("打字机");

  QVBoxLayout *layout = new QVBoxLayout();
  this->setLayout(layout);

  m_PreviewWidget = new PreviewWidget(&m_DisplayText,
                                      &m_ForeColor,
                                      &m_BackColor,
                                      &m_Font,
                                      &m_AlignmentFlag);

  QGroupBox *previewBox = new QGroupBox("预览", this);
  layout->addWidget(previewBox);
  {
    QHBoxLayout *displayLayout = new QHBoxLayout();
    previewBox->setLayout(displayLayout);
    displayLayout->addStretch();
    displayLayout->addWidget(m_PreviewWidget);
    displayLayout->addStretch();
  }

  {
    QHBoxLayout *hBox = new QHBoxLayout();
    layout->addLayout(hBox);

    QFontComboBox *fontBox = new QFontComboBox();
    hBox->addWidget(fontBox);

    connect(fontBox, &QFontComboBox::currentFontChanged, this, [this](QFont const &font) {
      m_Font.setFamily(font.family());
      updateFont();
    });

    QSpinBox *fontSizeSpin = new QSpinBox();
    hBox->addWidget(fontSizeSpin);
    fontSizeSpin->setFixedWidth(48);
    fontSizeSpin->setRange(8, 96);
    fontSizeSpin->setValue(m_Font.pointSize());

    connect(fontSizeSpin, &QSpinBox::valueChanged, this, [this](int fontSize) {
      m_Font.setPointSize(fontSize);
      updateFont();
    });

    hBox->addSpacing(24);

    QPushButton *foreColorButton = new QPushButton();
    hBox->addWidget(foreColorButton);
    foreColorButton->setToolTip("前景色");
    foreColorButton->setFixedWidth(24);
    foreColorButton->setStyleSheet(QStringLiteral("background-color: #%1").arg(m_ForeColor.rgb(), 6, 16, QLatin1Char('0')));
    connect(foreColorButton, &QPushButton::clicked, this, [this, foreColorButton] () {
      QColor color = QColorDialog::getColor(m_ForeColor, this, "前景色");
      if (color.isValid()) {
        m_ForeColor = color;
        foreColorButton->setStyleSheet(QStringLiteral("background-color: #%1").arg(m_ForeColor.rgb(), 6, 16, QLatin1Char('0')));
        updateCanvas();
      }
    });

    QPushButton *backColorButton = new QPushButton();
    hBox->addWidget(backColorButton);
    backColorButton->setToolTip("背景色");
    backColorButton->setFixedWidth(24);
    backColorButton->setStyleSheet(QStringLiteral("background-color: #%1").arg(m_BackColor.rgb(), 6, 16, QLatin1Char('0')));
    connect(backColorButton, &QPushButton::clicked, this, [this, backColorButton] () {
      QColor color = QColorDialog::getColor(m_BackColor, this, "背景色");
      if (color.isValid()) {
        m_BackColor = color;
        backColorButton->setStyleSheet(QStringLiteral("background-color: #%1").arg(m_BackColor.rgb(), 6, 16, QLatin1Char('0')));
        updateCanvas();
      }
    });

    QPushButton *boldButton = new QPushButton("B");
    hBox->addWidget(boldButton);
    boldButton->setToolTip("粗体");
    boldButton->setStyleSheet("font-weight: bold; font-family: sans");
    boldButton->setCheckable(true);
    boldButton->setFixedWidth(24);
    connect(boldButton, &QPushButton::toggled, this, [this] (bool checked) {
      m_Font.setBold(checked);
      updateFont();
    });

    QPushButton *italicButton = new QPushButton("I");
    hBox->addWidget(italicButton);
    italicButton->setToolTip("斜体");
    italicButton->setStyleSheet("font-style: italic; font-family: serif");
    italicButton->setCheckable(true);
    italicButton->setFixedWidth(24);
    connect(italicButton, &QPushButton::toggled, this, [this] (bool checked) {
      m_Font.setItalic(checked);
      updateFont();
    });

    QPushButton *antialiasing = new QPushButton("A");
    hBox->addWidget(antialiasing);
    antialiasing->setToolTip("禁用抗锯齿");
    antialiasing->setCheckable(true);
    antialiasing->setFixedWidth(24);
    {
      QFont aaFont = antialiasing->font();
      aaFont.setStyleStrategy(QFont::NoAntialias);
      antialiasing->setFont(aaFont);
    }
    connect(antialiasing, &QPushButton::toggled, this, [this] (bool checked) {
      m_Font.setStyleStrategy(checked ? QFont::NoAntialias : QFont::PreferAntialias);
      updateFont();
    });

    hBox->addSpacing(24);

    QFont materialIcons = QFont("Material Icons");
    QPushButton *leftAlign = new QPushButton("\ue236");
    hBox->addWidget(leftAlign);
    leftAlign->setToolTip("左对齐");
    leftAlign->setFont(materialIcons);
    leftAlign->setCheckable(true);
    leftAlign->setFixedWidth(24);
    leftAlign->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);

    QPushButton *centerAlign = new QPushButton("\ue234");
    hBox->addWidget(centerAlign);
    centerAlign->setToolTip("居中对齐");
    centerAlign->setFont(materialIcons);
    centerAlign->setCheckable(true);
    centerAlign->setFixedWidth(24);
    centerAlign->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);

    QPushButton *rightAlign = new QPushButton("\ue237");
    hBox->addWidget(rightAlign);
    rightAlign->setToolTip("右对齐");
    rightAlign->setFont(materialIcons);
    rightAlign->setCheckable(true);
    rightAlign->setFixedWidth(24);
    rightAlign->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);

    connect(leftAlign, &QPushButton::clicked, this, [this, leftAlign, centerAlign, rightAlign] (bool checked) {
      if (checked) {
        centerAlign->setChecked(false);
        rightAlign->setChecked(false);
        m_AlignmentFlag = Qt::AlignLeft | Qt::AlignVCenter;
        updateCanvas();
      } else {
        leftAlign->setChecked(true);
      }
    });

    connect(centerAlign, &QPushButton::clicked, this, [this, leftAlign, centerAlign, rightAlign] (bool checked) {
      if (checked) {
        leftAlign->setChecked(false);
        rightAlign->setChecked(false);
        m_AlignmentFlag = Qt::AlignHCenter | Qt::AlignVCenter;
        updateCanvas();
      } else {
        centerAlign->setChecked(true);
      }
    });

    connect(rightAlign, &QPushButton::clicked, this, [this, leftAlign, centerAlign, rightAlign] (bool checked) {
      rightAlign->setChecked(checked);
      if (checked) {
        leftAlign->setChecked(false);
        centerAlign->setChecked(false);
        m_AlignmentFlag = Qt::AlignRight | Qt::AlignVCenter;
        updateCanvas();
      } else {
        rightAlign->setChecked(true);
      }
    });

    centerAlign->setChecked(true);

    hBox->addSpacing(24);

    QPushButton *preview = new QPushButton("\uf1c5");
    hBox->addWidget(preview);
    preview->setToolTip("预览");
    preview->setFont(materialIcons);
    preview->setCheckable(true);
    preview->setFixedWidth(24);
    preview->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    connect(preview, &QPushButton::toggled, this, [this, previewBox] (bool checked) {
      previewBox->setVisible(checked);
      setFixedSize(sizeHint());
      updateCanvas();
    });
    preview->setChecked(true);
  }

  m_TextEdit = new QPlainTextEdit();
  layout->addWidget(m_TextEdit);
  m_TextEdit->setPlainText(m_DisplayText);
  m_TextEdit->setAttribute(Qt::WA_InputMethodEnabled);
  m_TextEdit->setAttribute(Qt::WA_InputMethodTransparent);
  m_TextEdit->setMinimumHeight(240);

  connect(m_TextEdit, &QPlainTextEdit::textChanged,
          this, &TypeWriterControl::updateText);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "VirtualCallInCtorOrDtor"
  // This should be safe since base class `QWidget` has been initialised,
  // and we did not reimplement `sizeHint`
  setFixedSize(sizeHint());
#pragma clang diagnostic pop
}

QImage TypeWriterControl::capture() const {
  QImage image(m_PreviewWidget->size(), QImage::Format_RGBA8888);
  m_PreviewWidget->render(&image);
  return image;
}

void TypeWriterControl::updateFont() {
  m_FontMetrics = QFontMetrics(m_Font);
  updateText();
}

void TypeWriterControl::updateText() {
  QString plainText = m_TextEdit->toPlainText();
  qsizetype lineCount = plainText.count('\n');
  qsizetype lineHeight = m_FontMetrics.lineSpacing();
  qsizetype expectedLineCount = (240 / lineHeight) - 2;

  if (lineCount > expectedLineCount) {
    QStringList lines = plainText.split('\n');
    lines = lines.mid(lines.size() - expectedLineCount);
    m_DisplayText = lines.join('\n');
  } else {
    m_DisplayText = plainText;
  }

  updateCanvas();
}

void TypeWriterControl::updateCanvas() {
  if (m_ContentChanged) {
    *m_ContentChanged = true;
  }
  m_PreviewWidget->update();
}
