/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtWidgets>
#include <QGLWidget>  //QT  +=  opengl
#include <QOpenGLFunctions>  //QT  +=  gui
#include "QMatrix4x4"

class GLWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = 0, QGLWidget *shareWidget = 0);
    ~GLWidget();
    int angle;

    struct MapKeyParams
    {
        int row;
        int col;
    }mapKey;

    QMap< QPair<int, int>, int > map;
    QPair<int, int> pairKey[2];


protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent*);
    void timerEvent(QTimerEvent*);

private:
    int countOfSelectedSlides();
    void LoadTextures();
    void drawSlides(GLuint texNumbers, GLuint texImages, float s);
    void drawSingleSlide(int left, int right, int top, int down, int row, int cell);
    QString pairString(int i, int j);
    void initialize();

    QPair<int, int> numberToRowAndColumn(int number);
    int rowAndColumnToNumber(int row, int col);
    QMap<int, int> equalityMap;
    QVector< QPoint > eq;

    GLuint texNumbers, texImages;
    int val;
    int sample;
    int halfOfSize;
    float sFactor;
    float tFactor;
    float sourceImageSFactor, sourceImageTFactor;
    QTime restartTimer, curTime;
    bool checkTime;

    int cellColumnOrRowIndex(int eX, float factor);
    QPair<int, int> p;
    int player1Score, player2Score;
    bool isPlayer1sTurn;
    int correctGuess;
    int timerId;
    int round;
    bool allowToStart;
    QVector<int> vecI, vecJ;
    QVector<int> fillRandomInVector(int factor);
    void createRandomVectorForSlides();
    void createStaticRandomVectorForSlides();
    QVector < QPair<int, int> > randVec;
    bool restartTheGame;
    int timerInterval;
    int initalRotateTime;
    int player1TotalWin, player2TotalWin, player1EqualPlayer2;
};

#endif
