TEMPLATE = lib
DEPENDPATH += . gslib
INCLUDEPATH += .

HEADERS += common.h \
           filter.h \
           library_filters_init.h \
           simulacre_filter.h \
           gslib/filter_qt_dialogs.h \
           gslib/gslib_filter.h \
           csv_filter_qt_dialogs.h \
           csv_filter.h \
           sgems_folder_filter.h \
           sgems_folder_grid_geometry.h \
           distribution_filter.h \
           save_project_objects.h

SOURCES += library_filters_init.cpp \
           simulacre_filter.cpp \
           gslib/filter_qt_dialogs.cpp \
           gslib/gslib_filter.cpp \
           csv_filter_qt_dialogs.cpp \
           csv_filter.cpp \
           sgems_folder_filter.cpp \
           sgems_folder_grid_geometry.cpp \
           distribution_filter.cpp \
           save_project_objects.cpp

FORMS += gslib/gslib_pointset_import.ui \
         gslib/gslibgridimport.ui \
         gslib/gslibimportwizard.ui \
         gslib/gslibmaskedimport.ui \
         gslib/importwizard.ui \
         csv_grid_import.ui \
         csv_masked_import.ui \
         csv_pointset_import.ui

TARGET=ar2gems_io

QT += xml
CONFIG      += plugin
INCLUDEPATH += $(QTDIR)/tools/designer/interfaces

contains( TEMPLATE, lib ) {
    DESTDIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM/obj/filters
}
contains( TEMPLATE, app ) {
    DESTDIR += $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM/obj/filters
}


include( $$GSTLAPPLI_HOME/config.qmake )

exists( MMakefile ) {
   include( MMakefile )
}
