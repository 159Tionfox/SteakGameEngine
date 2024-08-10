#include "steakgameengine.h"
#include "ui_steakgameengine.h"

SteakGameEngine::SteakGameEngine(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SteakGameEngine)
{
    ui->setupUi(this);

    InitUI();
    StyleUI();
}

SteakGameEngine::~SteakGameEngine()
{
    delete ui;
}

void SteakGameEngine::InitUI()
{
    resize(1600,900);
    setWindowFlag(Qt::FramelessWindowHint);
    tim = new QTimer();
    tim->setInterval(1000);
    connect(tim, SIGNAL(timeout()), this, SLOT(onTimeOut()));


    //QDockWidget
    DWrightA = new QDockWidget(this);
    DWrightA->setWindowTitle("World Object List");
    DWrightA->setAllowedAreas(Qt::RightDockWidgetArea);
    DWrightA->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
        LrlistA = new QListWidget(DWrightA);
        LrlistA->show();
    DWrightA->setWidget(LrlistA);
    DWrightB = new QDockWidget(this);
    DWrightB->setWindowTitle("Detail");
    DWrightB->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    DWrightB->setAllowedAreas(Qt::RightDockWidgetArea);
        LrlistB = new QListWidget(DWrightB);
        LrlistB->show();
    DWrightB->setWidget(LrlistB);

    Drdocklist << DWrightA;
    Drdocklist << DWrightB;
    Drsizelist << static_cast<int>(this->geometry().height() * 0.5);
    Drsizelist << static_cast<int>(this->geometry().height() * 0.5);

    this->addDockWidget(Qt::RightDockWidgetArea,DWrightA);
    this->addDockWidget(Qt::RightDockWidgetArea,DWrightB);
    this->resizeDocks(Drdocklist,Drsizelist,Qt::Vertical);

    DWbottomA = new QDockWidget(this);
    DWbottomA->setWindowTitle("File View");
    DWbottomA->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    DWbottomA->setAllowedAreas(Qt::BottomDockWidgetArea);
        LblistA = new QListWidget(DWrightB);
        LrlistA->show();
    DWbottomA->setWidget(LblistA);

    DWbottomB = new QDockWidget(this);
    DWbottomB->setWindowTitle("Resource Manager");
    DWbottomB->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    DWbottomB->setAllowedAreas(Qt::BottomDockWidgetArea);
        LblistB = new QListWidget(DWrightB);
        LblistB->show();
    DWbottomB->setWidget(LblistB);

    DWbottomC = new QDockWidget(this);
    DWbottomC->setWindowTitle("Log");
    DWbottomC->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    DWbottomC->setAllowedAreas(Qt::BottomDockWidgetArea);
        pte = new QPlainTextEdit(DWbottomC);
        if (!pte) {
            QMessageBox::warning(this, "Error Win Log", "Error:\n Engine Log System Init Error!", QMessageBox::Ok);
            QApplication* app;
            app->quit();
        }
        else
        {
            Log(pte, LogLevel::SUC, "Engine Log System Init Success");
        }
        pte->setReadOnly(true);
        pte->show();
    DWbottomC->setWidget(pte);

    Dbdocklist << DWbottomA;
    Dbdocklist << DWbottomB;
    Dbsizelist << static_cast<int>(this->geometry().width() * 0.1);
    Dbsizelist << static_cast<int>(this->geometry().width() * 0.9);

    this->addDockWidget(Qt::BottomDockWidgetArea,DWbottomA);
    this->addDockWidget(Qt::BottomDockWidgetArea,DWbottomB);
    this->addDockWidget(Qt::BottomDockWidgetArea,DWbottomC);
    this->tabifyDockWidget(DWbottomB, DWbottomC);
    this->resizeDocks(Dbdocklist,Dbsizelist,Qt::Horizontal);

    int widht = static_cast<int>(this->geometry().width() * 0.2);
    this->resizeDocks(Drdocklist,QList<int>({widht,widht}),Qt::Horizontal);



    //Menu
    menuBar = new QMenuBar(this);
    fileMenu = menuBar->addMenu("File");
        exit = new QAction("Eixt", this);
        fileMenu->addAction(exit);
        connect(exit, &QAction::triggered, this, [this]() { this->close(); });
    editMenu = menuBar->addMenu("Editor");
        obj2d = editMenu->addMenu("2D Object");
            rect = new QAction("Rect");
            obj2d->addAction(rect);
            connect(rect, &QAction::triggered, this, [this] {
                //WOpenGL->drawShape(WCopengl::Rect);
                });
    viewMenu = menuBar->addMenu("View");
        glview = viewMenu->addMenu("OpenGL ShowMode");
            glline = new QAction("Line Mode");
            glview->addAction(glline);
            connect(glline, &QAction::triggered, this, [this] {
                //WOpenGL->drawmode(WCopengl::LINE);
                });
            glface = new QAction("Face Mode");
            glview->addAction(glface);
            connect(glface, &QAction::triggered, this, [this] {
                //WOpenGL->drawmode(WCopengl::FACE);
                });
            glview->addSeparator();
            glclean = new QAction("Clean Win");
            glview->addAction(glclean);
            connect(glclean, &QAction::triggered, this, [this] {
                //WOpenGL->drawShape(WCopengl::None);
                });
    toolsMenu = menuBar->addMenu("Tools");
    winMenu = menuBar->addMenu("Windows");
    helpMenu = menuBar->addMenu("Help");

    QWidget* menu = new QWidget(this);
    QGridLayout* mwl = new QGridLayout(menu);
    menu->setLayout(mwl);

    QPushButton* btna = new QPushButton("A", this);
    btna->setStyleSheet("\
                        background-color:rgb(20,20,20);\
                        padding:5px;\
                        border-radius: 10px;");
    btna->installEventFilter(this);

    connect(btna, &QPushButton::clicked, this, [this] {
        if (clickcont % 2 == 0) {
            if (!ismax) {
                btnc->setText("S");
                showMaximized();
                ismax = true;
            }
            else
            {
                btnc->setText("M");
                showNormal();
                ismax = false;
            }
            clickcont++;
        }
        else
        {
            clickcont++;
        }
        tim->start();
        });

    connect(btna, &QPushButton::pressed, this,[this] {
            MousePress = true;
        });
    connect(btna, &QPushButton::released, this, [this] {
            MousePress = false;
        });
    QPushButton* btnb = new QPushButton("-", this);
    connect(btnb, &QPushButton::clicked, this, [this] {
            showMinimized();
        });
    btnc = new QPushButton("M", this);
    connect(btnc, &QPushButton::clicked, this, [this] {
            if (!ismax) {
                btnc->setText("S");
                showMaximized();
                ismax = true;
            }
            else
            {
                btnc->setText("M");
                showNormal();
                ismax = false;
            }
        });
    QPushButton* btnd = new QPushButton("×", this);
    connect(btnd, &QPushButton::clicked, this, [this] { this->close(); });
    btnb->setStyleSheet("color:rgb(255,255,255);font-size:20px;");
    btnc->setStyleSheet("color:rgb(255,255,255);font-size:20px;");
    btnd->setStyleSheet("color:rgb(255,255,255);font-size:20px;");

    mwl->addWidget(menuBar, 0, 0, 1, 1);
    mwl->addWidget(btna, 0, 1, 1, 1.5);
    mwl->addWidget(btnb, 0, 2, 1, 1);
    mwl->addWidget(btnc, 0, 3, 1, 1);
    mwl->addWidget(btnd, 0, 4, 1, 1);

    mwl->setColumnStretch(0, 0);
    mwl->setColumnStretch(1, 1);
    mwl->setColumnStretch(2, 0);
    mwl->setColumnStretch(3, 0);
    mwl->setColumnStretch(4, 0);


    setMenuWidget(menu);

    QMainWindow::DockOptions optionsa = dockOptions();
    optionsa |= QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks;
    setDockOptions(optionsa);

    //glwidget
    glwin = new OpenGLWin(pte);
    setCentralWidget(glwin);
}

