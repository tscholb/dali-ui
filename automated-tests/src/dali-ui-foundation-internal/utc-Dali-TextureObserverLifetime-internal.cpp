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
 */

#include <dali-ui-foundation/internal/texture-manager/texture-cache-manager.h>
#include <dali-ui-test-suite-utils.h>
#include <functional>
#include <memory>
#define private public
#include <dali-ui-foundation/internal/texture-manager/texture-manager-impl.h>
#undef private

namespace
{
Dali::Ui::Internal::TextureManager::TextureId AddUploadedTexture(Dali::Ui::Internal::TextureManager& textureManager,
                                                                 const char*                         url,
                                                                 int32_t                             referenceCount)
{
  auto&      cache     = textureManager.mTextureCacheManager;
  auto       textureId = cache.GenerateTextureId();
  const auto visualUrl = Dali::Ui::Internal::VisualUrl(url);
  auto       hash      = cache.GenerateHash(visualUrl, Dali::ImageDimensions(), Dali::SamplingMode::BOX_THEN_LINEAR,
                                            Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID, false, true, 0u);

  Dali::Ui::Internal::TextureManager::TextureInfo textureInfo(
    textureId, Dali::Ui::Internal::TextureManager::INVALID_TEXTURE_ID, visualUrl, Dali::ImageDimensions(), 1.0f,
    Dali::SamplingMode::BOX_THEN_LINEAR, false, false, hash, true, false, Dali::AnimatedImageLoading(), 0u, false);
  textureInfo.referenceCount = referenceCount;
  textureInfo.loadState      = Dali::Ui::Internal::TextureManager::LoadState::UPLOADED;
  cache.AppendCache(textureInfo);

  return textureId;
}

class LifetimeObserver : public Dali::Ui::TextureUploadObserver
{
public:
  void LoadComplete(bool, TextureInformation) override
  {
    // The callback may destroy this observer.
    auto callback = onComplete;
    if(callback)
    {
      callback();
    }
  }

  std::function<void()> onComplete;
};

int CheckObserverLifetimeAfterCancellation(bool sameTexture)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;
  TextureManager manager;
  auto           observer = std::make_unique<LifetimeObserver>();
  auto*          address  = observer.get();
  const auto     first    = AddUploadedTexture(manager, "observer-first.png", 2);
  const auto     second   = sameTexture ? first : AddUploadedTexture(manager, "observer-second.png", 1);
  auto&          cache    = manager.mTextureCacheManager;
  manager.ObserveTexture(cache[cache.GetCacheIndexFromId(first)], address);
  manager.ObserveTexture(cache[cache.GetCacheIndexFromId(second)], address);

  manager.RequestRemove(first, address);
  observer.reset();
  const bool removed = cache[cache.GetCacheIndexFromId(second)].observerList.Empty();
  // Clean up on the unfixed implementation without dereferencing the stale pointer.
  manager.ObserverDestroyed(address);
  DALI_TEST_CHECK(removed);
  END_TEST;
}
} // unnamed namespace

int UtcDaliTextureManagerObserverLifetimeDuplicateCancellation(void)
{
  return CheckObserverLifetimeAfterCancellation(true);
}

int UtcDaliTextureManagerObserverLifetimeSharedCancellation(void)
{
  return CheckObserverLifetimeAfterCancellation(false);
}

int UtcDaliTextureManagerObserverLifetimeDestroyDuringCompletion(void)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;
  TextureManager manager;
  auto           observer = std::make_unique<LifetimeObserver>();
  auto*          address  = observer.get();
  const auto     first    = AddUploadedTexture(manager, "observer-complete.png", 1);
  const auto     second   = AddUploadedTexture(manager, "observer-pending.png", 1);
  auto&          cache    = manager.mTextureCacheManager;
  manager.ObserveTexture(cache[cache.GetCacheIndexFromId(first)], address);
  manager.ObserveTexture(cache[cache.GetCacheIndexFromId(second)], address);
  observer->onComplete = [&]
  { observer.reset(); };

  manager.NotifyObservers(cache[cache.GetCacheIndexFromId(first)], true);
  const bool removed = cache[cache.GetCacheIndexFromId(second)].observerList.Empty();
  manager.ObserverDestroyed(address);
  DALI_TEST_CHECK(!observer);
  DALI_TEST_CHECK(removed);
  END_TEST;
}

int UtcDaliTextureManagerObserverLifetimeQueuedCompletion(void)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;
  TextureManager   manager;
  LifetimeObserver observer;
  const auto       first  = AddUploadedTexture(manager, "observer-queued-first.png", 1);
  const auto       second = AddUploadedTexture(manager, "observer-queued-second.png", 1);
  auto&            cache  = manager.mTextureCacheManager;
  manager.QueueLoadTexture(cache[cache.GetCacheIndexFromId(first)], &observer);
  manager.QueueLoadTexture(cache[cache.GetCacheIndexFromId(second)], &observer);
  bool         connectedDuringFirstCallback = false;
  unsigned int count                        = 0u;
  observer.onComplete                       = [&]
  {
    if(++count == 1u)
    {
      connectedDuringFirstCallback = !observer.DestructionSignal().Empty();
    }
  };

  manager.ProcessLoadQueue();
  DALI_TEST_EQUALS(count, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.DestructionSignal().Empty());
  DALI_TEST_CHECK(connectedDuringFirstCallback);
  END_TEST;
}

