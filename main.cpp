#include "mainwindow.h"
#include "nativeeventfilter.h"

#include <QApplication>

int main(int argc, char *argv[]){

	QApplication app(argc, argv);
	MainWindow window;
	NativeEventFilter * nef = new NativeEventFilter();
	app.installNativeEventFilter((QAbstractNativeEventFilter *) nef);

	// Install event filter to intercept key presses
	app.installEventFilter(&window);
	window.setNativeEventFilter(nef);

	//Connect the signals once the native event filter has been instantiated
	window.setConnects();

	return app.exec();
}
