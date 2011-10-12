// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/component_updater/recovery_component_installer.h"

#include "base/base_paths.h"
#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/process_util.h"
#include "base/string_util.h"
#include "base/values.h"
#include "chrome/browser/component_updater/component_updater_service.h"
#include "chrome/common/chrome_version_info.h"
#include "content/browser/browser_thread.h"

namespace {

// CRX hash. The extension id is: npdjjkjlcidkjlamlmmdelcjbcpdjocm.
const uint8 sha2_hash[] = {0xdf, 0x39, 0x9a, 0x9b, 0x28, 0x3a, 0x9b, 0x0c,
                           0xbc, 0xc3, 0x4b, 0x29, 0x12, 0xf3, 0x9e, 0x2c,
                           0x19, 0x7a, 0x71, 0x4b, 0x0a, 0x7c, 0x80, 0x1c,
                           0xf6, 0x29, 0x7c, 0x0a, 0x5f, 0xea, 0x67, 0xb7};

// File name of the recovery binary on different platforms.
const FilePath::CharType kRecoveryFileName[] =
#if defined(OS_WIN)
    FILE_PATH_LITERAL("ChromeRecovery.exe");
#else  // OS_LINUX, OS_MAC, etc.
    FILE_PATH_LITERAL("ChromeRecovery");
#endif

const char kRecoveryManifestName[] = "ChromeRecovery";

}  // namespace

class RecoveryComponentInstaller : public ComponentInstaller {
 public:
  explicit RecoveryComponentInstaller(const Version& version);

  virtual ~RecoveryComponentInstaller() {}

  virtual void OnUpdateError(int error) OVERRIDE;

  virtual bool Install(base::DictionaryValue* manifest,
                       const FilePath& unpack_path) OVERRIDE;

 private:
  Version current_version_;
};

RecoveryComponentInstaller::RecoveryComponentInstaller(
    const Version& version) : current_version_(version) {
  DCHECK(version.IsValid());
}

void RecoveryComponentInstaller::OnUpdateError(int error) {
  NOTREACHED() << "Recovery component update error: " << error;
}

bool RecoveryComponentInstaller::Install(base::DictionaryValue* manifest,
                                         const FilePath& unpack_path) {
  std::string name;
  manifest->GetStringASCII("name", &name);
  if (name != kRecoveryManifestName)
    return false;
  std::string proposed_version;
  manifest->GetStringASCII("version", &proposed_version);
  Version version(proposed_version.c_str());
  if (!version.IsValid())
    return false;
  if (current_version_.CompareTo(version) >= 0)
    return false;
  FilePath main_file = unpack_path.Append(kRecoveryFileName);
  if (!file_util::PathExists(main_file))
    return false;
  // Passed the basic tests. The installation continues with the
  // recovery component itself running from the temp directory.
  CommandLine cmdline(main_file);
  std::string arguments;
  if (manifest->GetStringASCII("x-recovery-args", &arguments))
    cmdline.AppendArg(arguments);
  std::string add_version;
  if (manifest->GetStringASCII("x-recovery-add-version", &add_version)) {
    if (add_version == "yes")
      cmdline.AppendSwitchASCII("version", current_version_.GetString());
  }
  current_version_ = version;
  return base::LaunchProcess(cmdline, base::LaunchOptions(), NULL);
}

void RegisterRecoveryComponent(ComponentUpdateService* cus,
                               const char* chrome_version) {
#if !defined(OS_CHROMEOS)
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  CrxComponent recovery;
  recovery.name = "recovery";
  Version version(chrome_version);
  if (!version.IsValid()) {
    NOTREACHED() << "Need valid chrome version.";
    return;
  }

  recovery.installer = new RecoveryComponentInstaller(version);
  recovery.version = version;
  recovery.pk_hash.assign(sha2_hash, &sha2_hash[sizeof(sha2_hash)]);
  if (cus->RegisterComponent(recovery) != ComponentUpdateService::kOk) {
    NOTREACHED() << "Recovery component registration failed.";
  }
#endif
}
