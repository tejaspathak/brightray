// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2013 Patrick Reynolds <piki@github.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-CHROMIUM file.

#ifndef BRIGHTRAY_BROWSER_NOTIFICATION_PRESENTER_WIN_H_
#define BRIGHTRAY_BROWSER_NOTIFICATION_PRESENTER_WIN_H_

#include "base/compiler_specific.h"
#include "browser/notification_presenter.h"

#include "windows_headers.h"
#include "string_reference_wrapper.h"
#include "toast_event_handler.h"

using namespace Microsoft::WRL;
using namespace ABI::Windows::UI::Notifications;
using namespace ABI::Windows::Data::Xml::Dom;
using namespace Windows::Foundation;

const wchar_t AppId[] = L"Electron.Windows.DesktopToast";

namespace brightray {

class NotificationPresenterWin : public NotificationPresenter {
 public:
  NotificationPresenterWin();
  ~NotificationPresenterWin();

  // NotificationPresenter:
  void ShowNotification(
      const content::PlatformNotificationData&,
      const SkBitmap& icon,
      scoped_ptr<content::DesktopNotificationDelegate> delegate,
      base::Closure* cancel_callback) override;
  
  void RemoveNotification();

 private:

  void CancelNotification();
  void DeleteNotification();
  
  HRESULT DisplayToast();
  HRESULT CreateToastXml(
  _In_ ABI::Windows::UI::Notifications::IToastNotificationManagerStatics *toastManager, 
  _Outptr_ ABI::Windows::Data::Xml::Dom::IXmlDocument **xml
  );

  HRESULT CreateToast(
  _In_ ABI::Windows::UI::Notifications::IToastNotificationManagerStatics *toastManager, 
  _In_ ABI::Windows::Data::Xml::Dom::IXmlDocument *xml
  );
  HRESULT SetImageSrc(
  _In_z_ wchar_t *imagePath, 
  _In_ ABI::Windows::Data::Xml::Dom::IXmlDocument *toastXml
  );
  HRESULT SetTextValues(
  _In_reads_(textValuesCount) wchar_t **textValues, 
  _In_ UINT32 textValuesCount, 
  _In_reads_(textValuesCount) UINT32 *textValuesLengths, 
  _In_ ABI::Windows::Data::Xml::Dom::IXmlDocument *toastXml
  );
  HRESULT SetNodeValueString(
  _In_ HSTRING onputString,
  _In_ ABI::Windows::Data::Xml::Dom::IXmlNode *node, 
  _In_ ABI::Windows::Data::Xml::Dom::IXmlDocument *xml
  );

};

}  // namespace brightray

#endif
