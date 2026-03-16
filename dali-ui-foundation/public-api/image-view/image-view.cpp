#include <dali-ui-foundation/integration-api/image-view/image-view-impl.h>
#include <dali-ui-foundation/public-api/image-view/image-view.h>
#include <dali-ui-foundation/public-api/ui-color.h>

namespace Dali
{
namespace Ui
{

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
  return Ui::Integration::ImageViewImpl::New();
}

ImageView ImageView::New(const Dali::String& url)
{
  ImageView imageView = Integration::ImageViewImpl::New();
  imageView.SetImage(url);
  return imageView;
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

Ui::Visual::ResourceStatus ImageView::GetLoadingStatus() const
{
  return Ui::GetImpl(*this).GetLoadingStatus();
}

ImageView::ImageViewSignal& ImageView::ResourceReadySignal()
{
  return Ui::GetImpl(*this).ResourceReadySignal();
}

ImageView::ImageViewSignal& ImageView::ResourceLoadedSignal()
{
  return Ui::GetImpl(*this).ResourceLoadedSignal();
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
