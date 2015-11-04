// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2013 Patrick Reynolds <piki@github.com>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-CHROMIUM file.

#include "browser/win/notification_presenter_win.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/files/file_enumerator.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/desktop_notification_delegate.h"
#include "content/public/common/platform_notification_data.h"
#include "common/application_info.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "third_party/skia/include/core/SkBitmap.h"
#include "base/win/windows_version.h"

// Windows Header
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace ABI::Windows::UI::Notifications;
using namespace ABI::Windows::Data::Xml::Dom;

namespace brightray {

namespace {

}  // namespace

// static
NotificationPresenter* NotificationPresenter::Create() {
  return new NotificationPresenterWin;
}

NotificationPresenterWin::NotificationPresenterWin() {
}

NotificationPresenterWin::~NotificationPresenterWin() {
}

void NotificationPresenterWin::ShowNotification(
  // https://msdn.microsoft.com/en-us/library/ee330740(v=VS.85).aspx
  // To display a notification, you must have an icon in the notification area. 
  // In certain cases, such as Microsoft Communicator or battery level, that icon will already be present. 
  // In many other cases, however, you will add an icon to the notification area only as long as is needed to show the notification. 
  // In either case, this is accomplished using the Shell_NotifyIcon function.

  const content::PlatformNotificationData& data,
  const SkBitmap& icon,
  scoped_ptr<content::DesktopNotificationDelegate> delegate_ptr,
  base::Closure* cancel_callback) {
  
  // Simple Fake Debugger, showing us what's going on
  FILE *out = fopen("C:\\Users\\feriese\\Desktop\\log.txt", "a");  
  fprintf(out, "%s", "Hi from notification!");  
  fclose(out);
}

void NotificationPresenterWin::CancelNotification() {
}

void NotificationPresenterWin::DeleteNotification() {
}

// Display the toast using classic COM. Note that is also possible to create and display the toast using the new C++ /ZW options (using handles,
// COM wrappers, etc.) 
HRESULT NotificationPresenterWin::DisplayToast()
{
    ComPtr<IToastNotificationManagerStatics> toastStatics;
    HRESULT hr = GetActivationFactory(StringReferenceWrapper(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager).Get(), &toastStatics);

    if (SUCCEEDED(hr))
    {
        ComPtr<IXmlDocument> toastXml;
        hr = CreateToastXml(toastStatics.Get(), &toastXml);
        if (SUCCEEDED(hr))
        {
            hr = CreateToast(toastStatics.Get(), toastXml.Get());
        }
    }
    return hr;
}

// Create the toast XML from a template
HRESULT NotificationPresenterWin::CreateToastXml(_In_ IToastNotificationManagerStatics *toastManager, _Outptr_ IXmlDocument** inputXml)
{    
    // Retrieve the template XML
    HRESULT hr = toastManager->GetTemplateContent(ToastTemplateType_ToastImageAndText04, inputXml);
    if (SUCCEEDED(hr))
    {    
        wchar_t *imagePath = _wfullpath(nullptr, L"toastImageAndText.png", MAX_PATH);

        hr = imagePath != nullptr ? S_OK : HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND); 
        if (SUCCEEDED(hr))
        {
            hr = SetImageSrc(imagePath, *inputXml);
            if (SUCCEEDED(hr))
            {
                // TODO
                wchar_t* textValues[] = {
                    L"Line 1",
                    L"Line 2",
                    L"Line 3"
                };

                UINT32 textLengths[] = {6, 6, 6};

                hr = SetTextValues(textValues, 3, textLengths, *inputXml);
            }
        }
    }
    return hr;
}

// Set the value of the "src" attribute of the "image" node
HRESULT NotificationPresenterWin::SetImageSrc(_In_z_ wchar_t *imagePath, _In_ IXmlDocument *toastXml)
{
    wchar_t imageSrc[MAX_PATH] = L"file:///";
    HRESULT hr = StringCchCat(imageSrc, ARRAYSIZE(imageSrc), imagePath);
    if (SUCCEEDED(hr))
    {
        ComPtr<IXmlNodeList> nodeList;
        hr = toastXml->GetElementsByTagName(StringReferenceWrapper(L"image").Get(), &nodeList);
        if (SUCCEEDED(hr))
        {
            ComPtr<IXmlNode> imageNode;
            hr = nodeList->Item(0, &imageNode);
            if (SUCCEEDED(hr))
            {
                ComPtr<IXmlNamedNodeMap> attributes;

                hr = imageNode->get_Attributes(&attributes);
                if (SUCCEEDED(hr))
                {
                    ComPtr<IXmlNode> srcAttribute;

                    hr = attributes->GetNamedItem(StringReferenceWrapper(L"src").Get(), &srcAttribute);
                    if (SUCCEEDED(hr))
                    {
                        hr = SetNodeValueString(StringReferenceWrapper(imageSrc).Get(), srcAttribute.Get(), toastXml);
                    }
                }
            }
        }
    }
    return hr;
}

