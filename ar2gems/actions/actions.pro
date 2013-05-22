######################################################################
# Automatically generated by qmake (2.01a) Sun Nov 1 16:20:30 2009
######################################################################

TEMPLATE = lib
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += action.h \
           algorithms_actions.h \
           common.h \
           defines.h \
           library_actions_init.h \
           misc_actions.h \
           obj_manag_actions.h \
           python_commands.h \
           python_wrapper.h \
           unary_action.h \
           python_script.h \
           python_group_script.h \
           maskedgrid_actions.h \
           categorical_definition_actions.h \
           categorical_property_actions.h \
           property_group_actions.h \
           distribution_action.h \
           property_transformer_actions.h
           
SOURCES += algorithms_actions.cpp \
           library_actions_init.cpp \
           misc_actions.cpp \
           obj_manag_actions.cpp \
           python_commands.cpp \
           python_wrapper.cpp \
           unary_action.cpp \
           python_script.cpp \
           python_group_script.cpp \
           maskedgrid_actions.cpp \
           categorical_definition_actions.cpp \
           categorical_property_actions.cpp \
           property_group_actions.cpp \
           distribution_action.cpp \
           property_transformer_actions.cpp

TARGET=ar2tech_actions
QT += xml


CONFIG      += plugin
INCLUDEPATH += $(QTDIR)/tools/designer/interfaces
LIBS += -lar2tech_io

contains( TEMPLATE, lib ) {
    DESTDIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM/obj/actions
}
contains( TEMPLATE, app ) {
    DESTDIR += $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM/obj/actions
}


include( $$GSTLAPPLI_HOME/config.qmake )

exists( MMakefile ) {
   include( MMakefile )
}

