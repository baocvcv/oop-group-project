#include <QTime>
#include <QPainter>
#include <QLabel>
#include <QFileDialog>
#include <QProcess>
#include "include/MixDisplayer.h"


#pragma region Init
MixDisplayer::MixDisplayer(int M, int N, int MaxFrame,OnePassSynth& ops, QWidget *parent)
    : QWidget(parent), G_M(M + 2), G_N(N + 2), C_M(M), C_N(N), MAX_FRAME(MaxFrame)
{
    G_WIDTH = parent->height()/(N+4);
	W_HEIGHT = G_M * G_WIDTH;
	W_WIDTH = G_N * G_WIDTH;

    resize(W_WIDTH + TOOLBAL_HEIGHT, W_HEIGHT);

    set_grid_data(ops.get_grid());
    set_detector_data(ops.get_detector_pos());
    set_sink_dispenser_data(ops.get_sink_dispenser_pos());

	frame = 0;
	getNodes(frame);
	loadResource();

#ifdef DEBUG
//	Detector_target.push_back(QRect(0, 0, G_WIDTH, G_WIDTH));
//	Disp_target.push_back(QRect(7 * G_WIDTH, 2 * G_WIDTH, G_WIDTH, G_WIDTH));
//	Disp_target.push_back(QRect(7 * G_WIDTH, 3 * G_WIDTH, G_WIDTH, G_WIDTH));
//	Disp_target.push_back(QRect(0 * G_WIDTH, 4 * G_WIDTH, G_WIDTH, G_WIDTH));
//	Disp_target.push_back(QRect(0 * G_WIDTH, 5 * G_WIDTH, G_WIDTH, G_WIDTH));
//	Sink_target.push_back(QRect(7 * G_WIDTH, 4 * G_WIDTH, G_WIDTH, G_WIDTH));
#endif // DEBUG

    vBox = new QVBoxLayout(this);
    vBox->setAlignment(Qt::AlignRight|Qt::AlignTop );
    vBox->setSpacing(10);

	PauseBtn = new QPushButton(this);
	PauseBtn->setIcon(QIcon(PAUSE_PATH));
	PauseBtn->setFixedSize(icon_size, icon_size);

	StartBtn = new QPushButton(this);
	StartBtn->setIcon(QIcon(START_PATH));
	StartBtn->setFixedSize(icon_size, icon_size);

	NextStepBtn = new QPushButton(this);
	NextStepBtn->setIcon(QIcon(NEXT_STEP_PATH));
	NextStepBtn->setFixedSize(icon_size, icon_size);

	QImage img(NEXT_STEP_PATH);
	QPixmap mirror = QPixmap::fromImage(img.mirrored(true, false));
	LastStepBtn = new QPushButton(this);
	LastStepBtn->setIcon(QIcon(mirror));
	LastStepBtn->setFixedSize(icon_size, icon_size);

	RestartBtn = new QPushButton(this);
	RestartBtn->setIcon(QIcon(RESTART_PATH));
	RestartBtn->setFixedSize(icon_size, icon_size);

	SelectFileBtn = new QPushButton(this);
	SelectFileBtn->setIcon(QIcon(SELECT_FILE_PATH));
	SelectFileBtn->setFixedSize(icon_size, icon_size);

	ShowPixBtn = new QPushButton("show", this);
	SelectFileBtn->setFixedSize(icon_size, icon_size);

	StartBtn->setVisible(false);
	NextStepBtn->setVisible(false);
	LastStepBtn->setVisible(false);
	RestartBtn->setVisible(false);
	ShowPixBtn->setVisible(false);

    vBox->addWidget(StartBtn, 0, Qt::AlignRight | Qt::AlignTop);
    vBox->addWidget(RestartBtn, 0, Qt::AlignRight | Qt::AlignTop);
    vBox->addWidget(PauseBtn, 0, Qt::AlignRight | Qt::AlignTop);
    vBox->addWidget(LastStepBtn, 0, Qt::AlignRight | Qt::AlignTop);
    vBox->addWidget(NextStepBtn, 0, Qt::AlignRight | Qt::AlignTop);
    vBox->addWidget(RestartBtn, 0, Qt::AlignRight | Qt::AlignTop);
    vBox->addWidget(SelectFileBtn, 0, Qt::AlignRight | Qt::AlignTop);
    vBox->addWidget(ShowPixBtn, 0, Qt::AlignRight | Qt::AlignTop);

	connect(PauseBtn, &QPushButton::clicked, this ,&MixDisplayer::Pause);
	connect(StartBtn, &QPushButton::clicked, this, &MixDisplayer::Start);
	connect(NextStepBtn, &QPushButton::clicked, this, &MixDisplayer::NextStep);
	connect(LastStepBtn, &QPushButton::clicked, this, &MixDisplayer::LastStep);
	connect(RestartBtn, &QPushButton::clicked, this, &MixDisplayer::Restart);
	connect(SelectFileBtn, &QPushButton::clicked, this, &MixDisplayer::SelectFile);
	connect(ShowPixBtn, &QPushButton::clicked, this, &MixDisplayer::ShowPix);

	repaint();
	timerID = startTimer(DELAY);
}

