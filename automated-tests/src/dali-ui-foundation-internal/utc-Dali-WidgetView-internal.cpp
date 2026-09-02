/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-environment-variable.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <test-native-image.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/views/widget/widget-view-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/views/widget/widget-view.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/widget-engine/widget-engine-plugin.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/public-api/object/weak-handle.h>
#include <array>
#include <dlfcn.h>

using namespace Dali;
using Dali::Ui::WidgetView;

namespace
{
constexpr const char* APP_ID = "test.widget.viewer";
using Event = WidgetEngineInstancePlugin::EventType;
enum Count { ENGINES_CREATED, ENGINES_DESTROYED, INSTANCES_CREATED, INSTANCES_DESTROYED, RETRIES, ACTIVE_ENGINES };
void* library;
int (*getCount)(int);
void (*emitEvent)(int, Event);
bool (*isPaused)(int);
int (*getDimension)(int, int);
int (*getTouchEventCount)(int);
std::array<int, 6> initialCounts;

int CountSinceStartup(Count count)
{
  return getCount(count) - initialCounts[count];
}

WidgetView NewWidget(const std::string& widgetId = "test.widget", const std::string& appId = APP_ID)
{
  return WidgetView::New(appId, widgetId, "content", 64, 64, 1.0f);
}

void ProcessTouch(UiTestApplication& application, PointState::Type state, uint32_t time)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(Vector2(32.0f, 32.0f));
  point.SetDeviceId(1);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  application.ProcessEvent(touchEvent);
}

struct Signals : ConnectionTracker
{
  void Added(WidgetView) { ++added; }
  void Deleted(WidgetView) { ++deleted; }
  void Faulted(WidgetView) { ++faulted; }
  int added{0};
  int deleted{0};
  int faulted{0};
};

struct LocalizationLookup
{
  void Apply(BaseHandle, const Dali::String&)
  {
    if(enabled)
    {
      enabled = false;
      view = NewWidget("nested.widget");
    }
  }
  WidgetView view;
  bool       enabled{false};
};
} // namespace

void utc_dali_widget_view_internal_startup(void)
{
  test_return_value = TET_UNDEF;
  const char* path = ADDON_LIBS_PATH "/libwidget-view-test-plugin.so";
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_WIDGET_ENGINE_PLUGIN", path);
  library = dlopen(path, RTLD_NOW);
  DALI_ASSERT_ALWAYS(library);
  getCount = reinterpret_cast<int (*)(int)>(dlsym(library, "GetWidgetTestCount"));
  emitEvent = reinterpret_cast<void (*)(int, Event)>(dlsym(library, "EmitWidgetTestEvent"));
  isPaused = reinterpret_cast<bool (*)(int)>(dlsym(library, "IsWidgetTestPaused"));
  getDimension = reinterpret_cast<int (*)(int, int)>(dlsym(library, "GetWidgetTestDimension"));
  getTouchEventCount = reinterpret_cast<int (*)(int)>(dlsym(library, "GetWidgetTestTouchEventCount"));
  DALI_ASSERT_ALWAYS(getCount && emitEvent && isPaused && getDimension && getTouchEventCount);
  for(size_t i = 0; i < initialCounts.size(); ++i) initialCounts[i] = getCount(i);
}

void utc_dali_widget_view_internal_cleanup(void)
{
  dlclose(library);
  EnvironmentVariable::SetTestEnvironmentVariable("DALI_WIDGET_ENGINE_PLUGIN", nullptr);
  test_return_value = TET_PASS;
}

