/*
 * any-rcsp-device-base.hpp
 *
 *  Created on: 11 окт. 2017 г.
 *      Author: dalexies
 */

#ifndef UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_RCSP_DEVICE_BASE_HPP_
#define UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_RCSP_DEVICE_BASE_HPP_

#include "any-device/any-onc-device-base.hpp"

class AnyRCSPClientDeviceBase : public AnyONCDeviceBase
{
protected:
	AnyRCSPClientDeviceBase();

    void initNetworkClient() override;
    RCSPNetworkListener m_networkPackagesListener{&RCSPAggregator::getActiveAggregator()};

	MainStateSaver m_stateSaver;
};

#endif /* UNIVERSAL_DEVICE_HIGH_LEVEL_INCLUDE_ANY_DEVICE_ANY_RCSP_DEVICE_BASE_HPP_ */
