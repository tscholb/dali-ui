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

#include <dali-ui-foundation/integration-api/image-view/image-view-impl.h>
#include <dali-ui-foundation/public-api/image-view/image-view.h>
#include <dali-ui-foundation/public-api/ui-color.h>

namespace Dali
{
namespace Ui
{

inline Integration::ImageViewImpl& GetImpl(ImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<Integration::ImageViewImpl&>(obj.GetImplementation());
}

inline const Integration::ImageViewImpl& GetImpl(const ImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<const Integration::ImageViewImpl&>(obj.GetImplementation());
}

ImageView::ImageView()
{
}

ImageView::ImageView(const ImageView& imageView)            = default;
ImageView::ImageView(ImageView&& rhs) noexcept              = default;
ImageView& ImageView::operator=(const ImageView& imageView) = default;
ImageView& ImageView::operator=(ImageView&& rhs) noexcept   = default;

ImageView::~ImageView()
{
}

ImageView ImageView::New()
{
  Integration::ImageViewImplPtr impl = Integration::ImageViewImpl::New();
  ImageView                     view(*impl);
  impl->Initialize();
  return view;
}

ImageView ImageView::New(const Dali::String& url)
{
  Integration::ImageViewImplPtr impl = Integration::ImageViewImpl::New();
  ImageView                     view(*impl);
  impl->Initialize();
  if(!url.Empty())
  {
    view.SetImage(url);
  }
  return view;
}

ImageView ImageView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<ImageView, Ui::Integration::ImageViewImpl>(handle);
}

void ImageView::Reload()
{
  Ui::GetImpl(*this).Reload();
}

ImageView& ImageView::SetImage(const Dali::String& url)
{
  Ui::GetImpl(*this).SetImage(url);
  return *this;
}

Dali::String ImageView::GetUrl() const
{
  return Ui::GetImpl(*this).GetUrl();
}

ImageView& ImageView::SetPixelArea(const Vector4& pixelArea)
{
  Ui::GetImpl(*this).SetPixelArea(pixelArea);
  return *this;
}

Vector4 ImageView::GetPixelArea() const
{
  return Ui::GetImpl(*this).GetPixelArea();
}

ImageView& ImageView::SetPreMultipliedAlpha(bool preMultiplied)
{
  Ui::GetImpl(*this).SetPreMultipliedAlpha(preMultiplied);
  return *this;
}

bool ImageView::GetPreMultipliedAlpha() const
{
  return Ui::GetImpl(*this).GetPreMultipliedAlpha();
}

ImageView& ImageView::SetSamplingMode(Dali::SamplingMode::Type samplingMode)
{
  Ui::GetImpl(*this).SetSamplingMode(samplingMode);
  return *this;
}

Dali::SamplingMode::Type ImageView::GetSamplingMode() const
{
  return Ui::GetImpl(*this).GetSamplingMode();
}

ImageView& ImageView::SetFittingMode(Ui::FittingMode::Type fittingMode)
{
  Ui::GetImpl(*this).SetFittingMode(fittingMode);
  return *this;
}

Ui::FittingMode::Type ImageView::GetFittingMode() const
{
  return Ui::GetImpl(*this).GetFittingMode();
}

ImageView& ImageView::SetDesiredSize(Ui::ImageDimensions size)
{
  Ui::GetImpl(*this).SetDesiredSize(size);
  return *this;
}

Ui::ImageDimensions ImageView::GetDesiredSize() const
{
  return Ui::GetImpl(*this).GetDesiredSize();
}

ImageView& ImageView::SetAlphaMaskUrl(const Dali::String& maskUrl)
{
  Ui::GetImpl(*this).SetAlphaMaskUrl(maskUrl);
  return *this;
}

Dali::String ImageView::GetAlphaMaskUrl() const
{
  return Ui::GetImpl(*this).GetAlphaMaskUrl();
}

ImageView& ImageView::SetCropToMask(bool cropToMask)
{
  Ui::GetImpl(*this).SetCropToMask(cropToMask);
  return *this;
}

bool ImageView::GetCropToMask() const
{
  return Ui::GetImpl(*this).GetCropToMask();
}

ImageView& ImageView::SetReleasePolicy(Ui::ReleasePolicy::Type releasePolicy)
{
  Ui::GetImpl(*this).SetReleasePolicy(releasePolicy);
  return *this;
}

Ui::ReleasePolicy::Type ImageView::GetReleasePolicy() const
{
  return Ui::GetImpl(*this).GetReleasePolicy();
}

ImageView& ImageView::SetSynchronousLoading(bool synchronous)
{
  Ui::GetImpl(*this).SetSynchronousLoading(synchronous);
  return *this;
}

bool ImageView::GetSynchronousLoading() const
{
  return Ui::GetImpl(*this).GetSynchronousLoading();
}

ImageView& ImageView::SetPlaceholderImage(const Dali::String& url)
{
  Ui::GetImpl(*this).SetPlaceholderImage(url);
  return *this;
}

Dali::String ImageView::GetPlaceholderImage() const
{
  return Ui::GetImpl(*this).GetPlaceholderImage();
}

ImageView& ImageView::SetWrapModeU(Ui::WrapMode::Type wrapMode)
{
  Ui::GetImpl(*this).SetWrapModeU(wrapMode);
  return *this;
}

Ui::WrapMode::Type ImageView::GetWrapModeU() const
{
  return Ui::GetImpl(*this).GetWrapModeU();
}

ImageView& ImageView::SetWrapModeV(Ui::WrapMode::Type wrapMode)
{
  Ui::GetImpl(*this).SetWrapModeV(wrapMode);
  return *this;
}

Ui::WrapMode::Type ImageView::GetWrapModeV() const
{
  return Ui::GetImpl(*this).GetWrapModeV();
}

ImageView& ImageView::SetSynchronousSizing(bool synchronous)
{
  Ui::GetImpl(*this).SetSynchronousSizing(synchronous);
  return *this;
}

bool ImageView::GetSynchronousSizing() const
{
  return Ui::GetImpl(*this).GetSynchronousSizing();
}

ImageView& ImageView::SetMaskingMode(Ui::MaskingType::Type maskingMode)
{
  Ui::GetImpl(*this).SetMaskingMode(maskingMode);
  return *this;
}

Ui::MaskingType::Type ImageView::GetMaskingMode() const
{
  return Ui::GetImpl(*this).GetMaskingMode();
}

ImageView& ImageView::SetImageColor(const UiColor& color)
{
  Ui::GetImpl(*this).SetImageColor(color);
  return *this;
}

UiColor ImageView::GetImageColor() const
{
  return Ui::GetImpl(*this).GetImageColor();
}

ImageView& ImageView::SetFastTrackUploading(bool fastTrack)
{
  Ui::GetImpl(*this).SetFastTrackUploading(fastTrack);
  return *this;
}

bool ImageView::GetFastTrackUploading() const
{
  return Ui::GetImpl(*this).GetFastTrackUploading();
}

ImageView& ImageView::SetOrientationCorrection(bool orientationCorrection)
{
  Ui::GetImpl(*this).SetOrientationCorrection(orientationCorrection);
  return *this;
}

bool ImageView::GetOrientationCorrection() const
{
  return Ui::GetImpl(*this).GetOrientationCorrection();
}

ImageView& ImageView::SetBorder(const Vector4& border)
{
  Ui::GetImpl(*this).SetBorder(border);
  return *this;
}

Vector4 ImageView::GetBorder() const
{
  return Ui::GetImpl(*this).GetBorder();
}

ImageView& ImageView::SetBorderOnly(bool borderOnly)
{
  Ui::GetImpl(*this).SetBorderOnly(borderOnly);
  return *this;
}

bool ImageView::GetBorderOnly() const
{
  return Ui::GetImpl(*this).GetBorderOnly();
}

ImageView& ImageView::SetAdjustViewSize(bool adjustViewSize)
{
  Ui::GetImpl(*this).SetAdjustViewSize(adjustViewSize);
  return *this;
}

bool ImageView::GetAdjustViewSize() const
{
  return Ui::GetImpl(*this).GetAdjustViewSize();
}

ImageView& ImageView::SetDepthIndex(int depthIndex)
{
  Ui::GetImpl(*this).SetDepthIndex(depthIndex);
  return *this;
}

Ui::Visual::ResourceStatus ImageView::GetLoadingStatus() const
{
  return Ui::GetImpl(*this).GetLoadingStatus();
}

View::ResourceReadySignalType& ImageView::ResourceReadySignal()
{
  return View::ResourceReadySignal();
}

ImageView::ImageView(Integration::ImageViewImpl& implementation)
: View(implementation)
{
}

ImageView::ImageView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::ImageViewImpl>(internal);
}

} // namespace Ui
} // namespace Dali
