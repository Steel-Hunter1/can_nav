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

#include "RawTxWindow.h"
#include "ui_RawTxWindow.h"

#include <QDomDocument>
#include <QTimer>
#include <core/Backend.h>
#include <driver/CanInterface.h>

#include <regex>
#include <queue>

//============================== хранилище меток ==========================================
#include "QVector"


QVector <Message> Messages;


//========================================================================

void tokenizeStrByDelim(const std::string &str,
    std::vector<std::string> &tokenizedStr,
    const std::string delimStr)
{
    std::regex delimiter(delimStr);
    tokenizedStr = std::vector<std::string>(
        std::sregex_token_iterator(
                str.begin(), str.end(), delimiter, -1), {});
}

RawTxWindow::RawTxWindow(QWidget *parent, Backend &backend) :
    ConfigurableWidget(parent),
    ui(new Ui::RawTxWindow),

    _backend(backend)
{
    ui->setupUi(this);

    connect(ui->singleSendButton, SIGNAL(released()), this, SLOT(sendRawMessage()));
    connect(ui->fileSendButton, SIGNAL(released()), this, SLOT(fileSend()));
    connect(ui->repeatSendButton, SIGNAL(toggled(bool)), this, SLOT(sendRepeatMessage(bool)));

    connect(ui->spinBox_RepeatRate, SIGNAL(valueChanged(int)), this, SLOT(changeRepeatRate(int)));

    connect(ui->comboBoxInterface, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCapabilities()));
    connect(ui->checkbox_FD, SIGNAL(stateChanged(int)), this, SLOT(updateCapabilities()));

    connect(&backend, SIGNAL(beginMeasurement()),  this, SLOT(refreshInterfaces()));

    // Timer for repeating messages
    repeatmsg_timer = new QTimer(this);
    connect(repeatmsg_timer, SIGNAL(timeout()), this, SLOT(sendRawMessage()));



    //====================================================================================================
    RepeatListMessages = new QTimer(this);
    connect(RepeatListMessages, SIGNAL(timeout()), this, SLOT(QueueModeration()));
    //====================================================================================================


    filemsg_timer = new QTimer(this);
    filemsg_timer->stop();
    connect(filemsg_timer, SIGNAL(timeout()), this, SLOT(sendQueueMessages()));


    // TODO: Grey out checkboxes that are invalid depending on DLC spinbox state
    //connect(ui->fieldDLC, SIGNAL(valueChanged(int)), this, SLOT(changeDLC(int)));
    connect(ui->comboBoxDLC, SIGNAL(currentIndexChanged(int)), this, SLOT(changeDLC()));

    // Disable TX until interfaces are present
    this->setDisabled(1);
    //////==================================================================================

    //////==================================================================================

}


RawTxWindow::~RawTxWindow()
{
    delete ui;
}


