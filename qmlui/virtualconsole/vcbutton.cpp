/*
  Q Light Controller Plus
  vcbutton.cpp

  Copyright (c) Massimo Callegari

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include <QtXml>

#include "qlcmacros.h"
#include "vcbutton.h"
#include "doc.h"

VCButton::VCButton(Doc *doc, QObject *parent)
    : VCWidget(doc, parent)
    , m_function(Function::invalidId())
    , m_isOn(false)
    , m_actionType(Toggle)
{
    setType(VCWidget::ButtonWidget);
    setBackgroundColor(QColor("#444"));
}

VCButton::~VCButton()
{
}

void VCButton::setID(quint32 id)
{
    VCWidget::setID(id);

    if (caption().isEmpty())
        setCaption(tr("Button %1").arg(id));
}

void VCButton::render(QQuickView *view, QQuickItem *parent)
{
    if (view == NULL || parent == NULL)
        return;

    QQmlComponent *component = new QQmlComponent(view->engine(), QUrl("qrc:/VCButtonItem.qml"));

    if (component->isError())
    {
        qDebug() << component->errors();
        return;
    }

    QQuickItem *item = qobject_cast<QQuickItem*>(component->create());

    item->setParentItem(parent);
    item->setProperty("buttonObj", QVariant::fromValue(this));
}

/*********************************************************************
 * Function attachment
 *********************************************************************/

void VCButton::setFunction(quint32 fid)
{
    bool running = false;

    Function* current = m_doc->function(m_function);
    if (current != NULL)
    {
        /* Get rid of old function connections */
        disconnect(current, SIGNAL(running(quint32)),
                this, SLOT(slotFunctionRunning(quint32)));
        disconnect(current, SIGNAL(stopped(quint32)),
                this, SLOT(slotFunctionStopped(quint32)));
        disconnect(current, SIGNAL(flashing(quint32,bool)),
                this, SLOT(slotFunctionFlashing(quint32,bool)));

        if(current->isRunning())
        {
            running = true;
            current->stop();
        }
    }

    Function* function = m_doc->function(fid);
    if (function != NULL)
    {
        /* Connect to the new function */
        connect(function, SIGNAL(running(quint32)),
                this, SLOT(slotFunctionRunning(quint32)));
        connect(function, SIGNAL(stopped(quint32)),
                this, SLOT(slotFunctionStopped(quint32)));
        connect(function, SIGNAL(flashing(quint32,bool)),
                this, SLOT(slotFunctionFlashing(quint32,bool)));

        m_function = fid;
        if(running)
            function->start(m_doc->masterTimer());
    }
    else
    {
        /* No function attachment */
        m_function = Function::invalidId();
    }
    setDocModified();
}

quint32 VCButton::function() const
{
    return m_function;
}

void VCButton::requestStateChange(bool pressed)
{
    switch(actionType())
    {
        case Toggle:
        {
            Function *f = m_doc->function(m_function);
            if (f == NULL)
                return;

            if (m_isOn == false && pressed == true)
            {
                static const QMetaMethod funcSignal = QMetaMethod::fromSignal(&VCButton::functionStarting);
                if (isSignalConnected(funcSignal))
                    emit functionStarting(this, m_function);
                else
                    notifyFunctionStarting(this, m_function, 1.0);
            }
            else if (m_isOn == true && pressed == false)
            {
                if (f->isRunning())
                    f->stop();
            }
        }
        break;
        case Flash:
        {
            Function *f = m_doc->function(m_function);
            if (f != NULL)
            {
                if (m_isOn == false && pressed == true)
                {
                    f->flash(m_doc->masterTimer());
                    setOn(true);
                }
                else if (m_isOn == true && pressed == false)
                {
                    f->unFlash(m_doc->masterTimer());
                    setOn(false);
                }
            }
        }
        break;
        default:
        break;
    }
}

void VCButton::notifyFunctionStarting(VCWidget *widget, quint32 fid, qreal fIntensity)
{
    Q_UNUSED(widget)
    Q_UNUSED(fIntensity)

    if (m_function == Function::invalidId() || actionType() != VCButton::Toggle)
        return;

    Function *f = m_doc->function(m_function);
    if (f == NULL)
        return;

    if (m_function != fid)
    {
        if (f->isRunning())
            f->stop();
    }
    else
    {
        if (isStartupIntensityEnabled() == true)
            f->adjustAttribute(startupIntensity() * intensity(), Function::Intensity);
        else
            f->adjustAttribute(intensity(), Function::Intensity);
        f->start(m_doc->masterTimer());
    }
}

void VCButton::slotFunctionRunning(quint32 fid)
{
    if (fid == m_function && actionType() == Toggle)
        setOn(true);
}

