/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali-ui-foundation/internal/image-loader/remote-decode-task.h>
#include <dali-ui-foundation/internal/texture-manager/texture-manager-impl.h>
#include <dali-ui-foundation/internal/texture-manager/texture-upload-observer.h>
#include <dali-ui-test-suite-utils.h>
#include <ui-environment-variable.h>
#include <ui-event-thread-callback.h>

#include <dlfcn.h>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>

using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::Internal::RemoteDecodeTask;
using Dali::Ui::Internal::TextureManager;
using Dali::Ui::Internal::VisualUrl;

namespace
{
const std::string IMAGE_PATH = std::string(__FILE__).substr(0u, std::string(__FILE__).find("/automated-tests/")) +
                               "/samples/image-view/res/sample.jpg";
const std::string ALPHA_IMAGE_PATH = std::string(__FILE__).substr(0u, std::string(__FILE__).find("/automated-tests/")) +
                                     "/samples/image-view/res/contact-cards-mask.png";

struct Observer : public TextureUploadObserver
{
  void LoadComplete(bool success, TextureInformation information) override
  {
    ++calls;
    loaded  = success;
    texture = information.textureSet;
  }
  unsigned int calls{0u};
  bool         loaded{false};
  TextureSet   texture;
};

// Hold requests until each test explicitly completes them; no network or timing assumptions.
struct DownloadPlugin
{
  DownloadPlugin()
  {
    EnvironmentVariable::SetTestEnvironmentVariable("DALI_FILE_DOWNLOAD_PLUGIN_NAME", ADDON_LIBS_PATH "/libremote-image-test-plugin.so");
    library = dlopen(ADDON_LIBS_PATH "/libremote-image-test-plugin.so", RTLD_NOW);
    if(library)
    {
      wait      = reinterpret_cast<bool (*)()>(dlsym(library, "TestFileDownloadWaitForRequest"));
      complete  = reinterpret_cast<void (*)(bool, const char*)>(dlsym(library, "TestFileDownloadComplete"));
      syncCount = reinterpret_cast<unsigned int (*)()>(dlsym(library, "TestFileDownloadSyncCount"));
    }
  }

  ~DownloadPlugin()
  {
    if(library)
    {
      dlclose(library);
    }
    EnvironmentVariable::SetTestEnvironmentVariable("DALI_FILE_DOWNLOAD_PLUGIN_NAME", nullptr);
    unsetenv("DALI_TEST_REMOTE_FILE_DOWNLOAD_PATH");
    unsetenv("DALI_TEST_REMOTE_ASYNC_UNSUPPORTED");
    unsetenv("DALI_TEST_REMOTE_START_FAIL");
  }

  bool Valid() const
  {
    return library && wait && complete && syncCount;
  }

  void Complete(bool success, const std::string& path = IMAGE_PATH)
  {
    std::thread([&]
    { complete(success, path.c_str()); })
      .join();
  }

  void* library{nullptr};
  bool (*wait)(){nullptr};
  void (*complete)(bool, const char*){nullptr};
  unsigned int (*syncCount)(){nullptr};
};

TextureManager::TextureId RequestRemote(TextureManager& manager, Observer& observer)
{
  auto preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  return manager.RequestLoad(VisualUrl("https://dali.test/image.jpg"), ImageDimensions(32u, 32u),
                             SamplingMode::BOX_THEN_LINEAR, &observer, true,
                             TextureManager::ReloadPolicy::FORCED, preMultiply);
}
} // namespace

