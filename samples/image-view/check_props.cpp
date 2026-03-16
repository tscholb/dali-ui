#include <iostream>
#include <dali-ui-foundation/integration-api/image-view/image-view-impl.h>

int main()
{
  std::cout << "IMAGE: " << Dali::Ui::Internal::ImageView::Property::IMAGE << std::endl;
  std::cout << "PRE_MULTIPLIED_ALPHA: " << Dali::Ui::Internal::ImageView::Property::PRE_MULTIPLIED_ALPHA << std::endl;
  std::cout << "PLACEHOLDER_IMAGE: " << Dali::Ui::Internal::ImageView::Property::PLACEHOLDER_IMAGE << std::endl;
  return 0;
}
