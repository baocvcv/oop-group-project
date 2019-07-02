#ifndef DISPLAYDIALOG_H
#define DISPLAYDIALOG_H

#include <QDialog>
#include "OnePassSynth.h"
#include "MixDisplayer.h"
#include <QLayout>

class DisplayDialog: public QDialog {
    Q_OBJECT

public:
    DisplayDialog(int M, int N, int MaxFrame, OnePassSynth &ops, QWidget *parent = 0): QDialog(parent)
    {
        resize((M+3)*D, (N+3)*D);
        displayer = new MixDisplayer(M, N, MaxFrame, ops, this);
    }

private:
    MixDisplayer *displayer;

    const int D = 80;
};

#endif // DISPLAYDIALOG_H
