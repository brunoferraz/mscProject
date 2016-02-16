#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    Ui::MainWindow *ui;

protected:
    void keyReleaseEvent(QKeyEvent *ev);
private slots:
    void on_checkBox_toggled(bool checked);
    void on_imageAlphaSlider_sliderMoved(int position);

    void on_reloadShadersButton_released();
};

#endif // MAINWINDOW_H