int UtcDaliWidgetViewSharesEngineP(void)
{
  UiTestApplication application;
  auto first = NewWidget();
  auto second = NewWidget("second.widget");
  DALI_TEST_CHECK(first && second && first != second);
  DALI_TEST_CHECK(WidgetView::DownCast(first) == first);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_CREATED), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(first.GetProperty<float>(WidgetView::Property::UPDATE_PERIOD), 1.0f, TEST_LOCATION);
  first.Reset();
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  DALI_TEST_CHECK(second.PauseWidget());
  second.Reset();
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewApiAndPropertiesShareStateP(void)
{
  UiTestApplication application;
  auto view = NewWidget();
  const WidgetView& readOnly = view;
  DALI_TEST_EQUALS(readOnly.GetWidgetId(), Dali::String("test.widget"), TEST_LOCATION);
  DALI_TEST_EQUALS(readOnly.GetInstanceId(), view.GetProperty<Dali::String>(WidgetView::Property::INSTANCE_ID), TEST_LOCATION);
  DALI_TEST_EQUALS(readOnly.GetContentInfo(), Dali::String("content"), TEST_LOCATION);
  DALI_TEST_EQUALS(readOnly.GetTitle(), Dali::String("test.widget"), TEST_LOCATION);
  DALI_TEST_EQUALS(readOnly.GetUpdatePeriod(), 1.0f, TEST_LOCATION);
  DALI_TEST_CHECK(!readOnly.IsWidgetFaulted());

  view.SetPreviewEnabled(false);
  DALI_TEST_CHECK(!view.GetProperty<bool>(WidgetView::Property::PREVIEW));
  view.SetProperty(WidgetView::Property::PREVIEW, true);
  DALI_TEST_CHECK(readOnly.IsPreviewEnabled());
  view.SetKeepWidgetSize(true);
  DALI_TEST_CHECK(view.GetProperty<bool>(WidgetView::Property::KEEP_WIDGET_SIZE));
  view.SetProperty(WidgetView::Property::KEEP_WIDGET_SIZE, false);
  DALI_TEST_CHECK(!readOnly.IsKeepWidgetSize());

  view.SetLoadingTextVisible(false);
  DALI_TEST_CHECK(!view.GetProperty<bool>(WidgetView::Property::LOADING_TEXT));
  view.SetProperty(WidgetView::Property::LOADING_TEXT, true);
  DALI_TEST_CHECK(readOnly.IsLoadingTextVisible());
  view.SetRetryTextVisible(true);
  DALI_TEST_CHECK(readOnly.IsRetryTextVisible());
  Property::Map retry;
  retry.Insert("textVisible", false);
  view.SetProperty(WidgetView::Property::RETRY_TEXT, retry);
  DALI_TEST_CHECK(!readOnly.IsRetryTextVisible());

  // State transitions must preserve settings from both entry points.
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
  emitEvent(0, Event::FAULTED);
  DALI_TEST_CHECK(readOnly.IsWidgetFaulted());
  DALI_TEST_CHECK(view.GetProperty<bool>(WidgetView::Property::WIDGET_STATE_FAULTED));
  DALI_TEST_CHECK(!readOnly.IsLoadingTextVisible());
  DALI_TEST_CHECK(!readOnly.IsRetryTextVisible());
  view.ActivateFaultedWidget();
  DALI_TEST_CHECK(!readOnly.IsWidgetFaulted());
  DALI_TEST_CHECK(readOnly.IsLoadingTextVisible());
  DALI_TEST_CHECK(readOnly.IsPreviewEnabled());
  END_TEST;
}

