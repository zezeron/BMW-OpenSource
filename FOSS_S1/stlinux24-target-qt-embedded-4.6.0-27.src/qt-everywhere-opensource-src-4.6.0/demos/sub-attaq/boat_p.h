/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BOAT_P_H
#define BOAT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

//Own
#include "bomb.h"
#include "graphicsscene.h"

// Qt
#include <QtGui/QKeyEventTransition>

static const int MAX_BOMB = 5;


//These transtion test if we have to stop the boat (i.e current speed is 1)
class KeyStopTransition : public QKeyEventTransition
{
public:
    KeyStopTransition(Boat *b, QEvent::Type t, int k)
    : QKeyEventTransition(b, t, k), boat(b), key(k)
    {
    }
protected:
    virtual bool eventTest(QEvent *event)
    {
        if (!QKeyEventTransition::eventTest(event))
            return false;
        return (boat->currentSpeed() == 1);
    }
private:
    Boat * boat;
    int key;
};

//These transtion test if we have to move the boat (i.e current speed was 0 or another value)
 class KeyMoveTransition : public QKeyEventTransition
{
public:
    KeyMoveTransition(Boat *b, QEvent::Type t, int k)
    : QKeyEventTransition(b, t, k), boat(b), key(k)
    {
    }
protected:
    virtual bool eventTest(QEvent *event)
    {
        if (!QKeyEventTransition::eventTest(event))
            return false;
        return (boat->currentSpeed() >= 0);
    }
    void onTransition(QEvent *)
    {
        //We decrease the speed if needed
        if (key == Qt::Key_Left && boat->currentDirection() == Boat::Right)
            boat->setCurrentSpeed(boat->currentSpeed() - 1);
        else if (key == Qt::Key_Right && boat->currentDirection() == Boat::Left)
            boat->setCurrentSpeed(boat->currentSpeed() - 1);
        else if (boat->currentSpeed() < 3)
            boat->setCurrentSpeed(boat->currentSpeed() + 1);
        boat->updateBoatMovement();
    }
private:
    Boat * boat;
    int key;
};

//This transition trigger the bombs launch
 class KeyLaunchTransition : public QKeyEventTransition
{
public:
    KeyLaunchTransition(Boat *boat, QEvent::Type type, int key)
    : QKeyEventTransition(boat, type, key), boat(boat), key(key)
    {
    }
protected:
    virtual bool eventTest(QEvent *event)
    {
        if (!QKeyEventTransition::eventTest(event))
            return false;
        //We have enough bomb?
        return (boat->bombsLaunched() < MAX_BOMB);
    }
private:
    Boat * boat;
    int key;
};

//This state is describing when the boat is moving right
class MoveStateRight : public QState
{
public:
    MoveStateRight(Boat *boat,QState *parent = 0) : QState(parent), boat(boat)
    {
    }
protected:
    void onEntry(QEvent *)
    {
        boat->setCurrentDirection(Boat::Right);
        boat->updateBoatMovement();
    }
private:
    Boat * boat;
};

 //This state is describing when the boat is moving left
class MoveStateLeft : public QState
{
public:
    MoveStateLeft(Boat *boat,QState *parent = 0) : QState(parent), boat(boat)
    {
    }
protected:
    void onEntry(QEvent *)
    {
        boat->setCurrentDirection(Boat::Left);
        boat->updateBoatMovement();
    }
private:
    Boat * boat;
};

//This state is describing when the boat is in a stand by position
class StopState : public QState
{
public:
    StopState(Boat *boat,QState *parent = 0) : QState(parent), boat(boat)
    {
    }
protected:
    void onEntry(QEvent *)
    {
        boat->setCurrentSpeed(0);
        boat->setCurrentDirection(Boat::None);
        boat->updateBoatMovement();
    }
private:
    Boat * boat;
};

//This state is describing the launch of the torpedo on the right
class LaunchStateRight : public QState
{
public:
    LaunchStateRight(Boat *boat,QState *parent = 0) : QState(parent), boat(boat)
    {
    }
protected:
    void onEntry(QEvent *)
    {
        Bomb *b = new Bomb();
        b->setPos(boat->x()+boat->size().width(),boat->y());
        GraphicsScene *scene = static_cast<GraphicsScene *>(boat->scene());
        scene->addItem(b);
        b->launch(Bomb::Right);
        boat->setBombsLaunched(boat->bombsLaunched() + 1);
    }
private:
    Boat * boat;
};

//This state is describing the launch of the torpedo on the left
class LaunchStateLeft : public QState
{
public:
    LaunchStateLeft(Boat *boat,QState *parent = 0) : QState(parent), boat(boat)
    {
    }
protected:
    void onEntry(QEvent *)
    {
        Bomb *b = new Bomb();
        b->setPos(boat->x() - b->size().width(), boat->y());
        GraphicsScene *scene = static_cast<GraphicsScene *>(boat->scene());
        scene->addItem(b);
        b->launch(Bomb::Left);
        boat->setBombsLaunched(boat->bombsLaunched() + 1);
    }
private:
    Boat * boat;
};

#endif // BOAT_P_H
