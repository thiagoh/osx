/*
 * Copyright (c) 1998-2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */


#include <libkern/OSAtomic.h>
#include <IOKit/IOUserClient.h>

#define IOFRAMEBUFFER_PRIVATE
#include <IOKit/IOHibernatePrivate.h>
#include <IOKit/graphics/IODisplay.h>
#include <IOKit/graphics/IOGraphicsPrivate.h>
#include <IOKit/IOPlatformExpert.h>
#include <IOKit/IODeviceTreeSupport.h>
#include <IOKit/IOKitKeys.h>
#include <IOKit/IOLib.h>
#include <IOKit/assert.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

const OSSymbol * gIODisplayParametersKey;
const OSSymbol * gIODisplayGUIDKey;

const OSSymbol * gIODisplayValueKey;
const OSSymbol * gIODisplayMinValueKey;
const OSSymbol * gIODisplayMaxValueKey;

const OSSymbol * gIODisplayBrightnessProbeKey;
const OSSymbol * gIODisplayLinearBrightnessProbeKey;
const OSSymbol * gIODisplayContrastKey;
const OSSymbol * gIODisplayBrightnessKey;
const OSSymbol * gIODisplayLinearBrightnessKey;
const OSSymbol * gIODisplayUsableLinearBrightnessKey;
const OSSymbol * gIODisplayBrightnessFadeKey;
const OSSymbol * gIODisplayHorizontalPositionKey;
const OSSymbol * gIODisplayHorizontalSizeKey;
const OSSymbol * gIODisplayVerticalPositionKey;
const OSSymbol * gIODisplayVerticalSizeKey;
const OSSymbol * gIODisplayTrapezoidKey;
const OSSymbol * gIODisplayPincushionKey;
const OSSymbol * gIODisplayParallelogramKey;
const OSSymbol * gIODisplayRotationKey;
const OSSymbol * gIODisplayOverscanKey;
const OSSymbol * gIODisplayVideoBestKey;
const OSSymbol * gIODisplaySelectedColorModeKey;

const OSSymbol * gIODisplayRedGammaScaleKey;
const OSSymbol * gIODisplayGreenGammaScaleKey;
const OSSymbol * gIODisplayBlueGammaScaleKey;
const OSSymbol * gIODisplayGammaScaleKey;

const OSSymbol * gIODisplayParametersTheatreModeKey;
const OSSymbol * gIODisplayParametersTheatreModeWindowKey;

const OSSymbol * gIODisplayMCCSVersionKey;
const OSSymbol * gIODisplayTechnologyTypeKey;
const OSSymbol * gIODisplayUsageTimeKey;
const OSSymbol * gIODisplayFirmwareLevelKey;

const OSSymbol * gIODisplaySpeakerVolumeKey;
const OSSymbol * gIODisplaySpeakerSelectKey;
const OSSymbol * gIODisplayMicrophoneVolumeKey;
const OSSymbol * gIODisplayAmbientLightSensorKey;
const OSSymbol * gIODisplayAudioMuteAndScreenBlankKey;
const OSSymbol * gIODisplayAudioTrebleKey;
const OSSymbol * gIODisplayAudioBassKey;
const OSSymbol * gIODisplayAudioBalanceLRKey;
const OSSymbol * gIODisplayAudioProcessorModeKey;
const OSSymbol * gIODisplayPowerModeKey;
const OSSymbol * gIODisplayManufacturerSpecificKey;

const OSSymbol * gIODisplayPowerStateKey;
const OSSymbol * gIODisplayControllerIDKey;
const OSSymbol * gIODisplayCapabilityStringKey;

const OSSymbol * gIODisplayParametersCommitKey;
const OSSymbol * gIODisplayParametersDefaultKey;
const OSSymbol * gIODisplayParametersFlushKey;

const OSSymbol * gIODisplayFadeTime1Key;
const OSSymbol * gIODisplayFadeTime2Key;
const OSSymbol * gIODisplayFadeTime3Key;
const OSSymbol * gIODisplayFadeStyleKey;

static const OSSymbol * gIODisplayFastBootEDIDKey;
static IODTPlatformExpert * gIODisplayFastBootPlatform;
static OSData *  gIODisplayZeroData;

enum {
    kIODisplayMaxUsableState  = kIODisplayMaxPowerState - 1
};

