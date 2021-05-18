#include <QtWidgets>
#include <QtOpenGL>
#include <QtMath>

#include "glwidget.h"
#include "QDebug"
#include "time.h"

GLWidget::GLWidget(QWidget *parent, QGLWidget *shareWidget)
    : QGLWidget(parent, shareWidget)
{
    srand(time(0));
    texNumbers = texImages = -1;
    round = 1;
    sourceImageSFactor = sourceImageTFactor = 8;
    restartTheGame = true;
    restartTimer = QTime::currentTime();
    timerInterval = 10;
    player1TotalWin = player2TotalWin = player1EqualPlayer2 = 0;
    initialize();
}

void GLWidget::initialize()
{
    sample = 1;
    val = 0;

    allowToStart = false;


    sFactor = 8;//column's count
    tFactor = 8;//row's count
    halfOfSize = qMax(200, qCeil(sFactor * tFactor * 4.7));

    if((int)sFactor % 2 != 0)
    {
        qDebug()<<"column's count must be even.";
        return;
    }

    createRandomVectorForSlides();

    equalityMap.clear();
    for(int i = 1 ; i <= sFactor * tFactor ; i += 2)
    {
        equalityMap.insert(i, i + 1);
    }


    map.clear();
    QPair<int, int> p;
    for(int i = 1 ; i <= sFactor ; i++)
    {
        for(int j = 1 ; j <= tFactor ; j++)
        {
            p.first = i;
            p.second = j;
            map.insert(p, 0);
        }
    }

    timerId = startTimer(timerInterval);
    initalRotateTime = sFactor * tFactor * 80 / timerInterval;

    player1Score = player2Score = correctGuess = 0;
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    LoadTextures();
}

void GLWidget::paintGL()
{
    glClearColor(0.5, 0.4, 1, 0.2);
    glClear(GL_COLOR_BUFFER_BIT);

    switch (sample)
    {
    case 1:
        drawSlides(texNumbers, texImages, 1 - ((val % 300) / 300.0f));
        break;
    }
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width/2, width/2, -height/2, height/2, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    this->setFocus();
}

int GLWidget::cellColumnOrRowIndex(int eX, float factor)
{
    int cellNumber = 0;

    if(((int)factor) % 2 == 0) /*If number of rows/columns is even.*/
    {
        cellNumber = qCeil(qAbs(eX) / (halfOfSize / (qFloor(factor / 2)))) + 1;
        if(eX >= 0)
        {
            cellNumber = cellNumber + qFloor(factor / 2);
        }
        else
        {
            cellNumber = qFloor(factor / 2) + 1 - cellNumber;
        }
    }
    else if(((int)factor) % 2 == 1) /*If number of rows/columns is odd.*/
    {
        float centerBoundary = halfOfSize / factor;
        if(eX <= centerBoundary && eX >= -centerBoundary)
        {
            cellNumber = qFloor(factor / 2) + 1;
        }
        else if(eX >= 0)
        {
            cellNumber = (qAbs((eX - (int)centerBoundary)) / ((int)centerBoundary * 2)) + 1;
            cellNumber = cellNumber + qFloor(factor / 2) + 1;
        }
        else
        {
            cellNumber =(qAbs((eX - (int)centerBoundary)) / ((int)centerBoundary * 2));
            cellNumber = qFloor(factor / 2) + 1 - cellNumber;
        }
    }

    return cellNumber;
}

