#include "include/windows_printing/windows_printing_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "windows_printing_plugin.h"

void WindowsPrintingPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  windows_printing::WindowsPrintingPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
