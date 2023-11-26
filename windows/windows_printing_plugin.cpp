#include "windows_printing_plugin.h"
// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>
// #include "converter.h"

namespace windows_printing
{
  class Converter
  {
  private:
  public:
    static std::string lpwstrToUtf8(std::wstring wstr)
    {
      std::string strTo;
      char *szTo = new char[wstr.length() + 1];
      szTo[wstr.size()] = '\0';
      WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
      strTo = szTo;
      delete[] szTo;
      return strTo;
    }

    static std::wstring toUtf16(std::string str)
    {
      std::wstring wstrTo;
      wchar_t *wszTo = new wchar_t[str.length() + 1];
      wszTo[str.size()] = L'\0';
      MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wszTo, (int)str.length());
      wstrTo = wszTo;
      delete[] wszTo;
      return wstrTo;
    }

    static std::string lpwstrToUtf8(const wchar_t *wstr)
    {
      std::string strTo;
      char *szTo = new char[wcslen(wstr) + 1];
      szTo[wcslen(wstr)] = '\0';
      WideCharToMultiByte(CP_UTF8, 0, wstr, -1, szTo, (int)wcslen(wstr), NULL, NULL);
      strTo = szTo;
      delete[] szTo;
      return strTo;
    }

    static std::wstring toUtf16(const char *str)
    {
      std::wstring wstrTo;
      wchar_t *wszTo = new wchar_t[strlen(str) + 1];
      wszTo[strlen(str)] = L'\0';
      MultiByteToWideChar(CP_UTF8, 0, str, -1, wszTo, (int)strlen(str));
      wstrTo = wszTo;
      delete[] wszTo;
      return wstrTo;
    }

