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
                QWidget* parent = nullptr)
    : QWidget(parent),
      m_Text(text),
      m_ForeColor(foreColor),
      m_BackColor(backColor),
      m_Font(font)
  {
    setFixedSize(320, 240);
  }

protected:
  void paintEvent(QPaintEvent*) override {
    QPainter painter(this);

    painter.fillRect(rect(), *m_BackColor);
    painter.setPen(*m_ForeColor);
    painter.setFont(*m_Font);
    painter.drawText(rect(), Qt::AlignCenter, *m_Text);
  }

private:
  QString const* m_Text;
  QColor const* m_ForeColor;
  QColor const* m_BackColor;
  QFont const* m_Font;
};

TypeWriterControl::TypeWriterControl(bool *contentChanged)
  : QWidget(nullptr, Qt::Window),
    m_ContentChanged(contentChanged),
    m_DisplayText("Hello, world!"),
    m_ForeColor(Qt::yellow),
    m_BackColor(Qt::black),
    m_Font(QFontDatabase::systemFont(QFontDatabase::FixedFont)),
    m_FontMetrics(m_Font)
{
  setWindowTitle("打字机");
  m_Font.setStyleStrategy(QFont::NoAntialias);

  QVBoxLayout *layout = new QVBoxLayout();
  this->setLayout(layout);

  m_PreviewWidget = new PreviewWidget(&m_DisplayText, &m_ForeColor, &m_BackColor, &m_Font);
  {
    QGroupBox *displayBox = new QGroupBox("预览", this);
    layout->addWidget(displayBox);
    QHBoxLayout *displayLayout = new QHBoxLayout();
    displayBox->setLayout(displayLayout);
    displayLayout->addStretch();
    displayLayout->addWidget(m_PreviewWidget);
    displayLayout->addStretch();
  }

  QHBoxLayout *hBox = new QHBoxLayout();
  layout->addLayout(hBox);
  {
    QFontComboBox *fontBox = new QFontComboBox();
    hBox->addWidget(fontBox);

    connect(fontBox, &QFontComboBox::currentFontChanged, this, [this] (QFont const& font) {
      m_Font.setFamily(font.family());
      updateFont();
    });

    QSpinBox *fontSizeSpin = new QSpinBox();
    hBox->addWidget(fontSizeSpin);
    fontSizeSpin->setFixedWidth(48);
    fontSizeSpin->setRange(8, 96);
    fontSizeSpin->setValue(m_Font.pointSize());

    connect(fontSizeSpin, &QSpinBox::valueChanged, this, [this] (int fontSize) {
      m_Font.setPointSize(fontSize);
      updateFont();
    });

    hBox->addStretch();

    QPushButton *foreColorButton = new QPushButton();
    hBox->addWidget(foreColorButton);
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
    antialiasing->setToolTip("抗锯齿");
    antialiasing->setCheckable(true);
    antialiasing->setFixedWidth(24);
    {
      QFont aaFont = antialiasing->font();
      aaFont.setStyleStrategy(QFont::NoAntialias);
      antialiasing->setFont(aaFont);
    }
    connect(antialiasing, &QPushButton::toggled, this, [this] (bool checked) {
      m_Font.setStyleStrategy(checked ? QFont::PreferAntialias : QFont::NoAntialias);
      updateFont();
    });
  }

  m_TextEdit = new QPlainTextEdit();
  layout->addWidget(m_TextEdit);
  m_TextEdit->setPlainText(m_DisplayText);
  m_TextEdit->setAttribute(Qt::WA_InputMethodEnabled);
  m_TextEdit->setAttribute(Qt::WA_InputMethodTransparent);

  connect(m_TextEdit, &QPlainTextEdit::textChanged,
          this, &TypeWriterControl::updateText);
}

QImage TypeWriterControl::capture() const {
  QImage image(m_PreviewWidget->size(), QImage::Format_ARGB32_Premultiplied);
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