void RawTxWindow::changeDLC()
{

    ui->fieldByte0_0->setEnabled(true);
    ui->fieldByte1_0->setEnabled(true);
    ui->fieldByte2_0->setEnabled(true);
    ui->fieldByte3_0->setEnabled(true);
    ui->fieldByte4_0->setEnabled(true);
    ui->fieldByte5_0->setEnabled(true);
    ui->fieldByte6_0->setEnabled(true);
    ui->fieldByte7_0->setEnabled(true);

    ui->fieldByte0_1->setEnabled(true);
    ui->fieldByte1_1->setEnabled(true);
    ui->fieldByte2_1->setEnabled(true);
    ui->fieldByte3_1->setEnabled(true);
    ui->fieldByte4_1->setEnabled(true);
    ui->fieldByte5_1->setEnabled(true);
    ui->fieldByte6_1->setEnabled(true);
    ui->fieldByte7_1->setEnabled(true);

    ui->fieldByte0_2->setEnabled(true);
    ui->fieldByte1_2->setEnabled(true);
    ui->fieldByte2_2->setEnabled(true);
    ui->fieldByte3_2->setEnabled(true);
    ui->fieldByte4_2->setEnabled(true);
    ui->fieldByte5_2->setEnabled(true);
    ui->fieldByte6_2->setEnabled(true);
    ui->fieldByte7_2->setEnabled(true);

    ui->fieldByte0_3->setEnabled(true);
    ui->fieldByte1_3->setEnabled(true);
    ui->fieldByte2_3->setEnabled(true);
    ui->fieldByte3_3->setEnabled(true);
    ui->fieldByte4_3->setEnabled(true);
    ui->fieldByte5_3->setEnabled(true);
    ui->fieldByte6_3->setEnabled(true);
    ui->fieldByte7_3->setEnabled(true);

    ui->fieldByte0_4->setEnabled(true);
    ui->fieldByte1_4->setEnabled(true);
    ui->fieldByte2_4->setEnabled(true);
    ui->fieldByte3_4->setEnabled(true);
    ui->fieldByte4_4->setEnabled(true);
    ui->fieldByte5_4->setEnabled(true);
    ui->fieldByte6_4->setEnabled(true);
    ui->fieldByte7_4->setEnabled(true);

    ui->fieldByte0_5->setEnabled(true);
    ui->fieldByte1_5->setEnabled(true);
    ui->fieldByte2_5->setEnabled(true);
    ui->fieldByte3_5->setEnabled(true);
    ui->fieldByte4_5->setEnabled(true);
    ui->fieldByte5_5->setEnabled(true);
    ui->fieldByte6_5->setEnabled(true);
    ui->fieldByte7_5->setEnabled(true);

    ui->fieldByte0_6->setEnabled(true);
    ui->fieldByte1_6->setEnabled(true);
    ui->fieldByte2_6->setEnabled(true);
    ui->fieldByte3_6->setEnabled(true);
    ui->fieldByte4_6->setEnabled(true);
    ui->fieldByte5_6->setEnabled(true);
    ui->fieldByte6_6->setEnabled(true);
    ui->fieldByte7_6->setEnabled(true);

    ui->fieldByte0_7->setEnabled(true);
    ui->fieldByte1_7->setEnabled(true);
    ui->fieldByte2_7->setEnabled(true);
    ui->fieldByte3_7->setEnabled(true);
    ui->fieldByte4_7->setEnabled(true);
    ui->fieldByte5_7->setEnabled(true);
    ui->fieldByte6_7->setEnabled(true);
    ui->fieldByte7_7->setEnabled(true);

    uint8_t dlc = ui->comboBoxDLC->currentData().toUInt();

    switch(dlc)
    {
        case 0:
            ui->fieldByte0_0->setEnabled(false);
            //fallthrough
        case 1:
            ui->fieldByte1_0->setEnabled(false);
            //fallthrough

        case 2:
            ui->fieldByte2_0->setEnabled(false);
            //fallthrough

        case 3:
            ui->fieldByte3_0->setEnabled(false);
            //fallthrough

        case 4:
            ui->fieldByte4_0->setEnabled(false);
            //fallthrough

        case 5:
            ui->fieldByte5_0->setEnabled(false);
            //fallthrough

        case 6:
            ui->fieldByte6_0->setEnabled(false);
            //fallthrough

        case 7:
            ui->fieldByte7_0->setEnabled(false);
            //fallthrough

        case 8:
            ui->fieldByte0_1->setEnabled(false);
            ui->fieldByte1_1->setEnabled(false);
            ui->fieldByte2_1->setEnabled(false);
            ui->fieldByte3_1->setEnabled(false);
            //fallthrough
        case 12:
            ui->fieldByte4_1->setEnabled(false);
            ui->fieldByte5_1->setEnabled(false);
            ui->fieldByte6_1->setEnabled(false);
            ui->fieldByte7_1->setEnabled(false);
            //fallthrough
    case 16:
        ui->fieldByte0_2->setEnabled(false);
        ui->fieldByte1_2->setEnabled(false);
        ui->fieldByte2_2->setEnabled(false);
        ui->fieldByte3_2->setEnabled(false);
        //fallthrough
    case 20:
        ui->fieldByte4_2->setEnabled(false);
        ui->fieldByte5_2->setEnabled(false);
        ui->fieldByte6_2->setEnabled(false);
        ui->fieldByte7_2->setEnabled(false);
        //fallthrough
    case 24:
        ui->fieldByte0_3->setEnabled(false);
        ui->fieldByte1_3->setEnabled(false);
        ui->fieldByte2_3->setEnabled(false);
        ui->fieldByte3_3->setEnabled(false);
        ui->fieldByte4_3->setEnabled(false);
        ui->fieldByte5_3->setEnabled(false);
        ui->fieldByte6_3->setEnabled(false);
        ui->fieldByte7_3->setEnabled(false);
        //fallthrough
    case 32:
        ui->fieldByte0_4->setEnabled(false);
        ui->fieldByte1_4->setEnabled(false);
        ui->fieldByte2_4->setEnabled(false);
        ui->fieldByte3_4->setEnabled(false);
        ui->fieldByte4_4->setEnabled(false);
        ui->fieldByte5_4->setEnabled(false);
        ui->fieldByte6_4->setEnabled(false);
        ui->fieldByte7_4->setEnabled(false);

        ui->fieldByte0_5->setEnabled(false);
        ui->fieldByte1_5->setEnabled(false);
        ui->fieldByte2_5->setEnabled(false);
        ui->fieldByte3_5->setEnabled(false);
        ui->fieldByte4_5->setEnabled(false);
        ui->fieldByte5_5->setEnabled(false);
        ui->fieldByte6_5->setEnabled(false);
        ui->fieldByte7_5->setEnabled(false);
        //fallthrough
    case 48:
        ui->fieldByte0_6->setEnabled(false);
        ui->fieldByte1_6->setEnabled(false);
        ui->fieldByte2_6->setEnabled(false);
        ui->fieldByte3_6->setEnabled(false);
        ui->fieldByte4_6->setEnabled(false);
        ui->fieldByte5_6->setEnabled(false);
        ui->fieldByte6_6->setEnabled(false);
        ui->fieldByte7_6->setEnabled(false);

        ui->fieldByte0_7->setEnabled(false);
        ui->fieldByte1_7->setEnabled(false);
        ui->fieldByte2_7->setEnabled(false);
        ui->fieldByte3_7->setEnabled(false);
        ui->fieldByte4_7->setEnabled(false);
        ui->fieldByte5_7->setEnabled(false);
        ui->fieldByte6_7->setEnabled(false);
        ui->fieldByte7_7->setEnabled(false);



        //==============================================================
        ui->DeleteButton->setEnabled(true);
        ui->DeleteIndex->setEnabled(true);
        ui->SendButton->setEnabled(true);

        ui->ListOfLabels->setEnabled(true);
        ui->ListOfLabels->isReadOnly();
        //===============================================================
    }
//    repeatmsg_timer->setInterval(ms);
}

