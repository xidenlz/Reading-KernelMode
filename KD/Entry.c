#include <ntddk.h>
#include <wdf.h>


DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD EvtDeviceAdd;
EVT_WDF_DEVICE_CONTEXT_CLEANUP EvtDeviceCleanup;
EVT_WDF_IO_QUEUE_IO_READ EvtIoRead;
EVT_WDF_IO_QUEUE_IO_WRITE EvtIoWrite;

// Entry function
NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    NTSTATUS status = STATUS_SUCCESS;
    WDF_DRIVER_CONFIG config;
    WDF_DRIVER_CONFIG_INIT(&config, EvtDeviceAdd);

    // Create driver object
    status = WdfDriverCreate(
        DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        WDF_NO_HANDLE
    );

    return status;
}

// EvtDeviceAdd function
NTSTATUS EvtDeviceAdd(
    _In_ WDFDRIVER Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
    NTSTATUS status = STATUS_SUCCESS;
    WDFDEVICE device;
    WDF_OBJECT_ATTRIBUTES attributes;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, DEVICE_CONTEXT);
    status = WdfDeviceCreate(&DeviceInit, &attributes, &device);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    // Configure I/O queue for the device
    WDF_IO_QUEUE_CONFIG queueConfig;
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchSequential);
    queueConfig.EvtIoRead = EvtIoRead;
    queueConfig.EvtIoWrite = EvtIoWrite;

    WDFQUEUE ioQueue;

    // Create the I/O queue
    status = WdfIoQueueCreate(
        device,
        &queueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        &ioQueue
    );

    // Perform device setup, initialization, and I/O queue creation if needed

    return status;
}

VOID EvtIoRead(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t Length
)
{
    NTSTATUS status = STATUS_SUCCESS;
    PVOID outputBuffer = NULL;
    size_t outputBufferLength;

    // Get a pointer to the output buffer and its length
    status = WdfRequestRetrieveOutputBuffer(Request, Length, &outputBuffer, &outputBufferLength);
    if (!NT_SUCCESS(status)) {
        WdfRequestCompleteWithInformation(Request, status, 0);
        return;

    WCHAR serialNumberBuffer[256];  
    // Call a function to read the actual serial number
    status = ReadSerialNumberFromHardware(serialNumberBuffer, sizeof(serialNumberBuffer));
    if (!NT_SUCCESS(status)) {
        WdfRequestCompleteWithInformation(Request, status, 0);
        return;
    }

    size_t serialNumberLength = wcslen(serialNumberBuffer) * sizeof(WCHAR);
    if (outputBufferLength >= serialNumberLength) {
        RtlCopyMemory(outputBuffer, serialNumberBuffer, serialNumberLength);
        WdfRequestCompleteWithInformation(Request, status, serialNumberLength);
    }
    else 
    {
        WdfRequestCompleteWithInformation(Request, STATUS_BUFFER_TOO_SMALL, 0);
    }
}
   // FUNCTION TO WRITE NEW VALUES "MODIFY THE CODE"
VOID EvtIoWrite(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t Length
)
{
    NTSTATUS status = STATUS_SUCCESS;
    PVOID inputBuffer = NULL;

    status = WdfRequestRetrieveInputBuffer(Request, Length, &inputBuffer, NULL);
    if (!NT_SUCCESS(status)) {
        WdfRequestCompleteWithInformation(Request, status, 0);
        return;
    }
    PWSTR inputData = (PWSTR)inputBuffer;

    KdPrint(("Received data from write request: %ws\n", inputData));
    WdfRequestCompleteWithInformation(Request, status, Length);
}
