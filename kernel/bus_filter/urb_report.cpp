#include "pch.h"

namespace {
    inline bool flag_on(const ULONG value, const ULONG flag) {
        return ((value & flag) == flag);
    }

    const char* UrbFunctionToString(USHORT function) {

        switch (function) {
            case URB_FUNCTION_SELECT_CONFIGURATION: return "URB_FUNCTION_SELECT_CONFIGURATION";
            case URB_FUNCTION_SELECT_INTERFACE: return "URB_FUNCTION_SELECT_INTERFACE";
            case URB_FUNCTION_ABORT_PIPE: return "URB_FUNCTION_ABORT_PIPE";
            case URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL: return "URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL";
            case URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL: return "URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL";
            case URB_FUNCTION_GET_FRAME_LENGTH: return "URB_FUNCTION_GET_FRAME_LENGTH";
            case URB_FUNCTION_SET_FRAME_LENGTH: return "URB_FUNCTION_SET_FRAME_LENGTH";
            case URB_FUNCTION_GET_CURRENT_FRAME_NUMBER: return "URB_FUNCTION_GET_CURRENT_FRAME_NUMBER";
            case URB_FUNCTION_CONTROL_TRANSFER: return "URB_FUNCTION_CONTROL_TRANSFER";
            case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER: return "URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER";
            case URB_FUNCTION_ISOCH_TRANSFER: return "URB_FUNCTION_ISOCH_TRANSFER";
            case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE: return "URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE";
            case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE: return "URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE";
            case URB_FUNCTION_SET_FEATURE_TO_DEVICE: return "URB_FUNCTION_SET_FEATURE_TO_DEVICE";
            case URB_FUNCTION_SET_FEATURE_TO_INTERFACE: return "URB_FUNCTION_SET_FEATURE_TO_INTERFACE";
            case URB_FUNCTION_SET_FEATURE_TO_ENDPOINT: return "URB_FUNCTION_SET_FEATURE_TO_ENDPOINT";
            case URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE: return "URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE";
            case URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE: return "URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE";
            case URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT: return "URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT";
            case URB_FUNCTION_GET_STATUS_FROM_DEVICE: return "URB_FUNCTION_GET_STATUS_FROM_DEVICE";
            case URB_FUNCTION_GET_STATUS_FROM_INTERFACE: return "URB_FUNCTION_GET_STATUS_FROM_INTERFACE";
            case URB_FUNCTION_GET_STATUS_FROM_ENDPOINT: return "URB_FUNCTION_GET_STATUS_FROM_ENDPOINT";
            case URB_FUNCTION_RESERVED_0X0016: return "URB_FUNCTION_RESERVED_0X0016";
            case URB_FUNCTION_VENDOR_DEVICE: return "URB_FUNCTION_VENDOR_DEVICE";
            case URB_FUNCTION_VENDOR_INTERFACE: return "URB_FUNCTION_VENDOR_INTERFACE";
            case URB_FUNCTION_VENDOR_ENDPOINT: return "URB_FUNCTION_VENDOR_ENDPOINT";
            case URB_FUNCTION_CLASS_DEVICE: return "URB_FUNCTION_CLASS_DEVICE";
            case URB_FUNCTION_CLASS_INTERFACE: return "URB_FUNCTION_CLASS_INTERFACE";
            case URB_FUNCTION_CLASS_ENDPOINT: return "URB_FUNCTION_CLASS_ENDPOINT";
            case URB_FUNCTION_RESERVE_0X001D: return "URB_FUNCTION_RESERVE_0X001D";
            case URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL: return "URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL";
            case URB_FUNCTION_CLASS_OTHER: return "URB_FUNCTION_CLASS_OTHER";
            case URB_FUNCTION_VENDOR_OTHER: return "URB_FUNCTION_VENDOR_OTHER";
            case URB_FUNCTION_GET_STATUS_FROM_OTHER: return "URB_FUNCTION_GET_STATUS_FROM_OTHER";
            case URB_FUNCTION_CLEAR_FEATURE_TO_OTHER: return "URB_FUNCTION_CLEAR_FEATURE_TO_OTHER";
            case URB_FUNCTION_SET_FEATURE_TO_OTHER: return "URB_FUNCTION_SET_FEATURE_TO_OTHER";
            case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT: return "URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT";
            case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT: return "URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT";
            case URB_FUNCTION_GET_CONFIGURATION: return "URB_FUNCTION_GET_CONFIGURATION";
            case URB_FUNCTION_GET_INTERFACE: return "URB_FUNCTION_GET_INTERFACE";
            case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE: return "URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE";
            case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE: return "URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE";
            case URB_FUNCTION_RESERVE_0X002B: return "URB_FUNCTION_RESERVE_0X002B";
            case URB_FUNCTION_RESERVE_0X002C: return "URB_FUNCTION_RESERVE_0X002C";
            case URB_FUNCTION_RESERVE_0X002D: return "URB_FUNCTION_RESERVE_0X002D";
            case URB_FUNCTION_RESERVE_0X002E: return "URB_FUNCTION_RESERVE_0X002E";
            case URB_FUNCTION_RESERVE_0X002F: return "URB_FUNCTION_RESERVE_0X002F";
            case URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR: return "URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR";
            case URB_FUNCTION_SYNC_RESET_PIPE: return "URB_FUNCTION_SYNC_RESET_PIPE";
            case URB_FUNCTION_SYNC_CLEAR_STALL: return "URB_FUNCTION_SYNC_CLEAR_STALL";
            case URB_FUNCTION_CONTROL_TRANSFER_EX: return "URB_FUNCTION_CONTROL_TRANSFER_EX";
            case URB_FUNCTION_RESERVE_0X0033: return "URB_FUNCTION_RESERVE_0X0033";
            case URB_FUNCTION_RESERVE_0X0034: return "URB_FUNCTION_RESERVE_0X0034";
            case URB_FUNCTION_OPEN_STATIC_STREAMS: return "URB_FUNCTION_OPEN_STATIC_STREAMS";
            case URB_FUNCTION_CLOSE_STATIC_STREAMS: return "URB_FUNCTION_CLOSE_STATIC_STREAMS";
            case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL: return "URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL";
            case URB_FUNCTION_ISOCH_TRANSFER_USING_CHAINED_MDL: return "URB_FUNCTION_ISOCH_TRANSFER_USING_CHAINED_MDL";
            case URB_FUNCTION_GET_ISOCH_PIPE_TRANSFER_PATH_DELAYS: return "URB_FUNCTION_GET_ISOCH_PIPE_TRANSFER_PATH_DELAYS";
            default: return "unknown URB function";
        }

    }

