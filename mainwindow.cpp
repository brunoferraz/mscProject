#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <interface.h>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget->initialize();
    //ui->label->setText(QString::fromUtf8(Interface::infoBox));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyReleaseEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Left){
        ui->widget->prevCamera();
    }else if(ev->key() == Qt::Key_Right){
        ui->widget->nextCamera();
    }

    if(ev->key() == Qt::Key_1){
        Interface::camera = Interface::CAMERA_TYPES::FREE;
        ui->widget->changeCam();
    }else if(ev->key() == Qt::Key_2){
        Interface::camera = Interface::CAMERA_TYPES::CALIBRATED;
        ui->widget->changeCam();
    }
    ui->widget->update();
    if(ev->key()== Qt::Key_Escape){
        this->close();
    }
    if(ev->key()==Qt::Key_R){
        this->on_reloadShadersButton_released();
    }
}



void MainWindow::on_checkBox_toggled(bool checked)
{
    Interface::showBackgroundImage = checked;
    ui->widget->update();
}

void MainWindow::on_imageAlphaSlider_sliderMoved(int position)
{
    Interface::alphaBackgroundImage = float(position)/100.0;
    ui->widget->update();
}

void MainWindow::on_reloadShadersButton_released()
{
    ui->widget->reload();
    ui->widget->update();
}

void MainWindow::on_saveImage_released()
{
    ui->widget->saveImage();
}

void MainWindow::on_eyedropper_toggled(bool checked)
{
    Interface::eyeDropper = checked;
}
