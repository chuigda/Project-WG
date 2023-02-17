#include <QApplication>
#include <QWidget>

class ConfigWidget final : public QWidget {
public:
  ConfigWidget() {
    setWindowTitle("Project-WG 配置工具");
    setFixedSize(400, 300);
  }
};

int main(int argc, char *argv[]) {
  QApplication a { argc, argv };
  QApplication::setWindowIcon(QIcon(QPixmap(":/icon-v2-config.png")));

  ConfigWidget w;
  w.show();

  return QApplication::exec();
}
