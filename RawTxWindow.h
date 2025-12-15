/*

  Copyright (c) 2015, 2016 Hubert Denkmair <hubert@denkmair.de>

  This file is part of cangaroo.

  cangaroo is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  cangaroo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with cangaroo.  If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#include <core/Backend.h>
#include <core/ConfigurableWidget.h>
#include <core/MeasurementSetup.h>
#include <queue>

//===============================================
#include "SendingWidget.h"
# include "QTableWidget"
//===============================================


namespace Ui {
class RawTxWindow;
}

class QDomDocument;
class QDomElement;

struct TxMsgParams {
    uint32_t time;
    uint32_t id;
    uint8_t length = 0;
    uint8_t* data = 0;
};
//===========================================================
struct Message
{
    uint32_t address ;
    uint8_t data[8];
    QString ID;
    QTimer * repeat_rate;
    int period;
    QString str_Data;
};
//===============================================================
class RawTxWindow : public ConfigurableWidget
{
    Q_OBJECT

public:
    explicit RawTxWindow(QWidget *parent, Backend &backend);
    ~RawTxWindow();

    virtual bool saveXML(Backend &backend, QDomDocument &xml, QDomElement &root);
    virtual bool loadXML(Backend &backend, QDomElement &el);

private slots:
    void changeDLC();
    void updateCapabilities();
    void changeRepeatRate(int ms);
    void sendRepeatMessage(bool enable);
    void disableTxWindow(int disable);
    void refreshInterfaces();
    void sendRawMessage();
    void sendQueueMessages();
    void fileSend();
//====================================================================================


    void QueueModeration();
    void Send_Label_Custom(Message *msg, int regime);
    void Add_Label ();
    void Delete_Label(int index);
    void on_SendButton_clicked();
    void sendListMessage(Message &message);


    void on_DeleteButton_clicked();
    void add_label_to_table(int index);
    void remove_label_from_table(int index);
    void on_activebut_clicked();
    void sendRepeatMessage_custom(bool enable);
    void changeRepeatRate_custom(int ms);
///===============================================================================
    void on_labelTableWidget_cellClicked(int row, int column);

private:

    //QTableWidget Label_Table;
    //=================================================================================
    QTimer * RepeatListMessages;
    QStringList names_of_colomns;
    //=================================================================================
    Ui::RawTxWindow *ui;
    Backend &_backend;
    QTimer *repeatmsg_timer;
    QTimer *filemsg_timer;
    std::queue<TxMsgParams> txMsgs;
    uint32_t currTime = 0;
    bool transmission = false;
    void readFile(const QString& fileName);
    void hideFDFields();
    void showFDFields();
    void sendRawMessage(const TxMsgParams& msg);

    bool negotCompl = false;
    uint8_t motorCounter = 0;
};
