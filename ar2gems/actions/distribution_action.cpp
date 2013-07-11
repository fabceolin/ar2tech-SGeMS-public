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




#include <actions/distribution_action.h>
#include <appli/action.h>
#include <utils/string_manipulation.h>
#include <math/continuous_distribution.h>
#include <utils/manager.h>
#include <utils/named_interface.h>
#include <utils/manager_repository.h>


Named_interface* New_distribution_action::create_new_interface( std::string& ){
  return new New_distribution_action;
}


New_distribution_action::New_distribution_action(void)
{
}


New_distribution_action::~New_distribution_action(void)
{
}


bool New_distribution_action::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ){

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if(params.size() <  3) {
    errors->report("Need at least three parameters (type, name, parameters)");
    return false;
  }
  dist_type_ = params[0];
  dist_name_ = params[1];
  Named_interface* ni = Root::instance()->new_interface_raw(dist_type_, continuous_distributions_manager+"/"+dist_name_ );
  if(ni != 0) {
    Parametric_continuous_distribution* dist = dynamic_cast<Parametric_continuous_distribution*>(ni);
    std::vector<std::string> pnames = dist->parameter_names();
    if(params.size()-2 != pnames.size()) {
      std::stringstream message;
      message<<"Distribution of type "<<dist_type_<<" needs "<<pnames.size()<< " parameters : ";
      for(int i=0; i<pnames.size(); ++i) {
        message<<pnames[i];
        if(i < pnames.size()-1)
          message<<",";
      }
      message<<".  Only "<<params.size()-2<<" were provided.";
      
      errors->report(message.str());
      return false;
    }
    for(int i=0; i<pnames.size(); ++i) {
      params_.push_back(String_Op::to_number<float>(params[i+2]));
    }
    is_continuous_ = true;
    return true;
  }
  ni = Root::instance()->new_interface_raw(dist_type_, categorical_distributions_manager+"/"+dist_name_ );

  return false;


}

bool New_distribution_action::exec() {

  if(is_continuous_) {
    Named_interface* ni = Root::instance()->new_interface(dist_type_, continuous_distributions_manager+"/"+dist_name_ ).raw_ptr();

    Continuous_distribution* dist = dynamic_cast<Continuous_distribution*>(ni);
    if(dist->is_parametric()) {
      dynamic_cast<Parametric_continuous_distribution*>(dist)->initialize( params_ );
      return true;
    }
    else {

    }

    
  }
  else {   //Categorical

  }
    
  return false;
}

/*
----------------------------------------
*/

Named_interface* Delete_distribution_action::create_new_interface( std::string& ){
  return new Delete_distribution_action;
}

Delete_distribution_action::Delete_distribution_action(void){

}
Delete_distribution_action::~Delete_distribution_action(void){

}

bool Delete_distribution_action::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ){

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if(params.empty() <  1) {
    errors->report("Need at least one parameter (dsitribution name)");
    return false;
  }

  Manager* dir = dynamic_cast<Manager*>(Root::instance()->interface( continuous_distributions_manager).raw_ptr());

  if(dir == 0) return false;

  for(int i=0; i<params.size(); ++i) {
    dir->delete_interface( params[i] );
  }

  return true;


}
bool Delete_distribution_action::exec(){
  return true;
}