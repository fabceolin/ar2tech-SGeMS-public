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



/**********************************************************************
 ** Author: Nicolas Remy
 ** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
 **   University
 ** All rights reserved.
 **
 ** This file is part of the "actions" module of the Geostatistical Earth
 ** Modeling Software (GEMS)
 **
 ** This file may be distributed and/or modified under the terms of the
 ** license defined by the Stanford Center for Reservoir Forecasting and
 ** appearing in the file LICENSE.XFREE included in the packaging of this file.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
 **
 ** Contact the Stanford Center for Reservoir Forecasting, Stanford University
 ** if any conditions of this licensing are not clear to you.
 **
 **********************************************************************/

#include <actions/library_actions_init.h>
#include <actions/python_commands.h>
#include <utils/manager_repository.h>
#include <actions/obj_manag_actions.h>
#include <actions/algorithms_actions.h>
#include <actions/misc_actions.h>
#include <utils/gstl_messages.h>
#include <actions/unary_action.h>
#include <actions/python_script.h>
#include <actions/python_group_script.h>
#include <actions/maskedgrid_actions.h>
#include <actions/categorical_property_actions.h>
#include <actions/categorical_definition_actions.h>
#include <actions/property_group_actions.h>
#include <actions/distribution_action.h>
#include <actions/property_transformer_actions.h>
#include <actions/add_property_to_grid.h>
#include <actions/tie_breaking_actions.h>

#if defined (RELEASE_PYTHON_IN_DEBUG) && defined (_DEBUG)
  #undef _DEBUG
  #include <Python.h>
  #define _DEBUG
#else
  #include <Python.h>
#endif

void init_python_interpreter();

int library_actions_init::references_ = 0;

int library_actions_init::init_lib()
{

	// initialize Python
	init_python_interpreter();

	references_++;
	if (references_ != 1)
	{
		GsTLlog << "actions library already registered \n";
		return 2;
	}

	GsTLlog << "\n\n registering actions library \n";

	SmartPtr<Named_interface> ni =  Root::instance()->interface( actions_manager);
	Manager* dir = dynamic_cast<Manager*> (ni.raw_ptr());

	if (!dir)
	{
		GsTLlog << "could not retrieve directory " << actions_manager << "\n";
		return 1;
	}

	bind_action_factories(dir);

	// Rahul: adding a Python Script Manager
	{
		SmartPtr<Named_interface> py_ni = Root::instance()->new_interface("directory://actions/python", python_script_manager);
		Manager* py_dir = dynamic_cast<Manager*> (py_ni.raw_ptr());
		if (!py_dir)
		{
			GsTLlog << "could not create directory " << python_script_manager << "\n";
			return 1;
		}
		py_dir->factory("pythonscript", Python_script::create_new_interface);
	}

	// Rahul: adding a Python Script Manager for groups
	{
		SmartPtr<Named_interface> py_ni = Root::instance()->new_interface("directory://actions/python_group", python_group_script_manager);
		Manager* py_dir = dynamic_cast<Manager*> (py_ni.raw_ptr());
		if (!py_dir)
		{
			GsTLlog << "could not create directory " << python_group_script_manager << "\n";
			return 1;
		}
		py_dir->factory("pythongroupscript", Python_group_script::create_new_interface);
	}

	GsTLlog << "Registration done \n\n";

	Root::instance()->list_all(GsTLlog);
	return 0;
}

int library_actions_init::release_lib()
{
	references_--;
	if (references_ == 0)
	{
		Root::instance()->delete_interface(actions_manager);
		Py_Finalize();
	}
	return 0;
}

