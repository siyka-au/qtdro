/***************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the QtBluetooth module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
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
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
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

#include <iostream>
#include <ostream>

#include "dial_indicator_handler.h"
#include <QDataStream>
#include <QtEndian>
#include <QRandomGenerator>


DialIndicatorHandler::DialIndicatorHandler(QObject *parent) :
    BluetoothBaseClass(parent),
    m_foundDialIndicatorService(false),
    m_measuring(false),
    m_position(0)
{

}

void DialIndicatorHandler::setAddressType(AddressType type)
{
    switch (type) {
    case DialIndicatorHandler::AddressType::PublicAddress:
        m_addressType = QLowEnergyController::PublicAddress;
        break;
    case DialIndicatorHandler::AddressType::RandomAddress:
        m_addressType = QLowEnergyController::RandomAddress;
        break;
    }
}

DialIndicatorHandler::AddressType DialIndicatorHandler::addressType() const
{
    if (m_addressType == QLowEnergyController::RandomAddress)
        return DialIndicatorHandler::AddressType::RandomAddress;

    return DialIndicatorHandler::AddressType::PublicAddress;
}

void DialIndicatorHandler::setDevice(const QString &addressString)
{
    clearMessages();
    this->address = new QBluetoothAddress(addressString);

    // Disconnect and delete old connection
    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;
    }

    // Create new controller and connect it if device available
    if (this->address) {

        // Make connections
        //! [Connect-Signals-1]
        m_control = new QLowEnergyController(*address, this);

        //! [Connect-Signals-1]
        m_control->setRemoteAddressType(m_addressType);

        //! [Connect-Signals-2]
        connect(m_control, &QLowEnergyController::serviceDiscovered, this, &DialIndicatorHandler::serviceDiscovered);
        connect(m_control, &QLowEnergyController::discoveryFinished, this, &DialIndicatorHandler::serviceScanDone);

        connect(m_control, static_cast<void (QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error), this, [this](QLowEnergyController::Error error) {
            Q_UNUSED(error);
            setError("Cannot connect to remote device.");
        });

        connect(m_control, &QLowEnergyController::connected, this, [this]() {
            setInfo("Controller connected. Search services...");
            m_control->discoverServices();
        });

        connect(m_control, &QLowEnergyController::disconnected, this, [this]() {
            setError("LowEnergy controller disconnected");
        });

        // Connect
        m_control->connectToDevice();
        //! [Connect-Signals-2]
    }
}

void DialIndicatorHandler::startMeasurement()
{
    if (alive()) {
        m_measuring = true;
        setInfo("Measuring...");
        emit measuringChanged();
    }
}

void DialIndicatorHandler::stopMeasurement()
{
    m_measuring = false;
    emit measuringChanged();
}

//! [Filter dial indicator service 1]
void DialIndicatorHandler::serviceDiscovered(const QBluetoothUuid &gatt)
{
    if (gatt == DIAL_INDICATOR_SERVICE_UUID) {
        setInfo("Dial Indicator service discovered. Waiting for service scan to be done...");
        m_foundDialIndicatorService = true;
    }
}
//! [Filter dial indicator service 1]

void DialIndicatorHandler::serviceScanDone()
{
    setInfo("Service scan done.");

    // Delete old service if available
    if (m_service) {
        delete m_service;
        m_service = nullptr;
    }

//! [Filter dial indicator service 2]
    // If dial indicator service found, create new service
    if (m_foundDialIndicatorService)
        m_service = m_control->createServiceObject(DIAL_INDICATOR_SERVICE_UUID, this);

    if (m_service) {
        connect(m_service, &QLowEnergyService::stateChanged, this, &DialIndicatorHandler::serviceStateChanged);
        connect(m_service, &QLowEnergyService::characteristicChanged, this, &DialIndicatorHandler::updatePositionValue);
        connect(m_service, &QLowEnergyService::descriptorWritten, this, &DialIndicatorHandler::confirmedDescriptorWrite);
        m_service->discoverDetails();
    } else {
        setError("Dial Indicator Service not found.");
    }
//! [Filter dial indicator service 2]
}

// Service functions
//! [Find position measurement characteristic]
void DialIndicatorHandler::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s) {
    case QLowEnergyService::DiscoveringServices:
        setInfo(tr("Discovering services..."));
        break;
    case QLowEnergyService::ServiceDiscovered:
    {
        setInfo(tr("Service discovered."));

        const QLowEnergyCharacteristic dialIndicatorChar = m_service->characteristic(DIAL_INDICATOR_POSITION_CHARACTERISTIC_UUID);
        if (!dialIndicatorChar.isValid()) {
            setError("Dial Indicator data not found.");
            break;
        }

        m_notificationDesc = dialIndicatorChar.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
        if (m_notificationDesc.isValid())
            m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));

        break;
    }
    default:
        //nothing for now
        break;
    }

    emit aliveChanged();
}
//! [Find position measurement characteristic]

//! [Reading value]
void DialIndicatorHandler::updatePositionValue(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() != DIAL_INDICATOR_POSITION_CHARACTERISTIC_UUID)
        return;

    // Position
    float position = 0;
    uint16_t flags = 0;
    QDataStream in(value);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    in >> position;
    in >> flags;

    emit newMeasurementReceived(position);
}
//! [Reading value]

void DialIndicatorHandler::confirmedDescriptorWrite(const QLowEnergyDescriptor &d, const QByteArray &value)
{
    if (d.isValid() && d == m_notificationDesc && value == QByteArray::fromHex("0000")) {
        //disabled notifications -> assume disconnect intent
        m_control->disconnectFromDevice();
        delete m_service;
        m_service = nullptr;
    } else {
        setInfo("Notifications enabled.");
    }
}

void DialIndicatorHandler::disconnectService()
{
    m_foundDialIndicatorService = false;

    //disable notifications
    if (m_notificationDesc.isValid() && m_service
            && m_notificationDesc.value() == QByteArray::fromHex("0100")) {
        m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0000"));
    } else {
        if (m_control)
            m_control->disconnectFromDevice();

        delete m_service;
        m_service = nullptr;
    }
}

bool DialIndicatorHandler::measuring() const
{
    return m_measuring;
}

bool DialIndicatorHandler::alive() const
{
    if (m_service)
        return m_service->state() == QLowEnergyService::ServiceDiscovered;

    return false;
}

double DialIndicatorHandler::position() const
{
    return m_position;
}