void RawTxWindow::updateCapabilities()
{

    // check if intf suports fd, if, enable, else dis
    //CanInterface *intf = _backend.getInterfaceById(idx);
    if(ui->comboBoxInterface->count() > 0)
    {
        // By default BRS should be available

        CanInterface *intf = _backend.getInterfaceById((CanInterfaceId)ui->comboBoxInterface->currentData().toUInt());

        if(intf == NULL)
        {
            return;
        }

        int idx_restore = ui->comboBoxDLC->currentIndex();

        // If CANFD is available
        if(intf->getCapabilities() & intf->capability_canfd)
        {
            ui->comboBoxDLC->clear();
            ui->comboBoxDLC->addItem("0", 0);
            ui->comboBoxDLC->addItem("1", 1);
            ui->comboBoxDLC->addItem("2", 2);
            ui->comboBoxDLC->addItem("3", 3);
            ui->comboBoxDLC->addItem("4", 4);
            ui->comboBoxDLC->addItem("5", 5);
            ui->comboBoxDLC->addItem("6", 6);
            ui->comboBoxDLC->addItem("7", 7);
            ui->comboBoxDLC->addItem("8", 8);
            ui->comboBoxDLC->addItem("12", 12);
            ui->comboBoxDLC->addItem("16", 16);
            ui->comboBoxDLC->addItem("20", 20);
            ui->comboBoxDLC->addItem("24", 24);
            ui->comboBoxDLC->addItem("32", 32);
            ui->comboBoxDLC->addItem("48", 48);
            ui->comboBoxDLC->addItem("64", 64);

            // Restore previous selected DLC if available
            if(idx_restore > 1 && idx_restore < ui->comboBoxDLC->count())
                ui->comboBoxDLC->setCurrentIndex(idx_restore);

            ui->checkbox_FD->setDisabled(0);

            // Enable BRS if this is an FD frame
            if(ui->checkbox_FD->isChecked())
            {
                // Enable BRS if FD enabled
                ui->checkbox_BRS->setDisabled(0);

                // Disable RTR if FD enabled
                ui->checkBox_IsRTR->setDisabled(1);
                ui->checkBox_IsRTR->setChecked(false);
            }
            else
            {
                // Disable BRS if FD disabled
                ui->checkbox_BRS->setDisabled(1);
                ui->checkbox_BRS->setChecked(false);

                // Enable RTR if FD disabled
                ui->checkBox_IsRTR->setDisabled(0);

            }
            showFDFields();
        }
        else
        {
            // CANFD not available
            ui->comboBoxDLC->clear();
            ui->comboBoxDLC->addItem("0", 0);
            ui->comboBoxDLC->addItem("1", 1);
            ui->comboBoxDLC->addItem("2", 2);
            ui->comboBoxDLC->addItem("3", 3);
            ui->comboBoxDLC->addItem("4", 4);
            ui->comboBoxDLC->addItem("5", 5);
            ui->comboBoxDLC->addItem("6", 6);
            ui->comboBoxDLC->addItem("7", 7);
            ui->comboBoxDLC->addItem("8", 8);

            // Restore previous selected DLC if available
            if(idx_restore > 1 && idx_restore < ui->comboBoxDLC->count())
                ui->comboBoxDLC->setCurrentIndex(idx_restore);

            // Unset/disable FD / BRS checkboxes
            ui->checkbox_FD->setDisabled(1);
            ui->checkbox_BRS->setDisabled(1);
            ui->checkbox_FD->setChecked(false);
            ui->checkbox_BRS->setChecked(false);

            // Enable RTR (could be disabled by FD checkbox being set)
            ui->checkBox_IsRTR->setDisabled(0);

            hideFDFields();

        }
    }
}

void RawTxWindow::changeRepeatRate(int ms)
{
    repeatmsg_timer->setInterval(ms);
}

void RawTxWindow::sendRepeatMessage(bool enable)
{
    if(enable)
    {
        negotCompl = false;
        motorCounter = 0;
        repeatmsg_timer->start(ui->spinBox_RepeatRate->value());
//        repeatmsg_timer->setS
// repeatmsg_timer->setSingleShot(true)
    }
    else
    {
        repeatmsg_timer->stop();
    }
}




void RawTxWindow::disableTxWindow(int disable)
{
    if(disable)
    {
        this->setDisabled(1);
    }
    else
    {
        // Only enable if an interface is present
        if(_backend.getInterfaceList().count() > 0)
            this->setDisabled(0);
        else
            this->setDisabled(1);
    }
}

void RawTxWindow::refreshInterfaces()
{
    ui->comboBoxInterface->clear();

    int cb_idx = 0;

    // TODO: Only show interfaces that are included in active MeasurementInterfaces!
    foreach (CanInterfaceId ifid, _backend.getInterfaceList()) {
        CanInterface *intf = _backend.getInterfaceById(ifid);

        if(intf->isOpen())
        {
            ui->comboBoxInterface->addItem(intf->getName() + " " + intf->getDriver()->getName());
            ui->comboBoxInterface->setItemData(cb_idx, QVariant(ifid));
            cb_idx++;
        }
    }

    if(cb_idx == 0)
        disableTxWindow(1);
    else
        disableTxWindow(0);

    updateCapabilities();
}

