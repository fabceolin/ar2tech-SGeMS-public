# #####################################################################
# Automatically generated by qmake (2.01a) Sun Nov 1 16:24:35 2009
# #####################################################################
TEMPLATE = lib
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += debug
# Input
HEADERS += gstl_item_model.h \
    gstl_object_item.h \
    common.h \
    directory.h \
    error_messages_handler.h \
    gstl_data_array.h \
    gstl_data_array.hpp \
    gstl_messages.h \
    gstl_messages_private.h \
    gstl_plugins.h \
    gstl_types.h \
    lineeditkey.h \
    manager.h \
    named_interface.h \
    progress_notifier.h \
    simpleps.h \
    singleton_holder.h \
    string_manipulation.h \
    xml_parsing.h
SOURCES += error_messages_handler.cpp \
    gstl_messages_private.cpp \
    manager.cpp \
    progress_notifier.cpp \
    simpleps.cpp \
    string_manipulation.cpp
TARGET = ar2tech_utils
CONFIG += plugin

INCLUDEPATH += $(QTDIR)/tools/designer/interfaces
contains( TEMPLATE, lib ) { 
    DESTDIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM/obj/utils
}
contains( TEMPLATE, app ) { 
    DESTDIR += $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM/obj/utils
}
include( $$GSTLAPPLI_HOME/config.qmake )
exists( MMakefile ):include( MMakefile )
