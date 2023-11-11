import 'package:windows_printing/models/printer.dart';

import 'windows_printing_platform_interface.dart';

class WindowsPrinting {
  Future<String?> getPlatformVersion() {
    return WindowsPrintingPlatform.instance.getPlatformVersion();
  }

  Future<List<Printer>?> getPrinterList() async {
    return await WindowsPrintingPlatform.instance.getPrinterList();
  }
}