int GLWidget::countOfSelectedSlides()
{
    int counter = 0;
    QMapIterator<QPair<int, int>, int> i(map);
    while (i.hasNext())
    {
        i.next();
        if(i.value() == 1)//1: temp show
        {
            if(counter <= 1)
            {
                pairKey[counter].first = i.key().first;
                pairKey[counter].second = i.key().second;
                counter++;
            }
        }
    }

    return counter;
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QPoint pos(e->x() - (width() / 2), (height() / 2) - e->y());

    if(!(qAbs(pos.x()) <= halfOfSize && qAbs(pos.y()) <= halfOfSize))
    {
        qDebug("Out of screen boundary...");
        return;
    }
    if(allowToStart == false)
    {
        qDebug()<<"You'r not allowed to start the game right now...";
        return;
    }

    int rowIndex    = cellColumnOrRowIndex(pos.y(), tFactor);
    int columnIndex = cellColumnOrRowIndex(pos.x(), sFactor);

    int countOfSelectedSlidesBefore = countOfSelectedSlides();

    int d = rowAndColumnToNumber(rowIndex, columnIndex);
    p = randVec[d];

    checkTime = false;

    //0: hide, 1: temp show, 2: always show
    if(map.value(p) == 2  && countOfSelectedSlidesBefore == 2)
    {
        map[p] = 2;
        map[pairKey[0]] = map[pairKey[1]] = 0;
    }
    else if(map.value(p) == 2)
    {
    }
    else if(map.value(p) == 1 && countOfSelectedSlidesBefore == 2)
    {
        map[pairKey[0]] = map[pairKey[1]] = 0;
    }
    else if(map.value(p) == 1 && countOfSelectedSlidesBefore==1)
    {
    }
    else if(map.value(p) == 1)
    {
        map[p] = 0;
    }
    else if(map.value(p) == 0 && countOfSelectedSlidesBefore == 0)
    {
        map[p] = 1;
    }
    else if(map.value(p) == 0 && countOfSelectedSlidesBefore == 1)
    {
        if(pairString(pairKey[0].first, pairKey[0].second) == pairString(p.first, p.second))
        {
            map[pairKey[0]] = map[p] = 2;

            if(isPlayer1sTurn)
            {
                player1Score++;
            }
            else
            {
                player2Score++;
            }

            //When game finishes...
            if(++correctGuess == (int)(sFactor * tFactor) / 2)
            {
                restartTheGame = true;
                qDebug()<<"Game is finished...";
            }
        }
        else
        {
            isPlayer1sTurn = (isPlayer1sTurn == true) ? false : true;
            map[p] = 1;
            checkTime = true;
            curTime = QTime::currentTime();
        }
    }
    else if(map.value(p) == 0 && countOfSelectedSlidesBefore == 2)
    {
        map[pairKey[0]] = map[pairKey[1]] = 0;
        map[p] = 1;
    }
    else
    {
        ((QMainWindow*)this->parent())->setWindowTitle("else");
    }

    QString windowsTitle = "";
    if(restartTheGame == true)
    {
        windowsTitle = "Game is finished... ";
        if(player1Score == player2Score)
        {
            windowsTitle += "Scores are equal.";;
            player1EqualPlayer2++;
        }
        else
        {
            if(player1Score > player2Score)
            {
                windowsTitle += " Player1 ";
                player1TotalWin++;
            }
            else
            {
                windowsTitle += " Player2 ";
                player2TotalWin++;
            }

            windowsTitle += "won the game ";
            windowsTitle += QString::number(qMax(player1Score, player2Score)) + " - " +
                    QString::number(qMin(player1Score, player2Score)) +".";
        }

        restartTimer = QTime::currentTime();
        round++;
    }
    else
    {
        QString player1 = "Player1: ", player2 = "  Player2: ";
        if(isPlayer1sTurn == true)
        {
            player1 = "[[Player1]]: ";
        }
        else
        {
            player2 = "  [[Player2]]: ";
        }
        windowsTitle = player1 + QString::number(player1Score) +
                player2 + QString::number(player2Score) + ".";

    }

    windowsTitle += QString("       Total score: Player1: %1, Player2: %2, Equal times: %3")
            .arg(player1TotalWin).arg(player2TotalWin).arg(player1EqualPlayer2);
    ((QMainWindow*)this->parent())->setWindowTitle(windowsTitle);
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    Q_UNUSED(e);
    updateGL();
}

void GLWidget::LoadTextures()
{
    texNumbers = bindTexture(QImage(":/Img/64-Numbers-Normal.png"));
    glBindTexture(GL_TEXTURE_2D, texNumbers);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);



    texImages = bindTexture(QImage(":/Img/64-Images-Normal.png"));
    glBindTexture(GL_TEXTURE_2D, texImages);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void GLWidget::timerEvent(QTimerEvent *)
{
    int stopTime = (round == 1) ? 100 : 2500;
    val++;
    updateGL();
    if(checkTime == true && curTime.msecsTo(QTime::currentTime()) >= 2200)
    {
        map[pairKey[0]] = map[p] = 0;
        checkTime = false;
    }
    if(restartTheGame == true && restartTimer.msecsTo(QTime::currentTime()) >= stopTime)
    {
        killTimer(timerId);
        initialize();

        if(restartTimer.msecsTo(QTime::currentTime()) >= stopTime + 2500)
        {
            restartTheGame = false;
            isPlayer1sTurn = (round % 2 == 1) ? true : false;
            QString vs = (isPlayer1sTurn == true) ? "[[Player1]] VS Player2":"Player1 VS [[Player2]]";
            QString windowsTitle = QString("%1.  Total score: Player1: %2, Player2: %3, Equal times: %4")
                        .arg(vs).arg(player1TotalWin).arg(player2TotalWin).arg(player1EqualPlayer2);
            ((QMainWindow*)this->parent())->setWindowTitle(windowsTitle);
        }
        else
        {
            ((QMainWindow*)this->parent())->setWindowTitle(QString("Round %1 is starting...").arg(round));
        }
    }
}