int UtcDaliWidgetViewTextApisPreservePropertyBehaviorP(void)
{
  UiTestApplication application;
  auto view = NewWidget();
  auto labels = view.GetChildAt(0).GetChildAt(1);
  auto loading = Ui::Label::DownCast(labels.GetChildAt(0));
  auto retry = Ui::Label::DownCast(labels.GetChildAt(1));
  DALI_TEST_CHECK(loading && retry);
  const Dali::String defaultLoading = loading.GetText();
  const Dali::String defaultRetry = retry.GetText();

  Property::Map text;
  text.Insert("stateText", "Loading custom widget");
  text.Insert("textPixelSize", 24.0f);
  text.Insert("textColor", Color::RED);
  text.Insert("fontStyle", "{\"weight\":\"bold\"}");
  view.SetLoadingTextProperties(text);
  DALI_TEST_EQUALS(loading.GetText(), Dali::String("Loading custom widget"), TEST_LOCATION);
  DALI_TEST_EQUALS(loading.GetFontSize(), 24.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(loading.GetProperty<Vector4>(Ui::Label::Property::TEXT_COLOR), Color::RED, TEST_LOCATION);
  DALI_TEST_CHECK(loading.GetFontWeight() == Ui::Text::FontWeight::BOLD);

  text.Clear();
  text.Insert("textPixelSize", 28.0f);
  view.SetProperty(WidgetView::Property::LOADING_TEXT, text);
  DALI_TEST_EQUALS(loading.GetFontSize(), 28.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(loading.GetText(), Dali::String("Loading custom widget"), TEST_LOCATION);

  text.Clear();
  text.Insert("stateText", "Retry custom widget");
  text.Insert("textPixelSize", 26.0f);
  text.Insert("textColor", Color::GREEN);
  text.Insert("textVisible", true);
  view.SetRetryTextProperties(text);
  DALI_TEST_EQUALS(retry.GetText(), Dali::String("Retry custom widget"), TEST_LOCATION);
  DALI_TEST_EQUALS(retry.GetFontSize(), 26.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(retry.GetProperty<Vector4>(Ui::Label::Property::TEXT_COLOR), Color::GREEN, TEST_LOCATION);
  DALI_TEST_CHECK(view.IsRetryTextVisible());

  // Empty text restores the existing localization binding through either API.
  text.Clear();
  text.Insert("stateText", "");
  view.SetLoadingTextProperties(text);
  view.SetProperty(WidgetView::Property::RETRY_TEXT, text);
  DALI_TEST_EQUALS(loading.GetText(), defaultLoading, TEST_LOCATION);
  DALI_TEST_EQUALS(retry.GetText(), defaultRetry, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewEffectApiAndPropertyP(void)
{
  UiTestApplication application;
  auto view = NewWidget();
  auto image = TestNativeImage::New(64, 64);
  auto url = Ui::ImageUrlUtils::GenerateUrl(image, true);
  Property::Map properties;
  properties.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  properties.Insert(Ui::Integration::ImageVisual::Property::URL, url.GetUrl());
  auto visual = Ui::Integration::VisualFactory::Get().CreateVisual(properties);
  Ui::Internal::ViewDataImpl::Get(Ui::Internal::GetImplementation(view)).RegisterVisual(WidgetView::Property::WIDGET_ID, visual);

  Property::Map shader;
  shader.Insert("fragmentShader", "void main() { gl_FragColor = texture2D(sTexture, vTexCoord); }");
  Property::Map effect;
  effect.Insert("shader", shader);
  view.SetEffect(effect);
  DALI_TEST_CHECK(Ui::GetImplementation(visual).IsUsingCustomShader());
  view.SetProperty(WidgetView::Property::EFFECT, Property::Map());
  DALI_TEST_CHECK(!Ui::GetImplementation(visual).IsUsingCustomShader());
  view.SetProperty(WidgetView::Property::EFFECT, effect);
  DALI_TEST_CHECK(Ui::GetImplementation(visual).IsUsingCustomShader());
  Property::Map invalid;
  invalid.Insert("shader", true);
  view.SetEffect(invalid);
  DALI_TEST_CHECK(Ui::GetImplementation(visual).IsUsingCustomShader());
  view.SetEffect(Property::Map());
  DALI_TEST_CHECK(!Ui::GetImplementation(visual).IsUsingCustomShader());
  END_TEST;
}

int UtcDaliWidgetViewRejectsDifferentAppIdN(void)
{
  UiTestApplication application;
  auto first = NewWidget();
  DALI_TEST_CHECK(first);
  DALI_TEST_CHECK(!NewWidget("test.widget", "different.viewer"));
  DALI_TEST_CHECK(!NewWidget("test.widget", ""));
  first.Reset();
  DALI_TEST_CHECK(!NewWidget("test.widget", "different.viewer"));
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
  DALI_TEST_CHECK(NewWidget());
  END_TEST;
}

int UtcDaliWidgetViewFailedCreationRetainsInitializedEngineP(void)
{
  UiTestApplication application;
  DALI_TEST_CHECK(!NewWidget("test.widget", "invalid.viewer"));
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 1, TEST_LOCATION);
  DALI_TEST_CHECK(!NewWidget(""));
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 1, TEST_LOCATION);
  auto view = NewWidget();
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewParentOwnsViewP(void)
{
  UiTestApplication application;
  auto parent = Actor::New();
  auto view = NewWidget();
  DALI_TEST_CHECK(view);
  WeakHandle<WidgetView> weak(view);
  parent.Add(view);
  view.Reset();
  DALI_TEST_CHECK(weak.GetHandle());
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 0, TEST_LOCATION);
  parent.Remove(weak.GetHandle());
  DALI_TEST_CHECK(!weak.GetHandle());
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewReusesEngineAfterLastReleaseP(void)
{
  UiTestApplication application;
  for(int cycle = 1; cycle <= 3; ++cycle)
  {
    auto view = NewWidget();
    DALI_TEST_CHECK(view);
    DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
    view.Reset();
    DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), cycle, TEST_LOCATION);
    DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliWidgetViewReleasesEngineOnApplicationShutdownP(void)
{
  // The retained engine belongs to the application, not a process-static handle.
  {
    UiTestApplication application;
    auto view = NewWidget();
    DALI_TEST_CHECK(view);
    view.Reset();
    DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
    DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
    DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewReentrantLocalizationCreationP(void)
{
  UiTestApplication application;
  LocalizationLookup lookup;
  auto target = Actor::New();
  auto localization = Ui::UiLocalizationManager::Get();
  localization.SetBindingResource(target, "text", "test.resource", "widget_viewer_dali",
                                  Ui::LocalizedStringCallback::New(&lookup, &LocalizationLookup::Apply));
  lookup.enabled = true;
  auto view = NewWidget();
  DALI_TEST_CHECK(view && lookup.view);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_CREATED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_CREATED), 2, TEST_LOCATION);
  localization.ClearBindings(target);
  END_TEST;
}

int UtcDaliWidgetViewDestroyBeforeInitialCallbackP(void)
{
  UiTestApplication application;
  Signals signals;
  auto view = NewWidget();
  DALI_TEST_CHECK(view);
  view.WidgetAddedSignal().Connect(&signals, &Signals::Added);
  view.WidgetDeletedSignal().Connect(&signals, &Signals::Deleted);
  DALI_TEST_EQUALS(signals.added, 0, TEST_LOCATION);
  view.Reset();
  DALI_TEST_EQUALS(signals.added, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.deleted, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewFaultRetryWithoutManagerP(void)
{
  UiTestApplication application;
  Signals signals;
  auto view = NewWidget();
  DALI_TEST_CHECK(view);
  view.WidgetAddedSignal().Connect(&signals, &Signals::Added);
  view.WidgetDeletedSignal().Connect(&signals, &Signals::Deleted);
  view.WidgetFaultedSignal().Connect(&signals, &Signals::Faulted);
  DALI_TEST_EQUALS(signals.added, 0, TEST_LOCATION);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
  DALI_TEST_EQUALS(signals.added, 1, TEST_LOCATION);
  emitEvent(0, Event::FAULTED);
  DALI_TEST_EQUALS(signals.deleted, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.faulted, 1, TEST_LOCATION);
  view.ActivateFaultedWidget();
  DALI_TEST_EQUALS(CountSinceStartup(RETRIES), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(signals.added, 2, TEST_LOCATION);
  view.ActivateFaultedWidget();
  DALI_TEST_EQUALS(CountSinceStartup(RETRIES), 1, TEST_LOCATION);
  view.Reset();
  DALI_TEST_EQUALS(signals.deleted, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewRetryStateBlocksTouchSequenceP(void)
{
  UiTestApplication application;
  auto              view = NewWidget();
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.SetPivot(Pivot::TOP_LEFT);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));

  ProcessTouch(application, PointState::DOWN, 100u);
  ProcessTouch(application, PointState::UP, 110u);
  DALI_TEST_EQUALS(getTouchEventCount(0), 2, TEST_LOCATION);

  emitEvent(0, Event::FAULTED);
  const int touchCount = getTouchEventCount(0);
  ProcessTouch(application, PointState::DOWN, 200u);
  ProcessTouch(application, PointState::MOTION, 210u);
  ProcessTouch(application, PointState::UP, 220u);

  // Retry is activated on UP, but no part of that sequence reaches the provider.
  DALI_TEST_EQUALS(getTouchEventCount(0), touchCount, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(RETRIES), 1, TEST_LOCATION);

  // ActivateFaultedWidget hides the retry state, so later input is routed normally.
  ProcessTouch(application, PointState::DOWN, 300u);
  ProcessTouch(application, PointState::UP, 310u);
  DALI_TEST_EQUALS(getTouchEventCount(0), touchCount + 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewRetainsFrameUntilDestructionP(void)
{
  UiTestApplication application;
  auto view = NewWidget();
  DALI_TEST_CHECK(view);
  application.GetScene().Add(view);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1));
  auto image = TestNativeImage::New(64, 64);
  {
    // Supply a native image without a platform graphics backend. Disconnection
    // retains it for reattachment; destruction releases it even if the image
    // itself is still held by a buffer queue.
    auto url = Ui::ImageUrlUtils::GenerateUrl(image, true);
    Property::Map properties;
    properties.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::IMAGE);
    properties.Insert(Ui::Integration::ImageVisual::Property::URL, url.GetUrl());
    properties.Insert(Ui::Integration::ImageVisual::Property::RELEASE_POLICY, Ui::Image::ReleasePolicy::DESTROYED);
    auto visual = Ui::Integration::VisualFactory::Get().CreateVisual(properties);
    Ui::Internal::ViewDataImpl::Get(Ui::Internal::GetImplementation(view)).RegisterVisual(WidgetView::Property::WIDGET_ID, visual);
  }
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  view.Unparent();
  for(int frame = 0; frame < 4; ++frame)
  {
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(image->mExtensionDestroyCalls, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 0, TEST_LOCATION);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
  view.Unparent();
  view.Reset();
  for(int frame = 0; frame < 4; ++frame)
  {
    application.RunIdles();
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_EQUALS(image->mExtensionDestroyCalls, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(ENGINES_DESTROYED), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewSceneConnectionControlsPauseP(void)
{
  UiTestApplication application;
  Signals signals;
  auto view = NewWidget();
  DALI_TEST_CHECK(view);
  auto parent = Ui::View::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(200, 200));
  parent.SetRequestedWidth(200);
  parent.SetRequestedHeight(200);
  view.WidgetDeletedSignal().Connect(&signals, &Signals::Deleted);
  DALI_TEST_CHECK(isPaused(0));
  parent.Add(view);
  // Adding to a parent that is still off scene must not resume the widget.
  DALI_TEST_CHECK(isPaused(0));
  const auto instanceId = view.GetProperty<Dali::String>(WidgetView::Property::INSTANCE_ID);
  application.GetScene().Add(parent);
  for(int frame = 0; frame < 3; ++frame)
  {
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(!isPaused(0));
  view.Unparent();
  DALI_TEST_CHECK(isPaused(0));
  DALI_TEST_EQUALS(signals.deleted, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 0, TEST_LOCATION);
  parent.Add(view);
  for(int frame = 0; frame < 3; ++frame)
  {
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(!isPaused(0));
  DALI_TEST_EQUALS(view.GetProperty<Dali::String>(WidgetView::Property::INSTANCE_ID), instanceId, TEST_LOCATION);
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_CREATED), 1, TEST_LOCATION);

  // An explicit manual pause survives reattachment until the app resumes it.
  view.PauseWidget();
  view.Unparent();
  parent.Add(view);
  DALI_TEST_CHECK(isPaused(0));
  view.ResumeWidget();
  DALI_TEST_CHECK(!isPaused(0));
  application.GetScene().Remove(parent);
  DALI_TEST_CHECK(isPaused(0));
  view.Unparent();
  view.Reset();
  DALI_TEST_EQUALS(CountSinceStartup(INSTANCES_DESTROYED), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewParentRotationUpdatesPauseP(void)
{
  UiTestApplication application;
  auto view = WidgetView::New(APP_ID, "test.widget", "", 300, 20, 0.0f);
  DALI_TEST_CHECK(view);
  // Rotate about the widget centre so position, size and scale stay unchanged.
  auto parent = Actor::New();
  parent.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  parent.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
  parent.SetProperty(Actor::Property::SIZE, Vector2(300, 20));
  parent.SetProperty(Actor::Property::POSITION, Vector3(160, -50, 0));
  parent.Add(view);
  application.GetScene().Add(parent);

  const auto render = [&application]()
  {
    for(int frame = 0; frame < 10; ++frame)
    {
      application.RunIdles();
      application.SendNotification();
      application.Render();
    }
  };
  render();
  auto bounds = DevelActor::CalculateCurrentScreenExtents(view);
  DALI_TEST_CHECK(bounds.y + bounds.height < 0.0f);
  DALI_TEST_CHECK(isPaused(0));

  parent.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(90), Vector3::ZAXIS));
  render();
  bounds = DevelActor::CalculateCurrentScreenExtents(view);
  DALI_TEST_CHECK(bounds.y + bounds.height > 0.0f);
  DALI_TEST_CHECK(!isPaused(0));

  parent.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(0), Vector3::ZAXIS));
  render();
  DALI_TEST_CHECK(isPaused(0));

  // Rotation must not override an explicit pause request.
  view.PauseWidget();
  parent.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(90), Vector3::ZAXIS));
  render();
  DALI_TEST_CHECK(isPaused(0));
  DALI_TEST_CHECK(view.ResumeWidget());
  DALI_TEST_CHECK(!isPaused(0));
  END_TEST;
}

int UtcDaliWidgetViewNaturalSizeTracksResizeP(void)
{
  UiTestApplication application;
  auto view = WidgetView::New(APP_ID, "test.widget", "", 100, 60, 0.0f);
  view.SetRequestedWidth(Ui::WRAP_CONTENT);
  view.SetRequestedHeight(Ui::WRAP_CONTENT);
  auto measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 60.0f, TEST_LOCATION);

  // Identical constraints must observe the resize, not the cached measurement.
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 200.0f, 120.0f));
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 0), 200, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 1), 120, TEST_LOCATION);

  // A direct actor size change also supplies the next natural size.
  view.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 160.0f));
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 240.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 160.0f, TEST_LOCATION);

  // Switching from an explicit requested size to WRAP_CONTENT keeps the resize.
  view.SetRequestedWidth(300.0f);
  view.SetRequestedHeight(180.0f);
  measured = view.Measure(500.0f, 400.0f);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, measured.width, measured.height));
  view.SetRequestedWidth(Ui::WRAP_CONTENT);
  view.SetRequestedHeight(Ui::WRAP_CONTENT);
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 300.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 180.0f, TEST_LOCATION);

  // Once arranged to a smaller parent constraint, that becomes the natural size.
  measured = view.Measure(80.0f, 50.0f);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, measured.width, measured.height));
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWidgetViewNaturalSizeScalesWithoutRoundingFeedbackP(void)
{
  UiTestApplication application;
  auto scaleManager = Ui::UiScaleManager::Get();
  const float originalScale = scaleManager.GetScale();
  scaleManager.SetScale(1.3f);
  auto view = WidgetView::New(APP_ID, "test.widget", "", 100, 60, 0.0f);
  view.SetRequestedWidth(Ui::WRAP_CONTENT);
  view.SetRequestedHeight(Ui::WRAP_CONTENT);
  auto measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 130.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 78.0f, 0.001f, TEST_LOCATION);

  // The provider takes integer pixels; layout must retain the fractional target.
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 130.5f, 78.5f));
  const int resizeCount = getDimension(0, 2);
  for(int i = 0; i < 20; ++i)
  {
    measured = view.Measure(500.0f, 400.0f);
    DALI_TEST_EQUALS(measured.width, 130.5f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(measured.height, 78.5f, 0.001f, TEST_LOCATION);
    view.Arrange(Ui::LayoutRect(0.0f, 0.0f, measured.width, measured.height));
  }
  DALI_TEST_EQUALS(getDimension(0, 0), 130, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 1), 78, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 2), resizeCount, TEST_LOCATION);

  // Even a subpixel resize that needs no provider request changes natural size.
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 130.75f, 78.75f));
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 130.75f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 78.75f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 2), resizeCount, TEST_LOCATION);

  // System scale changes invalidate registered scene roots.
  application.GetScene().Add(view);
  scaleManager.SetScale(2.0f);
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 130.75f / 1.3f * 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 78.75f / 1.3f * 2.0f, 0.001f, TEST_LOCATION);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, measured.width, measured.height));
  scaleManager.SetScale(1.0f);
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 130.75f / 1.3f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 78.75f / 1.3f, 0.001f, TEST_LOCATION);
  scaleManager.SetScale(originalScale);
  END_TEST;
}