    void Report(const _URB_BULK_OR_INTERRUPT_TRANSFER& urb) {
        const auto transferFlags = urb.TransferFlags;

        if (flag_on(transferFlags, USBD_TRANSFER_DIRECTION_IN)) {
            TraceInfo("USBD_TRANSFER_DIRECTION_IN");
        }
        else {
            TraceInfo("USBD_TRANSFER_DIRECTION_OUT");
        }
    }

    void Report(const _URB_CONTROL_TRANSFER& urb) {
        const auto transferFlags = urb.TransferFlags;

        if (flag_on(transferFlags,USBD_TRANSFER_DIRECTION_IN)) {
            TraceInfo("USBD_TRANSFER_DIRECTION_IN");
        }
        else {
            TraceInfo("USBD_TRANSFER_DIRECTION_OUT");
        }

        if (flag_on(transferFlags, USBD_DEFAULT_PIPE_TRANSFER)) {
            TraceInfo("USBD_DEFAULT_PIPE_TRANSFER");
        }

        TraceInfo("Pipe handle", TraceLoggingValue(urb.PipeHandle));
        TraceInfo("Transfer Buffer Length", TraceLoggingValue(urb.TransferBufferLength));
        TraceInfo("Transfer buffer", TraceLoggingValue(urb.TransferBuffer));
        TraceInfo("Trasfer buffer MDL", TraceLoggingPointer(urb.TransferBufferMDL));
        TraceInfo("USB request setup packet", TraceLoggingBinary(urb.SetupPacket, sizeof(urb.SetupPacket)));

        if (urb.TransferBufferLength && urb.TransferBuffer)
        {
            TraceInfo("Control Transfer Buffer", TraceLoggingBinary(urb.TransferBuffer, urb.TransferBufferLength));
        }
    }
}

void Report(const URB& urb)
{
    TraceInfo("URB Function", TraceLoggingString(UrbFunctionToString(urb.UrbHeader.Function)));

    switch (urb.UrbHeader.Function) {
    case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
        Report(reinterpret_cast<const _URB_BULK_OR_INTERRUPT_TRANSFER&>(urb));
        break;
    case URB_FUNCTION_CONTROL_TRANSFER:
        Report(reinterpret_cast<const _URB_CONTROL_TRANSFER&>(urb));
        break;
    }
}