int UtcDaliTextureManagerObserverLifetimeDestroyDuringQueuedCompletion(void)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;
  TextureManager manager;
  auto           observer = std::make_unique<LifetimeObserver>();
  const auto     first    = AddUploadedTexture(manager, "observer-destroy-queued-first.png", 1);
  const auto     second   = AddUploadedTexture(manager, "observer-destroy-queued-second.png", 1);
  auto&          cache    = manager.mTextureCacheManager;
  manager.QueueLoadTexture(cache[cache.GetCacheIndexFromId(first)], observer.get());
  manager.QueueLoadTexture(cache[cache.GetCacheIndexFromId(second)], observer.get());
  unsigned int count   = 0u;
  observer->onComplete = [&]
  {
    ++count;
    observer.reset();
  };

  manager.ProcessLoadQueue();
  DALI_TEST_CHECK(!observer);
  DALI_TEST_EQUALS(count, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(manager.mLoadQueue.Empty());
  END_TEST;
}

int UtcDaliTextureManagerObserverLifetimeListAndQueueCancellation(void)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;
  TextureManager manager;
  auto           observer = std::make_unique<LifetimeObserver>();
  auto*          address  = observer.get();
  const auto     first    = AddUploadedTexture(manager, "observer-list.png", 1);
  const auto     second   = AddUploadedTexture(manager, "observer-queue.png", 1);
  auto&          cache    = manager.mTextureCacheManager;
  manager.ObserveTexture(cache[cache.GetCacheIndexFromId(first)], address);
  manager.QueueLoadTexture(cache[cache.GetCacheIndexFromId(second)], address);

  manager.RequestRemove(first, address);
  observer.reset();
  const bool removed = manager.mLoadQueue[0].mTextureId == TextureManager::INVALID_TEXTURE_ID &&
                       manager.mLoadQueue[0].mObserver == nullptr;
  manager.ObserverDestroyed(address);
  manager.ProcessLoadQueue();
  DALI_TEST_CHECK(removed);
  END_TEST;
}

int UtcDaliTextureManagerObserverLifetimeTransferFromQueue(void)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;
  TextureManager   manager;
  LifetimeObserver observer;
  const auto       textureId   = AddUploadedTexture(manager, "observer-transferred.png", 1);
  auto&            cache       = manager.mTextureCacheManager;
  auto&            textureInfo = cache[cache.GetCacheIndexFromId(textureId)];
  textureInfo.loadState        = TextureManager::LoadState::LOADING;
  manager.QueueLoadTexture(textureInfo, &observer);

  manager.ProcessLoadQueue();
  DALI_TEST_CHECK(!observer.DestructionSignal().Empty());
  DALI_TEST_EQUALS(textureInfo.observerList.Count(), 1u, TEST_LOCATION);

  textureInfo.loadState = TextureManager::LoadState::UPLOADED;
  manager.NotifyObservers(textureInfo, true);
  DALI_TEST_CHECK(observer.DestructionSignal().Empty());
  END_TEST;
}

int UtcDaliTextureManagerObserverLifetimeClearRetainedCache(void)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;
  TextureManager   manager;
  LifetimeObserver observer;
  const auto       retainedId = AddUploadedTexture(manager, "observer-retained.png", 1);
  const auto       pendingId  = AddUploadedTexture(manager, "observer-still-pending.png", 1);
  auto&            cache      = manager.mTextureCacheManager;
  manager.ObserveTexture(cache[cache.GetCacheIndexFromId(retainedId)], &observer);
  manager.ObserveTexture(cache[cache.GetCacheIndexFromId(pendingId)], &observer);

  manager.RequestRemove(retainedId, nullptr, true);
  manager.ProcessRemoveQueue();
  DALI_TEST_CHECK(cache.GetCacheIndexFromId(retainedId) != TextureManager::INVALID_CACHE_INDEX);

  manager.RequestClearUnusedTextures();
  manager.ProcessRemoveQueue();
  DALI_TEST_CHECK(cache.GetCacheIndexFromId(retainedId) == TextureManager::INVALID_CACHE_INDEX);
  DALI_TEST_CHECK(!observer.DestructionSignal().Empty());

  manager.NotifyObservers(cache[cache.GetCacheIndexFromId(pendingId)], true);
  DALI_TEST_CHECK(observer.DestructionSignal().Empty());
  END_TEST;
}

int UtcDaliTextureManagerObserverLifetimeDiscardMissingTexture(void)
{
  UiTestApplication application;
  using TextureManager = Dali::Ui::Internal::TextureManager;
  TextureManager   manager;
  LifetimeObserver observer;
  const auto       textureId = AddUploadedTexture(manager, "observer-missing-texture.png", 1);
  auto&            cache     = manager.mTextureCacheManager;
  manager.QueueLoadTexture(cache[cache.GetCacheIndexFromId(textureId)], &observer);
  manager.RequestRemove(textureId, nullptr);
  manager.ProcessRemoveQueue();

  manager.ProcessLoadQueue();
  DALI_TEST_CHECK(observer.DestructionSignal().Empty());
  END_TEST;
}
