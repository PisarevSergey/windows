#include "pch.h"

#include <kcpp/scope_guard.h>

namespace {
    constexpr auto invalidGuid = GUID{ MAXULONG, MAXUSHORT, MAXUSHORT, {MAXUCHAR, MAXUCHAR, MAXUCHAR, MAXUCHAR, MAXUCHAR, MAXUCHAR, MAXUCHAR, MAXUCHAR } };

    GUID GetDeviceBus(WDFDEVICE device) {
        GUID busTypeGuid{};
        ULONG returnLength{};
        const auto status = WdfDeviceQueryProperty(device, DevicePropertyBusTypeGuid, sizeof(busTypeGuid), &busTypeGuid, &returnLength);
        if (!NT_SUCCESS(status))
        {
            TraceError("WdfDeviceQueryProperty failed to get DevicePropertyBusTypeGuid", TraceLoggingNTStatus(status));
            return invalidGuid;
        }

        return busTypeGuid;
    }

    GUID GetDeviceClass(WDFDEVICE device) {
        GUID deviceClass{};

        ULONG requiredSize{};
        DEVPROPTYPE type{};
        auto devPropertyData = kmdf::CreateDevicePropertyData(DEVPKEY_Device_ClassGuid);
        const auto status = WdfDeviceQueryPropertyEx(device, &devPropertyData, sizeof(deviceClass), &deviceClass, &requiredSize, &type);
        if (!NT_SUCCESS(status))
        {
            TraceError("WdfDeviceQueryPropertyEx failed to get DEVPKEY_Device_ClassGuid", TraceLoggingNTStatus(status));
            return invalidGuid;
        }

        NT_ASSERT(DEVPROP_TYPE_GUID == type);
        return deviceClass;
    }

