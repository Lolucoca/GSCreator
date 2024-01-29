#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent){


	RegisterHotKey(0, 1, MOD_NOREPEAT, VK_F7); //Register F7 as hotkey

	firstLine = sellLine; //Default value

	guiLayout->addWidget(rentSellLabel, 0, 0);
	guiLayout->addWidget(rentSellComboBox, 1, 0);
	guiLayout->addWidget(locationLabel, 2, 0);
	guiLayout->addWidget(locationName, 3, 0);
	guiLayout->addWidget(gsLabel, 4, 0);
	guiLayout->addWidget(gsName, 5, 0);
	guiLayout->addWidget(startGSLabel, 6, 0);
	guiLayout->addWidget(startGS, 7, 0);
	guiLayout->addWidget(auxInfoLabel, 8, 0);
	guiLayout->addWidget(auxInfo, 9, 0);

	rentSellComboBox->addItem("Verkaufen");
	rentSellComboBox->addItem("Mieten");

	startGS->setValue(1);
	startGS->setMaximum(999);

	QWidget *window = new QWidget();
	window->setLayout(guiLayout);
	setCentralWidget(window);
	this->show();

	connect(rentSellComboBox, &QComboBox::currentIndexChanged, [=](){
		firstLine = rentSellComboBox->currentIndex() == 0 ? sellLine : rentLine;
	});
}

MainWindow::~MainWindow(){}




void MainWindow::setConnects(){
	connect(nef, &NativeEventFilter::hotkeyReceived, this, &MainWindow::keyPressed);
}

/*
	This function "types" in the strings as if it were a keyboard. It does that by copying the string
	to the clipboard and then pasting it in, which is way easier than doing it "by hand" using the virtual keys.
*/
void printKeys(QString str){

	QClipboard* clipboard = QApplication::clipboard();

	clipboard->setText(str, QClipboard::Clipboard);

	INPUT ip;

	// Pause for 250 milliseconds. The whole process is kind of dependent on waits.
	Sleep(250);

		// Set up a generic keyboard event.
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0;
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		// Press control+v keys
		ip.ki.wVk = VK_LCONTROL;
		ip.ki.dwFlags = 0x00;
		SendInput(1, &ip, sizeof(INPUT));

		ip.ki.wVk = QChar('V').unicode();
		ip.ki.dwFlags = 0x00;
		SendInput(1, &ip, sizeof(INPUT));

		Sleep(50);

		// Release the V
		ip.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &ip, sizeof(INPUT));

		// Release the Control key
		ip.ki.wVk = VK_LCONTROL;
		ip.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &ip, sizeof(INPUT));

		Sleep(50);

		//Press and release the return key
		ip.ki.wVk = VK_RETURN;
		ip.ki.dwFlags = 0x00;
		SendInput(1, &ip, sizeof(INPUT));

		ip.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &ip, sizeof(INPUT));

}

void MainWindow::keyPressed(int id){
	qDebug() << "MainWindow::keyPressed(): " << id;

	printKeys(firstLine);
	Sleep(50);
	printKeys(locationName->text());
	Sleep(50);
	//Pad the number strings with leading zeroes, so that for example motel-1 becomes motel-001
	printKeys(gsName->text() + "-" + QStringLiteral("%1").arg(startGS->value(), 3, 10, QLatin1Char('0')));
	Sleep(50);
	printKeys(auxInfo->text());
	startGS->setValue(startGS->value() + 1);

}

void MainWindow::setNativeEventFilter(NativeEventFilter * filter){
	this->nef = filter;
}


