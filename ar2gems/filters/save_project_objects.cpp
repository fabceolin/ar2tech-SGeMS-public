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

#include <filters/save_project_objects.h>


Named_interface* Save_project_objects::create_new_interface(std::string&){
  return new Save_project_objects;
}

Save_project_objects::Save_project_objects(void)
{
  item_type_="Save_project_objects";
}


Save_project_objects::~Save_project_objects(void)
{
}



void Save_project_objects::add_object_filter(std::string directory_path, std::string object_classname,  
                       Output_filter_param filter_parameter)
{

  std::map<std::string, object_to_parameters_mapT >::iterator it = directory_to_save_.find(directory_path);
  if( it == directory_to_save_.end()) {
    it = directory_to_save_.insert(std::make_pair(directory_path, object_to_parameters_mapT() ) ).first;
  }

  it->second[object_classname] = filter_parameter;
}

/*
void Save_project_objects::add_directory(std::string directory_path, 
                                         std::pair<std::string, std::string> filter_param_name)
{
  directory_to_save_[directory_path] = filter_param_name;
}
*/

Save_project_objects::Output_filter_param Save_project_objects::filter_path(std::string directory_path, std::string classname){

  std::map<std::string, object_to_parameters_mapT >::iterator it = directory_to_save_.find(directory_path);
  if(it == directory_to_save_.end()) {
    return Output_filter_param("","");
  }

  object_to_parameters_mapT::iterator it_object = it->second.find(classname);
  if( it_object == it->second.end() ) {
    return Output_filter_param("","");
  }

  return it_object->second;

}
