#include "ui_next/ShaderHighlighter.h"

ShaderHighlighter::ShaderHighlighter(QTextDocument *parent)
  : QSyntaxHighlighter(parent)
{
  HighlightingRule rule;

  pragmaFormat.setForeground(Qt::darkRed);
  pragmaFormat.setFontWeight(QFont::Bold);
  rule.pattern = QRegularExpression(QStringLiteral("#(.+)"));
  rule.format = pragmaFormat;
  highlightingRules.append(rule);

  functionFormat.setFontItalic(true);
  functionFormat.setForeground(Qt::blue);
  rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
  rule.format = functionFormat;
  highlightingRules.append(rule);

  typeFormat.setForeground(Qt::blue);
  const QString typePatterns[] = {
    // scalar types
    QStringLiteral("\\bbool\\b"),
    QStringLiteral("\\bfloat\\b"),
    QStringLiteral("\\bint\\b"),
    QStringLiteral("\\buint\\b"),
    QStringLiteral("\\bdouble\\b"),

    // vector
    QStringLiteral("\\b(|b|i|u|d)vec[2-4]\\b"),

    // matrix
    QStringLiteral("\\bmat[2-4]\\b"),
    QStringLiteral("\\bmat[2-4]x[2-4]\\b"),

    // sampler
    QStringLiteral("\\b(|b|i|u|d)sampler[1-3]D\\b"),
    QStringLiteral("\\b(|b|i|u|d)samplerCube\\b"),
    QStringLiteral("\\b(|b|i|u|d)sampler2DRect\\b"),
    QStringLiteral("\\b(|b|i|u|d)sampler[1-2]DArray\\b"),
    QStringLiteral("\\b(|b|i|u|d)samplerCubeArray\\b"),
    QStringLiteral("\\b(|b|i|u|d)samplerBuffer\\b"),
    QStringLiteral("\\b(|b|i|u|d)sampler2DMS\\b"),
    QStringLiteral("\\b(|b|i|u|d)sampler2DMSArray\\b"),

    // shadow sampler
    QStringLiteral("\\bsampler[1-2]DShadow\\b"),
    QStringLiteral("\\bsamplerCubeShadow\\b"),
    QStringLiteral("\\bsampler2DRectShadow\\b"),
    QStringLiteral("\\bsampler[1-2]DArrayShadow\\b"),
    QStringLiteral("\\bsamplerCubeArrayShadow\\b"),
  };
  for (const QString &pattern: typePatterns) {
    rule.pattern = QRegularExpression(pattern);
    rule.format = typeFormat;
    highlightingRules.append(rule);
  }

  keywordFormat.setForeground(Qt::darkBlue);
  keywordFormat.setFontWeight(QFont::Bold);
  const QString keywordPatterns[] = {
    // we take void as a keyword instead of type
    QStringLiteral("\\bvoid\\b"),
    QStringLiteral("\\bstruct\\b"),

    // type qualifiers
    // constant qualifier
    QStringLiteral("\\bconst\\b"),
    // layout qualifier
    QStringLiteral("\\blayout\\b"),
    QStringLiteral("\\blocation\\b"),
    QStringLiteral("\\bindex\\b"),
    QStringLiteral("\\bbinding\\b"),
    QStringLiteral("\\bcomponent\\b"),
    // storage qualifiers
    QStringLiteral("\\bin\\b"),
    QStringLiteral("\\bout\\b"),
    QStringLiteral("\\buniform\\b"),
    QStringLiteral("\\battribute\\b"),
    QStringLiteral("\\bvarying\\b"),

    // common operators
    QStringLiteral("\\breturn\\b"),
    QStringLiteral("\\bif\\b"),
    QStringLiteral("\\belse\\b"),
    QStringLiteral("\\bfor\\b"),
    QStringLiteral("\\bwhile\\b"),
    QStringLiteral("\\bdo\\b"),
    QStringLiteral("\\bswitch\\b"),
    QStringLiteral("\\bcase\\b"),
    QStringLiteral("\\bbreak\\b"),
    QStringLiteral("\\bcontinue\\b"),
    QStringLiteral("\\bdiscard\\b"),
    QStringLiteral("\\bdefault\\b")
  };
  for (const QString &pattern: keywordPatterns) {
    rule.pattern = QRegularExpression(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  builtinVarFormat.setForeground(Qt::darkGray);
  builtinVarFormat.setFontWeight(QFont::Bold);
  builtinVarFormat.setFontItalic(true);
  const QString builtinVariablePatterns[] = {
    // vertex shader
    QStringLiteral("\\bgl_VertexID\\b"),
    QStringLiteral("\\bgl_InstanceID\\b"),
    QStringLiteral("\\bgl_DrawID\\b"),
    QStringLiteral("\\bgl_BaseVertex\\b"),
    QStringLiteral("\\bgl_BaseInstance\\b"),
    QStringLiteral("\\bgl_Position\\b"),
    QStringLiteral("\\bgl_PointSize\\b"),
    QStringLiteral("\\bgl_ClipDistance\\b"),

    // fragment shader
    QStringLiteral("\\bgl_FragCoord\\b"),
    QStringLiteral("\\bgl_FrontFacing\\b"),
    QStringLiteral("\\bgl_PointCoord\\b"),
    QStringLiteral("\\bgl_SampleID\\b"),
    QStringLiteral("\\bgl_SamplePosition\\b"),
    QStringLiteral("\\bgl_SampleMaskIn\\b"),
    QStringLiteral("\\bgl_ClipDistance\\b"),
    QStringLiteral("\\bgl_PrimitiveID\\b"),
    QStringLiteral("\\bgl_Layer\\b"),
    QStringLiteral("\\bgl_ViewportIndex\\b"),
    QStringLiteral("\\bgl_FragDepth\\b"),
    QStringLiteral("\\bgl_SampleMask\\b"),

    // take these constants as built-in variables
    QStringLiteral("\\btrue\\b"),
    QStringLiteral("\\bfalse\\b")
  };
  for (const QString &pattern: builtinVariablePatterns) {
    rule.pattern = QRegularExpression(pattern);
    rule.format = builtinVarFormat;
    highlightingRules.append(rule);
  }

  singleLineCommentFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);

  multiLineCommentFormat.setForeground(Qt::darkGreen);

  commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
  commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
void ShaderHighlighter::highlightBlock(const QString &text) {
  for (const HighlightingRule &rule: qAsConst(highlightingRules)) {
    QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
    while (matchIterator.hasNext()) {
      QRegularExpressionMatch match = matchIterator.next();
      setFormat(match.capturedStart(),
                match.capturedLength(),
                rule.format);
    }
  }

  setCurrentBlockState(0);

  int startIndex = 0;
  if (previousBlockState() != 1) {
    startIndex = text.indexOf(commentStartExpression);
  }

  while (startIndex >= 0) {
    QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
    int endIndex = match.capturedStart();
    int commentLength;
    if (endIndex == -1) {
      setCurrentBlockState(1);
      commentLength = text.length() - startIndex;
    } else {
      commentLength = endIndex - startIndex
                      + match.capturedLength();
    }
    setFormat(startIndex, commentLength, multiLineCommentFormat);
    startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
  }
}
#pragma clang diagnostic pop
