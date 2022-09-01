#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QTimer>
#include <QPushButton>

QT_BEGIN_NAMESPACE
//namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//устанавливает красный экран, меняет заголовок окна (по ТЗ), выводит счет
void setLoose(QWidget);

/*
Анализирует положение снежинки:
    - вышла за горизонтальные размеры окна - кнопка-снежинка удаляется;
    - низ кнопки-снежинки коснулся низа окна - игра проиграна;
    - для упавших снежинок обеспечивается их удержание внизу окна.
*/
void checkBorders(QPushButton, QWidget, QTimer);

//смещает кнопку вниз с заданной скоростью
void moveButton(QPushButton, int speed, QTimer);

//генерирует кнопку и запускает движение вниз
void createButton(QTimer, int x, int y, QWidget);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QHBoxLayout *m_mainLayout = nullptr;
};


#endif // MAINWINDOW_H
