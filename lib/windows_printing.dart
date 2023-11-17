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

  Future<Map<Object?, Object?>?> directPrint({
    required String printerName,
    required String filePath,
    required String jobName,
    required int copies,
  }) async {
    try {
      final Map<Object?, Object?>? print =
          await WindowsPrintingPlatform.instance.directPrint(
        printerName: printerName,
        filePath: filePath,
        jobName: jobName,
        copies: copies,
      );
      return print;
    } catch (e) {
      print(e);
      return null;
    }
  }
}
