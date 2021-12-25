#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtWidgets/QApplication>

#include <AppWindow.hh>

void load_stylesheet(QApplication& app) {
	QFile ssfile(":/dark/stylesheet.qss");
	ssfile.open(QFile::ReadOnly | QFile::Text);
	QTextStream ssstream(&ssfile);
	app.setStyleSheet(ssstream.readAll());
}

void init_app(QApplication& app) {
	QApplication::setApplicationName("GenshinArtifactRespawnStat");
	QApplication::setWindowIcon(QIcon{ ":/icon.png" });
	load_stylesheet(app);
}

int main(int argc, char** argv) {
	using namespace GenshinArtifactSpawnStat;

	QApplication app{ argc, argv };
	init_app(app);

	AppWindow window;

	return app.exec();
}
