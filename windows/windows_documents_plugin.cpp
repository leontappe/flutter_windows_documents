#include "include/windows_documents/windows_documents_plugin.h"

// This must be included before many other Windows headers.
#include <ShlObj.h>
#include <Windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>
#include <string>

namespace {

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
    if(wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Gets the path to the given folder ID, without verifying that it exists,
// or an empty string on failure.
std::string GetFolderPath(REFKNOWNFOLDERID folder_id)
{
  wchar_t *wide_path = nullptr;
  if (!SUCCEEDED(SHGetKnownFolderPath(folder_id, KF_FLAG_DONT_VERIFY, nullptr,
                                      &wide_path)))
  {
    return "";
  }
  return utf8_encode(wide_path);
}

class WindowsDocumentsPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  WindowsDocumentsPlugin();

  virtual ~WindowsDocumentsPlugin();

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

// static
void WindowsDocumentsPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "windows_documents",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<WindowsDocumentsPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

WindowsDocumentsPlugin::WindowsDocumentsPlugin() {}

WindowsDocumentsPlugin::~WindowsDocumentsPlugin() {}

void WindowsDocumentsPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getDocumentsDirectory") == 0) {
    std::string path = GetFolderPath(FOLDERID_Documents);
    result->Success(flutter::EncodableValue(path));
  } else {
    result->NotImplemented();
  }
}

}  // namespace

void WindowsDocumentsPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  WindowsDocumentsPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
