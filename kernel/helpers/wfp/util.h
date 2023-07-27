#pragma once

#include <wdm.h>

#include <fwpsk.h>
#include <netioapi.h>

namespace wfp::util
{
    const UNICODE_STRING& AddressTypeToString(NL_ADDRESS_TYPE addressType);

    const UNICODE_STRING& DirectionToString(FWP_DIRECTION direction);
}