// Set the values of each of the text nodes
HRESULT NotificationPresenterWin::SetTextValues(_In_reads_(textValuesCount) wchar_t **textValues, _In_ UINT32 textValuesCount, _In_reads_(textValuesCount) UINT32 *textValuesLengths, _In_ IXmlDocument *toastXml)
{
    HRESULT hr = textValues != nullptr && textValuesCount > 0 ? S_OK : E_INVALIDARG;
    if (SUCCEEDED(hr))
    {
        ComPtr<IXmlNodeList> nodeList;
        hr = toastXml->GetElementsByTagName(StringReferenceWrapper(L"text").Get(), &nodeList);
        if (SUCCEEDED(hr))
        {
            UINT32 nodeListLength;
            hr = nodeList->get_Length(&nodeListLength);
            if (SUCCEEDED(hr))
            {
                hr = textValuesCount <= nodeListLength ? S_OK : E_INVALIDARG;
                if (SUCCEEDED(hr))
                {
                    for (UINT32 i = 0; i < textValuesCount; i++)
                    {
                        ComPtr<IXmlNode> textNode;
                        hr = nodeList->Item(i, &textNode);
                        if (SUCCEEDED(hr))
                        {
                            hr = SetNodeValueString(StringReferenceWrapper(textValues[i], textValuesLengths[i]).Get(), textNode.Get(), toastXml);
                        }
                    }
                }
            }
        }
    }
    return hr;
}

HRESULT NotificationPresenterWin::SetNodeValueString(_In_ HSTRING inputString, _In_ IXmlNode *node, _In_ IXmlDocument *xml)
{
    ComPtr<IXmlText> inputText;

    HRESULT hr = xml->CreateTextNode(inputString, &inputText);
    if (SUCCEEDED(hr))
    {
        ComPtr<IXmlNode> inputTextNode;

        hr = inputText.As(&inputTextNode);
        if (SUCCEEDED(hr))
        {
            ComPtr<IXmlNode> pAppendedChild;
            hr = node->AppendChild(inputTextNode.Get(), &pAppendedChild);
        }
    }

    return hr;
}

// Create and display the toast
HRESULT NotificationPresenterWin::CreateToast(_In_ IToastNotificationManagerStatics *toastManager, _In_ IXmlDocument *xml)
{
    ComPtr<IToastNotifier> notifier;
    HRESULT hr = toastManager->CreateToastNotifierWithId(StringReferenceWrapper(AppId).Get(), &notifier);
    if (SUCCEEDED(hr))
    {
        ComPtr<IToastNotificationFactory> factory;
        hr = GetActivationFactory(StringReferenceWrapper(RuntimeClass_Windows_UI_Notifications_ToastNotification).Get(), &factory);
        if (SUCCEEDED(hr))
        {
            ComPtr<IToastNotification> toast;
            hr = factory->CreateToastNotification(xml, &toast);
            if (SUCCEEDED(hr))
            {
                // TODO: Register the event handlers
                // EventRegistrationToken activatedToken, dismissedToken, failedToken;
                // ComPtr<ToastEventHandler> eventHandler(new ToastEventHandler(_hwnd, _hEdit));
             
                // hr = toast->add_Activated(eventHandler.Get(), &activatedToken);
                // if (SUCCEEDED(hr))
                // {
                //     hr = toast->add_Dismissed(eventHandler.Get(), &dismissedToken);
                //     if (SUCCEEDED(hr))
                //     {
                //         hr = toast->add_Failed(eventHandler.Get(), &failedToken);
                //         if (SUCCEEDED(hr))
                //         {
                //             hr = notifier->Show(toast.Get());
                //         }
                //     }
                // }
            }
        }
    }
    return hr;
}

}  // namespace brightray