void RawTxWindow::readFile(const QString& fileName)
{
    FILE* f = fopen(fileName.toStdString().c_str(), "r");
    
    if (!f)
    {
        return;
    }

    int symb;
    std::vector<std::string> lines;
    std::string line;

    txMsgs.empty();

    while ((symb = fgetc(f)) != EOF)
    {
        if (symb != '\r' && symb != '\n')
        {
            line.push_back(symb);
        }
        else if (line.length())
        {
            lines.push_back(line);

            // fprintf(stderr, "LineLen %d, linesCnt %d\n", line.length(), lines.size());

            line.erase();
        }
    }

    if (line.length())
        lines.push_back(line);

    for (auto it = lines.begin(); it != lines.end(); ++it)
    {
        std::vector<std::string> tokenizedStr;

        tokenizeStrByDelim(*it, tokenizedStr, ",");
        fprintf(stderr, "Tokenized size %d\n", tokenizedStr.size());
        for (int tmpI = 0; tmpI < tokenizedStr.size(); ++tmpI)
        {
            fprintf(stderr, "Token %d <%s>\n", tmpI, tokenizedStr[tmpI].c_str());
        }

        if (tokenizedStr.size() >= 4)
        {
            TxMsgParams params;

            // fprintf(stderr, "Token line <%s><%s><%s><%s>\n",
            //     tokenizedStr[0].c_str(), tokenizedStr[1], tokenizedStr[2], tokenizedStr[3]);

            params.time = std::stol(tokenizedStr[0]) / 1000;
            params.id = std::stoi(tokenizedStr[1], nullptr, 16);
            params.length = (uint8_t)std::stoi(tokenizedStr[2]);

            params.data = new uint8_t[params.length];

            fprintf(stderr, "Time %d\n", params.time);

            std::vector<std::string> tokenizedData;

            // tokenizeStrByDelim(tokenizedStr[3], tokenizedData, " ");

            // if (tokenizedData.size() == params.length)
            // {
                for (int i = 0; i < params.length; ++i)
                {
                    params.data[i] = (uint8_t)std::stoi(tokenizedStr[3+i], nullptr, 16);
                }
            // }

            // fprintf(stderr, "Parsed time <%s>(%ld)(%d)\n", tokenizedStr[0].c_str(), std::stol(tokenizedStr[0]), params.time);

            txMsgs.push(params);
        }
    }
}

void RawTxWindow::fileSend()
{
    if (transmission)
    {
        filemsg_timer->stop();
        while (txMsgs.size())
            txMsgs.pop();
        log_info("Stop file sending\n");
        transmission = false;
    }
    else
    {
        const QString& fileName = ui->fileSendName->text();

        fprintf(stderr, "txMsgSize1 %ld\n", txMsgs.size());
        while (txMsgs.size())
            txMsgs.pop();
        fprintf(stderr, "txMsgSize2 %ld\n", txMsgs.size());

        log_info("Start file sendign: " + fileName);
        readFile(fileName);
        currTime = 0;
        transmission = true;
        filemsg_timer->setSingleShot(true);
        filemsg_timer->start(20);
        QString tmp = "filemsg_timer " + QString(filemsg_timer->isActive()) + " " + QString(filemsg_timer->interval());
        log_info(tmp);

        char outmsg[256];
        snprintf(outmsg, 256, "Messages to send BEGIN %d", txMsgs.size());
        log_info(outmsg);
    }
}

void RawTxWindow::sendQueueMessages()
{
    uint32_t saveLastTime = currTime;
    uint32_t waitTime = 0;
    static uint32_t perfCnt = 0;
    static uint32_t lastCount = 0;

    if (lastCount > txMsgs.size() && lastCount - txMsgs.size() > 100) {
        // char outmsg[256];
        // snprintf(outmsg, 256, "Messages to send %d", txMsgs.size());
        // log_info(outmsg);
        fprintf(stderr, "Messages %ld\n", txMsgs.size());
        lastCount = txMsgs.size();
    }
    else if (!lastCount)
    {
        lastCount = txMsgs.size();
    }

    while (txMsgs.size())
    {
        TxMsgParams& params = txMsgs.front();
        if (txMsgs.size() == 1)
            fprintf(stderr, "END: Messages %ld\n", txMsgs.size());
        
        if (params.time < currTime || params.time - currTime <= 1)
        {
            // fprintf(stderr, "perform send %d\n", perfCnt++);
            sendRawMessage(params);
            if (params.length)
            {
                delete[] params.data;
                params.length = 0;
            }
            txMsgs.pop();

            saveLastTime = params.time;
        }
        else
        {
            waitTime = params.time - saveLastTime;
            currTime = params.time;

            if (!waitTime)
                waitTime = 1;
            break;
        }
    }

    if (waitTime)
    {
        filemsg_timer->setSingleShot(true);
        filemsg_timer->start(waitTime);
        // fprintf(stderr, "TimerStart %d\n", waitTime);
    }
}

void RawTxWindow::sendRawMessage(const TxMsgParams& msgParams)
{
    CanMessage msg;

    bool en_extended = ui->checkBox_IsExtended->isChecked();
    bool en_rtr = ui->checkBox_IsRTR->isChecked();

    uint32_t address = msgParams.id;

    // If address is beyond std address namespace, force extended
    if(address > 0x7ff)
    {
        en_extended = true;
        ui->checkBox_IsExtended->setChecked(true);
    }

    // If address is larger than max for extended, clip
    if(address >= 0x1FFFFFFF)
    {
        address = address & 0x1FFFFFFF;
        ui->fieldAddress->setText(QString::number( address, 16 ).toUpper());
    }

    uint8_t dlc =ui->comboBoxDLC->currentData().toUInt();

    // If DLC > 8, must be FD
    if(dlc > 8)
    {
        ui->checkbox_FD->setChecked(true);
    }

    // Set payload data
    for(int i=0; i<msgParams.length; i++)
    {
        msg.setDataAt(i, msgParams.data[i]);
    }

    msg.setId(address);
    msg.setLength(dlc);

    msg.setExtended(en_extended);
    msg.setRTR(en_rtr);
    msg.setErrorFrame(false);

    if(ui->checkbox_BRS->isChecked())
        msg.setBRS(true);
    if(ui->checkbox_FD->isChecked())
        msg.setFD(true);

    CanInterface *intf = _backend.getInterfaceById((CanInterfaceId)ui->comboBoxInterface->currentData().toUInt());
    intf->sendMessage(msg);


    // char outmsg[256];
    // snprintf(outmsg, 256, "Send [%s] to %d on port %s [ext=%u rtr=%u err=%u fd=%u brs=%u]",
    //          msg.getDataHexString().toLocal8Bit().constData(), msg.getId(), intf->getName().toLocal8Bit().constData(),
    //          msg.isExtended(), msg.isRTR(), msg.isErrorFrame(), msg.isFD(), msg.isBRS());
    // log_info(outmsg);

}

