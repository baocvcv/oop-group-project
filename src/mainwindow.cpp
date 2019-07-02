#include "mainwindow.h"
#include "Graph.h"
#include "GraphCreator.h"
#include "OnePassSynth.h"

#include <QLabel>
#include <QProcess>
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    displayer = nullptr;

    window = new QWidget;
    mainBox = new QHBoxLayout;

    gridGroupBox = new QGroupBox;
    vBox = new QVBoxLayout;

    selectFileBtn = new QPushButton(this);
    selectFileBtn->setText("Select input file");
    vBox->addWidget(selectFileBtn);

    auto gridBox = new QGridLayout();
    auto label1 = new QLabel("Width", this);
    widthInput = new QSpinBox(this);
    widthInput->setRange(1, 20);
    widthInput->setSingleStep(1);
    widthInput->setValue(5);
    auto label2 = new QLabel("Height", this);
    heightInput = new QSpinBox(this);
    heightInput->setRange(1, 20);
    heightInput->setSingleStep(1);
    heightInput->setValue(5);
    auto label3 = new QLabel("Time limit", this);
    timeInput = new QSpinBox(this);
    timeInput->setMinimum(1);
    timeInput->setSingleStep(1);
    timeInput->setValue(9);


    gridBox->addWidget(label1, 1, 1);
    gridBox->addWidget(widthInput, 1, 2);
    gridBox->addWidget(label2, 2, 1);
    gridBox->addWidget(heightInput, 2, 2);
    gridBox->addWidget(label3, 3, 1);
    gridBox->addWidget(timeInput, 3, 2);

    vBox->addLayout(gridBox);

    confirmBtn = new QPushButton(this);
    confirmBtn->setText("Run");
    confirmBtn->setDisabled(true);
    vBox->addWidget(confirmBtn);

    gridGroupBox->setLayout(vBox);
    mainBox->addWidget(gridGroupBox, 1);
    window->setLayout(mainBox);
    setCentralWidget(window);

    connect(selectFileBtn, &QPushButton::released, this, &MainWindow::onSelectFile);
    connect(confirmBtn, &QPushButton::released, this, &MainWindow::run);
}

void MainWindow::run(){
    int width = widthInput->value();
    int height = heightInput->value();
    int time = timeInput->value();

    // create graph
    Graph* graph;
    GraphCreator creat;

    graph = creat.CreatGraph(env);

    if(!graph->load(filepath)){
        // prompt error
        return;
    }
    int N = graph->get_d();
    graph->Init(N);
    graph->compute();
    graph->translator();

    OnePassSynth ops("input.txt");
    for(int t = time - 2; t <= time + 2; t++){
        if(ops.solve(width, height, t)){
            ops.print_solution();
            if(displayer != nullptr){
                delete displayer;
            }

//            displayer = new MixDisplayer(height, width, time+1,ops, window);
//            mainBox->addWidget(displayer, 3);
//            resize(width*100 + 400, height*100 + 300);
            displayer = new DisplayDialog(height, width, time+1, ops, this);
            displayer->show();
            break;
        }
    }
}

void MainWindow::onSelectFile(){
    filepath = QFileDialog::getOpenFileName(this, "Please SelectInput", ".", "").toStdString();\
    if(filepath != ""){
        confirmBtn->setEnabled(true);
    }
}


MainWindow::~MainWindow()
{

    delete ui;

}