int UtcDaliRemoteImageDownloadSuccessP(void)
{
  DownloadPlugin plugin;
  DALI_TEST_CHECK(plugin.Valid());
  UiTestApplication application;
  Observer          observer;
  TextureManager    manager;
  DALI_TEST_CHECK(RequestRemote(manager, observer) != TextureManager::INVALID_TEXTURE_ID);
  DALI_TEST_CHECK(plugin.wait());
  DALI_TEST_EQUALS(observer.calls, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(plugin.syncCount(), 0u, TEST_LOCATION);

  // Local loads can finish while the remote provider is still holding its request.
  Observer localObserver;
  auto     preMultiply = TextureManager::MultiplyOnLoad::LOAD_WITHOUT_MULTIPLY;
  manager.RequestLoad(VisualUrl(IMAGE_PATH), ImageDimensions(), SamplingMode::BOX_THEN_LINEAR,
                      &localObserver, true, TextureManager::ReloadPolicy::CACHED, preMultiply);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  DALI_TEST_CHECK(localObserver.loaded);
  DALI_TEST_EQUALS(observer.calls, 0u, TEST_LOCATION);

  plugin.Complete(true);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  DALI_TEST_EQUALS(observer.calls, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.loaded);
  DALI_TEST_CHECK(observer.texture.GetTexture(0));
  DALI_TEST_EQUALS(plugin.syncCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRemoteImageDownloadFailureN(void)
{
  DownloadPlugin plugin;
  DALI_TEST_CHECK(plugin.Valid());
  UiTestApplication application;
  Observer          observer;
  TextureManager    manager;
  RequestRemote(manager, observer);
  DALI_TEST_CHECK(plugin.wait());
  plugin.Complete(false);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  DALI_TEST_EQUALS(observer.calls, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!observer.loaded);
  END_TEST;
}

int UtcDaliRemoteImageDecodeFailureN(void)
{
  DownloadPlugin plugin;
  DALI_TEST_CHECK(plugin.Valid());
  UiTestApplication application;
  Observer          observer;
  TextureManager    manager;
  RequestRemote(manager, observer);
  DALI_TEST_CHECK(plugin.wait());
  plugin.Complete(true, IMAGE_PATH + ".missing");
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  DALI_TEST_EQUALS(observer.calls, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!observer.loaded);
  END_TEST;
}

int UtcDaliRemoteImageDownloadStartFailureN(void)
{
  DownloadPlugin plugin;
  DALI_TEST_CHECK(plugin.Valid());
  setenv("DALI_TEST_REMOTE_START_FAIL", "1", 1);
  UiTestApplication application;
  Observer          observer;
  TextureManager    manager;
  RequestRemote(manager, observer);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  DALI_TEST_EQUALS(observer.calls, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!observer.loaded);
  END_TEST;
}

int UtcDaliRemoteImageDownloadSyncFallbackP(void)
{
  DownloadPlugin plugin;
  DALI_TEST_CHECK(plugin.Valid());
  setenv("DALI_TEST_REMOTE_ASYNC_UNSUPPORTED", "1", 1);
  setenv("DALI_TEST_REMOTE_FILE_DOWNLOAD_PATH", IMAGE_PATH.c_str(), 1);
  UiTestApplication application;
  Observer          observer;
  TextureManager    manager;
  RequestRemote(manager, observer);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  DALI_TEST_EQUALS(observer.calls, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.loaded);
  DALI_TEST_EQUALS(plugin.syncCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRemoteImageDownloadHelperDestroyedP(void)
{
  DownloadPlugin plugin;
  DALI_TEST_CHECK(plugin.Valid());
  UiTestApplication application;
  Observer          observer;
  auto              manager = std::make_unique<TextureManager>();
  RequestRemote(*manager, observer);
  DALI_TEST_CHECK(plugin.wait());
  manager.reset();
  plugin.Complete(true);
  DALI_TEST_EQUALS(observer.calls, 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRemoteImageDecodeHelperDestroyedP(void)
{
  DownloadPlugin plugin;
  DALI_TEST_CHECK(plugin.Valid());
  UiTestApplication application;
  Observer          observer;
  auto              manager = std::make_unique<TextureManager>();
  RequestRemote(*manager, observer);
  DALI_TEST_CHECK(plugin.wait());
  plugin.Complete(true);
  // The decode task has been submitted, but its event-thread callback has not run.
  manager.reset();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  DALI_TEST_EQUALS(observer.calls, 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRemoteImageDecodePremultiplyAndResizeP(void)
{
  UiTestApplication application;
  RemoteDecodeTask  task(1, ALPHA_IMAGE_PATH, ImageDimensions(32u, 32u), SamplingMode::BOX_THEN_LINEAR,
                         true, Dali::Ui::Integration::PreMultiplyOnLoad::ON, false, nullptr);
  task.Process();
  DALI_TEST_EQUALS(task.pixelBuffers.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(task.pixelBuffers[0].GetWidth(), 32u, TEST_LOCATION);
  DALI_TEST_EQUALS(task.pixelBuffers[0].GetHeight(), 32u, TEST_LOCATION);
  DALI_TEST_CHECK(task.pixelBuffers[0].IsAlphaPreMultiplied());
  END_TEST;
}
