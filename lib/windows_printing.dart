import 'package:windows_printing_models/windows_printing_models.dart';

import 'windows_printing_platform_interface.dart';

class WindowsPrinting {
  Future<List<Printer>?> getPrinterList() async {
    try {
      final List<Printer>? printerList =
          await WindowsPrintingPlatform.instance.getPrinterList();

      if (printerList != null) {
        return printerList;
      }

      return null;
    } catch (e) {
      print(e);
      return null;
    }
  }
}
