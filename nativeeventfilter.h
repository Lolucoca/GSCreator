#ifndef NATIVEEVENTFILTER_H
#define NATIVEEVENTFILTER_H


#include <QObject>
#include <QByteArray>
#include <QAbstractNativeEventFilter>

// This class receive the native Windows events, and emit a message when a hotkey is received
class NativeEventFilter : public QObject, QAbstractNativeEventFilter
{
	Q_OBJECT
	Q_DISABLE_COPY(NativeEventFilter)

  public:
	NativeEventFilter();
	bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

  signals:
	void keyPressed();
	void hotkeyReceived(int hotkeyId);
};

#endif // NATIVEEVENTFILTER_H