#include <QDateTime>

void RawTxWindow::sendRawMessage() // штатная отправка сообщений
{

    CanMessage msg;

    static uint64_t tPrev = 0;
    uint64_t t1 = QDateTime::currentMSecsSinceEpoch();
    fprintf(stderr, "Curr time %lld Diff %lld\n", t1, (t1 - tPrev));
    tPrev = t1;

    bool en_extended = ui->checkBox_IsExtended->isChecked();
    bool en_rtr = ui->checkBox_IsRTR->isChecked();

    static uint8_t data_int[64];
    int data_ctr = 0;

    if (!negotCompl)
    {
        // data_int[data_ctr++] = 0xAA;
        // data_int[data_ctr++] = 0xAA;
        // data_int[data_ctr++] = 0xAA;
        // data_int[data_ctr++] = 0xAA;
        // data_int[data_ctr++] = 0xAA;
        // data_int[data_ctr++] = 0xAA;
        // data_int[data_ctr++] = 0xAA;
        // data_int[data_ctr++] = 0xAA;

        data_int[data_ctr++] = ui->fieldByte0_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte1_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte2_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte3_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte4_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte5_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte6_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte7_0->text().toUpper().toInt(NULL, 16);

        negotCompl = true;
        ui->checkbox_UPD->setCheckState(Qt::CheckState::Checked);
    }
    else if (ui->checkbox_UPD->isChecked())
    {
        data_int[data_ctr++] = ui->fieldByte0_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte1_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte2_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte3_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte4_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte5_0->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte6_0->text().toUpper().toInt(NULL, 16);
        // data_int[data_ctr++] = motorCounter;
        data_int[data_ctr++] = ui->fieldByte7_0->text().toUpper().toInt(NULL, 16);

        motorCounter++;

        data_int[data_ctr++] = ui->fieldByte0_1->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte1_1->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte2_1->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte3_1->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte4_1->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte5_1->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte6_1->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte7_1->text().toUpper().toInt(NULL, 16);

        data_int[data_ctr++] = ui->fieldByte0_2->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte1_2->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte2_2->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte3_2->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte4_2->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte5_2->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte6_2->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte7_2->text().toUpper().toInt(NULL, 16);

        data_int[data_ctr++] = ui->fieldByte0_3->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte1_3->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte2_3->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte3_3->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte4_3->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte5_3->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte6_3->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte7_3->text().toUpper().toInt(NULL, 16);

        data_int[data_ctr++] = ui->fieldByte0_4->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte1_4->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte2_4->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte3_4->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte4_4->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte5_4->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte6_4->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte7_4->text().toUpper().toInt(NULL, 16);

        data_int[data_ctr++] = ui->fieldByte0_5->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte1_5->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte2_5->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte3_5->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte4_5->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte5_5->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte6_5->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte7_5->text().toUpper().toInt(NULL, 16);

        data_int[data_ctr++] = ui->fieldByte0_6->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte1_6->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte2_6->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte3_6->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte4_6->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte5_6->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte6_6->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte7_6->text().toUpper().toInt(NULL, 16);

        data_int[data_ctr++] = ui->fieldByte0_7->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte1_7->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte2_7->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte3_7->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte4_7->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte5_7->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte6_7->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByte7_7->text().toUpper().toInt(NULL, 16);

        ui->checkbox_UPD->setCheckState(Qt::CheckState::Unchecked);
    }
    else
    {
        // data_int[7] = motorCounter;
        data_int[7] = ui->fieldByte7_0->text().toUpper().toInt(NULL, 16);
        motorCounter++;
    }



    uint32_t address = 0;
    QString addrStr = ui->fieldAddress->text();
    std::vector<uint32_t> addresses;
    if (addrStr.contains(';'))
    {
        std::vector<std::string> tokens;
        tokenizeStrByDelim(addrStr.toStdString(), tokens, ";");
        
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            if (tokens[i].length())
            {
                if (i >= 1)
                {
                    addresses.push_back(std::stoul(tokens[i], 0, 16));
                }
                else
                {
                    address = std::stoul(tokens[i], 0, 16);
                }
            }
        }
    }
    else
    {
        address = addrStr.toUpper().toUInt(NULL, 16);
    }

    // If address is beyond std address namespace, force extended
    if(address > 0x7ff)
    {
        en_extended = true;
        ui->checkBox_IsExtended->setChecked(true);
    }

    // If address is larger than max for extended, clip
    if(address >= 0x1FFFFFFF)
    {
        address = address & 0x1FFFFFFF;
        ui->fieldAddress->setText(QString::number( address, 16 ).toUpper());
    }

    uint8_t dlc =ui->comboBoxDLC->currentData().toUInt();

    // If DLC > 8, must be FD
    if(dlc > 8)
    {
        ui->checkbox_FD->setChecked(true);
    }

    // Set payload data
    for(int i=0; i<dlc; i++)
    {
        msg.setDataAt(i, data_int[i]);
    }

    msg.setId(address);
    msg.setLength(dlc);

    msg.setExtended(en_extended);
    msg.setRTR(en_rtr);
    msg.setErrorFrame(false);

    if(ui->checkbox_BRS->isChecked())
        msg.setBRS(true);
    if(ui->checkbox_FD->isChecked())
        msg.setFD(true);

    CanInterface *intf = _backend.getInterfaceById((CanInterfaceId)ui->comboBoxInterface->currentData().toUInt());

    intf->sendMessage(msg);


    char outmsg[256];
    snprintf(outmsg, 256, "Send [%s] to %d on port %s [ext=%u rtr=%u err=%u fd=%u brs=%u]",
             msg.getDataHexString().toLocal8Bit().constData(), msg.getId(), intf->getName().toLocal8Bit().constData(),
             msg.isExtended(), msg.isRTR(), msg.isErrorFrame(), msg.isFD(), msg.isBRS());
    log_info(outmsg);

    for (int i = 0; i < addresses.size(); ++i)
    {
        msg.setId(addresses[i]);

        intf->sendMessage(msg);

        char outmsg[256];
        snprintf(outmsg, 256, "Send [%s] to %d on port %s [ext=%u rtr=%u err=%u fd=%u brs=%u]",
                msg.getDataHexString().toLocal8Bit().constData(), msg.getId(), intf->getName().toLocal8Bit().constData(),
                msg.isExtended(), msg.isRTR(), msg.isErrorFrame(), msg.isFD(), msg.isBRS());
        log_info(outmsg);
    }

    fprintf(stderr, "Time diff %lld\n", QDateTime::currentMSecsSinceEpoch() - t1);

}

