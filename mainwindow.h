#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QClipboard>
#include <windows.h>
#include <QApplication>
#include "nativeeventfilter.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	void setConnects();
	void setNativeEventFilter(NativeEventFilter * filter);

public slots:
	void keyPressed(int id);


private:

	NativeEventFilter * nef;
	QGridLayout * guiLayout = new QGridLayout(nullptr);
	QLabel * rentSellLabel = new QLabel("Mieten/Verkaufen");
	QComboBox * rentSellComboBox = new QComboBox(this);
	QLabel * locationLabel = new QLabel("Ortsname, z.B. ThatzVille");
	QLineEdit * locationName = new QLineEdit(nullptr);
	QLabel * gsLabel = new QLabel("GS-Name, z.B. Motel");
	QLineEdit * gsName = new QLineEdit(nullptr);
	QLabel * startGSLabel = new QLabel("Start-GS");
	QSpinBox * startGS = new QSpinBox(nullptr);
	QLabel * auxInfoLabel = new QLabel("Info, wie z.B. 100;7d;14d");
	QLineEdit * auxInfo = new QLineEdit(nullptr);


	const QString rentLine = "[arm-rent]";
	const QString sellLine = "[arm-sell]";

	QString firstLine;

};
#endif // MAINWINDOW_H