void GLWidget::drawSlides(GLuint texNumbers, GLuint texImages, float s)
{
    s = 1;
    Q_UNUSED(texNumbers)
    float xLeft, yDown, xRight, yTop = 0;

    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, texImages);
    glColor4f(1, 1, 1, s);


    QPair <int, int> p;

    for(int c = 1; c < randVec.length() ; c++)
    {
        QPair<int, int> pop = numberToRowAndColumn(c);

        xLeft  = -halfOfSize + (pop.second - 1) * (2 * halfOfSize) / (sFactor);
        xRight = -halfOfSize + (pop.second - 0) * (2 * halfOfSize) / (sFactor);

        yDown = -halfOfSize + (pop.first - 1) * (2 * halfOfSize) / (tFactor);
        yTop  = -halfOfSize + (pop.first - 0) * (2 * halfOfSize) / (tFactor);

        glLoadIdentity();



        //Center of current slide
        float xCenter = (xLeft + xRight) / 2;
        float yCenter = (yDown + yTop) / 2;



        //Rotate current slide.
        int rowIndex, columnIndex=0;
        if(val < initalRotateTime)
        {
            allowToStart = false;
            glBindTexture(GL_TEXTURE_2D, texImages);
            glTranslatef(xCenter, yCenter, 0);
            glRotatef(val, 0, 0, 1);
            glTranslatef(-xCenter, -yCenter, 0);

            p = randVec[c];
            rowIndex = p.first;
            columnIndex = p.second;
            drawSingleSlide(xLeft, xRight, yTop, yDown, rowIndex, columnIndex);
        }
        else
        {
            allowToStart = true;

            //Draw current slide.
            p = randVec[c];

            if (map.value(p) >= 1)
            {
                glBindTexture(GL_TEXTURE_2D, texImages);
                rowIndex = p.first;
                columnIndex = p.second;
                drawSingleSlide(xLeft, xRight, yTop, yDown, rowIndex, columnIndex);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, texNumbers);
                p=numberToRowAndColumn(c);
                rowIndex = p.first;
                columnIndex = p.second;
                drawSingleSlide(xLeft, xRight, yTop, yDown, rowIndex, columnIndex);
            }
        }



        //Draw center of current slide.
        {
            //            glColor3f(1, 0, 0);
            //            glDisable(GL_TEXTURE_2D);
            //            glPointSize(3);
            //            glBegin(GL_POINTS);
            //            glVertex2f(xCenter, yCenter);
            //            glEnd();
            //            glEnable(GL_TEXTURE_2D);
            //            glColor4f(1, 1, 1, s);
        }
    }


    //Draw grids
    {
        glColor4f(1, 1, 0, 1);

        glLoadIdentity();
        /*if(val<500)*/return;
        //Horizontal grid
        for(int j = 0 ; j <= tFactor ; j++)
        {
            glBegin(GL_LINES);
            glDisable(GL_BLEND);
            glVertex2f(-halfOfSize, -halfOfSize + j * (2 * halfOfSize) / (tFactor));
            glVertex2f(halfOfSize, -halfOfSize + j * (2 * halfOfSize) / (tFactor));
            glEnable(GL_BLEND);
            glEnd();
        }

        //Vertical grid
        for(int i = 0 ; i <= sFactor ; i++)
        {
            glBegin(GL_LINES);
            glDisable(GL_BLEND);
            glVertex2f(-halfOfSize + i * (2 * halfOfSize) / (sFactor), -halfOfSize);
            glVertex2f(-halfOfSize + i * (2 * halfOfSize) / (sFactor), halfOfSize);
            glEnable(GL_BLEND);
            glEnd();
        }
        glColor4f(1, 1, 1, s);
    }
}

