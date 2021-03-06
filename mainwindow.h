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
    void on_saveImage_released();
    void on_eyedropper_toggled(bool checked);
    void on_showCameras_clicked(bool checked);
    void on_useWeights_clicked(bool checked);
};

#endif // MAINWINDOW_H
