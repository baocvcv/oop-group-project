#pragma once

#include <QtWidgets/QMainWindow>
#include <QVector>
#include <vector>
#include <QPushButton>
#include <QHBoxLayout>
#include "OnePassSynth.h"

#define DEBUG 1

class MixDisplayer : public QWidget
{
	Q_OBJECT

public:
    MixDisplayer(int M, int N, int MaxFrame,OnePassSynth& ops ,QWidget *parent = Q_NULLPTR);

    void set_sink_dispenser_data(std::vector<int> data);
    void set_grid_data(std::vector<std::vector<std::vector<std::pair<int, int>>>> data);
    void set_detector_data(std::vector<std::vector<std::pair<bool, std::string>>> data);

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
	static const int TOOLBAL_HEIGHT = 100;
	static const int icon_size = 75;
	const int MAX_FRAME;
	int frame;
    int G_WIDTH;

    struct _Node {
        _Node() {}
        _Node(std::pair<int,int> pair,int x,int y){
            if(pair.first>=0){
                _concentration=pair.second;
            }
            else{
                _concentration=pair.first;
            }
            _x=x+1;
            _y=y+1;
        }
        _Node(int concentration, int x, int y) {
			_concentration = concentration;
			_x = x + 1;
			_y = y + 1;
		}
		int _concentration;
		int _x;
		int _y;
	};

    QVector <_Node> nodes;
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
#define DETECT_PATH "/home/fred/Projects/oop-group-project/src/resources/detect.png"
#define DISP_PATH "/home/fred/Projects/oop-group-project/src/resources/disp.png"
#define SINK_PATH "/home/fred/Projects/oop-group-project/src/resources/sink.png"
#define DROPLET_PATH "/home/fred/Projects/oop-group-project/src/resources/droplet.png"
#define PAUSE_PATH "/home/fred/Projects/oop-group-project/src/resources/pause.png"
#define START_PATH "/home/fred/Projects/oop-group-project/src/resources/start.png"
#define RESTART_PATH "/home/fred/Projects/oop-group-project/src/resources/restart.png"
#define NEXT_STEP_PATH "/home/fred/Projects/oop-group-project/src/resources/next_step.png"
#define SELECT_FILE_PATH "/home/fred/Projects/oop-group-project/src/resources/file.png"
#define ICON_PATH "/home/fred/Projects/oop-group-project/src/resources/icon.png"

#define PROCESSOR_PATH "resources\\src.exe" // TODO: need change when not needed
#endif // DEBUG

	QImage Detector_img;
	QRect Detector_source;
	QVector <QRect> Detector_target;
    std::vector<std::vector<std::pair<bool, std::string>>> DetectorData;

	QImage Disp_img;
	QRect Disp_source;
	QVector <QRect> Disp_target;

	QImage Sink_img;
	QRect Sink_source;
	QVector <QRect> Sink_target;
    std::vector<int> SinkDispData;

	QImage Droplet_img;
	QRect Droplet_source;
    std::vector<std::vector<std::vector<std::pair<int, int>>>> DropletData;

    QVBoxLayout* vBox;

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

    void getPos(int& x, int& y, int w, int m, int n, int z);//m 列数，n 行数

};

/*int Example[10][8][6]=
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
    };*/
