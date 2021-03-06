#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    GLWidget* glW;
    Ui::MainWindow *ui;

protected:
    void resizeEvent(QResizeEvent*);
};

#endif // MAINWINDOW_H
