#ifndef STEAKGAMEENGINE_H
#define STEAKGAMEENGINE_H

#include "Library.h"
#include "OpenGLWin.h"
#include "Engine.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SteakGameEngine;
}
QT_END_NAMESPACE

class SteakGameEngine : public QMainWindow
{
    Q_OBJECT

public:
    SteakGameEngine(QWidget *parent = nullptr);
    ~SteakGameEngine();

    void InitUI();
    void StyleUI();

    int menuBarHeight() const { return menuBar->height(); };

    OpenGLWin* glwin;
    Engine* engine;

private:
    Ui::SteakGameEngine *ui;
    //MenuBar
    QMenuBar* menuBar;
        QMenu* fileMenu;
            QAction* exit;
        QMenu* editMenu;
        QMenu* obj2d;
            QAction* rect;
        QMenu* viewMenu;
            QMenu* glview;
                QAction* glline;
                QAction* glface;
                QAction* glclean;
        QMenu* toolsMenu;
        QMenu* winMenu;
        QMenu* helpMenu;

    //QDockWidget
    QList<QDockWidget*> Drdocklist;
    QList<int> Drsizelist;
        QDockWidget *DWrightA;
            QListWidget *LrlistA;
        QDockWidget *DWrightB;
            QListWidget *LrlistB;
    QList<QDockWidget*> Dbdocklist;
    QList<int> Dbsizelist;
        QDockWidget *DWbottomA;
            QListWidget *LblistA;
        QDockWidget *DWbottomB;
            QListWidget *LblistB;
            QTableWidget *LbtableB;
    QDockWidget *DWbottomC;
        QPlainTextEdit *pte;


private:
    bool eventFilter(QObject* object, QEvent* event);
    bool MousePress;
    QPoint MousePoint;
    bool ismax = false;
    QPushButton* btnc;
    short clickcont = 1;
    QTimer* tim;

private slots:
    void onTimeOut();
};
#endif // STEAKGAMEENGINE_H
