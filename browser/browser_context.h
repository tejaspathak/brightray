// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-CHROMIUM file.

#ifndef __brightray__browser_context__
#define __brightray__browser_context__

#include "content/public/browser/browser_context.h"
#include "content/public/browser/content_browser_client.h"

namespace brightray {

class URLRequestContextGetter;

class BrowserContext : public content::BrowserContext {
public:
  BrowserContext();
  ~BrowserContext();

  net::URLRequestContextGetter* CreateRequestContext(content::ProtocolHandlerMap*);

private:
  class ResourceContext;

  virtual base::FilePath GetPath() OVERRIDE;
  virtual bool IsOffTheRecord() const OVERRIDE;
  virtual net::URLRequestContextGetter* GetRequestContext() OVERRIDE;
  virtual net::URLRequestContextGetter* GetRequestContextForRenderProcess(int renderer_child_id);
  virtual net::URLRequestContextGetter* GetMediaRequestContext() OVERRIDE;
  virtual net::URLRequestContextGetter* GetMediaRequestContextForRenderProcess(int renderer_child_id) OVERRIDE;
  virtual net::URLRequestContextGetter* GetMediaRequestContextForStoragePartition(const base::FilePath& partition_path, bool in_memory);
  virtual content::ResourceContext* GetResourceContext() OVERRIDE;
  virtual content::DownloadManagerDelegate* GetDownloadManagerDelegate() OVERRIDE;
  virtual content::GeolocationPermissionContext* GetGeolocationPermissionContext() OVERRIDE;
  virtual content::SpeechRecognitionPreferences* GetSpeechRecognitionPreferences() OVERRIDE;
  virtual quota::SpecialStoragePolicy* GetSpecialStoragePolicy() OVERRIDE;

  scoped_ptr<ResourceContext> resource_context_;
  scoped_ptr<URLRequestContextGetter> url_request_getter_;

  DISALLOW_COPY_AND_ASSIGN(BrowserContext);
};

}

#endif /* defined(__brightray__browser_context__) */