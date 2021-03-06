/*
  Q Light Controller Plus
  spiconfiguration.cpp

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

#include <QSettings>
#include <QString>

#include "spiconfiguration.h"
#include "spiplugin.h"

/*****************************************************************************
 * Initialization
 *****************************************************************************/

SPIConfiguration::SPIConfiguration(SPIPlugin* plugin, QWidget* parent)
        : QDialog(parent)
{
    Q_ASSERT(plugin != NULL);
    m_plugin = plugin;

    /* Setup UI controls */
    setupUi(this);
}

SPIConfiguration::~SPIConfiguration()
{
}

/*****************************************************************************
 * Dialog actions
 *****************************************************************************/

void SPIConfiguration::accept()
{
    QDialog::accept();
}

quint32 SPIConfiguration::frequency()
{
    int fIdx = m_freqCombo->currentIndex();
    switch(fIdx)
    {
        default:
        case 0: return 1000000; break;
        case 1: return 2000000; break;
        case 2: return 4000000; break;
        case 3: return 8000000; break;
    }
}

int SPIConfiguration::exec()
{
    return QDialog::exec();
}

