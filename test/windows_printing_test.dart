import 'package:flutter_test/flutter_test.dart';
import 'package:windows_printing/windows_printing.dart';
import 'package:windows_printing/windows_printing_platform_interface.dart';
import 'package:windows_printing/windows_printing_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockWindowsPrintingPlatform
    with MockPlatformInterfaceMixin
    implements WindowsPrintingPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final WindowsPrintingPlatform initialPlatform = WindowsPrintingPlatform.instance;

  test('$MethodChannelWindowsPrinting is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelWindowsPrinting>());
  });

  test('getPlatformVersion', () async {
    WindowsPrinting windowsPrintingPlugin = WindowsPrinting();
    MockWindowsPrintingPlatform fakePlatform = MockWindowsPrintingPlatform();
    WindowsPrintingPlatform.instance = fakePlatform;

    expect(await windowsPrintingPlugin.getPlatformVersion(), '42');
  });
}