int UtcDaliWidgetViewKeepSizePreservesNaturalSizeP(void)
{
  UiTestApplication application;
  auto view = WidgetView::New(APP_ID, "test.widget", "", 100, 60, 0.0f);
  view.SetRequestedWidth(Ui::WRAP_CONTENT);
  view.SetRequestedHeight(Ui::WRAP_CONTENT);
  view.Measure(500.0f, 400.0f);
  view.SetKeepWidgetSize(true);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 200.0f, 120.0f));
  auto measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 2), 0, TEST_LOCATION);

  // Disabling KEEP_WIDGET_SIZE alone must not synchronize the provider.
  view.SetKeepWidgetSize(false);
  DALI_TEST_EQUALS(getDimension(0, 2), 0, TEST_LOCATION);
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 60.0f, TEST_LOCATION);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 201.0f, 121.0f));
  measured = view.Measure(500.0f, 400.0f);
  DALI_TEST_EQUALS(measured.width, 201.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 121.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 0), 201, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 1), 121, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 2), 1, TEST_LOCATION);

  // KEEP preserves logical natural size even when the display scale changes.
  auto scaleManager = Ui::UiScaleManager::Get();
  const float originalScale = scaleManager.GetScale();
  view.SetKeepWidgetSize(true);
  application.GetScene().Add(view);
  scaleManager.SetScale(2.0f);
  view.Arrange(Ui::LayoutRect(0.0f, 0.0f, 800.0f, 480.0f));
  measured = view.Measure(1000.0f, 1000.0f);
  DALI_TEST_EQUALS(measured.width, 402.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measured.height, 242.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(getDimension(0, 2), 1, TEST_LOCATION);
  scaleManager.SetScale(originalScale);
  END_TEST;
}