void VCButton::slotFunctionStopped(quint32 fid)
{
    if (fid == m_function && actionType() == Toggle)
    {
        setOn(false);
        //blink(250);
    }
}

void VCButton::slotFunctionFlashing(quint32 fid, bool state)
{
    // Do not change the state of the button for Blackout or Stop All Functions buttons
    if (actionType() != Toggle && actionType() != Flash)
        return;

    if (fid != m_function)
        return;

    // if the function was flashed by another button, and the function is still running, keep the button pushed
    Function* f = m_doc->function(m_function);
    if (state == false && actionType() == Toggle && f != NULL && f->isRunning())
    {
        return;
    }

    setOn(state);
}

/*********************************************************************
 * Button state
 *********************************************************************/

bool VCButton::isOn() const
{
    return m_isOn;
}

void VCButton::setOn(bool isOn)
{
    if (m_isOn == isOn)
        return;

    if (m_function == Function::invalidId())
        return;

    m_isOn = isOn;
    emit isOnChanged(isOn);
}

/*********************************************************************
 * Button action
 *********************************************************************/

VCButton::Action VCButton::actionType() const
{
    return m_actionType;
}

void VCButton::setActionType(VCButton::Action actionType)
{
    if (m_actionType == actionType)
        return;

    m_actionType = actionType;
    emit actionTypeChanged(actionType);
}

QString VCButton::actionToString(VCButton::Action action)
{
    if (action == Flash)
        return QString(KXMLQLCVCButtonActionFlash);
    else if (action == Blackout)
        return QString(KXMLQLCVCButtonActionBlackout);
    else if (action == StopAll)
        return QString(KXMLQLCVCButtonActionStopAll);
    else
        return QString(KXMLQLCVCButtonActionToggle);
}

VCButton::Action VCButton::stringToAction(const QString& str)
{
    if (str == KXMLQLCVCButtonActionFlash)
        return Flash;
    else if (str == KXMLQLCVCButtonActionBlackout)
        return Blackout;
    else if (str == KXMLQLCVCButtonActionStopAll)
        return StopAll;
    else
        return Toggle;
}

/*****************************************************************************
 * Intensity adjustment
 *****************************************************************************/

void VCButton::enableStartupIntensity(bool enable)
{
    m_startupIntensityEnabled = enable;
}

bool VCButton::isStartupIntensityEnabled() const
{
    return m_startupIntensityEnabled;
}

void VCButton::setStartupIntensity(qreal fraction)
{
    m_startupIntensity = CLAMP(fraction, qreal(0), qreal(1));
}

qreal VCButton::startupIntensity() const
{
    return m_startupIntensity;
}

/*********************************************************************
 * Load & Save
 *********************************************************************/

bool VCButton::loadXML(const QDomElement* root)
{
    Q_ASSERT(root != NULL);

    if (root->tagName() != KXMLQLCVCButton)
    {
        qWarning() << Q_FUNC_INFO << "Button node not found";
        return false;
    }

    /* Widget commons */
    loadXMLCommon(root);

    QString str;
    QDomNode node = root->firstChild();

    while (node.isNull() == false)
    {
        QDomElement tag = node.toElement();
        if (tag.tagName() == KXMLQLCWindowState)
        {
            bool visible = false;
            int x = 0, y = 0, w = 0, h = 0;
            loadXMLWindowState(&tag, &x, &y, &w, &h, &visible);
            setGeometry(QRect(x, y, w, h));
        }
        else if (tag.tagName() == KXMLQLCVCWidgetAppearance)
        {
            loadXMLAppearance(&tag);
        }
        else if (tag.tagName() == KXMLQLCVCButtonFunction)
        {
            str = tag.attribute(KXMLQLCVCButtonFunctionID);
            setFunction(str.toUInt());
        }
        else if (tag.tagName() == KXMLQLCVCButtonAction)
        {
            setActionType(stringToAction(tag.text()));
            /*
            if (tag.hasAttribute(KXMLQLCVCButtonStopAllFadeTime))
                setStopAllFadeOutTime(tag.attribute(KXMLQLCVCButtonStopAllFadeTime).toInt());
            */
        }
        else if (tag.tagName() == KXMLQLCVCButtonIntensity)
        {
            bool adjust;
            if (tag.attribute(KXMLQLCVCButtonIntensityAdjust) == KXMLQLCTrue)
                adjust = true;
            else
                adjust = false;
            setStartupIntensity(qreal(tag.text().toInt()) / qreal(100));
            enableStartupIntensity(adjust);
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "Unknown button tag:" << tag.tagName();
        }

        node = node.nextSibling();
    }

    /* All buttons start raised... */
    //setOn(false);

    return true;
}
