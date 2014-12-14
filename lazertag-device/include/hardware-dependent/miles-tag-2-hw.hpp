/*
 * miles-tag-2-hal.hpp
 *
 *  Created on: 14 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_DEVICE_INCLUDE_HAL_MILES_TAG_2_HAL_HPP_
#define LAZERTAG_DEVICE_INCLUDE_HAL_MILES_TAG_2_HAL_HPP_

#include "hal/miles-tag-2-base.hpp"

class MilesTag2Transmitter : public MilesTag2TransmitterBase
{
public:
    MilesTag2Transmitter();
    ~MilesTag2Transmitter() {}
    void init();
    static void select();

private:
    void fireCallback(bool wasOnState);
    static void fireCallbackStaticWrapper(void* object, bool wasOnState);
    void beginTransmission();
    bool m_sendingHeader;
};

class MilesTag2Receiver : public MilesTag2ReceiverBase
{
public:
    MilesTag2Receiver();

    void init(unsigned int channel);
    void interruptionHandler();

    static void select();
private:
    void resetReceiver();
    enum ReceivingState
    {
        RS_WAITING_HEADER = 0,
        RS_HEADER_BEGINNED = 1,
        RS_SPACE = 2,
        RS_BIT = 3
    };

    ReceivingState m_state;
    unsigned int m_channel;

    bool m_falseImpulse;
};

#endif /* LAZERTAG_DEVICE_INCLUDE_HAL_MILES_TAG_2_HAL_HPP_ */
