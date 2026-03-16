/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/image-view/image-view.h>
#include <dali-ui-foundation/public-api/stack-layout.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * ImageView sample: demonstrates the explicit C++ API for ImageView.
 * - ResourceReadySignal callback
 * - Press Escape or Back to quit.
 *
 * ResourceReady signal test:
 *   [A] ImageView::ResourceReadySignal() — per-type signal (mResourceReadySignal)
 *   [B] View::ResourceReadySignal()      — base View signal (ViewDataImpl)
 *   Both are connected. Check which one(s) actually fire in the log.
 */
class ImageViewSampleController : public ConnectionTracker
{
public:
  explicit ImageViewSampleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ImageViewSampleController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window     = application.GetWindow();
    auto   windowSize = window.GetSize();
    float  halfW      = windowSize.GetWidth() * 0.5f;
    float  halfH      = windowSize.GetHeight() * 0.5f;
    window.SetBackgroundColor(Color::DARK_GRAY);

    window.Add(
      ImageView::New(RESOURCES_DIR "sample.jpg")
        .SetRequestedWidth(halfW - 4.0f)
        .SetRequestedHeight(halfH - 4.0f)
        .SetPositionX(0.0f)
        .SetPositionY(0.0f)
        .As(mImage));

    // [A] Connect ImageView-specific ResourceReadySignal
    DALI_LOG_RELEASE_INFO("[ResourceReady] Connecting ImageView::ResourceReadySignal...\n");
    mImage.ResourceReadySignal().Connect(this, &ImageViewSampleController::OnImageViewResourceReady);

    // [B] Connect base View::ResourceReadySignal (accessed via View cast)
    DALI_LOG_RELEASE_INFO("[ResourceReady] Connecting View::ResourceReadySignal...\n");
    View baseView = mImage;
    baseView.ResourceReadySignal().Connect(this, &ImageViewSampleController::OnViewResourceReady);

    DALI_LOG_RELEASE_INFO("[ResourceReady] Both signals connected. Waiting for image load...\n");

    window.KeyEventSignal().Connect(this, &ImageViewSampleController::OnKeyEvent);
  }

  // [A] ImageView-specific signal callback
  void OnImageViewResourceReady(ImageView view)
  {
    DALI_LOG_RELEASE_INFO("[ResourceReady][A] ImageView::ResourceReadySignal FIRED! status=%d\n",
                   static_cast<int>(view.GetLoadingStatus()));
  }

  // [B] Base View signal callback
  void OnViewResourceReady(View view)
  {
    Ui::ImageView imageView = Ui::ImageView::DownCast(view);
    if(imageView)
    {
      DALI_LOG_RELEASE_INFO("[ResourceReady][B] View::ResourceReadySignal FIRED (is ImageView)! status=%d\n",
                     static_cast<int>(imageView.GetLoadingStatus()));
    }
    else
    {
      DALI_LOG_RELEASE_INFO("[ResourceReady][B] View::ResourceReadySignal FIRED (not ImageView)\n");
    }
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;
  Ui::ImageView mImage;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ImageViewSampleController sample(application);
  application.MainLoop();
  return 0;
}
