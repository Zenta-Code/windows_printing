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
namespace windows_printing
{
  std::string toUtf8(std::wstring wstr)
  {
    if (wstr.empty())
      return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
  }

  std::wstring toUtf16(std::string str)
  {
    if (str.empty())
      return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
  }

  std::string toUtf8(const wchar_t *wstr)
  {
    if (wstr == NULL)
      return std::string();
    return toUtf8(std::wstring(wstr));
  }

  std::wstring toUtf16(const char *str)
  {
    if (str == NULL)
      return std::wstring();
    return toUtf16(std::string(str));
  }

  std::string toUtf8(const char *str)
  {
    if (str == NULL)
      return std::string();
    return std::string(str);
  }

  std::wstring toUtf16(const wchar_t *wstr)
  {
    if (wstr == NULL)
      return std::wstring();
    return std::wstring(wstr);
  }

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
    if (method_call.method_name().compare("getPlatformVersion") == 0)
    {
      std::ostringstream version_stream;
      version_stream << "Windows ";
      if (IsWindows10OrGreater())
      {
        version_stream << "s+";
      }
      else if (IsWindows8OrGreater())
      {
        version_stream << "8";
      }
      else if (IsWindows7OrGreater())
      {
        version_stream << "7";
      }
      result->Success(flutter::EncodableValue(version_stream.str()));
    }
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
        printer[flutter::EncodableValue("name")] = flutter::EncodableValue(toUtf8(pi2[i].pPrinterName));
        printerList.push_back(flutter::EncodableValue(printer));
      }

      result->Success(flutter::EncodableList(printerList));

      if (pi2 != NULL)
        GlobalFree(pi2);

      return;
    }
    else
    {
      result->NotImplemented();
    }
  }

} // namespace windows_printing
