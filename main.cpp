#include <windows.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;
// get all printers on the local machine
vector<PRINTER_INFO_2> getPrinters()
{
    DWORD numPrinters, bytesNeeded;
    PRINTER_INFO_2 *printerInfo = NULL;

    EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, NULL, 0, &bytesNeeded, &numPrinters);

    printerInfo = (PRINTER_INFO_2 *)malloc(bytesNeeded);

    EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, (LPBYTE)printerInfo, bytesNeeded, &bytesNeeded, &numPrinters);

    return vector<PRINTER_INFO_2>(printerInfo, printerInfo + numPrinters);
}

// get printer jobs with printer name as input
vector<JOB_INFO_1> getPrinterJobs(const LPSTR printerName)
{
    DWORD numJobs, bytesNeeded;
    JOB_INFO_1 *jobInfo = NULL;

    HANDLE hPrinter = NULL;
    OpenPrinter(printerName, &hPrinter, NULL);

    EnumJobs(hPrinter, 0, 100, 1, NULL, 0, &bytesNeeded, &numJobs);

    jobInfo = (JOB_INFO_1 *)malloc(bytesNeeded);

    EnumJobs(hPrinter, 0, 100, 1, (LPBYTE)jobInfo, bytesNeeded, &bytesNeeded, &numJobs);

    return vector<JOB_INFO_1>(jobInfo, jobInfo + numJobs);
}

// get printer status is on or off
bool getPrinterStatus(const LPSTR printerName)
{
    HANDLE hPrinter = NULL;
    OpenPrinter(printerName, &hPrinter, NULL);

    DWORD dwPrinterStatus, dwPrinterState;
    DWORD dwRet = GetPrinter(hPrinter, 2, NULL, 0, &dwPrinterStatus);
    if (dwRet == 0)
    {
        return false;
    }

    dwRet = GetPrinter(hPrinter, 2, (LPBYTE)&dwPrinterStatus, dwPrinterStatus, &dwPrinterStatus);
    if (dwRet == 0)
    {
        return false;
    }

    dwPrinterState = dwPrinterStatus & 0xf000;

    return dwPrinterState != 0x0000;
}

// get printer is busy or not
bool getPrinterBusy(const LPSTR printerName)
{
    HANDLE hPrinter = NULL;
    OpenPrinter(printerName, &hPrinter, NULL);

    DWORD dwPrinterStatus, dwPrinterState;
    DWORD dwRet = GetPrinter(hPrinter, 2, NULL, 0, &dwPrinterStatus);
    if (dwRet == 0)
    {
        return false;
    }

    dwRet = GetPrinter(hPrinter, 2, (LPBYTE)&dwPrinterStatus, dwPrinterStatus, &dwPrinterStatus);
    if (dwRet == 0)
    {
        return false;
    }

    dwPrinterState = dwPrinterStatus & 0x000f;

    return dwPrinterState != 0x0000;
}

// direct print file to printer without dialog
bool printFile(const LPSTR printerName, const LPSTR fileName)
{
    HANDLE hPrinter = NULL;
    OpenPrinter(printerName, &hPrinter, NULL);

    DOC_INFO_1 docInfo;
    docInfo.pDocName = "Test Print";
    docInfo.pOutputFile = NULL;
    docInfo.pDatatype = "RAW";

    DWORD dwJob = StartDocPrinter(hPrinter, 1, (LPBYTE)&docInfo);
    if (dwJob == 0)
    {
        return false;
    }

    DWORD dwBytesWritten;
    DWORD dwBytesRead;
    BYTE *pBuf = NULL;
    DWORD dwBufSize = 4096;
    pBuf = (BYTE *)malloc(dwBufSize);
    HANDLE hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    while (true)
    {
        if (ReadFile(hFile, pBuf, dwBufSize, &dwBytesRead, NULL) == 0)
        {
            return false;
        }

        if (dwBytesRead == 0)
        {
            break;
        }

        if (WritePrinter(hPrinter, pBuf, dwBytesRead, &dwBytesWritten) == 0)
        {
            return false;
        }
    }

    EndDocPrinter(hPrinter);

    ClosePrinter(hPrinter);

    CloseHandle(hFile);

    return true;
}

// main function
int main()
{
    vector<PRINTER_INFO_2> printers = getPrinters();

    cout << "==== printer ====" << endl;
    for (const PRINTER_INFO_2 &printer : printers)
    {
        cout << printer.pPrinterName << endl;
        cout << printer.pParameters << endl;
    }
    cout << "==== end ====" << endl
         << endl;

    // for (const PRINTER_INFO_2 &printer : printers)
    // {
    //     vector<JOB_INFO_1> jobs = getPrinterJobs(printer.pPrinterName);

    //     for (const JOB_INFO_1 &job : jobs)
    //     {
    //         cout << job.pDocument << endl;
    //     }
    // }

    // for (const PRINTER_INFO_2 &printer : printers)
    // {
    //     cout << printer.pPrinterName << " is " << (getPrinterStatus(printer.pPrinterName) ? "on" : "off") << endl;
    // }

    // for (const PRINTER_INFO_2 &printer : printers)
    // {
    //     cout << printer.pPrinterName << " is " << (getPrinterBusy(printer.pPrinterName) ? "busy" : "not busy") << endl;
    // }

    // // print file using last printers
    // if (printFile(printers[printers.size() - 1].pPrinterName, "C:/Users/Matt/Documents/2023-ProposalPUI.pdf"))
    // {
    //     cout << "print success" << endl;
    // }
    // else
    // {
    //     cout << "print failed" << endl;
    // }

    return 0;
}
