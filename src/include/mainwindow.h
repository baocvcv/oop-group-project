#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QTimer>
#include <string>
#include <QFileDialog>

#include "MixDisplayer.h"
#include "displaydialog.h"
#include "Graph.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    MainWindow *ui;

    DisplayDialog *displayer;

    QWidget *window;

    QHBoxLayout* mainBox;
    QVBoxLayout* vBox;
    QGroupBox* gridGroupBox;

    QPushButton *selectFileBtn;
    QSpinBox* widthInput;
    QSpinBox* heightInput;
    QSpinBox* timeInput;
    QSpinBox* w1Input;
    QSpinBox* w2Input;
    QPushButton* confirmBtn;

    std::string filepath;

    GRBEnv env;

    void run();
    void onSelectFile();


};

#endif // MAINWINDOW_H
