#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    glW = new GLWidget(0, 0);
    this->layout()->addWidget(glW);

    glW->setGeometry(0, 0, 600, 600);
    glW->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    glW->setGeometry(0,0,width(), height());
}
