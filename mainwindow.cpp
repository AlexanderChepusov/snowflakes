#define QT_NO_DEBUG_OUTPUT

#include "mainwindow.h"
#include <QHBoxLayout>
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>
#include <QString>

#define CUBE_SIZE 20
#define MAX_SPEED 4
#define FREQUENCY 100
#define FLAKES 2
#define TEST 0
#define RADIUS 50

bool isDefeated;    //флаг поражения
int score;          //счет
QCursor *mouseCursor;

void setLoose(QWidget *w){
    //qDebug() << "set";
    QString title;// = new QString("You've lost! Your score is ");
    title = QString("You've lost! Your score is %1")
                    .arg(score);
    w -> setWindowTitle(title);
    w -> setStyleSheet("background-color: red;");
}

void checkBorders(QPushButton *button, QWidget *w, QTimer *t){
    //qDebug() << "check";
    int width = w -> geometry().right();
    int bottom = w -> geometry().bottom();
    int y = button -> geometry().bottom();
    int x = button -> geometry().left();

    if (x >= width) {
        button -> disconnect();
        t -> disconnect();
        button -> deleteLater();
        t -> deleteLater();
    }

    if (y >= bottom) {
        //Снежинка упала - игра проиграна/завершена
        if (!isDefeated) {
            isDefeated = true;
            setLoose(w -> parentWidget());
        }
        //часть снежинки, которая останется видной внизу при падении
        int delta = w -> geometry().bottom() - button -> geometry().top();
        t -> stop(); //тормозим дальнейшее движение вниз


        QTimer *newT = new QTimer(); //новый таймер для удержания снежинки в области касания "дна"
        //удерживаем кнопку внизу окна по новому таймеру
        QObject::connect(newT, &QTimer::timeout, [=]() mutable {
                //убираем кнопки за пределами экрана
                width = w -> geometry().right();
                x = button -> geometry().left();
                if (x >= width) {
                    t -> deleteLater();
                    newT -> deleteLater();
                    button -> deleteLater();
                }

                bottom = w -> geometry().bottom();
                button -> setGeometry(x, bottom - delta , CUBE_SIZE, CUBE_SIZE); //- CUBE_SIZE
                //qDebug() << "bot=" << bottom << "d=" << delta;
        });
        newT -> start(10);

        //Удаление упавшей кнопки вместе с новым (и старым) таймером
        button -> disconnect();
        QObject::connect(button, &QPushButton::pressed, [=](){
            newT -> stop();
            t -> deleteLater();
            button -> deleteLater();
        });
    }
}

void moveButton(QPushButton *button, int &speed, QTimer *t) {
    QPoint buttonPosition = button ->pos();

    //ускоряем падение в 2 раза, если курсор попадает в область кнопки с радиусом RADIUS
    QPoint cursorPosition = button -> parentWidget() -> mapFromGlobal(mouseCursor -> pos());
    QPoint delta = buttonPosition - cursorPosition + QPoint(CUBE_SIZE / 2, CUBE_SIZE / 2);
    double r = sqrt(delta.x() * delta.x() + delta.y() * delta.y());
    if (r <= RADIUS) {
        buttonPosition.setY(buttonPosition.y() + 2* speed);
    } else {
        buttonPosition.setY(buttonPosition.y() + speed);
    }
    button -> move(buttonPosition);
    checkBorders(button, button -> parentWidget(), t);
}

void createButton(QTimer *timer, int x, int y, QWidget *widget){
    QPushButton *button = new QPushButton(widget);
    button -> setText("*");
    button -> setGeometry(x, y, CUBE_SIZE, CUBE_SIZE);
    button -> setStyleSheet("background-color: white");
    int speed = (QRandomGenerator::global()->generate()) % MAX_SPEED + 1;
    qDebug() << "Speed = for" << button << "is" <<speed; // демонтарция разных скоростей для каждой снежинки (по ТЗ)

    //удаление падающей кнопки при нажатии
    QObject::connect(button, &QPushButton::pressed, [button, timer](){
        score++;
        timer -> deleteLater();
        button -> deleteLater();
    });

    //движение кнопки, обеспечиваемое таймером
    QObject::connect(timer, &QTimer::timeout, [=]() mutable {
            moveButton(button, speed, timer);
    } );

    button->show();
    timer -> start(FREQUENCY);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    score = 0;
    isDefeated = false;
    mouseCursor = new QCursor();

    this -> resize(640, 480);
    m_mainLayout = new QHBoxLayout(this);
    setCentralWidget(new QWidget(this));
    centralWidget() -> setLayout(m_mainLayout);

    int winWidth;
    int winHeight;
    int x, y;

    //Таймер для генерации снежинок
    int flakesTime = QRandomGenerator::global()->generate() % (1000 - 100) + 100; //время между появлениями (по ТЗ)
    QTimer *generalTimer = new QTimer();

    QObject::connect(generalTimer, &QTimer::timeout, [=]() mutable {
        //qDebug() << "GenT connect";
        static int j = 0;  //счетчик снежинок
        qDebug() << "Delay for" << ++j << "flake is" << flakesTime; //разное время старта для каждой снежинки (по ТЗ)

        winWidth = this -> geometry().width();
        winHeight = this -> geometry().height();
        x = QRandomGenerator::global()->generate() % (winWidth - CUBE_SIZE);
        y = QRandomGenerator::global()->generate() % 100; // ордината появления (по ТЗ)

        qDebug() << "Coordinates:" << x << "," << y;

        createButton(new QTimer(), x, y, this -> centralWidget());

        //генерируется новое время для следующей снежинки
        flakesTime = QRandomGenerator::global()->generate() % (1000 - 100) + 100;
        generalTimer -> stop();
        generalTimer ->start(flakesTime);

#if TEST
        //запуск 2 снежинок
        static int i = 0;
        i++;
        if (i == FLAKES) {
            generalTimer -> stop();
        }
#endif

    });

    generalTimer->start(flakesTime);
}

MainWindow::~MainWindow()
{
}