int UtcDaliWidgetViewNaturalSizeSettlesOnSceneP(void)
{
  UiTestApplication application;
  auto scaleManager = Ui::UiScaleManager::Get();
  const float originalScale = scaleManager.GetScale();
  scaleManager.SetScale(1.0f);
  auto view = WidgetView::New(APP_ID, "test.widget", "", 100, 60, 0.0f);
  view.SetRequestedWidth(Ui::WRAP_CONTENT);
  view.SetRequestedHeight(Ui::WRAP_CONTENT);
  application.GetScene().Add(view);
  const auto settle = [&application]()
  {
    for(int i = 0; i < 10; ++i)
    {
      application.RunIdles();
      application.SendNotification();
      application.Render();
    }
  };
  settle();
  scaleManager.SetScale(1.3f);
  settle();
  auto size = view.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_EQUALS(size.x, 130.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.y, 78.0f, 0.001f, TEST_LOCATION);
  const int resizeCount = getDimension(0, 2);
  settle();
  DALI_TEST_EQUALS(getDimension(0, 2), resizeCount, TEST_LOCATION);
  scaleManager.SetScale(1.0f);
  settle();
  size = view.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_EQUALS(size.x, 100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.y, 60.0f, 0.001f, TEST_LOCATION);
  scaleManager.SetScale(originalScale);
  END_TEST;
}