bool library_actions_init::bind_action_factories(Manager* dir)
{
	// Grid management actions
	dir->factory("Help", Help_action::create_new_interface);

	dir->factory("NewRGrid", New_rgrid::create_new_interface);

	// TL modified
	/*
	 dir->factory( "LoadSimFromFile",
	 Load_sim::create_new_interface );
	 */
	dir->factory("RunScript", RunScript::create_new_interface);


	dir->factory("NewCartesianGrid", New_cartesian_grid_action::create_new_interface);

	dir->factory("CopyProperty", Copy_property::create_new_interface);
	dir->factory("SwapPropertyToDisk", Swap_property_to_disk::create_new_interface);
	dir->factory("SwapPropertyToRAM", Swap_property_to_ram::create_new_interface);
	dir->factory("DeleteObjects", Delete_objects::create_new_interface);
	dir->factory("DeleteObjectProperties", Delete_properties::create_new_interface);
	dir->factory("ClearPropertyValueIf", Clear_property_value_if::create_new_interface);
  dir->factory("ClearPropertyInsideRegion", Clear_property_inside_region::create_new_interface);
  dir->factory("ClearPropertyOutsideRegion", Clear_property_outside_region::create_new_interface);
	dir->factory("SetActiveRegion", Set_active_region::create_new_interface);
	dir->factory("SetHarddata", Set_hard_data::create_new_interface);
	dir->factory("DeleteObjectRegions", Delete_regions::create_new_interface);
	dir->factory("MergeObjectRegionsUnion", Merge_regions_union::create_new_interface);
	dir->factory("MergeObjectRegionsIntersection", Merge_regions_intersection::create_new_interface);
	dir->factory("SetRegionFromComplement", Set_region_complement::create_new_interface);
	dir->factory("SetRegionFromPropertyIf", Set_region_from_property::create_new_interface);
	dir->factory("SetRegionFromCategoricalPropertyIf", Set_region_from_categorical_property::create_new_interface);
	dir->factory("ClearPropertyValueFromProperty", Clear_property_value_from_property::create_new_interface);
	dir->factory("CreateTrend", Create_trend::create_new_interface);
  dir->factory("ScaleProperty", Scale_property::create_new_interface);
	dir->factory("CreateMgridFromCgrid", Create_mgrid_from_cgrid::create_new_interface);
	dir->factory("IndicatorCoding", Create_indicator_properties::create_new_interface);
  dir->factory("CapProperty", Cap_property::create_new_interface);
  dir->factory("ExtractCellVolume", Extract_cell_volume::create_new_interface);
  

  dir->factory("ConvertPropertyInDefinitionToSequential", Convert_definition_to_sequential_coding::create_new_interface);
  dir->factory("ConvertCategoricalPropertyToSequential", Convert_categorical_property_to_sequential_coding::create_new_interface);


  dir->factory("ConvertToCategoricalProperty", Convert_continuous_to_categorical_property::create_new_interface);
	dir->factory("NewCategoricalDefinition", New_categorical_definition::create_new_interface);
  dir->factory("NewCategoricalCodeDefinition", New_alpha_numerical_categorical_definition::create_new_interface);
	dir->factory("AssignCategoricalDefinition", Assign_categorical_definition::create_new_interface);

	dir->factory("NewPropertyGroup", New_property_group::create_new_interface);
	dir->factory("AddPropertiesToGroup", Add_properties_to_group::create_new_interface);
  dir->factory("RemovePropertiesFromGroup", Remove_properties_from_group::create_new_interface);
	dir->factory("RemoveAllPropertiesFromGroup", Delete_property_in_group::create_new_interface);
	dir->factory("RemoveGroup", Remove_group::create_new_interface);

  dir->factory("NewParametricDistribution", New_distribution_action::create_new_interface);
  dir->factory("DeleteContinuousDistribution", Delete_distribution_action::create_new_interface);

  dir->factory("DownscaleGrid", Downscale_grid::create_new_interface);
  dir->factory("UpscaleProperties", Upscale_properties::create_new_interface);
  dir->factory("UpscaleGroup", Upscale_group::create_new_interface);
 
  dir->factory("New_pca_transformer", New_pca_transformer::create_new_interface);
  dir->factory("Multivariate_forward_transform", Multivariate_forward_transform::create_new_interface);
  dir->factory("Multivariate_back_transform", Multivariate_back_transform::create_new_interface);
  dir->factory("Delete_multivariate_transforms", Delete_multivariate_transforms::create_new_interface);

  dir->factory("AddPropertyToGridFromBinaryFile", Add_property_to_grid_from_binary_file::create_new_interface);
  dir->factory("AddPropertyToGridFromTextFile", Add_property_to_grid_from_text_file::create_new_interface);
  dir->factory("AddPropertyToGridWthNodeidFromTextFile", Add_property_to_grid_with_nodeid_from_text_file::create_new_interface);

  dir->factory("BreakTiesRandom", Break_ties_random::create_new_interface);
  dir->factory("BreakTiesSpatial", Break_ties_spatial::create_new_interface);
  dir->factory("BreakTiesSecondary", Break_ties_with_secondary_property::create_new_interface);

	// algorithm related actions
	dir->factory("RunGeostatAlgorithm", Run_geostat_algo::create_new_interface);

	//Registering Unary_action
	dir->factory(Log_transform_action().name(), Log_transform_action::create_new_interface);
	dir->factory(Log10_transform_action().name(), Log10_transform_action::create_new_interface);
	dir->factory(Exponential_transform_action().name(), Exponential_transform_action::create_new_interface);
	dir->factory(Sine_transform_action().name(), Sine_transform_action::create_new_interface);
	dir->factory(Cosine_transform_action().name(), Cosine_transform_action::create_new_interface);
	dir->factory(Sqrt_transform_action().name(), Sqrt_transform_action::create_new_interface);
	dir->factory(Inverse_transform_action().name(), Inverse_transform_action::create_new_interface);
	dir->factory(Standardize_transform_action().name(), Standardize_transform_action::create_new_interface);
	dir->factory(Unit_scaling_transform_action().name(), Unit_scaling_transform_action::create_new_interface);
	dir->factory(Complement_transform_action().name(), Complement_transform_action::create_new_interface);
	dir->factory(Logit_transform_action().name(), Logit_transform_action::create_new_interface);
	dir->factory(Logistic_transform_action().name(), Logistic_transform_action::create_new_interface);
	dir->factory(Square_transform_action().name(), Square_transform_action::create_new_interface);
  dir->factory(Absolute_transform_action().name(), Absolute_transform_action::create_new_interface);


	return true;
}

extern "C"
{
int libGsTLAppli_actions_init()
{
	return library_actions_init::init_lib();
}
int libGsTLAppli_actions_release()
{
	return library_actions_init::release_lib();
}
}

//--------------------------------------

void init_python_interpreter()
{
//	Py_NoSiteFlag = 1;
	Py_Initialize();
	Py_InitModule("sgems", SGemsMethods);
	Py_InitModule("redirect", RedirectMethods);
}
