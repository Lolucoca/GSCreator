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
#include <QCheckBox>
#include <QGroupBox>
#include <QMap>
#include <QClipboard>
#include <QVector>
#include <windows.h>
#include <QApplication>
#include "nativeeventfilter.h"

typedef struct keyStroke{
	uint8_t keyboardChar;
	bool shift;
	bool ctrl;
	bool alt;
} KeyStroke;


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
	void printKeys(QString str);
	void populateKeyMap();
	void sendReturn();
	void openChat();

	NativeEventFilter * nef;

	QGridLayout * guiLayout = new QGridLayout(nullptr);
	QGroupBox * signBox = new QGroupBox(this);

	QGridLayout * signLayout = new QGridLayout(nullptr);
	QLabel * rentSellLabel = new QLabel("Mieten/Verkaufen");
	QComboBox * rentSellComboBox = new QComboBox(nullptr);
	QLabel * locationLabel = new QLabel("Ortsname, z.B. ThatzVille");
	QLineEdit * locationName = new QLineEdit(nullptr);
	QLabel * gsLabel = new QLabel("GS-Name, z.B. Motel");
	QLineEdit * gsName = new QLineEdit(nullptr);
	QLabel * startGSLabel = new QLabel("Start-GS");
	QSpinBox * startGS = new QSpinBox(nullptr);
	QLabel * auxInfoLabel = new QLabel("Info, wie z.B. 100;7d;14d");
	QLineEdit * auxInfo = new QLineEdit(nullptr);


	QGroupBox * commandBox = new QGroupBox(this);
	QGridLayout * commandLayout = new QGridLayout(nullptr);
	QLabel * hotelLabel = new QLabel("Hotel");
	QCheckBox * hotelCheckBox = new QCheckBox(nullptr);
	QLabel * inactivityResetLabel = new QLabel("Inactivity Reset");
	QCheckBox * inactivityResetCheckBox = new QCheckBox(nullptr);
	QLabel * autoRestoreLabel = new QLabel("Auto Restore");
	QCheckBox * autoRestoreCheckBox = new QCheckBox(nullptr);

	QGroupBox * delayBox = new QGroupBox(this);
	QGridLayout * delayLayout = new QGridLayout(nullptr);


	QLabel * keyReturnDelayLabel = new QLabel("T(Key-Return)");
	QSpinBox * keyReturnDelay = new QSpinBox(nullptr);
	QLabel * returnKeyDelayLabel = new QLabel("T(Return-Key)");
	QSpinBox * returnKeyDelay = new QSpinBox(nullptr);
	QLabel * chatKeyDelayLabel = new QLabel("T(T-Key)");
	QSpinBox * chatKeyDelay = new QSpinBox(nullptr);
	QLabel * chatKeyReturnDelayLabel = new QLabel("T(Key-Return) (Chat)");
	QSpinBox * chatKeyReturnDelay = new QSpinBox(nullptr);
	QLabel * keyDelayLabel = new QLabel("T(Key-Key)");
	QSpinBox * keyDelay = new QSpinBox(nullptr);

	QLabel * useClipboardLabel = new QLabel("Methode");
	QComboBox * useClipboardComboBox = new QComboBox(nullptr);


	const QString rentLine = "[arm-rent]";
	const QString sellLine = "[arm-sell]";
	const QString hotelCommand = "/arm sethotel ";
	const QString inactivityResetCommand = "/arm setinactivityreset ";
	const QString autoRestoreCommand = "/arm setautorestore ";
	const QVector<QChar> listOfCtrlAltWords = {'[', ']', '{', '}'};
	QMap<QChar, KeyStroke> keyMap;

	bool useClipboard;
	bool setHotel;
	bool setInactivityReset;
	bool setAutoRestore;

	QString firstLine;

};
#endif // MAINWINDOW_H