bool RawTxWindow::saveXML(Backend &backend, QDomDocument &xml, QDomElement &root)
{
    if (!ConfigurableWidget::saveXML(backend, xml, root)) { return false; }
    root.setAttribute("type", "RawTxWindow");
    return true;
}

bool RawTxWindow::loadXML(Backend &backend, QDomElement &el)
{
    if (!ConfigurableWidget::loadXML(backend, el)) { return false; }
    return true;
}

void RawTxWindow::hideFDFields() // прячет поля, куда пишутся байты
{

    ui->label_col21->hide();
    ui->label_col22->hide();
    ui->label_col23->hide();
    ui->label_col24->hide();
    ui->label_col25->hide();
    ui->label_col26->hide();
    ui->label_col27->hide();
    ui->label_col28->hide();

    ui->label_pay2->hide();
    ui->label_pay3->hide();
    ui->label_pay4->hide();
    ui->label_pay5->hide();
    ui->label_pay6->hide();
    ui->label_pay7->hide();
    ui->label_pay8->hide();

    ui->fieldByte0_1->hide();
    ui->fieldByte1_1->hide();
    ui->fieldByte2_1->hide();
    ui->fieldByte3_1->hide();
    ui->fieldByte4_1->hide();
    ui->fieldByte5_1->hide();
    ui->fieldByte6_1->hide();
    ui->fieldByte7_1->hide();

    ui->fieldByte0_2->hide();
    ui->fieldByte1_2->hide();
    ui->fieldByte2_2->hide();
    ui->fieldByte3_2->hide();
    ui->fieldByte4_2->hide();
    ui->fieldByte5_2->hide();
    ui->fieldByte6_2->hide();
    ui->fieldByte7_2->hide();

    ui->fieldByte0_3->hide();
    ui->fieldByte1_3->hide();
    ui->fieldByte2_3->hide();
    ui->fieldByte3_3->hide();
    ui->fieldByte4_3->hide();
    ui->fieldByte5_3->hide();
    ui->fieldByte6_3->hide();
    ui->fieldByte7_3->hide();

    ui->fieldByte0_4->hide();
    ui->fieldByte1_4->hide();
    ui->fieldByte2_4->hide();
    ui->fieldByte3_4->hide();
    ui->fieldByte4_4->hide();
    ui->fieldByte5_4->hide();
    ui->fieldByte6_4->hide();
    ui->fieldByte7_4->hide();

    ui->fieldByte0_5->hide();
    ui->fieldByte1_5->hide();
    ui->fieldByte2_5->hide();
    ui->fieldByte3_5->hide();
    ui->fieldByte4_5->hide();
    ui->fieldByte5_5->hide();
    ui->fieldByte6_5->hide();
    ui->fieldByte7_5->hide();

    ui->fieldByte0_6->hide();
    ui->fieldByte1_6->hide();
    ui->fieldByte2_6->hide();
    ui->fieldByte3_6->hide();
    ui->fieldByte4_6->hide();
    ui->fieldByte5_6->hide();
    ui->fieldByte6_6->hide();
    ui->fieldByte7_6->hide();

    ui->fieldByte0_7->hide();
    ui->fieldByte1_7->hide();
    ui->fieldByte2_7->hide();
    ui->fieldByte3_7->hide();
    ui->fieldByte4_7->hide();
    ui->fieldByte5_7->hide();
    ui->fieldByte6_7->hide();
    ui->fieldByte7_7->hide();
}


