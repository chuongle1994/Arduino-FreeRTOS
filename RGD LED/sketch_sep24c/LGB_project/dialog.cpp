#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QtWidgets>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    initializeArduino();
    connectSignalsAndSlots();
}

Dialog::~Dialog()
{
    if (arduino->isOpen()) {
        arduino->close();
    }
    delete ui;
}

void Dialog::initializeArduino()
{
    arduino_is_available = false;
    arduino_port_name = "";
    arduino = new QSerialPort;

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()) {
            if (serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id &&
                serialPortInfo.productIdentifier() == arduino_uno_product_id) {
                arduino_port_name = serialPortInfo.portName();
                arduino_is_available = true;
                break;
            }
        }
    }

    if (arduino_is_available) {
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::WriteOnly);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
    } else {
        QMessageBox::warning(this, "Port error", "Couldn't find the Arduino!");
    }
}

void Dialog::connectSignalsAndSlots()
{
    connect(ui->red_slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    connect(ui->green_slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    connect(ui->blue_slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
}

void Dialog::onSliderValueChanged(int value)
{
    QSlider* senderSlider = qobject_cast<QSlider*>(sender());
    if (senderSlider) {
        QString color;
        if (senderSlider == ui->red_slider) {
            color = "r";
        } else if (senderSlider == ui->green_slider) {
            color = "g";
        } else if (senderSlider == ui->blue_slider) {
            color = "b";
        }

        ui->red_value->setText(QString("<span style=\" font-size:18pt; font-weight:600;\">%1</span>").arg(ui->red_slider->value()));
        ui->green_value->setText(QString("<span style=\" font-size:18pt; font-weight:600;\">%1</span>").arg(ui->green_slider->value()));
        ui->blue_value->setText(QString("<span style=\" font-size:18pt; font-weight:600;\">%1</span>").arg(ui->blue_slider->value()));

        updateRGB(color + QString::number(value));
        qDebug() << value;
    }
}

void Dialog::updateRGB(const QString& command)
{
    if (arduino->isWritable()) {
        arduino->write(command.toStdString().c_str());
    } else {
        qDebug() << "Couldn't write to serial!";
    }
}
