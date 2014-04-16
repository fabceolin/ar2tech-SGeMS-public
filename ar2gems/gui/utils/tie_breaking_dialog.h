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


#ifndef __TIE_BREAKING_DIALOG_H__ 
#define __TIE_BREAKING_DIALOG_H__ 

#include <gui/common.h>

#include <QApplication>
#include <QDialog>
#include <QLineEdit>

class PropertySelector;

class GsTL_project;



class Tie_breaking_random_dialog : public QDialog {
  Q_OBJECT

public:
  Tie_breaking_random_dialog( GsTL_project* proj, QWidget* parent = 0  );
  QString selected_grid() const;
  QString selected_property() const ;
  QString selected_region() const ;
  QString new_property_name() const;


protected slots:
  bool create_property();
  void create_property_and_close();

protected:
  GsTL_project* project_;
  PropertySelector* prop_selector_;

  QLineEdit *output_name_;



};

 
#endif 
