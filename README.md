# Kernel-Mode Driver for Device Interaction
This repository contains a basic Windows kernel-mode driver built using the Windows Driver Framework (WDF) for handling read and write operations on a specific device. The driver is designed to interact with a hardware device, managing read requests to retrieve data (such as serial numbers) and write requests to update or modify device information.

# Key Features:
- Device Interaction: Supports read and write operations to and from the managed device.
- WDF Framework: Utilizes the Windows Driver Framework for robust and efficient driver development.
- Sample Read Operation: Demonstrates handling read requests by obtaining a serial number from hardware.
- Sample Write Operation: Handles write requests by printing received data.

## 1. Build Environment
Set up a Windows Kernel Mode Driver development environment using [Visual Studio 2022](https://visualstudio.microsoft.com/) and download the [Windows Driver Kit (WDK)](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk) to develop kernel drivers.

## 2. Device Integration
Customize `ReadSerialNumberFromHardware` in the code for your specific hardware. This function is responsible for reading data from the hardware device.

## 3. Enhancements
Extend the provided code to meet the specific requirements of your device. This may include additional functionality, improved error handling, or other customizations.

## Communicating with the Driver
This driver is designed to communicate with user-mode programs. To interact with the driver from any user-mode program, follow the instructions in the [km-um-communication](https://github.com/adspro15/km-um-communication) repository.

## Note:
This driver was created for educational purposes. It can be used to understand kernel-mode driver development and how to communicate with drivers from user-mode programs. You can utilize this driver with a C# program to interact with the device it manages.

Example 
```C#
using System;
using System.Runtime.InteropServices;

class Program
{
    // Define IOCTL codes for communication with the driver
    private const uint FILE_DEVICE_UNKNOWN = 0x00000022;
    private const uint METHOD_BUFFERED = 0;
    private const uint FILE_ANY_ACCESS = 0;

    // IOCTL codes for read and write operations
    private const uint IOCTL_READ = (FILE_DEVICE_UNKNOWN << 16) | (FILE_ANY_ACCESS << 14) | (0x800 << 2) | METHOD_BUFFERED;
    private const uint IOCTL_WRITE = (FILE_DEVICE_UNKNOWN << 16) | (FILE_ANY_ACCESS << 14) | (0x801 << 2) | METHOD_BUFFERED;

    // Path to the driver device
    private const string DevicePath = "\\\\.\\Driver.sys"; // Replace with the driver name

    // Import kernel32.dll for device interaction
    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr CreateFile(
        string lpFileName,
        uint dwDesiredAccess,
        uint dwShareMode,
        IntPtr lpSecurityAttributes,
        uint dwCreationDisposition,
        uint dwFlagsAndAttributes,
        IntPtr hTemplateFile
    );

    [DllImport("kernel32.dll", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool DeviceIoControl(
        IntPtr hDevice,
        uint dwIoControlCode,
        IntPtr lpInBuffer,
        uint nInBufferSize,
        IntPtr lpOutBuffer,
        uint nOutBufferSize,
        ref uint lpBytesReturned,
        IntPtr lpOverlapped
    );

    [DllImport("kernel32.dll", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool CloseHandle(IntPtr hObject);

    static void Main()
    {
        // Open a handle to the driver device
        IntPtr deviceHandle = CreateFile(DevicePath, 0xC0000000, 0, IntPtr.Zero, 3, 0, IntPtr.Zero);

        if (deviceHandle == IntPtr.Zero)
        {
            Console.WriteLine("Error opening device handle. Make sure the driver is loaded.");
            return;
        }

        // Example: Reading from the driver
        Console.WriteLine("Reading from the driver...");

        uint bytesReturned = 0;
        IntPtr outputBuffer = Marshal.AllocHGlobal(256); // Adjust buffer size accordingly

        // /* Error checking should be added in a real-world scenario */
        DeviceIoControl(deviceHandle, IOCTL_READ, IntPtr.Zero, 0, outputBuffer, 256, ref bytesReturned, IntPtr.Zero);

        string serialNumber = Marshal.PtrToStringUni(outputBuffer);
        Console.WriteLine($"Serial Number: {serialNumber}");

        // Example: Writing to the driver
        Console.WriteLine("Writing to the driver...");

        string dataToWrite = "NewData";
        IntPtr inputBuffer = Marshal.StringToHGlobalUni(dataToWrite);

        // /* Error checking should be added in a real-world scenario */
        DeviceIoControl(deviceHandle, IOCTL_WRITE, inputBuffer, (uint)(dataToWrite.Length + 1) * 2, IntPtr.Zero, 0, ref bytesReturned, IntPtr.Zero);

        // Cleanup
        Marshal.FreeHGlobal(outputBuffer);
        Marshal.FreeHGlobal(inputBuffer);
        CloseHandle(deviceHandle);
    }
}

```
