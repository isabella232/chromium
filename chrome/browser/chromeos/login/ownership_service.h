// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_LOGIN_OWNERSHIP_SERVICE_H_
#define CHROME_BROWSER_CHROMEOS_LOGIN_OWNERSHIP_SERVICE_H_
#pragma once

#include <string>

#include "base/ref_counted.h"
#include "base/singleton.h"
#include "chrome/browser/chromeos/login/owner_key_utils.h"
#include "chrome/browser/chromeos/login/owner_manager.h"

namespace chromeos {

class OwnershipService {
 public:
  // Returns the singleton instance of the OwnershipService.
  static OwnershipService* GetSharedInstance();
  virtual ~OwnershipService();

  bool IsAlreadyOwned();

  // If the device has been owned already, posts a task to the FILE thread to
  // fetch the public key off disk.
  // Returns true if the attempt was initiated, false otherwise.
  //
  // Sends out a OWNER_KEY_FETCH_ATTEMPT_SUCCESS notification on success,
  // OWNER_KEY_FETCH_ATTEMPT_FAILED on failure.
  bool StartLoadOwnerKeyAttempt();

  // If the device has not yet been owned, posts a task to the FILE
  // thread to generate the owner's keys and put them in the right
  // places.  Keeps them in memory as well, for later use.
  // Returns true if the attempt was initiated, false otherwise.
  //
  // Sends out a OWNER_KEY_FETCH_ATTEMPT_SUCCESS notification on success,
  // OWNER_KEY_FETCH_ATTEMPT_FAILED on failure.
  bool StartTakeOwnershipAttempt();

  // Initiate an attempt to sign |data| with |private_key_|.  Will call
  // d->OnKeyOpComplete() when done.  Upon success, the signature will be passed
  // as the |payload| argument to d->OnKeyOpComplete().
  // Returns true if the attempt was initiated, false otherwise.
  //
  // If you call this on a well-known thread, you'll be called back on that
  // thread.  Otherwise, you'll get called back on the UI thread.
  bool StartSigningAttempt(const std::string& data, OwnerManager::Delegate* d);

  // Initiate an attempt to verify that |signature| is valid over |data| with
  // |public_key_|.  When the attempt is completed, an appropriate KeyOpCode
  // will be passed to d->OnKeyOpComplete().
  // Returns true if the attempt was initiated, false otherwise.
  //
  // If you call this on a well-known thread, you'll be called back on that
  // thread.  Otherwise, you'll get called back on the UI thread.
  bool StartVerifyAttempt(const std::string& data,
                          const std::string& signature,
                          OwnerManager::Delegate* d);

  bool CurrentUserIsOwner();

 private:
  friend struct DefaultSingletonTraits<OwnershipService>;
  friend class OwnershipServiceTest;

  OwnershipService();

  scoped_refptr<OwnerManager> manager_;
  scoped_refptr<OwnerKeyUtils> utils_;
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_LOGIN_OWNERSHIP_SERVICE_H_