    static std::string lpwstrToUtf8(const char *str)
    {
      std::string strTo;
      char *szTo = new char[strlen(str) + 1];
      szTo[strlen(str)] = '\0';
      WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)str, -1, szTo, (int)strlen(str), NULL, NULL);
      strTo = szTo;
      delete[] szTo;
      return strTo;
    }

    static int DwordToInt(DWORD dw)
    {
      return static_cast<int>(dw);
    }
  };
  class PrinterPort
  {
  private:
  public:
    // get printer status is on or off
    bool getPrinterStatus(const LPWSTR printerName)
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
      GlobalFree(hPrinter);
      ClosePrinter(hPrinter);
      return dwPrinterState != 0x0000;
    };
  };

  // static
  void WindowsPrintingPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "windows_printing",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<WindowsPrintingPlugin>();

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result)
        {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    registrar->AddPlugin(std::move(plugin));
  }

  WindowsPrintingPlugin::WindowsPrintingPlugin() {}

  WindowsPrintingPlugin::~WindowsPrintingPlugin() {}

  void WindowsPrintingPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    ///
    /// Get the list of printers
    /// using the [EnumPrinters] (https://docs.microsoft.com/en-us/windows/win32/printdocs/enumprinters) function
    ///
    ///

    if (method_call.method_name().compare("getPrinterList") == 0)
    {
      DWORD numPrinters, bytesNeeded;
      PRINTER_INFO_2 *pi2 = NULL;

      if (!EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2, (LPBYTE)pi2, 0, &bytesNeeded, &numPrinters))
      {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
          pi2 = (PRINTER_INFO_2 *)GlobalAlloc(GPTR, bytesNeeded);
          if (pi2 == NULL)
          {
            result->Error("ERROR", "Memory allocation failed");
            return;
          }
        }
        else
        {
          result->Error("ERROR", "EnumPrinters failed");
          return;
        }
      }

      if (!EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2, (LPBYTE)pi2, bytesNeeded, &bytesNeeded, &numPrinters))
      {
        result->Error("ERROR", "EnumPrinters failed");
        return;
      }

      auto printerList = flutter::EncodableList();

      for (DWORD i = 0; i < numPrinters; i++)
      {
        auto printer = flutter::EncodableMap();

        // get printer status is on or off
        PrinterPort printerPort;
        bool isPrinterOn = printerPort.getPrinterStatus(pi2[i].pPrinterName);

        printer[flutter::EncodableValue("printerOnline")] = flutter::EncodableValue(isPrinterOn);

        printer[flutter::EncodableValue("printerName")] = flutter::EncodableValue(Converter::lpwstrToUtf8(pi2[i].pPrinterName));

        printer[flutter::EncodableValue("printerPort")] = flutter::EncodableValue(Converter::lpwstrToUtf8(pi2[i].pPortName));

        printer[flutter::EncodableValue("printerProcessor")] = flutter::EncodableValue(Converter::lpwstrToUtf8(pi2[i].pPrintProcessor));

        printer[flutter::EncodableValue("cJobs")] = flutter::EncodableValue(Converter::DwordToInt(pi2[i].cJobs));

        printerList.push_back(flutter::EncodableValue(printer));
      }

      std::cout << "DONE" << std::endl;

      result->Success(flutter::EncodableList(printerList));

      if (pi2 != NULL)
        GlobalFree(pi2);

      return;
    }
    if (method_call.method_name().compare("directPrint") == 0)
    {
      auto arguments = std::get<flutter::EncodableMap>(*method_call.arguments());
      auto printerName = std::get<std::string>(arguments[flutter::EncodableValue("printerName")]);
      auto filePath = std::get<std::string>(arguments[flutter::EncodableValue("filePath")]);
      auto jobName = std::get<std::string>(arguments[flutter::EncodableValue("jobName")]);
      // auto copies = std::get<int>(arguments[flutter::EncodableValue("copies")]);

      std::wstring wprinterName = Converter::toUtf16(printerName);
      std::wstring wfilePath = Converter::toUtf16(filePath);
      std::wstring wjobName = Converter::toUtf16(jobName);

      DOC_INFO_1 docInfo;
      HANDLE hPrinter;
      DWORD dwJob;
      DWORD dwBytesWritten;
      BOOL bSuccess;

      // Open a handle to the printer.
      bSuccess = OpenPrinter((LPWSTR)wprinterName.c_str(), &hPrinter, NULL);
      if (!bSuccess)
      {
        result->Error("ERROR", "OpenPrinter failed");
        return;
      }

      // Fill in the structure with info about this "document."
      docInfo.pDocName = (LPWSTR)wjobName.c_str();
      docInfo.pOutputFile = NULL;
      docInfo.pDatatype = (LPWSTR)L"RAW";

      // Inform the spooler the document is beginning.
      dwJob = StartDocPrinter(hPrinter, 1, (LPBYTE)&docInfo);
      if (dwJob == 0)
      {
        result->Error("ERROR", "StartDocPrinter failed");
        ClosePrinter(hPrinter);
        return;
      }

      // Start a page.
      bSuccess = StartPagePrinter(hPrinter);

      if (!bSuccess)
      {
        result->Error("ERROR", "StartPagePrinter failed");
        EndDocPrinter(hPrinter);
        ClosePrinter(hPrinter);
        return;
      }

      // Send the data to the printer.
      bSuccess = WritePrinter(hPrinter, (LPVOID)wfilePath.c_str(), (DWORD)wfilePath.length(), &dwBytesWritten);
      if (!bSuccess)
      {
        result->Error("ERROR", "WritePrinter failed");
        EndPagePrinter(hPrinter);
        EndDocPrinter(hPrinter);
        ClosePrinter(hPrinter);
        return;
      }

      // End the page.
      bSuccess = EndPagePrinter(hPrinter);
      if (!bSuccess)
      {
        result->Error("ERROR", "EndPagePrinter failed");
        EndDocPrinter(hPrinter);
        ClosePrinter(hPrinter);
        return;
      }

      // Inform the spooler that the document is ending.
      bSuccess = EndDocPrinter(hPrinter);
      if (!bSuccess)
      {
        result->Error("ERROR", "EndDocPrinter failed");
        ClosePrinter(hPrinter);
        return;
      }

      // Tidy up the printer handle.
      bSuccess = ClosePrinter(hPrinter);
      if (!bSuccess)
      {
        result->Error("ERROR", "ClosePrinter failed");
        return;
      }

      auto response = flutter::EncodableMap();

      response[flutter::EncodableValue("success")] = flutter::EncodableValue(true);
      response
          [flutter::EncodableValue("message")] = flutter::EncodableValue("Printed successfully");
      response
          [flutter::EncodableValue("job")] = flutter::EncodableValue(Converter::DwordToInt(dwJob));

      result->Success(flutter::EncodableValue(response));
    }
    else
    {
      result->NotImplemented();
    }
  }

} // namespace windows_printing
