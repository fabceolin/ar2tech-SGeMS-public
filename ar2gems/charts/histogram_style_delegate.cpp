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




#include <charts/histogram_style_delegate.h>
#include <charts/histogram_proxy_model.h>

#include <QComboBox>


Continuous_histo_style_delegate::Continuous_histo_style_delegate( QObject *parent)
  : QStyledItemDelegate(parent)
{

}

Continuous_histo_style_delegate::~Continuous_histo_style_delegate()
{

}

 void Continuous_histo_style_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
 {
   QStyledItemDelegate::paint(painter, option, index);
}



 QWidget *Continuous_histo_style_delegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const

 {
   
   Histogram_item *item = static_cast<Histogram_item*>(index.internalPointer());

   QComboBox* options = new QComboBox(parent);

   options->addItem(QIcon(":/icons/appli/Pixmaps/histo-bars.svg"), "Bars");
   options->addItem(QIcon(":/icons/appli/Pixmaps/histo-lines.svg"), "Lines"); 
   options->addItem(QIcon(":/icons/appli/Pixmaps/histo-bar-lines.svg"), "Bars and Lines");

   return options;

 }

void	Continuous_histo_style_delegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const{
  QComboBox* edit = static_cast<QComboBox*>(editor);
  int i = edit->findText(index.data().toString());
  edit->setCurrentIndex(i);
}

void	Continuous_histo_style_delegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const{
  QComboBox* edit = static_cast<QComboBox*>(editor);
  int i = edit->currentIndex();

  if(i==0) model->setData(index, QVariant(), Qt::EditRole );

  QString option = edit->itemText(i);
   
  model->setData(index, option, Qt::EditRole );

}


QSize Continuous_histo_style_delegate::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option, index);
  /*
    if (qVariantCanConvert<QColor>(index.data())) {
        QColor color = qVariantValue<QColor>(index.data());
        return starRating.sizeHint();
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    */
}

void Continuous_histo_style_delegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
 {
     editor->setGeometry(option.rect);
 }