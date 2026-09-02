#ifndef __DALI_UI_WIDGET_VIEW_WIDGET_VIEW_MANAGER_H__
#define __DALI_UI_WIDGET_VIEW_WIDGET_VIEW_MANAGER_H__

/*
 * Samsung API
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.1 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/public-api/adaptor-framework/application.h>
#include <dali/public-api/object/base-handle.h>
#include <string>

namespace Dali
{

namespace Ui
{

class WidgetView;

namespace Internal DALI_INTERNAL
{
class WidgetViewManager;
}

/**
 * @addtogroup dali_widget_view
 * @{
 */

/**
 * @brief WidgetViewManager manages addition of WidgetView controls.
 *
 * This class provides the functionality of adding the widget views and controlling the widgets.
 *
 * @since_tizen 3.0
 */
class DALI_UI_API WidgetViewManager : public BaseHandle
{
public:
  /**
   * @brief Create widget view manager.
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @param[in] application Application class for the widget view manager.
   * @param[in] appId App ID of app with widget viewer dali.
   * @return A handle to WidgetViewManager.
   */
  static WidgetViewManager New(Application application, const std::string& appId);

  /**
   * @brief Downcast a handle to WidgetViewManager handle.
   *
   * If the BaseHandle points is a WidgetViewManager the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @since_tizen 3.0
   * @param[in] handle Handle to an object
   * @return handle to a WidgetViewManager or an empty handle
   */
  static WidgetViewManager DownCast(BaseHandle handle);

  /**
   * @brief Creates an WidgetViewManager handle.
   *
   * Calling member functions with an uninitialized handle is not allowed.
   * @since_tizen 3.0
   */
  WidgetViewManager();

  /**
   * @brief Copy constructor.
   *
   * @since_tizen 3.0
   * @param[in] handle The handle to copy from.
   */
  WidgetViewManager(const WidgetViewManager& handle);

  /**
   * @brief Assignment operator.
   *
   * @since_tizen 3.0
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  WidgetViewManager& operator=(const WidgetViewManager& handle);

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @since_tizen 3.0
   */
  ~WidgetViewManager();

  /**
   * @brief Creates a new widget view object
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @param[in] widgetId The widget id.
   * @param[in] contentInfo Contents that will be given to the widget instance.
   * @param[in] width The widget width.
   * @param[in] height The widget height.
   * @param[in] updatePeriod The period of updating contents of the widget.
   * @return A handle to WidgetView.
   */
  WidgetView AddWidget(const std::string& widgetId, const std::string& contentInfo, int width, int height, float updatePeriod);

  /**
   * @brief Removes the widget view object
   *
   * @since_tizen 6.5
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @param[in] widgetView The widget view.
   * @return true on success, false otherwise.
   */
  bool RemoveWidget(WidgetView widgetView);

public: // Not intended for application developers
  /**
   * @internal
   * @brief Creates a handle using the Ui::Internal implementation.
   *
   * @since_tizen 3.0
   * @param[in] implementation The WidgetViewManager implementation.
   */
  explicit DALI_INTERNAL WidgetViewManager(Internal::WidgetViewManager* implementation);
};

/**
 * @}
 */

} // namespace Ui

} // namespace Dali

#endif // __DALI_UI_WIDGET_VIEW_WIDGET_VIEW_MANAGER_H__
