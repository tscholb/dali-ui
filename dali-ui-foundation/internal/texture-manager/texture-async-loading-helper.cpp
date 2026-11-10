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

// CLASS HEADER
#include <dali-ui-foundation/internal/texture-manager/texture-async-loading-helper.h>

// EXTERNAL HEADERS
#include <dali/integration-api/adaptor-framework/file-download/remote-file-download-manager.h>
#include <dali/integration-api/debug.h>

// INTERNAL HEADERS
#include <dali-ui-foundation/internal/image-loader/async-image-loader-impl.h>
#include <dali-ui-foundation/internal/image-loader/remote-decode-task.h>
#include <dali-ui-foundation/public-api/image-loader/sync-image-loader.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
#ifdef DEBUG_ENABLED
extern Debug::Filter* gTextureManagerLogFilter; ///< Define at texture-manager-impl.cpp
#endif

namespace
{
constexpr size_t MAXIMUM_DOWNLOAD_IMAGE_SIZE = 50 * 1024 * 1024;
} // namespace

TextureAsyncLoadingHelper::TextureAsyncLoadingHelper(TextureManager& textureManager)
: mTextureManager(textureManager),
  mLoadTaskId(0u)
{
}

void TextureAsyncLoadingHelper::LoadAnimatedImage(const TextureManager::TextureId textureId,
                                                  Dali::AnimatedImageLoading      animatedImageLoading,
                                                  const uint32_t frameIndex, const Dali::ImageDimensions& desiredSize,
                                                  const Dali::SamplingMode::Type                 samplingMode,
                                                  const Dali::Ui::Integration::PreMultiplyOnLoad preMultiplyOnLoad,
                                                  const bool                                     loadYuvPlanes)
{
  LoadingTaskPtr loadingTask = new LoadingTask(++mLoadTaskId, animatedImageLoading, frameIndex, desiredSize,
                                               samplingMode, preMultiplyOnLoad, loadYuvPlanes,
                                               MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
  loadingTask->SetTextureId(textureId);
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
}

void TextureAsyncLoadingHelper::Load(const TextureManager::TextureId textureId, const VisualUrl& url,
                                     const Dali::ImageDimensions&   desiredSize,
                                     const Dali::SamplingMode::Type samplingMode, const bool orientationCorrection,
                                     const Dali::Ui::Integration::PreMultiplyOnLoad preMultiplyOnLoad,
                                     const bool                                     loadYuvPlanes)
{
  LoadingTaskPtr loadingTask;
  if(DALI_UNLIKELY(url.IsBufferResource()))
  {
    loadingTask = new LoadingTask(++mLoadTaskId, mTextureManager.GetEncodedImageBuffer(url), desiredSize,
                                  samplingMode, orientationCorrection, preMultiplyOnLoad,
                                  MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
  }
  else if(!url.IsLocalResource() && Dali::RemoteFileDownloadManager::IsAsyncDownloadSupported())
  {
    auto asyncTaskManager = Dali::AsyncTaskManager::Get();
    auto weakLifetime     = std::weak_ptr<void>(mLifetimeToken);

    Dali::RemoteFileDownloadManager::Get().StartDownload(
      url.GetUrl(), MAXIMUM_DOWNLOAD_IMAGE_SIZE,
      [this, weakLifetime, asyncTaskManager, textureId, desiredSize, samplingMode, orientationCorrection, preMultiplyOnLoad, loadYuvPlanes](bool success, const std::string& localFilePath) mutable
    {
      // The provider callback may run on another thread. Only enqueue decode work here.
      if(weakLifetime.expired())
      {
        return;
      }

      auto callback = Dali::Callback<void(Dali::AsyncTaskPtr)>::New(
        [this, weakLifetime](Dali::AsyncTaskPtr task)
      {
        // Decode completion and helper destruction both run on the event thread.
        if(!weakLifetime.expired())
        {
          RemoteDecodeComplete(task);
        }
      });

      RemoteDecodeTaskPtr decodeTask = new RemoteDecodeTask(
        textureId, success ? localFilePath : std::string{}, desiredSize, samplingMode,
        orientationCorrection, preMultiplyOnLoad, loadYuvPlanes, callback.Release());
      asyncTaskManager.AddTask(decodeTask);
    });
    return;
  }
  else
  {
    loadingTask = new LoadingTask(++mLoadTaskId, url, desiredSize, samplingMode, orientationCorrection,
                                  preMultiplyOnLoad, loadYuvPlanes,
                                  MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
  }

  loadingTask->SetTextureId(textureId);
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
}

void TextureAsyncLoadingHelper::ApplyMask(const TextureManager::TextureId textureId, PixelBuffer pixelBuffer,
                                          PixelBuffer maskPixelBuffer, const float contentScale,
                                          const bool                                     cropToMask,
                                          const Dali::Ui::Integration::PreMultiplyOnLoad preMultiplyOnLoad)
{
  LoadingTaskPtr loadingTask =
    new LoadingTask(++mLoadTaskId, pixelBuffer, maskPixelBuffer, contentScale, cropToMask, preMultiplyOnLoad,
                    MakeCallback(this, &TextureAsyncLoadingHelper::AsyncLoadComplete));
  loadingTask->SetTextureId(textureId);
  Dali::AsyncTaskManager::Get().AddTask(loadingTask);
}

void TextureAsyncLoadingHelper::AsyncLoadComplete(LoadingTaskPtr task)
{
  // Call TextureManager::AsyncLoadComplete
  if(task->textureId != TextureManager::INVALID_TEXTURE_ID)
  {
    mTextureManager.AsyncLoadComplete(task->textureId, task->pixelBuffers);
  }
}

void TextureAsyncLoadingHelper::RemoteDecodeComplete(Dali::AsyncTaskPtr task)
{
  auto* decodeTask = static_cast<RemoteDecodeTask*>(task.Get());
  if(decodeTask->textureId != TextureManager::INVALID_TEXTURE_ID)
  {
    mTextureManager.AsyncLoadComplete(decodeTask->textureId, decodeTask->pixelBuffers);
  }
}

} // namespace Internal

} // namespace Ui

} //namespace DALI_NAMESPACE