void MixDisplayer::loadResource() {
	Detector_img.load(DETECT_PATH);
	Detector_source = QRect(0, 0, 200, 200);

	Disp_img.load(DISP_PATH);
	Disp_source = QRect(0, 0, 200, 200);

	Sink_img.load(SINK_PATH);
	Sink_source = QRect(0, 0, 200, 200);

	Droplet_img.load(DROPLET_PATH);
	Droplet_source = QRect(0, 0, 200, 200);
}
#pragma endregion

#pragma region Paint

void MixDisplayer::paintEvent(QPaintEvent* e) {
	Q_UNUSED(e);
	QPainter qp(this);

	drawGrids(&qp);
	drawDroplets(&qp);
	drawDetector(&qp);
	drawDisp(&qp);
	drawSink(&qp);
}

void MixDisplayer::drawGrids(QPainter* qp) {
	//color the chip
	qp->setPen(QColor("#d4d4d4"));
	qp->setBrush(QBrush("#4c4c4c"));

	qp->drawRect(G_WIDTH, G_WIDTH, (G_N - 2)*G_WIDTH, (G_M - 2)*G_WIDTH);


	QPen pen(Qt::black, 2, Qt::SolidLine);
	qp->setPen(pen);

	//draw rows
	for (int i = 1; i <= G_M; ++i) {
		qp->drawLine(0, G_WIDTH*i, W_WIDTH, G_WIDTH*i);
	}

	//draw coloums
	for (int i = 1; i <= G_N - 1; ++i) {
		qp->drawLine(G_WIDTH*i, 0, G_WIDTH*i, W_HEIGHT);
	}


}

void MixDisplayer::drawDroplets(QPainter* qp) {
	qp->setRenderHint(QPainter::Antialiasing, true);
	qp->setPen(QColor("#d4d4d4"));
	qp->setFont(QFont("Microsoft Yahei", 16));
    qp->setBrush(QBrush("#3c0404"));
	for (auto node : nodes) {
		//qp->drawEllipse(node._x*G_WIDTH, node._y*G_WIDTH, G_WIDTH, G_WIDTH);
        if(node._concentration ==-1){
            continue;
        }
        else if(node._concentration>=0){
            QRect target(node._x*G_WIDTH, node._y*G_WIDTH, G_WIDTH, G_WIDTH);
            qp->drawImage(target, Droplet_img, Droplet_source);
            QString text = QStringLiteral("%1").arg(node._concentration);
            qp->drawText(node._x*G_WIDTH, node._y*G_WIDTH, G_WIDTH, G_WIDTH, Qt::AlignCenter, text);
        }
        //Mixing
        else if(node._concentration==-2){
             QRect target(node._x*G_WIDTH, node._y*G_WIDTH, G_WIDTH, G_WIDTH);
             qp->drawRect(target);
        }
    }

}

void MixDisplayer::drawDetector(QPainter* qp) {
	for (auto target : Detector_target) {
		qp->drawImage(target, Detector_img, Detector_source);
	}
}

void MixDisplayer::drawSink(QPainter* qp) {
	for (auto target : Sink_target) {
		qp->drawImage(target, Sink_img, Sink_source);
	}
}

void MixDisplayer::drawDisp(QPainter* qp) {
	for (auto target : Disp_target) {
		bool horizental_mirror = target.left() > W_WIDTH / 2 ? true : false;
		bool vertical_mirror = target.bottom() > W_HEIGHT / 2 ? true : false;
		qp->drawImage(target, Disp_img.mirrored(horizental_mirror, vertical_mirror), Disp_source);
	}
}
#pragma endregion

