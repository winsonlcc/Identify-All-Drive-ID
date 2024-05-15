//Author: Winson Loh //
//Date: 25/04/2024 //
//Simple program to retrieve drive information//
//You can add more information base on the code structure below. //


#include <windows.h>
#include <iostream>
#include <vector>

class Command {
public:
    int OpenDevice(const char* driveLetter);
    int GetDriveInfo(HANDLE hDevice);
    int GetGeometryInfo(HANDLE hDevice);
    WORD ByteSwap(WORD swap_byte);
};

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <drive letter>" << std::endl;
        return 1;
    }

    Command cmd;
    int result = cmd.OpenDevice(argv[1]);
    if (result != 0) {
        return result;
    }

    return 0;
}

int Command::OpenDevice(const char* driveLetter)
{
    char devicePath[20];
    int result;
    snprintf(devicePath, sizeof(devicePath), "\\\\.\\%s:", driveLetter);

    wchar_t wDevicePath[MAX_PATH];
    if (MultiByteToWideChar(CP_UTF8, 0, devicePath, -1, wDevicePath, MAX_PATH) == 0) {
        perror("error converting device path to wide char");
        return 1;
    }

    HANDLE hDevice = CreateFileW(wDevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        std::cerr << "error opening device: " << GetLastError() << std::endl;
        return 1;
    }

    printf("Opening device successfully.\r\n");

    result = GetDriveInfo(hDevice);
    result = GetGeometryInfo(hDevice);

    CloseHandle(hDevice);
    return result;
}

static void printBusTypeDescription(unsigned int busType) {
    printf("Bus Type: %u (", busType);
    switch (busType) {
    case 1:
        printf("SCSI)");
        break;
    case 2:
        printf("ATAPI)");
        break;
    case 3:
        printf("ATA)");
        break;
    case 4:
        printf("1394)");
        break;
    case 5:
        printf("SSA)");
        break;
    case 6:
        printf("Fibre Channel)");
        break;
    case 7:
        printf("USB)");
        break;
    case 8:
        printf("RAID)");
        break;
    case 9:
        printf("iSCSI)");
        break;
    case 10:
        printf("SAS)");
        break;
    case 11:
        printf("SATA)");
        break;
    case 12:
        printf("SD Card)");
        break;
    case 13:
        printf("MMC Card)");
        break;
    case 14:
        printf("Virtual)");
        break;
    case 15:
        printf("File Backed Virtual)");
        break;
    case 16:
        printf("Spaces)");
        break;
    case 17:
        printf("NVMe)");
        break;
    case 18:
        printf("SCM)");
        break;
    case 19:
        printf("UFS)");
        break;
    case 20:
        printf("Max)");
        break;
    default:
        printf("Unknown)");
    }
}

WORD Command::ByteSwap(WORD swap_byte)
{
    WORD var1, var2;
    //printf("IN %x\n", swap_byte);
    var1 = swap_byte & 0x00ff;
    var2 = swap_byte & 0xff00;
    var1 = var1 << 8;
    var2 = var2 >> 8;
    var2 = var2 & 0x00ff;
    swap_byte = (var1 | var2);
    //printf("OUT %x\n", swap_byte);
    return swap_byte;
}