void RawTxWindow::showFDFields() // показывает поля, куда пишутся байты
{

    ui->label_col21->show();
    ui->label_col22->show();
    ui->label_col23->show();
    ui->label_col24->show();
    ui->label_col25->show();
    ui->label_col26->show();
    ui->label_col27->show();
    ui->label_col28->show();

    ui->label_pay2->show();
    ui->label_pay3->show();
    ui->label_pay4->show();
    ui->label_pay5->show();
    ui->label_pay6->show();
    ui->label_pay7->show();
    ui->label_pay8->show();


    ui->fieldByte0_1->show();
    ui->fieldByte1_1->show();
    ui->fieldByte2_1->show();
    ui->fieldByte3_1->show();
    ui->fieldByte4_1->show();
    ui->fieldByte5_1->show();
    ui->fieldByte6_1->show();
    ui->fieldByte7_1->show();

    ui->fieldByte0_2->show();
    ui->fieldByte1_2->show();
    ui->fieldByte2_2->show();
    ui->fieldByte3_2->show();
    ui->fieldByte4_2->show();
    ui->fieldByte5_2->show();
    ui->fieldByte6_2->show();
    ui->fieldByte7_2->show();

    ui->fieldByte0_3->show();
    ui->fieldByte1_3->show();
    ui->fieldByte2_3->show();
    ui->fieldByte3_3->show();
    ui->fieldByte4_3->show();
    ui->fieldByte5_3->show();
    ui->fieldByte6_3->show();
    ui->fieldByte7_3->show();

    ui->fieldByte0_4->show();
    ui->fieldByte1_4->show();
    ui->fieldByte2_4->show();
    ui->fieldByte3_4->show();
    ui->fieldByte4_4->show();
    ui->fieldByte5_4->show();
    ui->fieldByte6_4->show();
    ui->fieldByte7_4->show();

    ui->fieldByte0_5->show();
    ui->fieldByte1_5->show();
    ui->fieldByte2_5->show();
    ui->fieldByte3_5->show();
    ui->fieldByte4_5->show();
    ui->fieldByte5_5->show();
    ui->fieldByte6_5->show();
    ui->fieldByte7_5->show();

    ui->fieldByte0_6->show();
    ui->fieldByte1_6->show();
    ui->fieldByte2_6->show();
    ui->fieldByte3_6->show();
    ui->fieldByte4_6->show();
    ui->fieldByte5_6->show();
    ui->fieldByte6_6->show();
    ui->fieldByte7_6->show();

    ui->fieldByte0_7->show();
    ui->fieldByte1_7->show();
    ui->fieldByte2_7->show();
    ui->fieldByte3_7->show();
    ui->fieldByte4_7->show();
    ui->fieldByte5_7->show();
    ui->fieldByte6_7->show();
    ui->fieldByte7_7->show();
}




/////////////////////////////=================== пользовательский код ==================================================
///
///
///
///
///
///
void RawTxWindow::QueueModeration() // функция, которая оперирует с вектором Messages (в данном случае отправляет поочередно все метки в очереди один раз, привязана к таймеру)
{// удаляет все с текстового поля, посылает метки и выдавая информацию

    ui->ListOfLabels->clear();
    if (Messages.size() == 0) return;
    printf("==========================slot called==========================");
    for (int i = 0; i < Messages.size(); i++)
    {
        std::string data = std::to_string(Messages[i].data[0]) +' '+ std::to_string(Messages[i].data[1]) +std::to_string(Messages[i].data[2])
            +' '+ std::to_string(Messages[i].data[3]) +' '+ std::to_string(Messages[i].data[4]) +' '+ std::to_string(Messages[i].data[5])
            +' '+ std::to_string(Messages[i].data[6]) +' '+ std::to_string(Messages[i].data[7]);

        ui->ListOfLabels->append(QString::number(i) + " " + Messages[i].ID + ' ' + QString::fromStdString(data));

        sendListMessage(Messages[i]);

    }
}

void RawTxWindow::changeRepeatRate_custom(int ms)
{
    RepeatListMessages->setInterval(ms);
}

void RawTxWindow::sendRepeatMessage_custom(bool enable)
{
    if(enable)
    {
        negotCompl = false;
        motorCounter = 0;
        RepeatListMessages->start(ui->spinBox_RepeatRate->value());
        //        repeatmsg_timer->setS
        // repeatmsg_timer->setSingleShot(true)
    }
    else
    {
        RepeatListMessages->stop();
    }
}







