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
      auto printerPort = std::get<std::string>(arguments[flutter::EncodableValue("printerPort")]);
      auto printerProcessor = std::get<std::string>(arguments[flutter::EncodableValue("printerProcessor")]);
      auto printerJobName = std::get<std::string>(arguments[flutter::EncodableValue("printerJobName")]);
      auto printerJobData = std::get<std::string>(arguments[flutter::EncodableValue("printerJobData")]);

      std::cout << "printerName: " << printerName << std::endl;
      std::cout << "printerPort: " << printerPort << std::endl;
      std::cout << "printerProcessor: " << printerProcessor << std::endl;
      std::cout << "printerJobName: " << printerJobName << std::endl;
      std::cout << "printerJobData: " << printerJobData << std::endl;
    }
    else
    {
      result->NotImplemented();
    }
  }

} // namespace windows_printing