    const char* UsbFunctionString(USHORT function)
    {
        switch (function) {
            case URB_FUNCTION_SELECT_CONFIGURATION                        : return "URB_FUNCTION_SELECT_CONFIGURATION";
            case URB_FUNCTION_SELECT_INTERFACE                            : return "URB_FUNCTION_SELECT_INTERFACE";
            case URB_FUNCTION_ABORT_PIPE                                  : return "URB_FUNCTION_ABORT_PIPE";
            case URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL                   : return "URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL";
            case URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL                : return "URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL";
            case URB_FUNCTION_GET_FRAME_LENGTH                            : return "URB_FUNCTION_GET_FRAME_LENGTH";
            case URB_FUNCTION_SET_FRAME_LENGTH                            : return "URB_FUNCTION_SET_FRAME_LENGTH";
            case URB_FUNCTION_GET_CURRENT_FRAME_NUMBER                    : return "URB_FUNCTION_GET_CURRENT_FRAME_NUMBER";
            case URB_FUNCTION_CONTROL_TRANSFER                            : return "URB_FUNCTION_CONTROL_TRANSFER";
            case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER                  : return "URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER";
            case URB_FUNCTION_ISOCH_TRANSFER                              : return "URB_FUNCTION_ISOCH_TRANSFER";
            case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE                  : return "URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE";
            case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE                    : return "URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE";
            case URB_FUNCTION_SET_FEATURE_TO_DEVICE                       : return "URB_FUNCTION_SET_FEATURE_TO_DEVICE";
            case URB_FUNCTION_SET_FEATURE_TO_INTERFACE                    : return "URB_FUNCTION_SET_FEATURE_TO_INTERFACE";
            case URB_FUNCTION_SET_FEATURE_TO_ENDPOINT                     : return "URB_FUNCTION_SET_FEATURE_TO_ENDPOINT";
            case URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE                     : return "URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE";
            case URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE                  : return "URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE";
            case URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT                   : return "URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT";
            case URB_FUNCTION_GET_STATUS_FROM_DEVICE                      : return "URB_FUNCTION_GET_STATUS_FROM_DEVICE";
            case URB_FUNCTION_GET_STATUS_FROM_INTERFACE                   : return "URB_FUNCTION_GET_STATUS_FROM_INTERFACE";
            case URB_FUNCTION_GET_STATUS_FROM_ENDPOINT                    : return "URB_FUNCTION_GET_STATUS_FROM_ENDPOINT";
            case URB_FUNCTION_RESERVED_0X0016                             : return "URB_FUNCTION_RESERVED_0X0016";
            case URB_FUNCTION_VENDOR_DEVICE                               : return "URB_FUNCTION_VENDOR_DEVICE";
            case URB_FUNCTION_VENDOR_INTERFACE                            : return "URB_FUNCTION_VENDOR_INTERFACE";
            case URB_FUNCTION_VENDOR_ENDPOINT                             : return "URB_FUNCTION_VENDOR_ENDPOINT";
            case URB_FUNCTION_CLASS_DEVICE                                : return "URB_FUNCTION_CLASS_DEVICE";
            case URB_FUNCTION_CLASS_INTERFACE                             : return "URB_FUNCTION_CLASS_INTERFACE";
            case URB_FUNCTION_CLASS_ENDPOINT                              : return "URB_FUNCTION_CLASS_ENDPOINT";
            case URB_FUNCTION_RESERVE_0X001D                              : return "URB_FUNCTION_RESERVE_0X001D";
            case URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL             : return "URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL";
            case URB_FUNCTION_CLASS_OTHER                                 : return "URB_FUNCTION_CLASS_OTHER";
            case URB_FUNCTION_VENDOR_OTHER                                : return "URB_FUNCTION_VENDOR_OTHER";
            case URB_FUNCTION_GET_STATUS_FROM_OTHER                       : return "URB_FUNCTION_GET_STATUS_FROM_OTHER";
            case URB_FUNCTION_CLEAR_FEATURE_TO_OTHER                      : return "URB_FUNCTION_CLEAR_FEATURE_TO_OTHER";
            case URB_FUNCTION_SET_FEATURE_TO_OTHER                        : return "URB_FUNCTION_SET_FEATURE_TO_OTHER";
            case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT                : return "URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT";
            case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT                  : return "URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT";
            case URB_FUNCTION_GET_CONFIGURATION                           : return "URB_FUNCTION_GET_CONFIGURATION";
            case URB_FUNCTION_GET_INTERFACE                               : return "URB_FUNCTION_GET_INTERFACE";
            case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE               : return "URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE";
            case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE                 : return "URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE";
            case URB_FUNCTION_RESERVE_0X002B                              : return "URB_FUNCTION_RESERVE_0X002B";
            case URB_FUNCTION_RESERVE_0X002C                              : return "URB_FUNCTION_RESERVE_0X002C";
            case URB_FUNCTION_RESERVE_0X002D                              : return "URB_FUNCTION_RESERVE_0X002D";
            case URB_FUNCTION_RESERVE_0X002E                              : return "URB_FUNCTION_RESERVE_0X002E";
            case URB_FUNCTION_RESERVE_0X002F                              : return "URB_FUNCTION_RESERVE_0X002F";
            case URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR                   : return "URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR";
            case URB_FUNCTION_SYNC_RESET_PIPE                             : return "URB_FUNCTION_SYNC_RESET_PIPE";
            case URB_FUNCTION_SYNC_CLEAR_STALL                            : return "URB_FUNCTION_SYNC_CLEAR_STALL";
            case URB_FUNCTION_CONTROL_TRANSFER_EX                         : return "URB_FUNCTION_CONTROL_TRANSFER_EX";
            case URB_FUNCTION_RESERVE_0X0033                              : return "URB_FUNCTION_RESERVE_0X0033";
            case URB_FUNCTION_RESERVE_0X0034                              : return "URB_FUNCTION_RESERVE_0X0034";
            case URB_FUNCTION_OPEN_STATIC_STREAMS                         : return "URB_FUNCTION_OPEN_STATIC_STREAMS";
            case URB_FUNCTION_CLOSE_STATIC_STREAMS                        : return "URB_FUNCTION_CLOSE_STATIC_STREAMS";
            case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL: return "URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL";
            case URB_FUNCTION_ISOCH_TRANSFER_USING_CHAINED_MDL            : return "URB_FUNCTION_ISOCH_TRANSFER_USING_CHAINED_MDL";
            case URB_FUNCTION_GET_ISOCH_PIPE_TRANSFER_PATH_DELAYS         : return "URB_FUNCTION_GET_ISOCH_PIPE_TRANSFER_PATH_DELAYS";
            default                                                       : return "UNKNOWN URB FUNCTION";
        }
    }

