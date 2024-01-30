#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent){


	RegisterHotKey(0, 1, MOD_NOREPEAT, VK_F7); //Register F7 as hotkey

	firstLine = sellLine; //Default value
	useClipboard = true;

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
	guiLayout->addWidget(useClipboardLabel, 10, 0);
	guiLayout->addWidget(useClipboardComboBox, 11, 0);

	rentSellComboBox->addItem("Verkaufen");
	rentSellComboBox->addItem("Mieten");

	useClipboardComboBox->addItem("Legacy - Keystrokes");
	useClipboardComboBox->addItem("Clipboard");

	useClipboardComboBox->setCurrentIndex(1); //Set to clipboard

	startGS->setValue(1);
	startGS->setMaximum(999);

	QWidget *window = new QWidget();
	window->setLayout(guiLayout);
	setCentralWidget(window);
	this->show();

	connect(rentSellComboBox, &QComboBox::currentIndexChanged, [=](){
		firstLine = rentSellComboBox->currentIndex() == 0 ? sellLine : rentLine;
	});

	connect(useClipboardComboBox, &QComboBox::currentIndexChanged, [=](){
		useClipboard = (bool) useClipboardComboBox->currentIndex(); //There are only two ways so we can use it as a bool
	});

	populateKeyMap();
}

MainWindow::~MainWindow(){}




void MainWindow::setConnects(){
	connect(nef, &NativeEventFilter::hotkeyReceived, this, &MainWindow::keyPressed);
}

/*
	This function "types" in the strings as if it were a keyboard. It does that by copying the string
	to the clipboard and then pasting it in, which is way easier than doing it "by hand" using the virtual keys.
*/
void MainWindow::printKeys(QString str){

	QClipboard* clipboard = QApplication::clipboard();

	clipboard->setText(str, QClipboard::Clipboard);

	INPUT ip;

	// Pause for 250 milliseconds. The whole process is kind of dependent on waits.
	// Sleep(250);

	if(useClipboard){
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

	} else { //Send keystrokes manually

		// Set up a generic keyboard event.
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0;
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		for(int i = 0; i < str.length(); i++){

			if(!keyMap.contains(str.at(i))){
				qDebug() << "KeyMap doesn't contain character!";
				break;
			}

			bool shift = keyMap[str.at(i)].shift;
			bool ctrl = keyMap[str.at(i)].ctrl;
			bool alt = keyMap[str.at(i)].alt;


			if(shift){
				ip.type = INPUT_KEYBOARD;
				ip.ki.wScan = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
				ip.ki.time = 0;
				ip.ki.wVk = VK_LSHIFT;
				ip.ki.dwFlags = 0x00;
				SendInput(1, &ip, sizeof(INPUT));
			}

			if(ctrl){
				ip.type = INPUT_KEYBOARD;
				ip.ki.wScan = MapVirtualKey(VK_LCONTROL, MAPVK_VK_TO_VSC);
				ip.ki.time = 0;
				ip.ki.wVk = VK_LCONTROL;
				ip.ki.dwFlags = 0x00;
				SendInput(1, &ip, sizeof(INPUT));
			}

			if(alt){
				ip.type = INPUT_KEYBOARD;
				ip.ki.wScan = MapVirtualKey(VK_LMENU, MAPVK_VK_TO_VSC);
				ip.ki.time = 0;
				ip.ki.wVk = VK_LMENU;
				ip.ki.dwFlags = 0x00;
				SendInput(1, &ip, sizeof(INPUT));
			}

			ip.type = INPUT_KEYBOARD;
			ip.ki.wScan = MapVirtualKey(keyMap[str.at(i)].keyboardChar, MAPVK_VK_TO_VSC);
			ip.ki.time = 0;
			ip.ki.wVk = keyMap[str.at(i)].keyboardChar;
			ip.ki.dwFlags = 0x00;
			SendInput(1, &ip, sizeof(INPUT));

			Sleep(20);

			// Release
			ip.ki.dwFlags = KEYEVENTF_KEYUP;
			SendInput(1, &ip, sizeof(INPUT));

			//Also release alt
			if(alt){
				ip.ki.wVk = VK_LMENU;
				ip.ki.dwFlags = KEYEVENTF_KEYUP;
				SendInput(1, &ip, sizeof(INPUT));
			}

			//Also release ctrl
			if(ctrl){
				ip.ki.wVk = VK_LCONTROL;
				ip.ki.dwFlags = KEYEVENTF_KEYUP;
				SendInput(1, &ip, sizeof(INPUT));
			}

			//Also release shift
			if(shift){
				ip.ki.wVk = VK_LSHIFT;
				ip.ki.dwFlags = KEYEVENTF_KEYUP;
				SendInput(1, &ip, sizeof(INPUT));
			}
		}
	}
}

