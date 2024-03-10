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
    /* Define the IOCTL code for communication with the driver */
    private const uint IOCTL_SIMPLEDRIVER_METHOD = 0x222000;
    
    /* Import the kernel32.dll library for device I/O operations */
    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern IntPtr CreateFile(
        string lpFileName,
        uint dwDesiredAccess,
        uint dwShareMode,
        IntPtr lpSecurityAttributes,
        uint dwCreationDisposition,
        uint dwFlagsAndAttributes,
        IntPtr hTemplateFile);

    /* Import the kernel32.dll library for sending IOCTL requests */
    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool DeviceIoControl(
        IntPtr hDevice,
        uint dwIoControlCode,
        IntPtr lpInBuffer,
        uint nInBufferSize,
        IntPtr lpOutBuffer,
        uint nOutBufferSize,
        ref uint lpBytesReturned,
        IntPtr lpOverlapped);

    /* Import the kernel32.dll library for closing the device handle */
    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern bool CloseHandle(IntPtr hObject);

    static void Main()
    {
        /* Specify the path to the driver device */
        string devicePath = @"\\.\SimpleDriver";

        /* Open a handle to the driver device */
        IntPtr deviceHandle = CreateFile(
            devicePath,
            0xC0000000, // GENERIC_READ | GENERIC_WRITE
            0,
            IntPtr.Zero,
            3, // OPEN_EXISTING
            0,
            IntPtr.Zero);

        if (deviceHandle.ToInt32() == -1)
        {
            Console.WriteLine("Failed to open the device. Error code: " + Marshal.GetLastWin32Error());
            return;
        }

        try
        {
            /* Send a sample IOCTL request to the driver */
            uint bytesReturned = 0;
            bool success = DeviceIoControl(
                deviceHandle,
                IOCTL_SIMPLEDRIVER_METHOD,
                IntPtr.Zero,
                0,
                IntPtr.Zero,
                0,
                ref bytesReturned,
                IntPtr.Zero);

            if (success)
            {
                Console.WriteLine("IOCTL request sent successfully!");
            }
            else
            {
                Console.WriteLine("Failed to send IOCTL request. Error code: " + Marshal.GetLastWin32Error());
            }
        }
        finally
        {
            /* Close the handle to the driver device */
            CloseHandle(deviceHandle);
        }
    }
}


```
