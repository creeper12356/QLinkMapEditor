#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "config.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QString path = "";
    FileState fState = not_open;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public:
    //getters
    Ui::MainWindow* getUi(){return ui;}
    FileState getFileState() const{return fState;}
    //setters
    void setFileState(FileState fs);
    //ui functions
public:
//    void updateStatusBar();
private:
    bool initMapSize();//使用对话框初始化地图规模，返回是否成功
    void initBoxList();
private slots:
    void on_actionNew_triggered();
    void on_actionQuit_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionClear_triggered();

    void fileStateChangedSlot(FileState state);


private:
    Ui::MainWindow *ui;
signals:
    void fileStateChanged(FileState state);
};
#endif // MAINWINDOW_H
