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



/*
 * sgems_input_filter.h
 *
 *  Created on: Jun 16, 2010
 *      Author: aboucher
 */

#ifndef SGEMS_INPUT_FILTER_H_
#define SGEMS_INPUT_FILTER_H_

#include <filters/common.h>
#include <filters/filter.h>
#include <QtXml>
#include <QDir>
#include <QDomDocument>

class Geostat_grid;
class Log_data;
class CategoricalPropertyDefinitionName;

class FILTERS_DECL Sgems_folder_input_filter: public Input_filter {
public:
  static Named_interface* create_new_interface( std::string& );
public:
	Sgems_folder_input_filter();
	virtual ~Sgems_folder_input_filter();

  virtual std::string filter_name() const { return "sgems_beta" ; }
  virtual std::string file_extensions() const { return "*.grid"; }

  virtual bool can_handle( const std::string& filename );
  virtual Named_interface* read( const std::string& filename,
                              std::string* errors = 0 );

protected :

  bool read_properties(QDir dir,const QDomElement& dom, Geostat_grid* grid, std::string* errors);
  bool read_regions(QDir dir,const QDomElement& dom, Geostat_grid* grid, std::string* errors);
  bool read_group(const QDomElement& dom, Geostat_grid* grid, std::string* errors);
  bool read_category_definition(const QDomElement& root, Geostat_grid* grid, std::string* errors);

  bool check_for_conflict(CategoricalPropertyDefinitionName* def, const QStringList& cat_names, const QList<int>& codes);
  bool create_categorial_definition( QString& name, QStringList& cat_names, QList<int>& codes);
};

class FILTERS_DECL Sgems_folder_output_filter: public Output_filter {
public:
  static Named_interface* create_new_interface( std::string& );
public:
  Sgems_folder_output_filter();
  virtual ~Sgems_folder_output_filter();

  virtual std::string filter_name() const { return "sgems_beta" ; }
  virtual std::string file_extensions() const { return "*.sgems"; }

  virtual bool write( std::string outfile, const Named_interface* ni,
                      std::string* errors = 0 );

  virtual std::string type_data() const {return "Grid";}

protected :

  bool removeDir(const QString &dirName);

  QDomElement write_grid_geometry(QDir dir,QDomDocument& dom, const Geostat_grid* grid);
  QDomElement write_masked_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid);
  QDomElement write_pointset_geometry( QDir dir,  QDomDocument& dom, const Geostat_grid* grid );
  QDomElement write_cartesian_grid_geometry( QDir dir, QDomDocument& dom, const Geostat_grid* grid );
  QDomElement write_log_data_grid_geometry( QDir dir, QDomDocument& dom, const Geostat_grid* grid );
  QDomElement write_log_data_geometry( QDomDocument& dom, const Log_data* ldata );
  QDomElement write_structured_grid_geometry(QDir dir, QDomDocument& dom, const Geostat_grid* grid );

  QDomElement write_properties(QDir dir, QDomDocument& dom, const Geostat_grid* grid);
  QDomElement write_regions(QDir dir, QDomDocument& dom, const Geostat_grid* grid);
  QDomElement write_group(QDomDocument& dom, const Geostat_grid* grid);
  QDomElement write_category_definition(QDomDocument& dom, const Geostat_grid* grid);

	std::string get_grid_name(const Geostat_grid* grid);

};

#endif /* SGEMS_INPUT_FILTER_H_ */