inline void sendReturn(){
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	//Press and release the return key
	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = 0x00;
	SendInput(1, &ip, sizeof(INPUT));

	ip.ki.wVk = VK_RETURN;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

void MainWindow::keyPressed(int id){


	qDebug() << "MainWindow::keyPressed(): " << id;

	printKeys(firstLine);
	Sleep(50);
	sendReturn();
	Sleep(50);

	printKeys(locationName->text());
	Sleep(50);
	sendReturn();
	Sleep(100);

	//Pad the number strings with leading zeroes, so that for example motel-1 becomes motel-001
	printKeys(gsName->text() + "-" + QStringLiteral("%1").arg(startGS->value(), 3, 10, QLatin1Char('0')));
	Sleep(100);
	sendReturn();
	Sleep(50);

	printKeys(auxInfo->text());
	startGS->setValue(startGS->value() + 1);
	Sleep(50);
	sendReturn();
	Sleep(50);

	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	//Press and release the return key
	ip.ki.wVk = VK_ESCAPE;
	ip.ki.dwFlags = 0x00;
	SendInput(1, &ip, sizeof(INPUT));

	ip.ki.wVk = VK_ESCAPE;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

void MainWindow::setNativeEventFilter(NativeEventFilter * filter){
	this->nef = filter;
}

void MainWindow::populateKeyMap(){
	keyMap.insert('A', {'A', true, false, false});
	keyMap.insert('B', {'B', true, false, false});
	keyMap.insert('C', {'C', true, false, false});
	keyMap.insert('D', {'D', true, false, false});
	keyMap.insert('E', {'E', true, false, false});
	keyMap.insert('F', {'F', true, false, false});
	keyMap.insert('G', {'G', true, false, false});
	keyMap.insert('H', {'H', true, false, false});
	keyMap.insert('I', {'I', true, false, false});
	keyMap.insert('J', {'J', true, false, false});
	keyMap.insert('K', {'K', true, false, false});
	keyMap.insert('L', {'L', true, false, false});
	keyMap.insert('M', {'M', true, false, false});
	keyMap.insert('N', {'N', true, false, false});
	keyMap.insert('O', {'O', true, false, false});
	keyMap.insert('P', {'P', true, false, false});
	keyMap.insert('Q', {'Q', true, false, false});
	keyMap.insert('R', {'R', true, false, false});
	keyMap.insert('S', {'S', true, false, false});
	keyMap.insert('T', {'T', true, false, false});
	keyMap.insert('U', {'U', true, false, false});
	keyMap.insert('V', {'V', true, false, false});
	keyMap.insert('W', {'W', true, false, false});
	keyMap.insert('X', {'X', true, false, false});
	keyMap.insert('Y', {'Y', true, false, false});
	keyMap.insert('Z', {'Z', true, false, false});

	keyMap.insert('a', {'A', false, false, false});
	keyMap.insert('b', {'B', false, false, false});
	keyMap.insert('c', {'C', false, false, false});
	keyMap.insert('d', {'D', false, false, false});
	keyMap.insert('e', {'E', false, false, false});
	keyMap.insert('f', {'F', false, false, false});
	keyMap.insert('g', {'G', false, false, false});
	keyMap.insert('h', {'H', false, false, false});
	keyMap.insert('i', {'I', false, false, false});
	keyMap.insert('j', {'J', false, false, false});
	keyMap.insert('k', {'K', false, false, false});
	keyMap.insert('l', {'L', false, false, false});
	keyMap.insert('m', {'M', false, false, false});
	keyMap.insert('n', {'N', false, false, false});
	keyMap.insert('o', {'O', false, false, false});
	keyMap.insert('p', {'P', false, false, false});
	keyMap.insert('q', {'Q', false, false, false});
	keyMap.insert('r', {'R', false, false, false});
	keyMap.insert('s', {'S', false, false, false});
	keyMap.insert('t', {'T', false, false, false});
	keyMap.insert('u', {'U', false, false, false});
	keyMap.insert('v', {'V', false, false, false});
	keyMap.insert('w', {'W', false, false, false});
	keyMap.insert('x', {'X', false, false, false});
	keyMap.insert('y', {'Y', false, false, false});
	keyMap.insert('z', {'Z', false, false, false});

	keyMap.insert('1', {'1', false, false, false});
	keyMap.insert('2', {'2', false, false, false});
	keyMap.insert('3', {'3', false, false, false});
	keyMap.insert('4', {'4', false, false, false});
	keyMap.insert('5', {'5', false, false, false});
	keyMap.insert('6', {'6', false, false, false});
	keyMap.insert('7', {'7', false, false, false});
	keyMap.insert('8', {'8', false, false, false});
	keyMap.insert('9', {'9', false, false, false});
	keyMap.insert('0', {'0', false, false, false});

	keyMap.insert('!', {'1', true, false, false});
	keyMap.insert('"', {'2', true, false, false});
	keyMap.insert('$', {'4', true, false, false});
	keyMap.insert('%', {'5', true, false, false});
	keyMap.insert('&', {'6', true, false, false});
	keyMap.insert('/', {'7', true, false, false});
	keyMap.insert('(', {'8', true, false, false});
	keyMap.insert(')', {'9', true, false, false});
	keyMap.insert('=', {'0', true, false, false});

	keyMap.insert('{', {'7', false, true, true});
	keyMap.insert('[', {'8', false, true, true});
	keyMap.insert(']', {'9', false, true, true});
	keyMap.insert('}', {'0', false, true, true});

	keyMap.insert('?',  {(uint8_t) VK_OEM_4, true, false, false});
	keyMap.insert('\\', {(uint8_t) VK_OEM_4, false, true, true});
	keyMap.insert('^',  {(uint8_t) VK_OEM_5, false, false, false});

	keyMap.insert('<', {(uint8_t) VK_OEM_102,    false, false, false});
	keyMap.insert(',', {(uint8_t) VK_OEM_COMMA,  false, false, false});
	keyMap.insert('.', {(uint8_t) VK_OEM_PERIOD, false, false, false});
	keyMap.insert('-', {(uint8_t) VK_OEM_MINUS,  false, false, false});
	keyMap.insert('#', {(uint8_t) VK_OEM_2,      false, false, false});
	keyMap.insert('+', {(uint8_t) VK_OEM_PLUS,   false, false, false});

	keyMap.insert('>',  {(uint8_t) VK_OEM_102,    true, false, false});
	keyMap.insert(';',  {(uint8_t) VK_OEM_COMMA,  true, false, false});
	keyMap.insert(':',  {(uint8_t) VK_OEM_PERIOD, true, false, false});
	keyMap.insert('_',  {(uint8_t) VK_OEM_MINUS,  true, false, false});
	keyMap.insert('\'', {(uint8_t) VK_OEM_2,      true, false, false});
	keyMap.insert('*',  {(uint8_t) VK_OEM_PLUS,   true, false, false});

	keyMap.insert('|', {(uint8_t) VK_OEM_102,    false, true, true});
	keyMap.insert('~', {(uint8_t) VK_OEM_PLUS,   false, true, true});


}

