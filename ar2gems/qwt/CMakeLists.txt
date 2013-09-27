# Process all headers with Q_OBJECT
QT4_WRAP_CPP(qwt_MOC
  qwt_text_label.h
  qwt_compass.h
  qwt_plot_panner.h
  qwt_plot_zoomer.h
  qwt_scale_widget.h
  qwt_counter.h
  qwt_dyngrid_layout.h
  qwt_legend.h
  qwt_slider.h
  qwt_thermo.h
  qwt_dial.h
  qwt_knob.h
  qwt_plot_magnifier.h
  qwt_plot_canvas.h
  qwt_panner.h
  qwt_abstract_slider.h
  qwt_picker.h
  qwt_plot_picker.h
  qwt_analog_clock.h
  qwt_plot.h
  qwt_wheel.h
  qwt_legend_item.h
)

add_library(ar2gems_qwt
  qwt_abstract_scale.cpp
  qwt_abstract_scale_draw.cpp
  qwt_abstract_slider.cpp
  qwt_analog_clock.cpp
  qwt_arrow_button.cpp
  qwt_color_map.cpp
  qwt_compass.cpp
  qwt_compass_rose.cpp
  qwt_counter.cpp
  qwt_curve_fitter.cpp
  qwt_data.cpp
  qwt_dial.cpp
  qwt_dial_needle.cpp
  qwt_double_interval.cpp
  qwt_double_range.cpp
  qwt_double_rect.cpp
  qwt_dyngrid_layout.cpp
  qwt_event_pattern.cpp
  qwt_interval_data.cpp
  qwt_knob.cpp
  qwt_layout_metrics.cpp
  qwt_legend.cpp
  qwt_legend_item.cpp
  qwt_math.cpp
  qwt_paint_buffer.cpp
  qwt_painter.cpp
  qwt_panner.cpp
  qwt_picker.cpp
  qwt_picker_machine.cpp
  qwt_plot_axis.cpp
  qwt_plot_canvas.cpp
  qwt_plot.cpp
  qwt_plot_curve.cpp
  qwt_plot_dict.cpp
  qwt_plot_grid.cpp
  qwt_plot_item.cpp
  qwt_plot_layout.cpp
  qwt_plot_magnifier.cpp
  qwt_plot_marker.cpp
  qwt_plot_panner.cpp
  qwt_plot_picker.cpp
  qwt_plot_print.cpp
  qwt_plot_printfilter.cpp
  qwt_plot_rasteritem.cpp
  qwt_plot_spectrogram.cpp
  qwt_plot_svgitem.cpp
  qwt_plot_xml.cpp
  qwt_plot_zoomer.cpp
  qwt_raster_data.cpp
  qwt_rect.cpp
  qwt_round_scale_draw.cpp
  qwt_scale_div.cpp
  qwt_scale_draw.cpp
  qwt_scale_engine.cpp
  qwt_scale_map.cpp
  qwt_scale_widget.cpp
  qwt_slider.cpp
  qwt_spline.cpp
  qwt_symbol.cpp
  qwt_text.cpp
  qwt_text_engine.cpp
  qwt_text_label.cpp
  qwt_thermo.cpp
  qwt_wheel.cpp
  ${qwt_MOC}
)

SOURCE_GROUP("Moc Files" REGULAR_EXPRESSION moc_.*cxx$)
SOURCE_GROUP("Uic Files" REGULAR_EXPRESSION ui_.*h)

target_link_libraries(ar2gems_qwt
  ${QT_LIBRARIES}
)


if (WIN32)
  add_definitions(-DQWT_DLL -DQWT_MAKEDLL)
endif()

INSTALL(TARGETS ar2gems_qwt DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