    void Report(const _URB_BULK_OR_INTERRUPT_TRANSFER& transfer)
    {
        const auto flags = transfer.TransferFlags;
        if (flags & USBD_TRANSFER_DIRECTION_IN) {
            TraceInfo("USBD_TRANSFER_DIRECTION_IN");
        }

        if (flags & USBD_TRANSFER_DIRECTION_OUT) {
            TraceInfo("USBD_TRANSFER_DIRECTION_OUT");
        }

        if (flags & USBD_SHORT_TRANSFER_OK) {
            TraceInfo("USBD_SHORT_TRANSFER_OK");
        }

        TraceInfo("transfer length", TraceLoggingValue(transfer.TransferBufferLength));
        TraceInfo("transfer buffer", TraceLoggingValue(transfer.TransferBuffer));
        TraceInfo("transfer MDL", TraceLoggingPointer(transfer.TransferBufferMDL));

        //if (transfer.TransferBuffer) {
        //    TraceInfo("sending buffer to HCI", TraceLoggingBinary(transfer.TransferBuffer, transfer.TransferBufferLength));
        //}
        //else if (transfer.TransferBufferMDL) {
        //    TraceInfo("need to decode MDL");
        //}
    }

    void Report(const _URB_HEADER& urbHeader) {
        TraceInfo("Length", TraceLoggingValue(urbHeader.Length));
        TraceInfo("URB function", TraceLoggingHexUInt16(urbHeader.Function), TraceLoggingValue(UsbFunctionString(urbHeader.Function)));

        switch (urbHeader.Function) {
        case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
            Report(reinterpret_cast<const _URB_BULK_OR_INTERRUPT_TRANSFER&>(urbHeader));
            break;
        }
    }

    void ForwardAndForget(WDFREQUEST request, WDFQUEUE queue) {
        auto device = WdfIoQueueGetDevice(queue);
        auto ioTarget = WdfDeviceGetIoTarget(device);
        const auto status = kmdf::ForwardAndForget(request, ioTarget);
        if (!NT_SUCCESS(status))
        {
            TraceError("Failed to forward request", TraceLoggingPointer(request), TraceLoggingNTStatus(status));
            WdfRequestComplete(request, status);
        }
    }

    void OnIoctl(WDFQUEUE queue,
        WDFREQUEST request,
        [[maybe_unused]] size_t OutputBufferLength,
        [[maybe_unused]] size_t InputBufferLength,
        ULONG IoControlCode) {
        auto forwardGuard = kcpp::scope_guard([request, queue] { ForwardAndForget(request, queue); });
        if (GetUsbFilterContext(WdfIoQueueGetDevice(queue))->IsPassthrough()) { return; }
        TraceInfo("ioctl", TraceLoggingHexULong(IoControlCode));
    }

    void OnInternalIoctl(WDFQUEUE queue,
        WDFREQUEST request,
        [[maybe_unused]] size_t OutputBufferLength,
        [[maybe_unused]] size_t InputBufferLength,
        ULONG code){

        auto forwardGuard = kcpp::scope_guard([request, queue] { ForwardAndForget(request, queue); });
        if (GetUsbFilterContext(WdfIoQueueGetDevice(queue))->IsPassthrough()) { return; }

        const auto requestParams = kmdf::GetRequestParameters(request);
        NT_ASSERT(requestParams.Type == WdfRequestTypeDeviceControlInternal);

        switch (code) {
        case IOCTL_INTERNAL_USB_SUBMIT_URB:
        {
            TraceInfo("IOCTL_INTERNAL_USB_SUBMIT_URB");
            const auto& urb = *static_cast<URB*>(requestParams.Parameters.Others.Arg1);
            Report(urb.UrbHeader);
        }
            break;
        case IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION:
            TraceInfo("IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION");
            break;
        default:
            TraceInfo("internal ioctl", TraceLoggingHexULong(code));
        }
    }

