#ifndef PROJECT_WG_UINEXT_SHADER_HIGHLIGHT_H
#define PROJECT_WG_UINEXT_SHADER_HIGHLIGHT_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class ShaderHighlighter : public QSyntaxHighlighter {
  Q_OBJECT

public:
  explicit ShaderHighlighter(QTextDocument *parent = nullptr);

protected:
  void highlightBlock(const QString &text) override;

private:
  struct HighlightingRule {
    QRegularExpression pattern;
    QTextCharFormat format;
  };
  QList<HighlightingRule> highlightingRules;

  QRegularExpression commentStartExpression;
  QRegularExpression commentEndExpression;

  QTextCharFormat pragmaFormat;
  QTextCharFormat keywordFormat;
  QTextCharFormat typeFormat;
  QTextCharFormat functionFormat;
  QTextCharFormat builtinVarFormat;
  QTextCharFormat singleLineCommentFormat;
  QTextCharFormat multiLineCommentFormat;
};

#endif // PROJECT_WG_UINEXT_SHADER_HIGHLIGHT_H
