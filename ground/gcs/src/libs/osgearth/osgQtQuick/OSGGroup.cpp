/**
 ******************************************************************************
 *
 * @file       OSGGroup.cpp
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

#include "OSGGroup.hpp"

#include <osg/Group>

#include <QQmlListProperty>
#include <QDebug>

namespace osgQtQuick {
enum DirtyFlag { Children = 1 << 0 };

struct OSGGroup::Hidden : public QObject {
    Q_OBJECT

private:
    OSGGroup * self;

    osg::ref_ptr<osg::Group>     group;

    QMap<OSGNode *, osg::Node *> cache;

public:
    Hidden(OSGGroup *node) : QObject(node), self(node), group(new osg::Group)
    {
        group = new osg::Group();
        self->setNode(group);
    }

    QList<OSGNode *> children;

    void appendChild(OSGNode *childNode)
    {
        cache[childNode] = childNode->node();
        children.append(childNode);
        connect(childNode, SIGNAL(nodeChanged(osg::Node *)), this, SLOT(onChildNodeChanged(osg::Node *)), Qt::UniqueConnection);
        self->setDirty(Children);
    }

    int countChild() const
    {
        return children.size();
    }

    OSGNode *atChild(int index) const
    {
        if (index >= 0 && index < children.size()) {
            return children[index];
        }
        return 0;
    }

    void clearChild()
    {
        while (!children.isEmpty()) {
            OSGNode *node = children.takeLast();
            disconnect(node);
        }
        children.clear();
        cache.clear();
        self->setDirty(Children);
    }

    void updateGroupNode()
    {
        bool updated = false;
        unsigned int index = 0;

        QListIterator<OSGNode *> i(children);
        while (i.hasNext()) {
            OSGNode *childNode = i.next();
            if (index < group->getNumChildren()) {
                updated |= group->replaceChild(group->getChild(index), childNode->node());
            } else {
                updated |= group->addChild(childNode->node());
            }
            index++;
        }
        // removing eventual left overs
        if (index < group->getNumChildren()) {
            updated |= group->removeChild(index, group->getNumChildren() - index);
        }
        // if (updated) {
        self->emitNodeChanged();
        // }
    }

    /* QQmlListProperty<OSGNode> */

    static void append_child(QQmlListProperty<OSGNode> *list, OSGNode *childNode)
    {
        Hidden *h = qobject_cast<Hidden *>(list->object);

        h->appendChild(childNode);
    }

    static int count_child(QQmlListProperty<OSGNode> *list)
    {
        Hidden *h = qobject_cast<Hidden *>(list->object);

        return h->countChild();
    }

    static OSGNode *at_child(QQmlListProperty<OSGNode> *list, int index)
    {
        Hidden *h = qobject_cast<Hidden *>(list->object);

        return h->atChild(index);
    }

    static void clear_child(QQmlListProperty<OSGNode> *list)
    {
        Hidden *h = qobject_cast<Hidden *>(list->object);

        h->clearChild();
    }

private slots:
    void onChildNodeChanged(osg::Node *node)
    {
        qDebug() << "OSGGroup::onChildNodeChanged" << node;
        OSGNode *obj = qobject_cast<OSGNode *>(sender());
        if (obj) {
            osg::Node *cacheNode = cache.value(obj, NULL);
            if (cacheNode) {
                group->replaceChild(cacheNode, node);
            } else {
                // should not happen...
            }
            cache[obj] = node;
            // emit self->nodeChanged(group.get());
        }
    }
};

/* class OSGGGroupNode */

OSGGroup::OSGGroup(QObject *parent) : OSGNode(parent), h(new Hidden(this))
{
    qDebug() << "OSGGroup::OSGGroup";
}

OSGGroup::~OSGGroup()
{
    // qDebug() << "OSGGroup::~OSGGroup";
    delete h;
}

QQmlListProperty<OSGNode> OSGGroup::children()
{
    return QQmlListProperty<OSGNode>(h, 0,
                                     &Hidden::append_child,
                                     &Hidden::count_child,
                                     &Hidden::at_child,
                                     &Hidden::clear_child);
}

void OSGGroup::update()
{
    if (isDirty(Children)) {
        h->updateGroupNode();
    }
}

void OSGGroup::attach(osgViewer::View *view)
{
    // qDebug() << "OSGGroup::attach " << view;
    QListIterator<OSGNode *> i(h->children);
    while (i.hasNext()) {
        OSGNode *node = i.next();
        // qDebug() << "OSGGroup::attach - child" << node;
        OSGNode::attach(node, view);
    }
    update();
    clearDirty();
}

void OSGGroup::detach(osgViewer::View *view)
{
    // qDebug() << "OSGGroup::detach " << view;
    QListIterator<OSGNode *> i(h->children);
    while (i.hasNext()) {
        OSGNode *node = i.next();
        // qDebug() << "OSGGroup::detach - child" << node;
        OSGNode::detach(node, view);
    }
}
} // namespace osgQtQuick

#include "OSGGroup.moc"
