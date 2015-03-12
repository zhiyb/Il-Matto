#include <QThread>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QSerialPortInfo>
#include <QList>
#include <QInputDialog>
#include <QProgressDialog>
#include "connection.h"

#define DEFAULT_SERIAL_PORT	"COM1"
#define DEFAULT_SERIAL_SPEED	BAUD
#define DATA_RATE_AVERAGE	5	// Need to be divisible by 60

ConnectionSelection::ConnectionSelection(QWidget *parent) : QDialog(parent)
{
	serialPort = DEFAULT_SERIAL_PORT;
	serialSpeed = DEFAULT_SERIAL_SPEED;

	QVBoxLayout *vLayout = new QVBoxLayout(this);
	QHBoxLayout *hSettingsLayout = new QHBoxLayout, *hButtonLayout = new QHBoxLayout;
	vLayout->addLayout(hSettingsLayout);
	hSettingsLayout->addWidget(lHost = new QLabel);
	hSettingsLayout->addWidget(leHost = new QLineEdit, 2);
	hSettingsLayout->addWidget(pbScan = new QPushButton(tr("Sc&an")));
	hSettingsLayout->addWidget(lPort = new QLabel);
	hSettingsLayout->addWidget(lePort = new QLineEdit, 1);
	vLayout->addLayout(hButtonLayout);
	hButtonLayout->addWidget(pbOpen = new QPushButton(tr("&Open")));
	pbOpen->setDefault(true);
	hButtonLayout->addWidget(pbCancel = new QPushButton(tr("&Cancel")));
	setWindowTitle("Connection settings");
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

	connect(pbScan, SIGNAL(clicked()), this, SLOT(scan()));
	connect(pbOpen, SIGNAL(clicked()), this, SLOT(accept()));
	connect(pbCancel, SIGNAL(clicked()), this, SLOT(reject()));

	normalize();
	lHost->setText(tr("Serial port:"));
	leHost->setText(serialPort);
	lPort->setText(tr("Speed:"));
	lePort->setText(QString::number(serialSpeed));
}

void ConnectionSelection::scan(void)
{
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	QStringList names;
	for (int i = 0; i < ports.count(); i++)
		names.append(ports.at(i).description() + " (" + ports.at(i).portName() + ")");
	if (names.count() == 0) {
		QMessageBox::information(this, tr("Serial port selection"), tr("No serial port available!"));
		return;
	}
	QString sel = QInputDialog::getItem(this, tr("Serial port selection"), tr("Please select a serial port hardware from the list:"), names, 0, false);
	if (!sel.isEmpty())
		for (int i = 0; i < ports.count(); i++)
			if (names.at(i) == sel) {
				leHost->setText(ports.at(i).portName());
				return;
			}
}

void ConnectionSelection::normalize(void)
{
	bool ok;
	QString tmpHost = leHost->text();
	int tmpPort = lePort->text().toInt(&ok);
	if (tmpHost.isEmpty())
		tmpHost = DEFAULT_SERIAL_PORT;
	if (!ok || tmpPort <= 0)
		tmpPort = DEFAULT_SERIAL_SPEED;
	serialPort = tmpHost;
	serialSpeed = tmpPort;
}

void ConnectionSelection::accept(void)
{
	normalize();
	if (!leHost->text().isEmpty())
		QDialog::accept();
}
