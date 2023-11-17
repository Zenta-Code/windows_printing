import 'package:plugin_platform_interface/plugin_platform_interface.dart';
import 'package:windows_printing_models/windows_printing_models.dart';

import 'windows_printing_method_channel.dart';

abstract class WindowsPrintingPlatform extends PlatformInterface {
  /// Constructs a WindowsPrintingPlatform.
  WindowsPrintingPlatform() : super(token: _token);

  static final Object _token = Object();

  static WindowsPrintingPlatform _instance = MethodChannelWindowsPrinting();

  /// The default instance of [WindowsPrintingPlatform] to use.
  ///
  /// Defaults to [MethodChannelWindowsPrinting].
  static WindowsPrintingPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [WindowsPrintingPlatform] when
  /// they register themselves.
  static set instance(WindowsPrintingPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<List<Printer>?> getPrinterList() {
    throw UnimplementedError('getPrintersList() has not been implemented.');
  }

  Future directPrint({
    required String printerName,
    required String filePath,
    required String jobName,
    required int copies,
  }) {
    throw UnimplementedError('print() has not been implemented.');
  }
}
