/*
  Q Light Controller Plus
  app.h

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

#ifndef APP_H
#define APP_H

#include <QQmlEngine>
#include <QQuickView>
#include <QObject>
#include "doc.h"

class MainView2D;
class QDomElement;
class QDomDocument;
class FixtureBrowser;
class FixtureManager;
class ContextManager;
class FunctionManager;
class InputOutputManager;
class VirtualConsole;

#define KXMLQLCWorkspace "Workspace"

class App : public QQuickView
{
    Q_OBJECT
    Q_DISABLE_COPY(App)
    Q_PROPERTY(bool docLoaded READ docLoaded NOTIFY docLoadedChanged)
    Q_PROPERTY(QStringList recentFiles READ recentFiles NOTIFY recentFilesChanged)

public:
    App();
    ~App();

    void startup();

    void show();

protected:
    void keyPressEvent(QKeyEvent * e);

private:
    QQmlEngine m_engine;
    FixtureBrowser *m_fixtureBrowser;
    FixtureManager *m_fixtureManager;
    ContextManager *m_contextManager;
    FunctionManager *m_functionManager;
    InputOutputManager *m_ioManager;
    VirtualConsole *m_virtualConsole;

    /*********************************************************************
     * Doc
     *********************************************************************/
public:
    void clearDocument();

    Doc *doc();

    bool docLoaded() { return m_docLoaded; }

private slots:
    void slotDocModified(bool state);

private:
    void initDoc();

signals:
    void docLoadedChanged();

private:
    Doc* m_doc;
    bool m_docLoaded;

    /*********************************************************************
     * Main operating mode
     *********************************************************************/
public:
    void enableKioskMode();
    void createKioskCloseButton(const QRect& rect);

public slots:
    void slotModeOperate();
    void slotModeDesign();
    void slotModeToggle();
    void slotModeChanged(Doc::Mode mode);

    /*********************************************************************
     * Load & Save
     *********************************************************************/
public:
    /** Set the name of the current workspace file */
    void setFileName(const QString& fileName);

    /** Get the name of the current workspace file */
    QString fileName() const;

    /** Load the workspace with the given $fileName */
    Q_INVOKABLE bool loadWorkspace(const QString& fileName);

    /** Reset everything and start a new workspace */
    Q_INVOKABLE bool newWorkspace();

    /** Return the list of the recently opened files */
    QStringList recentFiles() const;

    /**
     * Load workspace contents from a XML file with the given name.
     *
     * @param fileName The name of the file to load from.
     * @return QFile::NoError if successful.
     */
    QFile::FileError loadXML(const QString& fileName);

    /**
     * Load workspace contents from the given XML document.
     *
     * @param doc The XML document to load from.
     */
    bool loadXML(const QDomDocument& doc, bool goToConsole = false, bool fromMemory = false);

private:
    /**
     * Update the list of the recently open files.
     * If filename is specified, it will be removed from the list
     * if present and added to the beginning of the list
     */
    void updateRecentFilesList(QString filename = QString());

signals:
    void recentFilesChanged();

private:
    QString m_fileName;
    QStringList m_recentFiles;
};
#endif // APP_H