void GLWidget::drawSingleSlide(int left, int right, int top, int down, int row, int cell)
{
    glBegin(GL_QUADS);
    glTexCoord2f((cell - 1) / sourceImageSFactor, (row - 1) / sourceImageTFactor);
    glVertex2f(left, down);

    glTexCoord2f((cell - 1) / sourceImageSFactor, (row - 0) / sourceImageTFactor);
    glVertex2f(left, top);

    glTexCoord2f((cell - 0) / sourceImageSFactor, (row - 0) / sourceImageTFactor);
    glVertex2f(right, top);

    glTexCoord2f((cell - 0) / sourceImageSFactor, (row - 1) / sourceImageTFactor);
    glVertex2f(right, down);
    glEnd();
}

QString GLWidget::pairString(int i, int j)
{
    int cellNumber = rowAndColumnToNumber(i, j);
    int pairNumber = (equalityMap.value(cellNumber) > 0) ? equalityMap.value(cellNumber) : equalityMap.key(cellNumber);
    QString first=QString::number(qMin(pairNumber, cellNumber));
    first = (first.length() == 1) ? "0" + first : first;
    QString second=QString::number(qMax(pairNumber, cellNumber));
    second = (second.length() == 1) ? "0" + second : second;
    QString pair = first + "-" + second;
    return pair;
}

QPair<int, int> GLWidget::numberToRowAndColumn(int number)
{
    QPair<int, int> rowAndColumn;
    rowAndColumn.first = qCeil(number / sFactor);
    int column = number % (int)sFactor;
    rowAndColumn.second = (column == 0) ?sFactor : column;
    return rowAndColumn;
}

int GLWidget::rowAndColumnToNumber(int row, int col)
{
    return (((row - 1) * sFactor) + col);
}

QVector<int> GLWidget::fillRandomInVector(int factor)
{
    QVector<int> vec;
    vec.clear();
    vec.append(-1);

    int randI = (qrand()%((int)(factor))) + 1;
    for(int i = 0 ; i < factor ; i++)
    {
        while(vec.contains(randI))
        {
            randI = (qrand()%((int)(factor))) + 1;
        }

        vec.append(randI);
    }

    return vec;
}


void GLWidget::createRandomVectorForSlides()
{
    randVec.clear();
    QPair<int, int> pair;

    pair.first=pair.second=-1;
    randVec.append(pair);

    for(int i = 1 ; i <= sFactor ; i++)
    {
        for(int j = 1 ; j <= tFactor ; j++)
        {
            pair.first=j;
            pair.second=i;
            randVec.append(pair);
        }
    }

    int rand = 1;
    for(int k=1; k<randVec.length(); k++)
    {
        rand = (qrand()%((int)(randVec.length()-1))) + 1;//A random number in [1, sFactor * tFactor].
        pair = randVec.at(k);
        randVec[k] = randVec.at(rand);
        randVec[rand] = pair;
    }
}

void GLWidget::createStaticRandomVectorForSlides()
{
    randVec.clear();
    QPair <int, int> pp;

    pp.first  = -1;
    pp.second = -1;
    randVec.append(pp);



    //1
    pp.first  = 2;
    pp.second = 3;
    randVec.append(pp);

    pp.first  = 2;
    pp.second = 2;
    randVec.append(pp);

    pp.first  = 2;
    pp.second = 1;
    randVec.append(pp);

    pp.first  = 2;
    pp.second = 4;
    randVec.append(pp);


    pp.first  = 1;
    pp.second = 5;
    randVec.append(pp);




    //2
    pp.first  = 3;
    pp.second = 3;
    randVec.append(pp);

    pp.first  = 3;
    pp.second = 2;
    randVec.append(pp);

    pp.first  = 3;
    pp.second = 1;
    randVec.append(pp);

    pp.first  = 3;
    pp.second = 4;
    randVec.append(pp);

    pp.first  = 2;
    pp.second = 5;
    randVec.append(pp);



    //3
    pp.first  = 4;
    pp.second = 3;
    randVec.append(pp);

    pp.first  = 4;
    pp.second = 2;
    randVec.append(pp);

    pp.first  = 4;
    pp.second = 1;
    randVec.append(pp);

    pp.first  = 4;
    pp.second = 4;
    randVec.append(pp);


    pp.first  = 3;
    pp.second = 5;
    randVec.append(pp);



    //4
    pp.first  = 1;
    pp.second = 3;
    randVec.append(pp);

    pp.first  = 1;
    pp.second = 2;
    randVec.append(pp);

    pp.first  = 1;
    pp.second = 1;
    randVec.append(pp);

    pp.first  = 1;
    pp.second = 4;
    randVec.append(pp);

    pp.first  = 4;
    pp.second = 5;
    randVec.append(pp);
}
