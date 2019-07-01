#pragma once

#include <QtWidgets/QMainWindow>
#include <QVector>
#include <vector>
#include <QPushButton>
#include <QHBoxLayout>
#include "ui_QtGuiApplication1.h"
//#include "OnePassSynth.h"

#define DEBUG 1

class MixDisplayer : public QWidget
{
	Q_OBJECT

public:
	MixDisplayer(int M, int N, int MaxFrame, QWidget *parent = Q_NULLPTR);

protected:
	void paintEvent(QPaintEvent*);
	void timerEvent(QTimerEvent*);

private slots:
	void Pause();
	void Start();
	void NextStep();
	void LastStep();
	void Restart();
	void SelectFile();
	void ShowPix();

private:
	static const int DELAY = 2000;
	static const int G_WIDTH = 120;
	static const int TOOLBAL_HEIGHT = 100;
	static const int icon_size = 75;
	const int MAX_FRAME;
	int frame;

	struct Node {
		Node() {}
		Node(int concentration, int x, int y) {
			_concentration = concentration;
			_x = x + 1;
			_y = y + 1;
		}
		int _concentration;
		int _x;
		int _y;
	};

	QVector <Node> nodes;
	//窗口大小
	int W_WIDTH;
	int W_HEIGHT;

	//窗口网格数
	int G_M;						
	int G_N;

	//芯片网格数
	int C_M;
	int C_N;

	//图像资源
#ifdef DEBUG
#define DETECT_PATH "C:\\GroupProject\\resources\\detect.png"
#define DISP_PATH "C:\\GroupProject\\resources\\disp.png"
#define SINK_PATH "C:\\GroupProject\\resources\\sink.png"
#define DROPLET_PATH "C:\\GroupProject\\resources\\droplet.png"
#define PAUSE_PATH "C:\\GroupProject\\resources\\pause.png"
#define START_PATH "C:\\GroupProject\\resources\\start.png"
#define RESTART_PATH "C:\\GroupProject\\resources\\restart.png"
#define NEXT_STEP_PATH "C:\\GroupProject\\resources\\next_step.png"
#define SELECT_FILE_PATH "C:\\GroupProject\\resources\\file.png"
#define ICON_PATH "C:\\GroupProject\\resources\\icon.png"

#define PROCESSOR_PATH "C:\\GroupProject\\resources\\src.exe"
#endif // DEBUG

	QImage Detector_img;
	QRect Detector_source;
	QVector <QRect> Detector_target;

	QImage Disp_img;
	QRect Disp_source;
	QVector <QRect> Disp_target;

	QImage Sink_img;
	QRect Sink_source;
	QVector <QRect> Sink_target;

	QImage Droplet_img;
	QRect Droplet_source;

	QHBoxLayout* hBox;

	QPushButton* PauseBtn;
	QPushButton* StartBtn;
	QPushButton* NextStepBtn;
	QPushButton* LastStepBtn;
	QPushButton* RestartBtn;
	QPushButton* SelectFileBtn;
	QPushButton* ShowPixBtn;

	QString filepath;

	int timerID;

	void loadResource();

	void getNodes(int frame);

	void drawGrids(QPainter* qp);
	void drawDroplets(QPainter* qp);
	void drawDetector(QPainter* qp);
	void drawSink(QPainter* qp);
	void drawDisp(QPainter* qp);

};

const int Example[10][8][6]=
{
	{
	{0,0,0,0,0,0},
	{0,0,0,0,0,1},
	{0,0,0,0,0,1},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	},
	{
	{0,0,0,0,0,0},
	{0,0,0,0,0,1},
	{0,0,0,0,0,1},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	},
	{
	{0,0,0,0,0,0},
	{0,0,0,0,0,2},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	},
	{
	{0,0,0,0,0,0},
	{0,0,0,0,0,2},
	{0,0,0,0,0,2},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	},
	{
	{0,0,0,0,0,0},
	{0,0,0,0,0,4},
	{0,0,0,0,0,4},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	},
	{
	{0,0,0,0,0,0},
	{0,0,0,0,0,4},
	{0,0,0,0,0,4},
	{1,0,0,0,0,0},
	{1,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	},
	{
	{0,0,0,0,0,0},
	{0,0,0,0,0,4},
	{0,0,0,0,0,4},
	{2,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	{0,0,0,0,0,0},
	},
	};