int Command::GetDriveInfo(HANDLE hDevice)
{
    STORAGE_PROPERTY_QUERY query;
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    STORAGE_DESCRIPTOR_HEADER header = { 0 };
    DWORD bytesReturned = 0;

    if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), &header, sizeof(header), &bytesReturned, NULL)) {
        std::cerr << "error sending IOCTL_STORAGE_QUERY_PROPERTY: " << GetLastError() << std::endl;
        return 1;
    }

    // Allocate memory for the full response
    DWORD bufferSize = header.Size;
    std::vector<BYTE> buffer(bufferSize);
    if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), buffer.data(), bufferSize, &bytesReturned, NULL)) {
        std::cerr << "error sending IOCTL_STORAGE_QUERY_PROPERTY: " << GetLastError() << std::endl;
        return 1;
    }

    // Cast to the actual structure type (STORAGE_DEVICE_DESCRIPTOR)
    STORAGE_DEVICE_DESCRIPTOR* deviceDescriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(buffer.data());

    // Output the retrieved information //

    // Serial Number
    std::string strSerial;
    for (DWORD i = deviceDescriptor->SerialNumberOffset; buffer[i] != '\0' && i < buffer.size(); i += 2) {
        // Swap the bytes and add them to the serial string
        WORD swap_byte = buffer[i + 1] << 8 | buffer[i];
        swap_byte = ByteSwap(swap_byte);

        // Convert swapped bytes back to characters
        strSerial += static_cast<char>((swap_byte & 0xFF00) >> 8);
        strSerial += static_cast<char>(swap_byte & 0x00FF);
    }
    printf("Serial Number: %s \r\n", strSerial.c_str());  //TODO need to trim the string
   
    // Model Number
    std::string strModel;
    for (DWORD i = deviceDescriptor->ProductIdOffset; buffer[i] != '\0' && i < buffer.size(); i += 2) {
        // Swap the bytes and add them to the model string
        WORD swap_byte = buffer[i + 1] << 8 | buffer[i];
        swap_byte = ByteSwap(swap_byte);

        // Convert swapped bytes back to characters
        strModel += static_cast<char>((swap_byte & 0xFF00) >> 8);
        strModel += static_cast<char>(swap_byte & 0x00FF);
    }
    printf("Model Number: %s \r\n", strModel.c_str());


    // Vendor ID
    std::string strVendor;
    for (DWORD i = deviceDescriptor->VendorIdOffset; buffer[i] != '\0' && i < buffer.size(); i += 2) {
        // Swap the bytes and add them to the vendor ID string
        WORD swap_byte = buffer[i + 1] << 8 | buffer[i];
        swap_byte = ByteSwap(swap_byte);

        // Convert swapped bytes back to characters
        strVendor += static_cast<char>((swap_byte & 0xFF00) >> 8);
        strVendor += static_cast<char>(swap_byte & 0x00FF);
    }
    printf("Vendor ID: %s \r\n", strVendor.c_str());

    // Product ID
    std::string strProduct;
    for (DWORD i = deviceDescriptor->ProductIdOffset; buffer[i] != '\0' && i < buffer.size(); i += 2) {
        // Swap the bytes and add them to the product ID string
        WORD swap_byte = buffer[i + 1] << 8 | buffer[i];
        swap_byte = ByteSwap(swap_byte);

        // Convert swapped bytes back to characters
        strProduct += static_cast<char>((swap_byte & 0xFF00) >> 8);
        strProduct += static_cast<char>(swap_byte & 0x00FF);
    }
    printf("Product ID: %s \r\n", strProduct.c_str());


    // Product Revision
    std::string strProductRev;
    for (DWORD i = deviceDescriptor->ProductRevisionOffset; buffer[i] != '\0' && i < buffer.size(); i += 2) {
        // Swap the bytes and add them to the product rev string
        WORD swap_byte = buffer[i + 1] << 8 | buffer[i];
        swap_byte = ByteSwap(swap_byte);

        // Convert swapped bytes back to characters
        strProductRev += static_cast<char>((swap_byte & 0xFF00) >> 8);
        strProductRev += static_cast<char>(swap_byte & 0x00FF);
    }
    printf("Product Rev: %s \r\n", strProductRev.c_str());

    // Device Type
    printf("Device Type: %u\n", deviceDescriptor->DeviceType);

    // Removable Media
    printf("Removable Media: %s\n", (deviceDescriptor->RemovableMedia) ? "Yes" : "No");

    // Bus Type
    printBusTypeDescription(deviceDescriptor->BusType);

    printf("\n");

    // Raw Properties
    // Note: Raw properties can vary depending on the device and protocol, so interpretation may be complex
    // Here, we'll just print the raw data as hexadecimal values
 /*   for (DWORD i = 0; i < bufferSize; ++i) {
        printf("%02X ", buffer[i]);
        // Print human-readable character if printable ASCII, else print '.'
        printf("%c ", (isprint(buffer[i]) ? buffer[i] : '.'));
        // Add newline after every 16 bytes
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }*/
    return 0;
}


int Command::GetGeometryInfo(HANDLE hDevice)
{
    DISK_GEOMETRY diskGeometry = { 0 };
    DWORD bytesReturned = 0;

    if (!DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &diskGeometry, sizeof(diskGeometry), &bytesReturned, NULL)) {
        std::cerr << "error sending IOCTL_DISK_GET_DRIVE_GEOMETRY: " << GetLastError() << std::endl;
        return 1;
    }

    GET_LENGTH_INFORMATION lengthInfo = { 0 };

    if (!DeviceIoControl(hDevice, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &lengthInfo, sizeof(lengthInfo), &bytesReturned, NULL)) {
        std::cerr << "error sending IOCTL_DISK_GET_LENGTH_INFO: " << GetLastError() << std::endl;
        return 1;
    }

    // Check if the drive has a 4096-byte (4Kn) block size
    //if (diskGeometry.BytesPerSector == 4096) {
    //    lengthInfo.Length.QuadPart *= 8; // Multiply by 8 to get the correct capacity for 4Kn drives
    //}


    // Calculate the capacity in binary gigabytes
    //double capacityGB = static_cast<double>(lengthInfo.Length.QuadPart) / (1024 * 1024 * 1024);
    double capacityGB = static_cast<double>(lengthInfo.Length.QuadPart) / (1000 * 1000 * 1000);

    // Calculate the maximum LBA
    ULONGLONG maxLBA = lengthInfo.Length.QuadPart / diskGeometry.BytesPerSector;

    printf("Block Size: %lu bytes\n", diskGeometry.BytesPerSector);
    printf("Capacity: %.1f GB\n", capacityGB);
    printf("Max LBA: %llu\n", maxLBA);

    // Debug information
    //printf("Bytes Per Sector: %lu\n", diskGeometry.BytesPerSector);
    //printf("Total Length: %llu\n", lengthInfo.Length.QuadPart);
    //printf("Capacity (GB): %.1f\n", capacityGB);
    //printf("Max LBA: %llu\n", maxLBA);

    return 0;
}
