#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent){


	RegisterHotKey(0, 1, MOD_NOREPEAT, VK_F7); //Register F7 as hotkey

	firstLine = sellLine; //Default value
	useClipboard = true;
	setHotel = false;
	setInactivityReset = false;
	setAutoRestore = false;

	guiLayout->addWidget(signBox, 0, 0, 2, 1);
	guiLayout->addWidget(commandBox, 0, 1, 1, 1);
	guiLayout->addWidget(delayBox, 1, 1, 1, 1);
	guiLayout->addWidget(useClipboardLabel, 2, 0, 1, 2);
	guiLayout->addWidget(useClipboardComboBox, 3, 0, 1, 2);

	signLayout->addWidget(rentSellLabel, 0, 0);
	signLayout->addWidget(rentSellComboBox, 1, 0);
	signLayout->addWidget(locationLabel, 2, 0);
	signLayout->addWidget(locationName, 3, 0);
	signLayout->addWidget(gsLabel, 4, 0);
	signLayout->addWidget(gsName, 5, 0);
	signLayout->addWidget(startGSLabel, 6, 0);
	signLayout->addWidget(startGS, 7, 0);
	signLayout->addWidget(auxInfoLabel, 8, 0);
	signLayout->addWidget(auxInfo, 9, 0);
	signBox->setLayout(signLayout);
	signBox->setTitle("Sign Data");

	commandLayout->addWidget(hotelLabel, 0, 0);
	commandLayout->addWidget(hotelCheckBox, 0, 1);
	commandLayout->addWidget(inactivityResetLabel, 1, 0);
	commandLayout->addWidget(inactivityResetCheckBox, 1, 1);
	commandLayout->addWidget(autoRestoreLabel, 2, 0);
	commandLayout->addWidget(autoRestoreCheckBox, 2, 1);
	commandBox->setLayout(commandLayout);
	commandBox->setTitle("Command Data");

	delayLayout->addWidget(keyReturnDelayLabel, 0, 0);
	delayLayout->addWidget(keyReturnDelay, 1, 0);
	delayLayout->addWidget(returnKeyDelayLabel, 2, 0);
	delayLayout->addWidget(returnKeyDelay, 3, 0);
	delayLayout->addWidget(chatKeyDelayLabel, 4, 0);
	delayLayout->addWidget(chatKeyDelay, 5, 0);
	delayLayout->addWidget(chatKeyReturnDelayLabel, 6, 0);
	delayLayout->addWidget(chatKeyReturnDelay, 7, 0);
	delayLayout->addWidget(pasteReleaseDelayLabel, 8, 0);
	delayLayout->addWidget(pasteReleaseDelay, 9, 0);
	delayLayout->addWidget(keyDelayLabel, 10, 0);
	delayLayout->addWidget(keyDelay, 11, 0);

	delayBox->setLayout(delayLayout);
	delayBox->setTitle("Delay Constants");

	rentSellComboBox->addItem("Verkaufen");
	rentSellComboBox->addItem("Mieten");

	useClipboardComboBox->addItem("Legacy - Keystrokes");
	useClipboardComboBox->addItem("Clipboard");

	useClipboardComboBox->setCurrentIndex(1); //Set to clipboard

	startGS->setValue(1);
	startGS->setMaximum(999);

	keyReturnDelay->setMaximum(999);
	keyReturnDelay->setValue(30);
	returnKeyDelay->setMaximum(999);
	returnKeyDelay->setValue(30);
	chatKeyDelay->setMaximum(999);
	chatKeyDelay->setValue(50);
	chatKeyReturnDelay->setMaximum(999);
	chatKeyReturnDelay->setValue(10);
	pasteReleaseDelay->setMaximum(999);
	pasteReleaseDelay->setValue(50);
	keyDelay->setMaximum(999);
	keyDelay->setValue(5);



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

	connect(hotelCheckBox, &QCheckBox::stateChanged, [=](){
		setHotel = hotelCheckBox->isChecked();
	});

	connect(inactivityResetCheckBox, &QCheckBox::stateChanged, [=](){
		setInactivityReset = inactivityResetCheckBox->isChecked();
	});

	connect(autoRestoreCheckBox, &QCheckBox::stateChanged, [=](){
		setAutoRestore = autoRestoreCheckBox->isChecked();
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

		Sleep(pasteReleaseDelay->value());

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

			Sleep(keyDelay->value());

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

inline void MainWindow::sendReturn(){
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

inline void MainWindow::openChat(){

	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	//Open Chat
	ip.ki.wVk = keyMap['T'].keyboardChar;
	ip.ki.dwFlags = 0x00;
	SendInput(1, &ip, sizeof(INPUT));

	ip.ki.wVk = keyMap['T'].keyboardChar;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

void MainWindow::keyPressed(int id){


	qDebug() << "MainWindow::keyPressed(): " << id;
	QString currentGSName = gsName->text() + "-" + QStringLiteral("%1").arg(startGS->value(), 3, 10, QLatin1Char('0'));


	printKeys(firstLine);
	Sleep(keyReturnDelay->value());
	sendReturn();
	Sleep(returnKeyDelay->value());

	printKeys(locationName->text());
	Sleep(keyReturnDelay->value());
	sendReturn();
	Sleep(returnKeyDelay->value());

	//Pad the number strings with leading zeroes, so that for example motel-1 becomes motel-001
	printKeys(currentGSName);
	Sleep(keyReturnDelay->value());
	sendReturn();
	Sleep(returnKeyDelay->value());

	printKeys(auxInfo->text());
	Sleep(keyReturnDelay->value());
	sendReturn();
	Sleep(returnKeyDelay->value());

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

	if(setHotel){
		//Open Chat
		openChat();
		Sleep(chatKeyDelay->value());
		printKeys(hotelCommand + currentGSName + " true");
		Sleep(chatKeyReturnDelay->value());
		sendReturn();
	}

	if(setInactivityReset){
		//Open Chat
		openChat();
		Sleep(chatKeyDelay->value());
		printKeys(inactivityResetCommand + currentGSName + " true");
		Sleep(chatKeyReturnDelay->value());
		sendReturn();
	}

	if(setAutoRestore){
		//Open Chat
		openChat();
		Sleep(chatKeyDelay->value());
		printKeys(autoRestoreCommand + currentGSName + " true");
		Sleep(chatKeyReturnDelay->value());
		sendReturn();
	}


	startGS->setValue(startGS->value() + 1);
}

void MainWindow::setNativeEventFilter(NativeEventFilter * filter){
	this->nef = filter;
}

void MainWindow::populateKeyMap(){
	keyMap.insert(' ', {' ', false, false, false});

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