    void OnRead(WDFQUEUE queue,
        WDFREQUEST request,
        size_t length
    ) {
        auto forwardGuard = kcpp::scope_guard([request, queue] { ForwardAndForget(request, queue); });
        if (GetUsbFilterContext(WdfIoQueueGetDevice(queue))->IsPassthrough()) { return; }
        TraceInfo("read", TraceLoggingValue(length));
    }

    void OnWrite(WDFQUEUE queue,
        WDFREQUEST request,
        size_t length) {
        auto forwardGuard = kcpp::scope_guard([request, queue] { ForwardAndForget(request, queue); });
        if (GetUsbFilterContext(WdfIoQueueGetDevice(queue))->IsPassthrough()) { return; }
        TraceInfo("write", TraceLoggingValue(length));
    }
}

void FilterDeviceContextCleanup(WDFOBJECT filterDevice) {
    auto context = GetUsbFilterContext(filterDevice);

    if (context->Flink && context->Blink) { // context was inserted in global list
        g_driver.Remove(*context);
    }

    context->~UsbFilterDeviceContext();
}

NTSTATUS AddDevice([[maybe_unused]] WDFDRIVER driver, PWDFDEVICE_INIT deviceInit) {
    WdfFdoInitSetFilter(deviceInit);

    auto filterDeviceAttributes = kmdf::CreateObjectAttributes(nullptr, FilterDeviceContextCleanup);
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&filterDeviceAttributes, UsbFilterDeviceContext);

    WDFDEVICE filterDevice{};
    auto status = WdfDeviceCreate(&deviceInit, &filterDeviceAttributes, &filterDevice);
    if (STATUS_SUCCESS != status) {
        TraceError("WdfDeviceCreate failed to create filter device", TraceLoggingNTStatus(status));
        return status;
    }

    auto filterContext = new(GetUsbFilterContext(filterDevice)) UsbFilterDeviceContext;

    const auto deviceClass = GetDeviceClass(filterDevice);
    TraceVerbose("attaching ?; device class", TraceLoggingValue(deviceClass));

    if (deviceClass != GUID_DEVCLASS_USB) { // do not attach to anything other than USB
        TraceInfo("device class is not USB, won't attach", TraceLoggingValue(deviceClass));

        if ((deviceClass == GUID_DEVCLASS_BLUETOOTH) &&
            (GetDeviceBus(filterDevice) == GUID_BUS_TYPE_USB)) { // root BTH device
            TraceInfo("root BTH device");

            auto parentIdMemory = kmdf::AllocAndQueryDeviceProperty(status, filterDevice, DEVPKEY_Device_Parent, filterDevice);
            if (!NT_SUCCESS(status)) {
                TraceError("kmdf::AllocAndQueryDeviceProperty failed to get DEVPKEY_Device_Parent", TraceLoggingNTStatus(status));
                return status;
            }

            auto parentId = helpers::ToStringView(status, parentIdMemory);
            if (!NT_SUCCESS(status)) {
                TraceError("failed to convert parent id to string", TraceLoggingNTStatus(status));
                return status;
            }
            TraceInfo("ParentId", TraceLoggingUnicodeString(&parentId));

            g_driver.StartFilterUsbDevice(parentId);
        }

        return STATUS_FLT_DO_NOT_ATTACH;
    }

    status = filterContext->Init(filterDevice);
    if (!NT_SUCCESS(status))
    {
        TraceError("SetInstanceId failed", TraceLoggingNTStatus(status));
        return status;
    }
    TraceInfo("Instance ID", TraceLoggingUnicodeString(&filterContext->GetInstanceId()));
    g_driver.Insert(*filterContext);

    auto queueConfig = kmdf::CreateQueueConfig();
    queueConfig.EvtIoDeviceControl = OnIoctl;
    queueConfig.EvtIoInternalDeviceControl = OnInternalIoctl;
    queueConfig.EvtIoRead = OnRead;
    queueConfig.EvtIoWrite = OnWrite;
    status = WdfIoQueueCreate(filterDevice, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);
    if (STATUS_SUCCESS != status)
    {
        TraceError("WdfIoQueueCreate failed", TraceLoggingNTStatus(status));
        return status;
    }

    return STATUS_SUCCESS;
}
