import 'dart:async';

import 'package:flutter/services.dart';

const MethodChannel _channel = MethodChannel('windows_documents');

Future<String?> getDocumentsDirectory() async =>
    await _channel.invokeMethod<String>('getDocumentsDirectory');
