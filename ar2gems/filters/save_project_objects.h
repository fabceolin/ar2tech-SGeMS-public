/* -----------------------------------------------------------------------------
** Copyright (c) 2012 Advanced Resources and Risk Technology, LLC
** All rights reserved.
**
** This file is part of Advanced Resources and Risk Technology, LLC (AR2TECH) 
** version of the open source software sgems.  It is a derivative work by 
** AR2TECH (THE LICENSOR) based on the x-free license granted in the original 
** version of the software (see notice below) and now sublicensed such that it 
** cannot be distributed or modified without the explicit and written permission 
** of AR2TECH.
**
** Only AR2TECH can modify, alter or revoke the licensing terms for this 
** file/software.
**
** This file cannot be modified or distributed without the explicit and written 
** consent of AR2TECH.
**
** Contact Dr. Alex Boucher (aboucher@ar2tech.com) for any questions regarding
** the licensing of this file/software
**
** The open-source version of sgems can be downloaded at 
** sourceforge.net/projects/sgems.
** ----------------------------------------------------------------------------*/


#ifndef __GSTLAPPLI_FILTERS_SAVE_PROPJECT_H__ 
#define __GSTLAPPLI_FILTERS_SAVE_PROPJECT_H__ 

#include <utils/named_interface.h>
#include <map>
#include <string>
#include <utility>

#include <filters/common.h>

class FILTERS_DECL Save_project_objects :  public Named_interface
{
public:

  struct Output_filter_param {
    std::string filter_param;
    std::string filter_name;

    Output_filter_param() : filter_param(""), filter_name(""){}
    Output_filter_param( std::string _filter_param, std::string _filter_name ) : filter_param(_filter_param), filter_name(_filter_name){}

  };

  typedef std::map<std::string, Output_filter_param>  object_to_parameters_mapT;

  static Named_interface* create_new_interface(std::string&);

  Save_project_objects(void);
  virtual ~Save_project_objects(void);

  void add_object_filter(std::string directory_path, std::string object_classname,  Output_filter_param filter_parameter);
  Output_filter_param filter_path(std::string directory_path, std::string classname);

  const std::map<std::string, object_to_parameters_mapT >& directory_to_save() const {return directory_to_save_;}

private:  
  std::map<std::string, object_to_parameters_mapT > directory_to_save_;

};

#endif
