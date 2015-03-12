#include "mainwindow.h"
#include "connection.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	count = 0;
	ConnectionSelection sel(this);
	if (!(init = sel.exec()))
		return;
	sp = new QSerialPort(this);
	sp->setPortName(sel.serialPort);
	sp->setBaudRate(sel.serialSpeed);
	sp->open(sp->ReadWrite);
	if (!sp->isOpen()) {
		init = false;
		QMessageBox::critical(this, tr("Error"), tr("Cannot open selected serial port"));
		return;
	}
	while (sp->waitForReadyRead(1000))
		sp->readAll();
	connect(sp, SIGNAL(error(QSerialPort::SerialPortError)), qApp, SLOT(quit()));

	QWidget *w = new QWidget(this);
	QLayout *l = new QVBoxLayout(w);
	setCentralWidget(w);

	pbReq = new QPushButton(tr("&Capture"));
	pbReq->setDefault(true);
	l->addWidget(pbReq);
	connect(pbReq, SIGNAL(clicked()), this, SLOT(send()));

	lCapture = new QLabel;
	l->addWidget(lCapture);

	pbSave = new QPushButton(tr("&Save"));
	l->addWidget(pbSave);
	connect(pbSave, SIGNAL(clicked()), this, SLOT(save()));

	connect(sp, SIGNAL(readyRead()), this, SLOT(receive()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::send(void)
{
	sp->write("W", 1);
}

void MainWindow::receive(void)
{
	while (sp->bytesAvailable() && count != WIDTH * HEIGHT * 3)
		count += sp->read((char *)(buffer + count), WIDTH * HEIGHT * 3 - count);
	if (count == WIDTH * HEIGHT * 3) {
		img = QImage(buffer, WIDTH, HEIGHT, QImage::Format_RGB888);
		QPixmap pix(WIDTH, HEIGHT);
		QPainter p(&pix);
		p.drawImage(0, 0, img);
		p.end();
		lCapture->setPixmap(pix);
		lCapture->resize(pix.size());
		count = 0;
	}
}

void MainWindow::save(void)
{
	QString path = QFileDialog::getSaveFileName(this, tr("Save image file"));
	if (path.isEmpty())
		return;
	if (!img.save(path))
		QMessageBox::warning(this, tr("Error saving file"), tr("Save unsuccessful!"));
}
