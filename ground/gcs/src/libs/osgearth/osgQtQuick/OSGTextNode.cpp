/**
 ******************************************************************************
 *
 * @file       OSGTextNode.cpp
 * @author     The LibrePilot Project, http://www.librepilot.org Copyright (C) 2015.
 * @addtogroup
 * @{
 * @addtogroup
 * @{
 * @brief
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "OSGTextNode.hpp"

#include "../utility.h"

#include <osgText/Text>
#include <osg/Geode>
#include <osg/Group>

#include <QFont>
#include <QColor>

namespace osgQtQuick {
struct OSGTextNode::Hidden : public QObject {
    Q_OBJECT

private:
    OSGTextNode * const self;

    osg::ref_ptr<osgText::Text> text;

public:
    QString textString;
    QColor  color;

    Hidden(OSGTextNode *node) : QObject(node), self(node)
    {
        osg::ref_ptr<osgText::Font> textFont = createFont(QFont("Times"));

        text = createText(osg::Vec3(-100, 20, 0), "Hello World", 20.0f,
                          textFont.get());
        osg::ref_ptr<osg::Geode> textGeode = new osg::Geode();
        textGeode->addDrawable(text.get());
    #if 0
        text->setAutoRotateToScreen(true);
        self->setNode(textGeode.get());
    #else
        osg::Camera *camera = createHUDCamera(-100, 100, -100, 100);
        camera->addChild(textGeode.get());
        camera->getOrCreateStateSet()->setMode(
            GL_LIGHTING, osg::StateAttribute::OFF);
        self->setNode(camera);
    #endif
    }

    void updateText()
    {
        text->setText(textString.toStdString(), osgText::String::ENCODING_UTF8);
    }

    void updateColor()
    {
        osg::Vec4 osgColor(
            color.redF(),
            color.greenF(),
            color.blueF(),
            color.alphaF());

        text->setColor(osgColor);
    }
};

/* class OSGTextNode */

enum DirtyFlag { Text = 1 << 0, Color = 1 << 1 };

OSGTextNode::OSGTextNode(QObject *node) : OSGNode(node), h(new Hidden(this))
{}

OSGTextNode::~OSGTextNode()
{
    delete h;
}

QString OSGTextNode::text() const
{
    return h->textString;
}

void OSGTextNode::setText(const QString &text)
{
    if (h->textString != text) {
        h->textString != text;
        setDirty(Text);
        emit textChanged(text);
    }
}

QColor OSGTextNode::color() const
{
    return h->color;
}

void OSGTextNode::setColor(const QColor &color)
{
    if (h->color != color) {
        h->color != color;
        setDirty(Color);
        emit colorChanged(color);
    }
}

void OSGTextNode::update()
{
    if (isDirty(Text)) {
        h->updateText();
    }
    if (isDirty(Color)) {
        h->updateColor();
    }
}

void OSGTextNode::attach(osgViewer::View *view)
{
    update();
    clearDirty();
}

void OSGTextNode::detach(osgViewer::View *view)
{}
} // namespace osgQtQuick

#include "OSGTextNode.moc"
