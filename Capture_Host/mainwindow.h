#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QSerialPort>

#define PORTRAIT

#ifdef PORTRAIT
#define WIDTH	240
#define HEIGHT	320
#else
#define WIDTH	320
#define HEIGHT	240
#endif

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	bool success(void) {return init;}

private slots:
	void send(void);
	void receive(void);
	void save(void);

private:
	bool init;
	quint32 count;
	quint8 buffer[WIDTH * HEIGHT * 3];
	QSerialPort *sp;
	QPushButton *pbReq, *pbSave;
	QLabel *lCapture;
	QImage img;
};

#endif // MAINWINDOW_H