#pragma region Action
void MixDisplayer::Pause() {
	StartBtn->setVisible(true);
	NextStepBtn->setVisible(true);
	LastStepBtn->setVisible(true);
	RestartBtn->setVisible(true);
	SelectFileBtn->setVisible(false);
	ShowPixBtn->setVisible(true);
	killTimer(timerID);
}

void MixDisplayer::Start() {
	timerID = startTimer(DELAY);
	StartBtn->setVisible(false);
	NextStepBtn->setVisible(false);
	LastStepBtn->setVisible(false);
	RestartBtn->setVisible(false);
	SelectFileBtn->setVisible(true);
	ShowPixBtn->setVisible(false);
}

void MixDisplayer::NextStep() {
	if (frame < MAX_FRAME - 1) {
		frame++;
		getNodes(frame);
		repaint();
	}
}

void MixDisplayer::LastStep() {
	if (frame > 0) {
		frame--;
		getNodes(frame);
		repaint();
	}
}

void MixDisplayer::Restart() {
	frame = 0;
	getNodes(0);
	repaint();
	Start();
}

void MixDisplayer::SelectFile() {
	filepath = QFileDialog::getOpenFileName(this, "Please SelectInput", ".", "");

	if (filepath == 0)
		return;
	//compute...
	QProcess* caller = new QProcess(this);
	caller->start(PROCESSOR_PATH, QStringList() << filepath);
	

	Restart();
}

void MixDisplayer::ShowPix() {
	QWidget* Png = new QWidget(this);
	Png->setWindowFlag(Qt::Window);
	Png->setWindowTitle("MixingStepTree");
	Png->resize(1000, 1000);
	Png->show();
}
#pragma endregion

#pragma region Timer

void MixDisplayer::timerEvent(QTimerEvent* e) {
	//finish condition
	if (frame == MAX_FRAME - 1) {
		repaint();
		return;
	}
	frame++;
	getNodes(frame);
	repaint();
}

void MixDisplayer::getNodes(int frame) {
	nodes.clear();
	for (int i = 0; i < C_M; ++i) {
		for (int j = 0; j < C_N; ++j) {
			//浓度为0，不加入
            if (DropletData[frame][i][j].first==-3)
				continue;
			else {
                nodes.push_back(_Node(DropletData[frame][i][j], j, i));
			}
		}
	}
}
#pragma endregion 

#pragma region DataProcess
// first = type(-3: empty, -1:detecting, -2:mixing, >=0 droplet), second = con
void MixDisplayer::set_grid_data(std::vector<std::vector<std::vector<std::pair<int, int> > > > data){
    DropletData = data;

}

void MixDisplayer::set_detector_data(std::vector<std::vector<std::pair<bool, std::string> > > data){
    for(unsigned int y = 0; y < data.size(); y++){
        for(unsigned int x = 0; x < data[y].size(); x++){
            if(data[y][x].first){
                Detector_target.push_back(QRect((x+1)*G_WIDTH, (y+1)*G_WIDTH, G_WIDTH, G_WIDTH));
            }
        }
    }
}

void MixDisplayer::set_sink_dispenser_data(std::vector<int> data){
    for(unsigned int p = 0; p < data.size(); p++){
        if(data[p] != 0){
            int x;
            int y;
            getPos(x, y, G_WIDTH, C_M, C_N, p);
            if(data[p] == 1){ // sink
                Sink_target.push_back(QRect(x, y, G_WIDTH, G_WIDTH));
            }else if(data[p] == 2){ // dispenser
                Disp_target.push_back(QRect(x, y, G_WIDTH, G_WIDTH));
            }
        }

    }
}

#pragma endregion

void MixDisplayer::getPos(int& x, int& y, int w, int m, int n, int z){//m 列数，n 行数
    if(z <= m-1){
        y = 0;
        x = w*(z+1);
    }
    else if(z<=m+n-1){
        x = w*(m+1);
        y = w*(z-m+1);
    }
    else if(z<=2*m + n -1){
        y = w*(n+1);
        x = w*(2*m+n-z);
    }
    else if(z<=2*m + 2*n -1){
        x = 0;
        y = w*(2*m+2*n-z);
    }
}
