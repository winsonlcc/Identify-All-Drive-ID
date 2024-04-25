Identify All Type Device ID
This repository contains a C++ program that demonstrates how to identify various types of device IDs, including vendor ID, model number, and serial number, from storage devices connected to a Windows system. 
The program utilizes Windows IOCTL commands to retrieve device information, such as drive geometry and length, and then parses the data to extract the relevant device IDs.

How It Works
The program uses the following steps to identify device IDs:

Open Device: The program opens the target storage device using the appropriate Windows API functions.
Retrieve Device Information: It retrieves the device geometry and length information using IOCTL commands (IOCTL_DISK_GET_DRIVE_GEOMETRY and IOCTL_DISK_GET_LENGTH_INFO, respectively).
Parse and Display IDs: Using the retrieved information, the program parses and displays the vendor ID, model number, and serial number of the device.
Byte Swapping: The program includes a byte swapping function to ensure correct interpretation of multi-byte values, such as vendor IDs, which may require swapping bytes for correct representation.
Bus Type Identification: The program also includes a function to identify the bus type of the storage device, using a predefined enum (STORAGE_BUS_TYPE), to provide a human-readable description of the bus type.

Usage
To use the program, simply compile it using Visual Studio 2022 or a compatible C++ compiler for Windows, and run the compiled executable. 
Provide the drive letter of the target storage device as a command-line argument to the program.

Example Output
Here is an example of the output generated by the program:
PS C:\Users\winson\source\repos\Identify\x64\Debug> .\Identify.exe D
Opening device successfully.
Serial Number: 0773083830C0
Model Number: FLASH DRIVE
Vendor ID: USB3.0
Product ID: FLASH DRIVE
Product Rev: PMAP
Device Type: 0
Removable Media: Yes
Bus Type: 7 (USB)
Block Size: 512 bytes
Capacity: 14.4 GB
Max LBA: 30283008
