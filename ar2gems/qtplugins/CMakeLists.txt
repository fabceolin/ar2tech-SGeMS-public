# Process all headers with Q_OBJECT
QT4_WRAP_CPP(ar2gems_widgets_MOC
  categorical_definition_proxy_model.h
  categorical_selectors.h
  color_delegate.h
  distribution_parameter_widget.h
  distribution_proxy_model.h
  distribution_selectors.h
  ellipsoid_input.h
  filechooser.h
  grid_filter_delegate.h
  grid_filter_model.h
  grid_filter_widgets.h
  grid_proxy_model.h
  group_selectors.h
  gstl_table.h
  gstl_widgets_plugin.h
  kriging_type_selector.h
  kriging_type_selector_base.h
  neighborhood_filter_input.h
  non_param_cdf_input.h
  project_proxy_model.h
  property_proxy_model.h
  region_proxy_model.h
  region_weight_proxy_model.h
  rename_dialog.h
  selectors.h
  variogram_input.h
  weight_property_proxy_model.h
  weight_property_selector.h
)

QT4_WRAP_UI(ar2gems_widgets_FORMS
  order_properties_dialog.ui
  variogram_structure_base.ui
  variogram_input_base.ui
)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/designer)

add_library(ar2gems_widgets
  categorical_definition_proxy_model.cpp
  categorical_selectors.cpp
  color_delegate.cpp
  distribution_parameter_widget.cpp
  distribution_proxy_model.cpp
  distribution_selectors.cpp
  ellipsoid_input.cpp
  filechooser.cpp
  grid_filter_delegate.cpp
  grid_filter_model.cpp
  grid_filter_model_item.cpp
  grid_filter_named_interface.cpp
  grid_filter_widgets.cpp
  grid_proxy_model.cpp
  group_selectors.cpp
  gstl_table.cpp
  gstl_widgets_plugin.cpp
  kriging_type_selector.cpp
  kriging_type_selector_base.cpp
  library_qtplugins_init.cpp
  neighborhood_filter_input.cpp
  non_param_cdf_input.cpp
  project_proxy_model.cpp
  property_proxy_model.cpp
  region_proxy_model.cpp
  region_weight_proxy_model.cpp
  rename_dialog.cpp
  selectors.cpp
  variogram_input.cpp
  weight_property_proxy_model.cpp
  weight_property_selector.cpp
  
  categorical_definition_proxy_model.h
  categorical_selectors.h
  color_delegate.h
  distribution_parameter_widget.h
  distribution_proxy_model.h
  distribution_selectors.h
  ellipsoid_input.h
  filechooser.h
  grid_filter_delegate.h
  grid_filter_model.h
  grid_filter_model_item.h
  grid_filter_named_interface.h
  grid_filter_widgets.h
  grid_proxy_model.h
  group_selectors.h
  gstl_table.h
  gstl_widgets_plugin.h
  kriging_type_selector.h
  kriging_type_selector_base.h
  neighborhood_filter_input.h
  non_param_cdf_input.h
  project_proxy_model.h
  property_proxy_model.h
  region_proxy_model.h
  region_weight_proxy_model.h
  rename_dialog.h
  selectors.h
  variogram_input.h
  weight_property_proxy_model.h
  weight_property_selector.h  
  ${ar2gems_widgets_MOC}
  ${ar2gems_widgets_FORMS}
)

SOURCE_GROUP("Moc Files" REGULAR_EXPRESSION moc_.*cxx$)
SOURCE_GROUP("Uic Files" REGULAR_EXPRESSION ui_.*h)

target_link_libraries(ar2gems_widgets
  ${QT_LIBRARIES}
  ar2gems_appli
  ar2gems_grid
  ar2gems_math
  ar2gems_utils
)
set_target_properties( ar2gems_widgets PROPERTIES OUTPUT_NAME_RELEASE ar2gems_widgets )
set_target_properties( ar2gems_widgets PROPERTIES OUTPUT_NAME_DEBUG   ar2gems_widgets_d )

if (WIN32)
  add_definitions(-DQTPLUGINS_EXPORTS)
endif()

INSTALL(TARGETS ar2gems_widgets DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
INSTALL(TARGETS ar2gems_widgets DESTINATION "${INSTALL_DIR_PLUGINS}/plugins/designer" COMPONENT application)


#COMMAND ${CMAKE_COMMAND} -E copy
#${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ar2gems_qtplugins.dll
#${CMAKE_SOURCE_DIR}/plugins/designer