void SteakGameEngine::StyleUI()
{
    setStyleSheet(" QWidget{background: rgb(50,50,50);}\
                    QMenuBar{color:rgb(255,255,255);font-size:20px;}\
                    QMenuBar::item::selected{background-color:rgb(235,110,36);}\
                    QMenu::separator{color:red; height:10px;}\
                    QMenu{ margin:3px; background-color:rgb(70,70,70);}\
                    QMenu::item{font-size:20px;color:rgb(255,255,255);padding:10px; padding-right:20px;}\
                    QMenu::item::selected{background-color:rgb(235,110,36);}\
                    QDockWidget{color:rgb(0,0,0);font-size:15px;background-color:rgb(150,255,255);}\
                    QTabBar::tab{color:rgb(255,255,255);font-size:15px;background-color:rgb(50,50,50);}\
                    ");
}

bool SteakGameEngine::eventFilter(QObject *object, QEvent *event)
{
    if (object->inherits("QPushButton")) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            MousePoint = mouseEvent->globalPos() - this->pos();
        }
        else if (event->type() == QEvent::MouseMove) {
            if (MousePress) {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                QPoint globalPos = mouseEvent->globalPos();
                QPoint newPos = globalPos - MousePoint;
                this->move(newPos);
            }
        }
    }
    return QMainWindow::eventFilter(object, event);
}

void SteakGameEngine::onTimeOut()
{
    clickcont = 1;
    tim->stop();
}