void RawTxWindow::Delete_Label(int index)
{
    if (Messages.size() == 0) return;

    Messages.remove(index);
}
void RawTxWindow::Add_Label ()
{

    Message msg;
    static uint64_t tPrev = 0;
    uint64_t t1 = QDateTime::currentMSecsSinceEpoch();
    fprintf(stderr, "Curr time %lld Diff %lld\n", t1, (t1 - tPrev));
    tPrev = t1;

    bool en_extended = ui->checkBox_IsExtended->isChecked();
    bool en_rtr = ui->checkBox_IsRTR->isChecked();

    static uint8_t data_int[64];
    int data_ctr = 0;

    if (!negotCompl)
    {

        data_int[data_ctr++] = ui->fieldByteCustom1->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom2->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom3->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom4->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom5->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom6->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom7->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom8->text().toUpper().toInt(NULL, 16);

        negotCompl = true;
        ui->checkbox_UPD->setCheckState(Qt::CheckState::Checked);
    }
    else if (ui->checkbox_UPD->isChecked())
    {
        data_int[data_ctr++] = ui->fieldByteCustom1->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom2->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom3->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom4->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom5->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom6->text().toUpper().toInt(NULL, 16);
        data_int[data_ctr++] = ui->fieldByteCustom7->text().toUpper().toInt(NULL, 16);
        // data_int[data_ctr++] = motorCounter;
        data_int[data_ctr++] = ui->fieldByteCustom8->text().toUpper().toInt(NULL, 16);

        motorCounter++;

        ui->checkbox_UPD->setCheckState(Qt::CheckState::Unchecked);
    }
    else
    {
        // data_int[7] = motorCounter;
        data_int[7] = ui->fieldByteCustom8->text().toUpper().toInt(NULL, 16);
        motorCounter++;
    }



    uint32_t address = 0;
    QString addrStr = ui->fieldAddressCustom->text();

    msg.ID = ui->fieldAddressCustom->text();



    std::vector<uint32_t> addresses;
    if (addrStr.contains(';'))
    {
        std::vector<std::string> tokens;
        tokenizeStrByDelim(addrStr.toStdString(), tokens, ";");

        for (size_t i = 0; i < tokens.size(); ++i)
        {
            if (tokens[i].length())
            {
                if (i >= 1)
                {
                    addresses.push_back(std::stoul(tokens[i], 0, 16));
                }
                else
                {
                    address = std::stoul(tokens[i], 0, 16);
                }
            }
        }
    }
    else
    {
        address = addrStr.toUpper().toUInt(NULL, 16);
    }

    // If address is beyond std address namespace, force extended
    if(address > 0x7ff)
    {
        en_extended = true;
        ui->checkBox_IsExtended->setChecked(true);
    }

    // If address is larger than max for extended, clip
    if(address >= 0x1FFFFFFF)
    {
        address = address & 0x1FFFFFFF;
        ui->fieldAddressCustom->setText(QString::number( address, 16 ).toUpper());
    }

    //uint8_t dlc =ui->comboBoxDLC->currentData().toUInt();
    uint8_t dlc = 8;
    // If DLC > 8, must be FD
    if(dlc > 8)
    {
        ui->checkbox_FD->setChecked(true);
    }

    // Set payload data
    for(int i=0; i<dlc; i++)
    {
        msg.data[i]  = data_int[i];
    }

    msg.address = address;

    //==================================================

    Messages.append(msg);
}


void RawTxWindow::sendListMessage(Message message) // упрощенная версия функци отправки
{

    CanMessage msg;

    static uint64_t tPrev = 0;
    uint64_t t1 = QDateTime::currentMSecsSinceEpoch();
    fprintf(stderr, "Curr time %lld Diff %lld\n", t1, (t1 - tPrev));
    tPrev = t1;

    bool en_extended = ui->checkBox_IsExtended->isChecked();
    bool en_rtr = ui->checkBox_IsRTR->isChecked();

    static uint8_t data_int[64];
    int data_ctr = 0;





    uint32_t address = message.address;

    std::vector<uint32_t> addresses;





    // If address is beyond std address namespace, force extended
    if(address > 0x7ff)
    {
        en_extended = true;
        ui->checkBox_IsExtended->setChecked(true);
    }

    // If address is larger than max for extended, clip
    if(address >= 0x1FFFFFFF)
    {
        address = address & 0x1FFFFFFF;
        ui->fieldAddressCustom->setText(QString::number( address, 16 ).toUpper());
    }

    //uint8_t dlc =ui->comboBoxDLC->currentData().toUInt();
    uint8_t dlc = 8;
    // If DLC > 8, must be FD
    if(dlc > 8)
    {
        ui->checkbox_FD->setChecked(true);
    }

    // Set payload data

    msg.setData(message.data[0],message.data[1],message.data[2],message.data[3],message.data[4],message.data[5],message.data[6],message.data[7]);


    msg.setId(address);
    msg.setLength(dlc);

    msg.setExtended(en_extended);
    msg.setRTR(en_rtr);
    msg.setErrorFrame(false);

    if(ui->checkbox_BRS->isChecked())
        msg.setBRS(true);
    if(ui->checkbox_FD->isChecked())
        msg.setFD(true);





    CanInterface *intf = _backend.getInterfaceById((CanInterfaceId)ui->comboBoxInterface->currentData().toUInt());

    intf->sendMessage(msg);


    char outmsg[256];
    snprintf(outmsg, 256, "Send [%s] to %d on port %s [ext=%u rtr=%u err=%u fd=%u brs=%u]",
             msg.getDataHexString().toLocal8Bit().constData(), msg.getId(), intf->getName().toLocal8Bit().constData(),
             msg.isExtended(), msg.isRTR(), msg.isErrorFrame(), msg.isFD(), msg.isBRS());
    log_info(outmsg);

    for (int i = 0; i < addresses.size(); ++i)
    {
        msg.setId(addresses[i]);

        intf->sendMessage(msg);

        char outmsg[256];
        snprintf(outmsg, 256, "Send [%s] to %d on port %s [ext=%u rtr=%u err=%u fd=%u brs=%u]",
                 msg.getDataHexString().toLocal8Bit().constData(), msg.getId(), intf->getName().toLocal8Bit().constData(),
                 msg.isExtended(), msg.isRTR(), msg.isErrorFrame(), msg.isFD(), msg.isBRS());
        log_info(outmsg);
    }

    fprintf(stderr, "Time diff %lld\n", QDateTime::currentMSecsSinceEpoch() - t1);
}

void RawTxWindow::on_SendButton_clicked()
{
    Add_Label();
}


void RawTxWindow::on_DeleteButton_clicked()
{
    bool ok = false;
    int index;
    index = ui->DeleteIndex->text().toInt(&ok);
    if (ok)
    {
        Delete_Label(index);
    }

}


void RawTxWindow::on_activebut_clicked()
{
    if (RepeatListMessages->isActive())
    {
        sendRepeatMessage_custom(false);
        ui->activebut->setText("Активировать");
    }
    else
    {
        sendRepeatMessage_custom(true);
         ui->activebut->setText("Деактивировать");
    }

}

