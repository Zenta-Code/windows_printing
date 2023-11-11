#ifndef FLUTTER_PLUGIN_WINDOWS_PRINTING_PLUGIN_H_
#define FLUTTER_PLUGIN_WINDOWS_PRINTING_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace windows_printing {

class WindowsPrintingPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  WindowsPrintingPlugin();

  virtual ~WindowsPrintingPlugin();

  // Disallow copy and assign.
  WindowsPrintingPlugin(const WindowsPrintingPlugin&) = delete;
  WindowsPrintingPlugin& operator=(const WindowsPrintingPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace windows_printing

#endif  // FLUTTER_PLUGIN_WINDOWS_PRINTING_PLUGIN_H_
