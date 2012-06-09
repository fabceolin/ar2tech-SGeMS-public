/* -----------------------------------------------------------------------------
** Copyright� 2012 Advanced Resources and Risk Technology, LLC
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
** This file is part of the "gui" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_QCOMBOBOX_WRAPPER_H__ 
#define __GSTLAPPLI_QCOMBOBOX_WRAPPER_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/utils/qwidget_value_accessor.h> 
 
#include <qcombobox.h> 
 
#include <string> 
 
class ComboBoxHelper;

 
class GUI_DECL QComboBox_accessor : public QWidget_value_accessor { 
 public: 
  static Named_interface* create_new_interface(std::string&); 
 
 public: 
  QComboBox_accessor( QWidget* widget = 0 ); 
  virtual ~QComboBox_accessor() {} 
 
  virtual bool initialize( QWidget* widget = 0 ); 
  virtual bool is_valid() const { return combo_box_ != 0; }  
  virtual std::string value() const; 
  virtual bool set_value( const std::string& str ); 
  virtual void clear(); 
     
 private: 
  QComboBox* combo_box_; 
  ComboBoxHelper* helper_;
}; 
 


/** This class is a work-around for a Qt problem:
* Using setCurrentItem on a combobox doesn't send the activated signal.
* Doing it through ComboBoxHelper allows the signal to be sent when the
* combobox is updated.
*/
class ComboBoxHelper : public QObject {

  Q_OBJECT

public:
  ComboBoxHelper( QObject* parent );
  void activateComboBox( QComboBox* box, int  i );
  void activateComboBox( QComboBox* box, QString  text );

signals:
  void activateText( QString );
  void doInternalActivate( int );
};

 
#endif  