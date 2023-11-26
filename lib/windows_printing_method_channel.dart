import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'windows_printing_platform_interface.dart';

/// An implementation of [WindowsPrintingPlatform] that uses method channels.
class MethodChannelWindowsPrinting extends WindowsPrintingPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('windows_printing');

  @override
  Future<List<dynamic>?> getPrinterList() async {
    try {
      final List<dynamic>? printerList =
          await methodChannel.invokeMethod('getPrinterList');

      if (printerList != null) {
        // final List<dynamic> typedPrinterList =
        //     printerList.map((e) => Printer.fromMap(e)).toList();
        // return typedPrinterList;
        return printerList;
      }

      return null;
    } catch (e) {
      print(e);
      return null;
    }
  }

  @override
  Future<Map<Object?, Object?>?> directPrint({
    required String printerName,
    required String filePath,
    required String jobName,
    required int copies,
  }) async {
    try {
      final Map<Object?, Object?>? jobs =
          await methodChannel.invokeMethod('directPrint', {
        'printerName': printerName,
        'filePath': filePath,
        'jobName': jobName,
        'copies': copies,
      });
      return jobs;
    } catch (e) {
      print(e);
      return null;
    }
  }
}
