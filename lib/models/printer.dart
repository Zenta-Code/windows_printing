import 'package:meta/meta.dart';

@immutable
class Printer {
  const Printer({
    required this.name,
  });

  /// Create an information object from a dictionnary
  factory Printer.fromMap(Map<dynamic, dynamic> map) => Printer(
        name: map['name'],
      );

  final String name;

  @override
  String toString() => '''$runtimeType 
    { 
      name: $name, 
    } 
    ''';

  Map<String, Object?> toMap() => {
        'name': name,
      };
}
