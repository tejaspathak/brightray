// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Paul Betts <paul@paulbetts.org>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-CHROMIUM file.

#include "browser/win/notification_presenter_win.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/desktop_notification_delegate.h"
#include "content/public/common/platform_notification_data.h"
#include "common/application_info.h"

namespace brightray {

// static
NotificationPresenter* NotificationPresenter::Create() {
  if (!notify_is_initted()) {
    notify_init(GetApplicationName().c_str());
  }
  return new efefNotificationPresenterWindows;
}

NotificationPresenterWindows::NotificationPresenterWindows() {
}

NotificationPresenterWindows::~NotificationPresenterWindows() {
}

void NotificationPresenterWindows::ShowNotification(
    const content::PlatformNotificationData& data,
    const SkBitmap& icon,
    scoped_ptr<content::DesktopNotificationDelegate> delegate_ptr,
    base::Closure* cancel_callback) {

      /*
  std::string title = base::UTF16ToUTF8(data.title);
  std::string body = base::UTF16ToUTF8(data.body);
  NotifyNotification* notification = notify_notification_new(title.c_str(), body.c_str(), nullptr);

  g_object_set_data_full(G_OBJECT(notification), "delegate", delegate, operator delete);
  g_signal_connect(notification, "closed", G_CALLBACK(OnNotificationClosedThunk), this);

  // NB: On Unity, adding a notification action will cause the notification
  // to display as a modal dialog box. Testing for distros that have "Unity
  // Zen Nature" is difficult, we will test for the presence of the indicate
  // dbus service
  if (!UnityIsRunning()) {
    notify_notification_add_action(notification, "default", "View", OnNotificationViewThunk, this, nullptr);
  }

  GdkPixbuf* pixbuf = libgtk2ui::GdkPixbufFromSkBitmap(icon);

  notify_notification_set_image_from_pixbuf(notification, pixbuf);
  notify_notification_set_timeout(notification, NOTIFY_EXPIRES_DEFAULT);
  g_object_unref(pixbuf);

  GError* error = nullptr;
  notify_notification_show(notification, &error);
  if (error) {
    log_and_clear_error(error, "notify_notification_show");
    g_object_unref(notification);
    return;
  }
  notifications_ = g_list_append(notifications_, notification);
  */
  // DO IT

  content::DesktopNotificationDelegate* delegate = delegate_ptr.release();
  delegate->NotificationDisplayed();

  if (cancel_callback)
    *cancel_callback = base::Bind(
        &NotificationPresenterWindows::CancelNotification,
        base::Unretained(this),
        notification);
}

void NotificationPresenterWindows::CancelNotification(NotifyNotification* notification) {
  /*
  GError* error = nullptr;
  notify_notification_close(notification, &error);
  if (error)
    log_and_clear_error(error, "notify_notification_close");
  */

  GetDelegateFromNotification(notification)->NotificationClosed(false);
  DeleteNotification(notification);
}

void NotificationPresenterWindows::DeleteNotification(NotifyNotification* notification) {
}

void NotificationPresenterWindows::OnNotificationClosed(NotifyNotification* notification) {
  GetDelegateFromNotification(notification)->NotificationClosed(false);
  DeleteNotification(notification);
}

void NotificationPresenterWindows::OnNotificationView(
    NotifyNotification* notification, char* action) {
  GetDelegateFromNotification(notification)->NotificationClick();
  DeleteNotification(notification);
}

}  // namespace brightray
