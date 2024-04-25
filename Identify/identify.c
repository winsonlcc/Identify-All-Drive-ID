#include <stdio.h>
#include <windows.h>
#include <ntddscsi.h>

#define IDENTIFY_BUFFER_SIZE 512

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s drive_letter\n", argv[0]);
        return 1;
    }

    char drivePath[7];
    sprintf(drivePath, "\\\\.\\%c:", argv[1][0]);

    HANDLE hDevice = CreateFileA(drivePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("Failed to open drive %s. Error code: %lu\n", drivePath, GetLastError());
        return 2;
    }

    SENDCMDINPARAMS scip = { 0 };
    SENDCMDOUTPARAMS scop = { 0 };
    DWORD bytesReturned;
    char identifyBuffer[IDENTIFY_BUFFER_SIZE] = { 0 };

    scip.irDriveRegs.bCommandReg = 0xEC; // ATA IDENTIFY command
    scip.bDriveNumber = 0;
    scip.irDriveRegs.bFeaturesReg = 0;
    scip.irDriveRegs.bSectorCountReg = 1;
    scip.irDriveRegs.bSectorNumberReg = 1;
    scip.irDriveRegs.bCylLowReg = 0;
    scip.irDriveRegs.bCylHighReg = 0;
    scip.cBufferSize = IDENTIFY_BUFFER_SIZE;

    if (!DeviceIoControl(hDevice, SMART_RCV_DRIVE_DATA, &scip, sizeof(scip), &scop, sizeof(scop), &bytesReturned, NULL)) {
        printf("Failed to send SMART_RCV_DRIVE_DATA command. Error code: %lu\n", GetLastError());
        CloseHandle(hDevice);
        return 3;
    }

    memcpy(identifyBuffer, scop.bBuffer, IDENTIFY_BUFFER_SIZE);

    printf("Drive information for %s:\n", drivePath);
    // Process the identifyBuffer to get the drive information
    // Refer to ATA specifications for details on interpreting the data

    CloseHandle(hDevice);
    return 0;
}