enum 
{
    kIODisplayBlankValue   = 0x100,
    kIODisplayUnblankValue = 0x200,
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#undef super
#define super IOService

OSDefineMetaClassAndAbstractStructorsWithInit( IODisplay, IOService, IODisplay::initialize() )

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

struct EDID
{
    UInt8       header[8];
    UInt8       vendorProduct[4];
    UInt8       serialNumber[4];
    UInt8       weekOfManufacture;
    UInt8       yearOfManufacture;
    UInt8       version;
    UInt8       revision;
    UInt8       displayParams[5];
    UInt8       colorCharacteristics[10];
    UInt8       establishedTimings[3];
    UInt16      standardTimings[8];
    UInt8       detailedTimings[72];
    UInt8       extension;
    UInt8       checksum;
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void IODisplay::initialize( void )
{
    gIODisplayParametersKey     = OSSymbol::withCStringNoCopy(
                                  kIODisplayParametersKey );
    gIODisplayGUIDKey           = OSSymbol::withCStringNoCopy(
                                        kIODisplayGUIDKey );
    gIODisplayValueKey          = OSSymbol::withCStringNoCopy(
                                        kIODisplayValueKey );
    gIODisplayMinValueKey       = OSSymbol::withCStringNoCopy(
                                        kIODisplayMinValueKey );
    gIODisplayMaxValueKey       = OSSymbol::withCStringNoCopy(
                                        kIODisplayMaxValueKey );
    gIODisplayContrastKey       = OSSymbol::withCStringNoCopy(
                                        kIODisplayContrastKey );
    gIODisplayBrightnessKey     = OSSymbol::withCStringNoCopy(
                                        kIODisplayBrightnessKey );
    gIODisplayLinearBrightnessKey = OSSymbol::withCStringNoCopy(
                                        kIODisplayLinearBrightnessKey );
    gIODisplayUsableLinearBrightnessKey = OSSymbol::withCStringNoCopy(
                                        kIODisplayUsableLinearBrightnessKey );
    gIODisplayBrightnessFadeKey = OSSymbol::withCStringNoCopy(
                                        kIODisplayBrightnessFadeKey );
    gIODisplayHorizontalPositionKey = OSSymbol::withCStringNoCopy(
                                          kIODisplayHorizontalPositionKey );
    gIODisplayHorizontalSizeKey = OSSymbol::withCStringNoCopy(
                                        kIODisplayHorizontalSizeKey );
    gIODisplayVerticalPositionKey = OSSymbol::withCStringNoCopy(
                                        kIODisplayVerticalPositionKey );
    gIODisplayVerticalSizeKey   = OSSymbol::withCStringNoCopy(
                                        kIODisplayVerticalSizeKey );
    gIODisplayTrapezoidKey      = OSSymbol::withCStringNoCopy(
                                        kIODisplayTrapezoidKey );
    gIODisplayPincushionKey     = OSSymbol::withCStringNoCopy(
                                        kIODisplayPincushionKey );
    gIODisplayParallelogramKey  = OSSymbol::withCStringNoCopy(
                                        kIODisplayParallelogramKey );
    gIODisplayRotationKey       = OSSymbol::withCStringNoCopy(
                                        kIODisplayRotationKey );

    gIODisplayOverscanKey       = OSSymbol::withCStringNoCopy(
                                        kIODisplayOverscanKey );
    gIODisplayVideoBestKey      = OSSymbol::withCStringNoCopy(
                                        kIODisplayVideoBestKey );
    gIODisplaySelectedColorModeKey = OSSymbol::withCStringNoCopy(
                                        kIODisplaySelectedColorModeKey );
    gIODisplayRedGammaScaleKey = OSSymbol::withCStringNoCopy(
                                        kIODisplayRedGammaScaleKey );
    gIODisplayGreenGammaScaleKey = OSSymbol::withCStringNoCopy(
                                        kIODisplayGreenGammaScaleKey );
    gIODisplayBlueGammaScaleKey = OSSymbol::withCStringNoCopy(
                                        kIODisplayBlueGammaScaleKey );
    gIODisplayGammaScaleKey = OSSymbol::withCStringNoCopy(
                                        kIODisplayGammaScaleKey );

    gIODisplayParametersCommitKey = OSSymbol::withCStringNoCopy(
                                        kIODisplayParametersCommitKey );
    gIODisplayParametersDefaultKey = OSSymbol::withCStringNoCopy(
                                         kIODisplayParametersDefaultKey );
    gIODisplayParametersFlushKey = OSSymbol::withCStringNoCopy(
                                         kIODisplayParametersFlushKey );

    gIODisplayParametersTheatreModeKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayTheatreModeKey);
    gIODisplayParametersTheatreModeWindowKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayTheatreModeWindowKey);

    gIODisplayMCCSVersionKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayMCCSVersionKey);
    gIODisplayTechnologyTypeKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayTechnologyTypeKey);
    gIODisplayUsageTimeKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayUsageTimeKey);
    gIODisplayFirmwareLevelKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayFirmwareLevelKey);
    gIODisplaySpeakerVolumeKey = OSSymbol::withCStringNoCopy(
                                            kIODisplaySpeakerVolumeKey);
    gIODisplaySpeakerSelectKey = OSSymbol::withCStringNoCopy(
                                            kIODisplaySpeakerSelectKey);
    gIODisplayMicrophoneVolumeKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayMicrophoneVolumeKey);
    gIODisplayAmbientLightSensorKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayAmbientLightSensorKey);
    gIODisplayAudioMuteAndScreenBlankKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayAudioMuteAndScreenBlankKey);
    gIODisplayAudioTrebleKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayAudioTrebleKey);
    gIODisplayAudioBassKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayAudioBassKey);
    gIODisplayAudioBalanceLRKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayAudioBalanceLRKey);
    gIODisplayAudioProcessorModeKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayAudioProcessorModeKey);
    gIODisplayPowerModeKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayPowerModeKey);
    gIODisplayManufacturerSpecificKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayManufacturerSpecificKey);

    gIODisplayPowerStateKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayPowerStateKey);

	gIODisplayControllerIDKey = OSSymbol::withCStringNoCopy(
											kIODisplayControllerIDKey);
	gIODisplayCapabilityStringKey = OSSymbol::withCStringNoCopy(
											kIODisplayCapabilityStringKey);
    gIODisplayBrightnessProbeKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayBrightnessProbeKey);
    gIODisplayLinearBrightnessProbeKey = OSSymbol::withCStringNoCopy(
                                            kIODisplayLinearBrightnessProbeKey);

	gIODisplayFadeTime1Key = OSSymbol::withCStringNoCopy("fade-time1");
	gIODisplayFadeTime2Key = OSSymbol::withCStringNoCopy("fade-time2");
	gIODisplayFadeTime3Key = OSSymbol::withCStringNoCopy("fade-time3");
	gIODisplayFadeStyleKey = OSSymbol::withCStringNoCopy("fade-style");

    IORegistryEntry * entry;
    if ((entry = getServiceRoot())
     && (0 != entry->getProperty("has-safe-sleep")))
    {
        gIODisplayFastBootPlatform = OSDynamicCast(IODTPlatformExpert, IOService::getPlatform());
        gIODisplayFastBootEDIDKey  = OSSymbol::withCStringNoCopy( kIODisplayFastBootEDIDKey );
        gIODisplayZeroData         = OSData::withCapacity(0);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IOService * IODisplay::probe(   IOService *     provider,
                              SInt32 *  score )
{
    fConnection = OSDynamicCast(IODisplayConnect, provider);

    return (this);
}

IODisplayConnect * IODisplay::getConnection( void )
{
    return (fConnection);
}


IOReturn IODisplay::getGammaTableByIndex(
    UInt32 * /* channelCount */, UInt32 * /* dataCount */,
    UInt32 * /* dataWidth */, void ** /* data */ )
{
    return (kIOReturnUnsupported);
}

void IODisplayUpdateNVRAM( IOService * entry, OSData * property )
{
    if (true && gIODisplayFastBootPlatform)
    {
        while (entry && !entry->inPlane(gIODTPlane))
        {
            entry = entry->getProvider();
        }
        if (entry)
        {
            gIODisplayFastBootPlatform->writeNVRAMProperty(entry, gIODisplayFastBootEDIDKey, 
                                                            property);
        }
    }
}

void IODisplay::searchParameterHandlers(IORegistryEntry * entry)
{
    IORegistryIterator *        iter;
    IODisplayParameterHandler * parameterHandler;

    iter = IORegistryIterator::iterateOver(entry, gIOServicePlane,
                                           kIORegistryIterateRecursively);
    if (!iter) return;
	do
	{
		iter->reset();
		while( (entry = iter->getNextObject()))
		{
			if (!(parameterHandler = OSDynamicCast(IODisplayParameterHandler, entry)))
				continue;
			addParameterHandler(parameterHandler);
		}
	} 
	while (!iter->isValid());
	iter->release();
}

bool IODisplay::start( IOService * provider )
{
    IOFramebuffer *     framebuffer;
    uintptr_t           connectFlags;
    OSData *            edidData;
    EDID *              edid;
    uint32_t            vendor = 0;
    uint32_t            product = 0;
    uint32_t            serial = 0;

    if (!super::start(provider))
        return (false);

    if (!fConnection)
        // as yet unmatched display device (ADB)
        return (true);

    framebuffer = fConnection->getFramebuffer();
    assert( framebuffer );

    fConnection->getAttributeForConnection(kConnectionFlags, &connectFlags);
    uint32_t flagsData = (uint32_t) connectFlags;
    setProperty(kIODisplayConnectFlagsKey, &flagsData, sizeof(flagsData));

    edidData = OSDynamicCast( OSData, getProperty( kIODisplayEDIDKey ));
    if (!edidData)
    {
        readFramebufferEDID();
        edidData = OSDynamicCast( OSData, getProperty( kIODisplayEDIDKey ));
    }

    if (edidData)
    {
        do
        {
            edid = (EDID *) edidData->getBytesNoCopy();
            DEBG(framebuffer->thisName, " EDID v%d.%d\n", edid->version, edid->revision );

            if (edid->version != 1)
                continue;
            // vendor
            vendor = (edid->vendorProduct[0] << 8) | edid->vendorProduct[1];

#if 0
			if (true && (0x10ac == vendor))
			{
				vendor = 0;
				edidData = 0;
				edid = 0;
				removeProperty(kIODisplayEDIDKey);
				break;
			}
#endif

            // product
            product = (edid->vendorProduct[3] << 8) | edid->vendorProduct[2];

			serial = (edid->serialNumber[3] << 24)
				   | (edid->serialNumber[2] << 16)
				   | (edid->serialNumber[1] << 8)
				   | (edid->serialNumber[0]);
			if (serial == 0x01010101) serial = 0;

            DEBG(framebuffer->thisName, " vendor/product/serial 0x%02x/0x%02x/0x%x\n", 
											vendor, product, serial );
        }
        while (false);
    }

    IODisplayUpdateNVRAM(this, edidData);

    do
    {
        UInt32  sense, extSense;
        UInt32  senseType, displayType;

        if (kIOReturnSuccess != fConnection->getAttributeForConnection(
                    kConnectionSupportsAppleSense, NULL))
            continue;

        if (kIOReturnSuccess != framebuffer->getAppleSense(
                    fConnection->getConnection(),
                    &senseType, &sense, &extSense, &displayType))
            continue;

        setProperty( kAppleDisplayTypeKey, displayType, 32);
        setProperty( kAppleSenseKey, ((sense & 0xff) << 8) | (extSense & 0xff), 32);

        if (0 == vendor)
        {
            vendor = kDisplayVendorIDUnknown;
            if (0 == senseType)
                product = ((sense & 0xff) << 8) | (extSense & 0xff);
            else
                product = (displayType & 0xff) << 16;
        }
    }
    while (false);

    if (0 == vendor)
    {
        vendor = kDisplayVendorIDUnknown;
        product = kDisplayProductIDGeneric;
    }

    if (0 == getProperty(kDisplayVendorID))
        setProperty( kDisplayVendorID, vendor, 32);
    if (0 == getProperty(kDisplayProductID))
        setProperty( kDisplayProductID, product, 32);
    if (0 == getProperty(kDisplaySerialNumber))
        setProperty( kDisplaySerialNumber, serial, 32);

    enum
    {
        kMaxKeyLen = 1024,
        kMaxKeyVendorProduct = 20 /* "-12345678-12345678" */
    };
    int pathLen = kMaxKeyLen - kMaxKeyVendorProduct;
    char * prefsKey = IONew(char, kMaxKeyLen);

    if (prefsKey)
    {
        bool ok = false;
        OSObject * obj;
        OSData * data;
        if ((obj = copyProperty("AAPL,display-alias", gIOServicePlane)))
        {
            ok = (data = OSDynamicCast(OSData, obj));
            if (ok)
                pathLen = snprintf(prefsKey, kMaxKeyLen, "Alias:%d/%s",
                                ((uint32_t *) data->getBytesNoCopy())[0], getName());
            obj->release();
        }
        if (!ok)
            ok = getPath(prefsKey, &pathLen, gIOServicePlane);
        if (ok)
        {
            snprintf(prefsKey + pathLen, kMaxKeyLen - pathLen, "-%x-%x", (int) vendor, (int) product);
            const OSSymbol * sym = OSSymbol::withCString(prefsKey);
            if (sym)
            {
                setProperty(kIODisplayPrefKeyKey, (OSObject *) sym);
                sym->release();
            }
        }
        IODelete(prefsKey, char, kMaxKeyLen);
    }

    OSNumber * num;
    if ((num = OSDynamicCast(OSNumber, framebuffer->getProperty(kIOFBTransformKey))))
    {
        if ((kIOScaleSwapAxes | kIOFBSwapAxes) & num->unsigned32BitValue())
            setName("AppleDisplay-Portrait");
    }

    // display parameter hooks

    IODisplayParameterHandler * parameterHandler;

	searchParameterHandlers(framebuffer);

    if (OSDynamicCast(IOBacklightDisplay, this))
    {
        OSDictionary * matching = nameMatching("backlight");
        OSIterator *   iter = NULL;
        IOService  *   look;
        if (matching)
        {
            iter = getMatchingServices(matching);
            matching->release();
        }
        if (iter)
        {
            look = OSDynamicCast(IOService, iter->getNextObject());
            if (look) searchParameterHandlers(look);
            iter->release();
        }
    }

    if ((parameterHandler = OSDynamicCast(IODisplayParameterHandler,
                                            framebuffer->getProperty(gIODisplayParametersKey))))
    {
        addParameterHandler(parameterHandler);
    }

    doUpdate();

    // initialize power management of the display

    fDisplayPMVars = IONew(IODisplayPMVars, 1);
    assert( fDisplayPMVars );
    bzero(fDisplayPMVars, sizeof(IODisplayPMVars));

    fDisplayPMVars->maxState = kIODisplayMaxPowerState;
    fDisplayPMVars->currentState = kIODisplayMaxPowerState;

    initPowerManagement( provider );

	uint32_t options = 0;
	if (NULL != OSDynamicCast(IOBacklightDisplay, this))
		options |= kIODisplayOptionBacklight;
	if (fDisplayPMVars->minDimState)
		options |= kIODisplayOptionDimDisable;

    framebuffer->displayOnline(this, +1, options);

    fNotifier = framebuffer->addFramebufferNotification(
                    &IODisplay::_framebufferEvent, this, NULL );

    registerService();

    return (true);
}

bool IODisplay::addParameterHandler( IODisplayParameterHandler * parameterHandler )
{
    OSArray * array;
    array = OSDynamicCast(OSArray, fParameterHandler);

    if (array && ((unsigned int) -1 != array->getNextIndexOfObject(parameterHandler, 0)))
        return (true);

    if (!parameterHandler->setDisplay(this))
        return (false);

    if (!array)
    {
        array = OSArray::withCapacity(2);
        if (!array)
            return (false);
       fParameterHandler = (IODisplayParameterHandler *) array;
    }

    array->setObject(parameterHandler);

    return (true);
}

bool IODisplay::removeParameterHandler( IODisplayParameterHandler * parameterHandler )
{
    OSArray * array;

    if (parameterHandler == fParameterHandler)
    {
        fParameterHandler->release();
        fParameterHandler = 0;
        return (true);
    }

    array = OSDynamicCast(OSArray, fParameterHandler);
    if (array)
    {
        unsigned int idx = array->getNextIndexOfObject(parameterHandler, 0);
        if (idx != (unsigned int)-1)
        {
            array->removeObject(idx);
            return (true);
        }
    }
    return (false);
}

void IODisplay::stop( IOService * provider )
{
    if (fConnection)
    {
        fConnection->getFramebuffer()->displayOnline(this, -1, 0);
        fConnection = 0;
    }

    IODisplayUpdateNVRAM(this, 0);

    if ( initialized )
        PMstop();
    if (fNotifier)
    {
        fNotifier->remove();
        fNotifier = 0;
    }

	removeProperty(gIODisplayParametersKey);
}

void IODisplay::free()
{
    if (fParameterHandler)
    {
        fParameterHandler->release();
        fParameterHandler = 0;
    }
    super::free();
}

IOReturn IODisplay::readFramebufferEDID( void )
{
    IOReturn            err;
    IOFramebuffer *     framebuffer;
    OSData *            data;
    IOByteCount         length;
    EDID                readEDID;
    UInt8               edidBlock[128];
    UInt32              index;
    UInt32              numExts;

    assert( fConnection );
    framebuffer = fConnection->getFramebuffer();
    assert( framebuffer );

    do
    {
        err = fConnection->getAttributeForConnection(
                  kConnectionSupportsHLDDCSense, NULL );
        if (err)
            continue;

        if (!framebuffer->hasDDCConnect(fConnection->getConnection()))
        {
            err = kIOReturnUnsupported;
            continue;
        }
        length = sizeof( EDID);
        err = framebuffer->getDDCBlock( fConnection->getConnection(),
                                        1, kIODDCBlockTypeEDID, 0, (UInt8 *) &readEDID, &length );
        if (err || (length != sizeof(EDID)))
            continue;


        data = OSData::withBytes( &readEDID, sizeof( EDID ));
        if (!data)
            continue;

        numExts = readEDID.extension;
        for (index = 2; index < (2 + numExts); index++)
        {
            length = sizeof(EDID);
            err = framebuffer->getDDCBlock( fConnection->getConnection(),
                                            index, kIODDCBlockTypeEDID, 0, edidBlock, &length );
            if (err || (length != sizeof(EDID)))
                break;
            if (0 == bcmp(edidBlock, &readEDID, sizeof(EDID)))
                break;
            if (!data->appendBytes(edidBlock, sizeof(EDID)))
                break;
        }

        setProperty( kIODisplayEDIDKey, data );
        data->release();
    }
    while (false);

    return (err);
}

IOReturn IODisplay::getConnectFlagsForDisplayMode(
    IODisplayModeID mode, UInt32 * flags )
{
    IOReturn            err;
    IOFramebuffer *     framebuffer;

    assert( fConnection );
    framebuffer = fConnection->getFramebuffer();
    assert( framebuffer );

    err = framebuffer->connectFlags(
              fConnection->getConnection(),
              mode, flags );

    if (kIOReturnUnsupported == err)
    {
        *flags = kDisplayModeValidFlag | kDisplayModeSafeFlag;
        err = kIOReturnSuccess;
    }

    return (err);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

OSDictionary * IODisplay::getIntegerRange( OSDictionary * params,
        const OSSymbol * sym, SInt32 * value, SInt32 * min, SInt32 * max )
{
    OSNumber *          num;

    params = OSDynamicCast( OSDictionary, params->getObject( sym ));

    if (params)
    {
        do
        {
            if (value)
            {
                num = OSDynamicCast( OSNumber, params->getObject(gIODisplayValueKey));
                if (!num)
                    continue;
                *value = num->unsigned32BitValue();
            }
            if (min)
            {
                num = OSDynamicCast( OSNumber, params->getObject(gIODisplayMinValueKey));
                if (!num)
                    continue;
                *min = num->unsigned32BitValue();
            }
            if (max)
            {
                num = OSDynamicCast( OSNumber, params->getObject(gIODisplayMaxValueKey));
                if (!num)
                    continue;
                *max = num->unsigned32BitValue();
            }
            return (params);
        }
        while (false);
    }

    return (false);
}

bool IODisplay::setForKey( OSDictionary * params, const OSSymbol * sym,
                           SInt32 value, SInt32 min, SInt32 max )
{
    SInt32 adjValue;
    bool ok;

    // invert rotation
    if (sym == gIODisplayRotationKey)
        adjValue = max - value + min;
    else
        adjValue = value;

    if ((ok = doIntegerSet(params, sym, adjValue)))
        updateNumber(params, gIODisplayValueKey, value);

    return (ok);
}

IOReturn IODisplay::setProperties( OSObject * properties )
{
    IOReturn                    err = kIOReturnSuccess;
    OSDictionary *              dict;
    OSDictionary *              dict2;
    OSSymbol *                  sym;
    IODisplayParameterHandler * parameterHandler;
    OSArray *                   array;
    OSDictionary *              displayParams;
    OSDictionary *              params;
    OSNumber *                  valueNum;
    OSObject *                  valueObj;
    OSDictionary *              valueDict;
    OSIterator *                iter;
    SInt32                      min, max, value;
    bool                        doCommit = false;
    bool                        allOK = true;
    bool                        ok;

    IOFramebuffer *             framebuffer = NULL;

    if (isInactive())
        return (kIOReturnNotReady);

    if (fConnection)
        framebuffer = fConnection->getFramebuffer();
    if (!framebuffer)
        return (kIOReturnNotReady);

    framebuffer->fbLock();

    parameterHandler = OSDynamicCast(IODisplayParameterHandler, fParameterHandler);
    if (parameterHandler)
    {
        err = parameterHandler->setProperties( properties );
        if (kIOReturnUnsupported == err)
            err = kIOReturnSuccess;
    }
    else if ((array = OSDynamicCast(OSArray, fParameterHandler)))
    {
        for (unsigned int i = 0;
            (parameterHandler = OSDynamicCast(IODisplayParameterHandler, array->getObject(i)));
            i++)
        {
            err = parameterHandler->setProperties( properties );
            if (kIOReturnUnsupported == err)
                err = kIOReturnSuccess;
        }
    }

    dict = OSDynamicCast(OSDictionary, properties);
    if (!dict || !(displayParams = OSDynamicCast(OSDictionary, copyProperty(gIODisplayParametersKey))))
    {
        framebuffer->fbUnlock();
        return (kIOReturnUnsupported);
    }

    dict2 = OSDynamicCast(OSDictionary, dict->getObject(gIODisplayParametersKey));
    if (dict2)
        dict = dict2;

    if ((properties != displayParams) && dict->getObject(gIODisplayParametersDefaultKey))
    {
        params = OSDynamicCast( OSDictionary,
                                displayParams->getObject(gIODisplayParametersDefaultKey));
        doIntegerSet( params, gIODisplayParametersDefaultKey, 0 );
        doUpdate();
        setProperties( displayParams );
    }

    iter = OSCollectionIterator::withCollection( dict );
    if (iter)
    {
        OSSymbol * doLast = 0;

        for (; ; allOK &= ok)
        {
            sym = (OSSymbol *) iter->getNextObject();
            if (!sym)
            {
                if (doLast)
                {
                    sym = doLast;
                    doLast = 0;
                }
                else
                    break;
            }
            else if (sym == gIODisplayVideoBestKey)
            {
                doLast = sym;
                ok = true;
                continue;
            }

            if (sym == gIODisplayParametersCommitKey)
            {
                if (properties != displayParams)
                    doCommit = true;
                ok = true;
                continue;
            }
            if (sym == gIODisplayParametersDefaultKey)
            {
                ok = true;
                continue;
            }

            OSData * valueData = OSDynamicCast( OSData, dict->getObject(sym) );
            if (valueData)
            {
                ok = doDataSet( sym, valueData );
                continue;
            }

            ok = false;
            if (0 == (params = getIntegerRange(displayParams, sym, 0, &min, &max)))
                continue;

            valueObj = dict->getObject(sym);
            if (!valueObj)
                continue;
            if ((valueDict = OSDynamicCast(OSDictionary, valueObj)))
                valueObj = valueDict->getObject( gIODisplayValueKey );
            valueNum = OSDynamicCast( OSNumber, valueObj );
            if (!valueNum)
                continue;
            value = valueNum->unsigned32BitValue();

            if (value < min) value = min;
            if (value > max) value = max;

			if (kIOGDbgForceBrightness & gIOGDebugFlags)
			{
				if (sym == gIODisplayLinearBrightnessKey) continue;
				if (sym == gIODisplayBrightnessKey)
				{
					value = (((max - min) * 3) / 4 + min);
					updateNumber(params, gIODisplayValueKey, value);
				}
			}

            ok = setForKey( params, sym, value, min, max );
        }
        iter->release();
    }

    if (doCommit)
        doIntegerSet( OSDynamicCast( OSDictionary, displayParams->getObject(gIODisplayParametersCommitKey)),
                      gIODisplayParametersCommitKey, 0 );

    doIntegerSet( OSDynamicCast( OSDictionary, displayParams->getObject(gIODisplayParametersFlushKey)),
                  gIODisplayParametersFlushKey, 0 );

    framebuffer->fbUnlock();

    displayParams->release();

    return (allOK ? err : kIOReturnError);
}

bool IODisplay::updateNumber( OSDictionary * params, const OSSymbol * key,
                              SInt32 value )
{
    OSNumber * num;

    if ((OSCollection::kImmutable & params->setOptions(0, 0)) 
		&& (num = (OSNumber *) params->getObject(key)))
    {
        num->setValue(value);
	}
    else
    {
        num = OSNumber::withNumber( value, 32 );
        if (num)
        {
            params->setObject( key, num );
            num->release();
        }
    }
    return (num != 0);
}

bool IODisplay::addParameter( OSDictionary * params, const OSSymbol * paramName,
                              SInt32 min, SInt32 max )
{
    OSDictionary *      paramDict;
    bool                ok = true;

    paramDict = (OSDictionary *) params->getObject(paramName);
    if (!paramDict)
    {
        paramDict = OSDictionary::withCapacity(3);
        if (!paramDict)
            return (false);
        params->setObject(paramName, paramDict);
        paramDict->release();
    }

    paramDict->setCapacityIncrement(1);

    updateNumber(paramDict, gIODisplayMinValueKey, min);
    updateNumber(paramDict, gIODisplayMaxValueKey, max);
    if (!paramDict->getObject(gIODisplayValueKey))
        updateNumber(paramDict, gIODisplayValueKey, min);

    return (ok);
}

bool IODisplay::setParameter( OSDictionary * params, const OSSymbol * paramName,
                              SInt32 value )
{
    OSDictionary *      paramDict;
    bool                ok = true;

    paramDict = (OSDictionary *) params->getObject(paramName);
    if (!paramDict)
        return (false);

    // invert rotation
    if (paramName == gIODisplayRotationKey)
    {
        SInt32 min, max;
        getIntegerRange( params, paramName, NULL, &min, &max );
        value = max - value + min;
    }

    updateNumber( paramDict, gIODisplayValueKey, value );

    return (ok);
}

IOReturn IODisplay::_framebufferEvent( OSObject * _self, void * ref,
                                       IOFramebuffer * framebuffer, IOIndex event, void * info )
{
    IODisplay * self = (IODisplay *) _self;

    return (self->framebufferEvent(framebuffer , event, info));
}

IOReturn IODisplay::framebufferEvent( IOFramebuffer * framebuffer,
                                      IOIndex event, void * info )
{
    IOReturn       err;
    OSDictionary * displayParams;

    switch (event)
    {
        case kIOFBNotifyDisplayModeDidChange:

            displayParams = OSDynamicCast(OSDictionary, copyProperty(gIODisplayParametersKey));
            if (doUpdate() && displayParams)
                setProperties(displayParams);
            if (displayParams)
                displayParams->release();
            /* fall thru */

        default:
            err = kIOReturnSuccess;
            break;
    }

    return (err);
}

UInt32 gIODisplayFadeTime1;
UInt32 gIODisplayFadeTime2;
UInt32 gIODisplayFadeTime3;
UInt32 gIODisplayFadeStyle;

bool IODisplay::doIntegerSet( OSDictionary * params,
                              const OSSymbol * paramName, UInt32 value )
{
    IODisplayParameterHandler * parameterHandler;
    OSArray *                   array;
    bool                        ok = false;

    if (gIODisplayFadeTime1Key == paramName)
    {
    	gIODisplayFadeTime1 = value;
        return (true);
    }
    if (gIODisplayFadeTime2Key == paramName)
    {
    	gIODisplayFadeTime2 = value;
        return (true);
    }
    if (gIODisplayFadeTime3Key == paramName)
    {
    	gIODisplayFadeTime3 = value;
        return (true);
    }
    if (gIODisplayFadeStyleKey == paramName)
    {
    	gIODisplayFadeStyle = value;
        return (true);
    }

    parameterHandler = OSDynamicCast(IODisplayParameterHandler, fParameterHandler);

    if (parameterHandler)
        ok = parameterHandler->doIntegerSet(params, paramName, value);

    else if ((array = OSDynamicCast(OSArray, fParameterHandler)))
    {
        for (unsigned int i = 0;
            !ok && (parameterHandler = OSDynamicCast(IODisplayParameterHandler, array->getObject(i)));
            i++)
        {
            ok = parameterHandler->doIntegerSet(params, paramName, value);
        }
    }

    return (ok);
}

bool IODisplay::doDataSet( const OSSymbol * paramName, OSData * value )
{
    IODisplayParameterHandler * parameterHandler;
    OSArray *                   array;
    bool                        ok = false;

    parameterHandler = OSDynamicCast(IODisplayParameterHandler, fParameterHandler);

    if (parameterHandler)
        ok = parameterHandler->doDataSet(paramName, value);

    else if ((array = OSDynamicCast(OSArray, fParameterHandler)))
    {
        for (unsigned int i = 0;
            !ok && (parameterHandler = OSDynamicCast(IODisplayParameterHandler, array->getObject(i)));
            i++)
        {
            ok = parameterHandler->doDataSet(paramName, value);
        }
    }

    return (ok);
}

bool IODisplay::doUpdate( void )
{
    IODisplayParameterHandler * parameterHandler;
    OSArray *                   array;
    bool                        ok = true;

    parameterHandler = OSDynamicCast(IODisplayParameterHandler, fParameterHandler);

    if (parameterHandler)
        ok = parameterHandler->doUpdate();

    else if ((array = OSDynamicCast(OSArray, fParameterHandler)))
    {
        for (unsigned int i = 0;
            (parameterHandler = OSDynamicCast(IODisplayParameterHandler, array->getObject(i)));
            i++)
        {
            ok &= parameterHandler->doUpdate();
        }
    }

    return (ok);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
    This is the power-controlling driver for a display. It also acts as an
    agent of the policy-maker for display power which is the DisplayWrangler.
    The Display Wrangler calls here to lower power by one state when it senses
    no user activity.  It also calls here to make the display usable after it
    has been idled down, and it also calls here to make the display barely
    usable if it senses a power emergency (e.g. low battery).
    
    This driver assumes a video display, and it calls the framebuffer driver
    to control the sync signals.  Non-video display drivers (e.g. flat panels)
    subclass IODisplay and override this and other appropriate methods.
 */

void IODisplay::initPowerManagement( IOService * provider )
{
    static const IOPMPowerState defaultPowerStates[kIODisplayNumPowerStates] = {
        // version,
        // capabilityFlags, outputPowerCharacter, inputPowerRequirement,
        { 1, 0,                                     0, 0,           0,0,0,0,0,0,0,0 },
        { 1, 0,                                     0, 0,           0,0,0,0,0,0,0,0 },
        { 1, IOPMDeviceUsable,                      0, kIOPMPowerOn, 0,0,0,0,0,0,0,0 },
        { 1, IOPMDeviceUsable | IOPMMaxPerformance, 0, kIOPMPowerOn, 0,0,0,0,0,0,0,0 }
        // staticPower, unbudgetedPower, powerToAttain, timeToAttain, settleUpTime,
        // timeToLower, settleDownTime, powerDomainBudget
    };
    IOPMPowerState ourPowerStates[kIODisplayNumPowerStates];

    OSDictionary * displayParams;
    SInt32         value, min, max;

    bcopy(defaultPowerStates, ourPowerStates, sizeof(ourPowerStates));
    displayParams = OSDynamicCast(OSDictionary, copyProperty(gIODisplayParametersKey));
    if (displayParams)
    {
        if (getIntegerRange(displayParams, gIODisplayAudioMuteAndScreenBlankKey,
                             &value, &min, &max))
        {
            if ((max >= kIODisplayBlankValue)
                || (kIOGDbgK59Mode & gIOGDebugFlags))
            {
                fDisplayPMVars->minDimState = 1;
            }
        }
        displayParams->release();
    }
    fDisplayPMVars->currentState = kIODisplayMaxPowerState;

    // initialize superclass variables
    PMinit();
    // attach into the power management hierarchy
    provider->joinPMtree(this);

    // register ourselves with policy-maker (us)
    registerPowerDriver(this, ourPowerStates, kIODisplayNumPowerStates);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// setDisplayPowerState
//
// Called by the display wrangler when it decides there hasn't been user
// activity for a while.  We drop one power level.  This can be called by the
// display wrangler before we have been completely initialized, or:
// The DisplayWrangler has sensed user activity after we have idled the
// display and wants us to make it usable again.  We are running on its
// workloop thread.  This can be called before we are completely
// initialized.

void IODisplay::setDisplayPowerState(unsigned long state)
{
    if (initialized)
    {
        if (state)
        {
            state--;
            if (state < fDisplayPMVars->minDimState)
                state = fDisplayPMVars->minDimState;
        }
        fDisplayPMVars->displayIdle = (state != fDisplayPMVars->maxState);
        changePowerStateToPriv(state);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// obsolete
void IODisplay::dropOneLevel(void)		{}
void IODisplay::makeDisplayUsable(void)	{}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// setPowerState
//
// Called by the superclass to change the display power state.

IOReturn IODisplay::setPowerState( unsigned long powerState, IOService * whatDevice )
{
    OSDictionary * displayParams;

    if (fDisplayPMVars->minDimState)
    {
        displayParams = OSDynamicCast(OSDictionary, copyProperty(gIODisplayParametersKey));
        if (displayParams)
        {
            doIntegerSet(displayParams, gIODisplayAudioMuteAndScreenBlankKey, 
                                (powerState > fDisplayPMVars->minDimState) 
                                    ? kIODisplayUnblankValue : kIODisplayBlankValue);
            if (kIOGDbgK59Mode & gIOGDebugFlags)
            {
                doIntegerSet(displayParams, gIODisplayBrightnessKey, 
                                (powerState > fDisplayPMVars->minDimState) 
                                    ? 255 : 0);
            }
            displayParams->release();
        }
    }

    return (kIOReturnSuccess);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// maxCapabilityForDomainState
//
// This simple device needs only power.  If the power domain is supplying
// power, the display can go to its highest state.  If there is no power
// it can only be in its lowest state, which is off.

unsigned long IODisplay::maxCapabilityForDomainState( IOPMPowerFlags domainState )
{
    if (domainState & IOPMPowerOn)
        return (kIODisplayMaxPowerState);
    else
        return (0);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// initialPowerStateForDomainState
//
// The power domain may be changing state.  If power is on in the new
// state, that will not affect our state at all.  In that case return
// what our current state is.  If domain power is off, we can attain
// only our lowest state, which is off.

unsigned long IODisplay::initialPowerStateForDomainState( IOPMPowerFlags domainState )
{
    if (domainState & IOPMPowerOn)
        // domain has power
        return (kIODisplayMaxPowerState);
    else
        // domain is down, so display is off
        return (kIODisplayMaxPowerState);
    return (0);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// powerStateForDomainState
//
// The power domain may be changing state.  If power is on in the new
// state, that will not affect our state at all.  In that case ask the ndrv
// what our current state is.  If domain power is off, we can attain
// only our lowest state, which is off.

unsigned long IODisplay::powerStateForDomainState( IOPMPowerFlags domainState )
{
    if (domainState & IOPMPowerOn)
        // domain has power
        return (getPowerState());
    else
        // domain is down, so display is off
        return (0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class AppleDisplay : public IODisplay
{
    OSDeclareDefaultStructors(AppleDisplay)
};

#undef super
#define super IODisplay

OSDefineMetaClassAndStructors(AppleDisplay, IODisplay)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#undef super
#define super IOService

OSDefineMetaClass( IODisplayParameterHandler, IOService )
OSDefineAbstractStructors( IODisplayParameterHandler, IOService )

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

OSMetaClassDefineReservedUnused(IODisplay, 0);
OSMetaClassDefineReservedUnused(IODisplay, 1);
OSMetaClassDefineReservedUnused(IODisplay, 2);
OSMetaClassDefineReservedUnused(IODisplay, 3);
OSMetaClassDefineReservedUnused(IODisplay, 4);
OSMetaClassDefineReservedUnused(IODisplay, 5);
OSMetaClassDefineReservedUnused(IODisplay, 6);
OSMetaClassDefineReservedUnused(IODisplay, 7);
OSMetaClassDefineReservedUnused(IODisplay, 8);
OSMetaClassDefineReservedUnused(IODisplay, 9);
OSMetaClassDefineReservedUnused(IODisplay, 10);
OSMetaClassDefineReservedUnused(IODisplay, 11);
OSMetaClassDefineReservedUnused(IODisplay, 12);
OSMetaClassDefineReservedUnused(IODisplay, 13);
OSMetaClassDefineReservedUnused(IODisplay, 14);
OSMetaClassDefineReservedUnused(IODisplay, 15);
OSMetaClassDefineReservedUnused(IODisplay, 16);
OSMetaClassDefineReservedUnused(IODisplay, 17);
OSMetaClassDefineReservedUnused(IODisplay, 18);
OSMetaClassDefineReservedUnused(IODisplay, 19);

OSMetaClassDefineReservedUnused(IODisplayParameterHandler, 0);
OSMetaClassDefineReservedUnused(IODisplayParameterHandler, 1);
OSMetaClassDefineReservedUnused(IODisplayParameterHandler, 2);
OSMetaClassDefineReservedUnused(IODisplayParameterHandler, 3);
OSMetaClassDefineReservedUnused(IODisplayParameterHandler, 4);
OSMetaClassDefineReservedUnused(IODisplayParameterHandler, 5);
OSMetaClassDefineReservedUnused(IODisplayParameterHandler, 6);
OSMetaClassDefineReservedUnused(IODisplayParameterHandler, 7);
OSMetaClassDefineReservedUnused(IODisplayParameterHandler, 8);
OSMetaClassDefineReservedUnused(IODisplayParameterHandler, 9);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
