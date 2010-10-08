// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_DOM_UI_MENU_UI_H_
#define CHROME_BROWSER_CHROMEOS_DOM_UI_MENU_UI_H_
#pragma once

#include <string>

#include "chrome/browser/dom_ui/chrome_url_data_manager.h"
#include "chrome/browser/dom_ui/dom_ui.h"

class DictionaryValue;

namespace menus {
class MenuModel;
}  // namespace menus

namespace chromeos {

class DOMUIMenuControl;

class MenuUI : public DOMUI {
 public:
  explicit MenuUI(TabContents* contents);

  // A callback method that is invoked when a menu model associated
  // with the DOMUI Menu gets updated.
  virtual void ModelUpdated(const menus::MenuModel* new_model);

  // Creates a menu item for the menu item at index in the model.
  virtual DictionaryValue* CreateMenuItem(const menus::MenuModel* model,
                                          int index,
                                          const char* type,
                                          int* max_icon_width) const;

  // Subclass can add extra parameters or replaces default configuration.
  virtual void AddCustomConfigValues(DictionaryValue* config) const {};

  // Create HTML Data source for the menu.  Extended menu
  // implementation may provide its own menu implmentation.
  virtual ChromeURLDataManager::DataSource* CreateDataSource();

  // A utility function that create a concrete html file from
  // template for given |menu_class|.
  static ChromeURLDataManager::DataSource* CreateMenuUIHTMLSource(
      const MenuUI& menu_ui,
      Profile* profile,
      const std::string& menu_class,
      const std::string& menu_source);

 private:
  DISALLOW_COPY_AND_ASSIGN(MenuUI);
};

// Base class for MenuUI's DOMMessageHandler.
class MenuHandlerBase : public DOMMessageHandler {
 public:
  MenuHandlerBase() : DOMMessageHandler() {}

  // Returns the menu control that is associated with the
  // MenuUI. This may return null when menu is being deleted.
  DOMUIMenuControl* GetMenuControl();

  // Returns the menu model for this menu ui.
  // This may return null when menu is being deleted.
  menus::MenuModel* GetMenuModel();

 private:
  DISALLOW_COPY_AND_ASSIGN(MenuHandlerBase);
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_DOM_UI_MENU_UI_H_
