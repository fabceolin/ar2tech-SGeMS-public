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

#include <charts/histogram_statistics.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkDataArrayIteratorMacro.h>

Continuous_statistics* build_histogram_table(int number_bins, const Grid_continuous_property* prop,const Grid_region* region, const Grid_filter* filter, float min, float max ){
  
  bool need_memory_swap = !prop->is_in_memory();
  if(need_memory_swap) {
    prop->swap_to_memory();
  }

  // always build the array manually since vtk 
  // output nan if nay data is a nan
  std::vector<bool> mask(prop->size(), false );

  bool filter_min_max = min<max;

  for(int i=0; i<prop->size(); ++i ) {
    if( !prop->is_informed(i) ) continue;
    if( region && !region->is_inside_region( i ) ) continue;
    if( filter && !filter->is_valid_nodeid( i ) ) continue;
    float v = prop->get_value(i);
    if(filter_min_max && (v< min || v > max) ) continue;
    mask[i] = true;
  }

  return build_histogram_table(number_bins, prop, mask, min,max );
}

Continuous_statistics* build_histogram_table(int number_bins, const Grid_continuous_property* prop, const std::vector<bool>& mask, float min, float max ){

  bool need_memory_swap = !prop->is_in_memory();
  if(need_memory_swap) {
    prop->swap_to_memory();
  }

  int ndata = 0;
  std::vector<bool>::const_iterator it = mask.begin();
  for( ; it != mask.end(); ++it) { 
    if(*it) ndata++;
  }

  if(ndata == 0 ) {
    return 0;
  }

  vtkSmartPointer<vtkFloatArray> x = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> cumulative_freq = vtkSmartPointer<vtkFloatArray>::New();
  
  x->Allocate(ndata);
  cumulative_freq->Allocate(ndata);
  for(int i=0; i< prop->size(); ++i) {
    if(mask[i]) {
      x->InsertNextValue(prop->get_value(i));
      cumulative_freq->InsertNextValue((float)i/(ndata+1));
    }
  }


  x->SetName(prop->name().c_str());

//  vtkDataArrayIteratorMacro(cumulative_freq,
  std::sort(x->Begin(),x->End());

  vtkSmartPointer<vtkTable> cumulative_freq_table = vtkSmartPointer<vtkTable>::New(); 
  cumulative_freq_table->AddColumn(x);
  cumulative_freq_table->AddColumn(cumulative_freq);

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New(); 
  table->AddColumn(x);

  //Compute statistics

  vtkSmartPointer<vtkDescriptiveStatistics> descriptive_stats  = vtkSmartPointer<vtkDescriptiveStatistics>::New();
  descriptive_stats->SetInputData(0, table );
  descriptive_stats->AddColumn( prop->name().c_str() );
  descriptive_stats->SetColumnStatus( prop->name().c_str(), 1 );
  descriptive_stats->RequestSelectedColumns();
  descriptive_stats->SetDeriveOption(true);
  descriptive_stats->SetAssessOption( false );
  descriptive_stats->Update();

  vtkSmartPointer<vtkMultiBlockDataSet> mblock_descriptive = vtkMultiBlockDataSet::SafeDownCast( descriptive_stats->GetOutputDataObject( vtkStatisticsAlgorithm::OUTPUT_MODEL ) );
  vtkSmartPointer<vtkTable> dtable_primary = vtkTable::SafeDownCast( mblock_descriptive->GetBlock( 0 ) );
  vtkSmartPointer<vtkTable> dtable_derived = vtkTable::SafeDownCast( mblock_descriptive->GetBlock( 1 ) );

  vtkSmartPointer<vtkVariantArray> desc_stats_array = vtkSmartPointer<vtkVariantArray>::New();
  desc_stats_array->SetNumberOfValues(10);
  desc_stats_array->SetValue(charts::DATA, prop->name().c_str());
  desc_stats_array->SetValue(charts::GRID, prop->grid_name().c_str());
  desc_stats_array->SetValue(charts::N, vtkVariant(ndata));
  desc_stats_array->SetValue(charts::MEAN, dtable_primary->GetValueByName(0,"Mean"));
  desc_stats_array->SetValue(charts::VARIANCE, dtable_derived->GetValueByName(0,"Variance"));
  desc_stats_array->SetValue(charts::MIN, dtable_primary->GetValueByName(0,"Minimum"));
  desc_stats_array->SetValue(charts::MAX, dtable_primary->GetValueByName(0,"Maximum"));
  desc_stats_array->SetValue(charts::SUM, desc_stats_array->GetValue(charts::MEAN).ToFloat()*ndata);
  desc_stats_array->SetValue(charts::SKEWNESS, dtable_derived->GetValueByName(0,"Skewness"));
  desc_stats_array->SetValue(charts::KURTOSIS, dtable_derived->GetValueByName(0,"Kurtosis"));

  vtkSmartPointer<vtkOrderStatistics> decile_stats = vtkSmartPointer<vtkOrderStatistics>::New();
  decile_stats->SetInputData( 0, table );
  decile_stats->AddColumn( prop->name().c_str() );
  decile_stats->SetColumnStatus( prop->name().c_str(), 1 );
  decile_stats->RequestSelectedColumns();
  decile_stats->SetQuantileDefinition( vtkOrderStatistics::InverseCDFAveragedSteps );
  decile_stats->SetNumberOfIntervals(10);
  decile_stats->SetQuantize(false);
  decile_stats->SetDeriveOption(true);
  decile_stats->SetAssessOption( false );
  decile_stats->Update();

  vtkSmartPointer<vtkMultiBlockDataSet> mblock_ordered = vtkMultiBlockDataSet::SafeDownCast( decile_stats->GetOutputDataObject( vtkStatisticsAlgorithm::OUTPUT_MODEL ) );
  unsigned nbq = mblock_ordered->GetNumberOfBlocks() - 1;

  vtkSmartPointer<vtkTable> ordered_stats_table = vtkTable::SafeDownCast( mblock_ordered->GetBlock( nbq ) );
  vtkAbstractArray* quantiles = ordered_stats_table->GetColumn(1);
  vtkSmartPointer<vtkVariantArray> quantile_stats_array = vtkSmartPointer<vtkVariantArray>::New();
  quantile_stats_array->SetNumberOfValues(13);
  quantile_stats_array->SetValue(0, prop->name().c_str());
  quantile_stats_array->SetValue(1, prop->grid_name().c_str());
  for(int i=0;i<11;++i) {
    quantile_stats_array->SetValue(i+2, quantiles->GetVariantValue(i));
  }

  // Compute the histogram with n bins
  vtkSmartPointer<vtkFloatArray> histo_p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_vmid = vtkSmartPointer<vtkFloatArray>::New();
  histo_p->SetName(prop->name().c_str());
  histo_vmid->SetName("Mid binning");
  histo_p->SetNumberOfValues(number_bins);
  histo_vmid->SetNumberOfValues(number_bins);


   //Find the max for each range
  if(min >= max) {  // do not use the user input min/max. get from data
    min =desc_stats_array->GetValue(charts::MIN).ToFloat();
    max = desc_stats_array->GetValue(charts::MAX).ToFloat();
  }
  float range= max-min;
  float bin_width = range/number_bins;

  vtkTable* full_histo = vtkTable::SafeDownCast( mblock_ordered->GetBlock( nbq - 2 ));
  vtkDoubleArray* d = vtkDoubleArray::SafeDownCast(full_histo->GetColumn(0));
  vtkDoubleArray* p = vtkDoubleArray::SafeDownCast( full_histo->GetColumn(2));
  for(int i=0; i<number_bins; ++i) {
    histo_p->SetValue(i,0.0);
    histo_vmid->SetValue(i, min+i*bin_width + bin_width/2);
  }
  for(int i=0; i<d->GetNumberOfTuples(); ++i) {
    float val = d->GetValue(i);
    float prob = p->GetValue(i);
    int bin_id = (val - min)/bin_width;
    if(bin_id >= number_bins) bin_id  = number_bins-1; // if the max value in data is the max range
    histo_p->SetValue( bin_id, histo_p->GetValue(bin_id) + prob );
  }

  vtkSmartPointer<vtkTable> histo_table =  vtkSmartPointer<vtkTable>::New();
  histo_table->AddColumn(histo_vmid);
  histo_table->AddColumn(histo_p);


  //Set the table for the line plot
  // same as histo_p but with min and max prepended and appended
  vtkSmartPointer<vtkFloatArray> histo_line_p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_line_bin = vtkSmartPointer<vtkFloatArray>::New();
  histo_line_p->SetName(prop->name().c_str());
  histo_line_bin->SetName("Mean binning");
  histo_line_p->SetNumberOfValues(2*number_bins);
  histo_line_bin->SetNumberOfValues(2*number_bins);


  for(int i=0; i<number_bins; ++i) {
    float mid_point  = histo_vmid->GetValue( i );
    histo_line_bin->SetValue(2*i,mid_point - bin_width/2);
    histo_line_bin->SetValue(2*i+1,mid_point + bin_width/2);
    
    histo_line_p->SetValue(2*i,histo_p->GetValue(i));
    histo_line_p->SetValue(2*i+1,histo_p->GetValue(i));

  }
  vtkSmartPointer<vtkTable> histo_line_table =  vtkSmartPointer<vtkTable>::New();
  histo_line_table->AddColumn(histo_line_bin);
  histo_line_table->AddColumn(histo_line_p);


  if(need_memory_swap) {
    prop->swap_to_disk();
  }
  return new Continuous_statistics(desc_stats_array, quantile_stats_array, histo_table, histo_line_table, cumulative_freq_table );

}

Continuous_statistics* build_histogram_table(int number_bins, const Grid_continuous_property* prop, const Grid_weight_property* weights,const Grid_region* region, const Grid_filter* filter, bool normalized_weights , float min, float max ){
  std::vector< std::pair<float, float> > data_weights;
  data_weights.reserve(prop->size());

  bool filter_min_max = min<max;

  double sum_weights = 0;
  for(int i=0; i<prop->size(); ++i ) {
    if( !prop->is_informed(i) ) continue;
    if( !weights->is_informed(i) ) continue;
    if( region && !region->is_inside_region( i ) ) continue;
    if( filter && !filter->is_valid_nodeid( i ) ) continue;

    float w = weights->get_value(i);
    if(w==0) continue;
    float v = prop->get_value(i);
    if(filter_min_max && ( v< min || v>max  )) continue;

    sum_weights += w;
    data_weights.push_back(  std::make_pair( prop->get_value(i), w ) );
  }

  if(normalized_weights) {
    std::vector< std::pair<float, float> >::iterator it = data_weights.begin();
    for(  ; it != data_weights.end(); ++it) {
      it->second /= sum_weights;
    }
  }
     
  return build_histogram_table(number_bins, data_weights, prop->name(), prop->grid_name(), min,max );
}

Continuous_statistics* build_histogram_table(int number_bins, const Grid_continuous_property* prop,const std::vector<float>& weights, const Grid_region* region, const Grid_filter* filter,bool normalized_weights , float min, float max ){


  bool need_memory_swap = !prop->is_in_memory();
  if(need_memory_swap) {
    prop->swap_to_memory();
  }
  bool filter_min_max = min<max;
  std::vector< std::pair<float, float> > data_weights;
  data_weights.reserve(prop->size());
  double sum_weights = 0;
  for(int i=0; i<prop->size(); ++i ) {
    if( !prop->is_informed(i) ) continue;
    if( region && !region->is_inside_region( i ) ) continue;
    if( filter && !filter->is_valid_nodeid( i ) ) continue;
    if(weights[i]==0) continue;
    float v = prop->get_value(i);
    if(filter_min_max && ( v< min || v>max  )) continue;


    sum_weights += weights[i];
    data_weights.push_back(  std::make_pair( v,weights[i] ) );
  }

  if(normalized_weights) {
    std::vector< std::pair<float, float> >::iterator it = data_weights.begin();
    for(  ; it != data_weights.end(); ++it) {
      it->second /= sum_weights;
    }
  }

  if(need_memory_swap) {
    prop->swap_to_disk();
  }

  return build_histogram_table(number_bins, data_weights, prop->name(), prop->grid_name() , min,max);

}

Continuous_statistics* build_histogram_table( int number_bins, vtkFloatArray* data, float min, float max ){

  int n = data->GetNumberOfTuples();
  std::vector< std::pair<float, float> > data_weights;
  data_weights.reserve(n);

  double weight = 1.0/n;
  for(int i=0; i< n; ++i) {
    data_weights.push_back(  std::make_pair( data->GetValue(i),weight ) );
  }

  return build_histogram_table(number_bins, data_weights, data->GetName(), data->GetName() , min,max);

}

Continuous_statistics* build_histogram_table(int number_bins, std::vector< std::pair<float, float> >& data_weights, std::string prop_name, std::string grid_name, float histo_min, float histo_max ){


  //Compute the first pass
  // Mean, min, max

  double mean = 0.0;
  float min = 9e20; 
  float max = -9e20;
  double sum=0;
  int n = data_weights.size();

  std::vector< std::pair<float, float> >::const_iterator it = data_weights.begin();
  for(  ; it != data_weights.end(); ++it) {

    mean += it->first * it->second;
    sum += it->first;
    if(it->first<min) min = it->first;
    if(it->first>max) max = it->first;

  }


  //Second pass compute
  double var = 0.0;
  double skew = 0.0; 
  double kurt = 0.0;

  for( it = data_weights.begin() ; it != data_weights.end(); ++it) {
    float v = it->first;
    float res = v - mean;
    float w = it->second;
    var += w*res*res;
    skew += w*res*res*res;
    kurt += w*res*res*res*res;
  }
  skew /= std::pow((double)var,1.5);
  kurt = kurt/std::pow((double)var,2) -3;

  vtkSmartPointer<vtkVariantArray> desc_stats_array = vtkSmartPointer<vtkVariantArray>::New();
  desc_stats_array->Allocate(10);

  desc_stats_array->SetValue(charts::DATA, prop_name.c_str());
  desc_stats_array->SetValue(charts::GRID, grid_name.c_str());
  desc_stats_array->SetValue(charts::N, vtkVariant(n));
  desc_stats_array->SetValue(charts::MEAN, mean);
  desc_stats_array->SetValue(charts::VARIANCE, var);
  desc_stats_array->SetValue(charts::MIN, min);
  desc_stats_array->SetValue(charts::MAX, max);
  desc_stats_array->SetValue(charts::SUM, sum);
  desc_stats_array->SetValue(charts::SKEWNESS, skew);
  desc_stats_array->SetValue(charts::KURTOSIS, kurt);


  // Compute the order stats
  vtkSmartPointer<vtkTable> cumulative_freq_table = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkFloatArray> x = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> cumulative_freq = vtkSmartPointer<vtkFloatArray>::New();
  x->SetNumberOfValues(data_weights.size());
  std::sort(data_weights.begin(), data_weights.end());
  //Loop over to compute the cumulative weight
  float w_cumul = 0;
  std::vector<float> cweights;
  cweights.reserve(data_weights.size());
  int index = 0;
  for( it =  data_weights.begin(); it != data_weights.end(); ++it, ++index ) {
    x->SetValue(index,it->first);
    w_cumul += it->second;
    cweights.push_back(w_cumul );
  }
  cumulative_freq->SetArray(&cweights[0],cweights.size(),1);
  cumulative_freq_table->AddColumn(x);
  cumulative_freq_table->AddColumn(cumulative_freq);



  //Find the decile
  vtkSmartPointer<vtkVariantArray> quantile_stats_array = vtkSmartPointer<vtkVariantArray>::New();
  quantile_stats_array->Allocate(13);
  quantile_stats_array->SetValue(charts::DATA, prop_name.c_str());
  quantile_stats_array->SetValue(charts::GRID, grid_name.c_str());
  int ip = 3;
  for(float p = 0.1; p<1.0; p+=0.1, ++ip) {
    std::vector<float>::iterator it = std::lower_bound(cweights.begin(),cweights.end(), p );
    int i = std::distance(cweights.begin(), it);
    float low_v = (i==0) ?  min : data_weights[i-1].first;
    float up_v = (it==cweights.end()) ?  max : data_weights[i].first;
    float low_p = (i==0) ?  0.0 : cweights[i-1];
    float up_p = (it==cweights.end()) ?  1.0 : cweights[i];
    float perc = (p-low_p)/(up_p - low_p)*(up_v-low_v) + low_v;
    
    quantile_stats_array->SetValue(ip,perc);
  }
  quantile_stats_array->SetValue(2,min);
  quantile_stats_array->SetValue(12,max);

  // Compute the histogram with n bins
  vtkSmartPointer<vtkFloatArray> histo_p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_vmid = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_vmean = vtkSmartPointer<vtkFloatArray>::New();
  histo_p->SetName(prop_name.c_str());
  histo_vmid->SetName("Mid binning");
  histo_vmean->SetName("Mean binning");
  histo_p->SetNumberOfValues(number_bins);
  histo_vmid->SetNumberOfValues(number_bins);
  histo_vmean->SetNumberOfValues(number_bins);

  //Find the max for each range

  if(histo_min != histo_max) {
    min = histo_min;
    max = histo_max;
  }

  float range= max-min;
  float bin_width = range/number_bins;

  //Get the first bin

  std::vector<std::pair<float,float> >::iterator low_it_cw = data_weights.begin();
  std::vector<std::pair<float,float> >::iterator up_it_cw;
  int low_index = 0;
  int up_index;
  
  for( int b = 0; b<number_bins; ++b ) {
    float lower_v = (float)(b+1)*bin_width + min;
    //up_it_cw = std::lower_bound(data_weights.begin(),data_weights.end(), std::make_pair(lower_v,(float)0.0));
    up_it_cw = std::lower_bound(low_it_cw,data_weights.end(), std::make_pair(lower_v,(float)0.0));

    //The itertator is one pass the last item in the bin
    up_index = std::distance(data_weights.begin(),up_it_cw);
    up_index = up_index <= low_index ? low_index : up_index-1;
    histo_p->SetValue( b, (up_index==0) ? 0 : cweights[up_index]-cweights[low_index] );
    histo_vmid->SetValue( b, (float)(b+0.5)*bin_width + min );
    int n_v = std::distance(low_it_cw,up_it_cw);
    if(n_v == 0)
      histo_vmean->SetValue( b, lower_v );
    else
      histo_vmean->SetValue( b, std::accumulate(low_it_cw,up_it_cw, 0.0, accumulate_pairs() )/n_v );

    low_it_cw = up_it_cw;
    low_index = up_index;
  }
  
  vtkSmartPointer<vtkTable> histo_table =  vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkTable> cum_histo_table =  vtkSmartPointer<vtkTable>::New();
  histo_table->AddColumn(histo_vmid);
  histo_table->AddColumn(histo_vmean);
  histo_table->AddColumn(histo_p);


  //Set the table for the line plot
  // same as histo_p but with min and max prepended and appended
  vtkSmartPointer<vtkFloatArray> histo_line_p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_line_bin = vtkSmartPointer<vtkFloatArray>::New();
  histo_line_p->SetName(prop_name.c_str());
  histo_line_bin->SetName("Mean binning");
  histo_line_bin->SetName("Mean binning");
  histo_line_p->SetNumberOfValues(2*number_bins);
  histo_line_bin->SetNumberOfValues(2*number_bins);

  for(int i=0; i<number_bins; ++i) {
    float mid_point  = histo_vmid->GetValue( i );
    histo_line_bin->SetValue(2*i,mid_point - bin_width/2);
    histo_line_bin->SetValue(2*i+1,mid_point + bin_width/2);
    
    histo_line_p->SetValue(2*i,histo_p->GetValue(i));
    histo_line_p->SetValue(2*i+1,histo_p->GetValue(i));

  }
  vtkSmartPointer<vtkTable> histo_line_table =  vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkTable> cum_histo_line_table =  vtkSmartPointer<vtkTable>::New();
  histo_line_table->AddColumn(histo_line_bin);
  histo_line_table->AddColumn(histo_line_p);

  return new Continuous_statistics(desc_stats_array, quantile_stats_array, histo_table, histo_line_table, cumulative_freq_table);
}

Continuous_statistics* build_histogram_table(Non_parametric_distribution* distribution ){
  
  std::vector<Non_parametric_distribution::z_iterator::value_type> z_values(distribution->z_begin(),distribution->z_end());
  
  // Transfer statistics from the distribution to the histogram
  vtkSmartPointer<vtkVariantArray> desc_stats_array = vtkSmartPointer<vtkVariantArray>::New();
  desc_stats_array->Allocate(10);
  desc_stats_array->SetValue(charts::DATA, "DATA");
  desc_stats_array->SetValue(charts::GRID, "GRID");
  desc_stats_array->SetValue(charts::N, vtkVariant(z_values.size()) );
  desc_stats_array->SetValue(charts::MEAN, distribution->mean());
  desc_stats_array->SetValue(charts::VARIANCE, distribution->variance());
  desc_stats_array->SetValue(charts::MIN, distribution->quantile(0.0));
  desc_stats_array->SetValue(charts::MAX, distribution->quantile(1.0));
  desc_stats_array->SetValue(charts::SUM, 1.0);
  desc_stats_array->SetValue(charts::SKEWNESS, distribution->skewness());
  desc_stats_array->SetValue(charts::KURTOSIS, distribution->kurtosis());

  // Find the decile
  vtkSmartPointer<vtkVariantArray> quantile_stats_array = vtkSmartPointer<vtkVariantArray>::New();
  quantile_stats_array->Allocate(13);
  quantile_stats_array->SetValue(charts::DATA, "DATA");
  quantile_stats_array->SetValue(charts::GRID, "GRID");
  quantile_stats_array->SetValue(2,desc_stats_array->GetValue(charts::MIN) );
  int ip = 3;
  for(float p = 0.1; p<1.0; p+=0.1, ++ip) {
    quantile_stats_array->SetValue(ip,distribution->quantile(p) );
  }
  quantile_stats_array->SetValue(12,desc_stats_array->GetValue(charts::MAX));

  // construct the data histogram and cumulative histogram (no binning, uses data values directly)
  vtkSmartPointer<vtkFloatArray> histo_value    = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_freq     = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_cumfreq  = vtkSmartPointer<vtkFloatArray>::New();
  histo_value->SetName("DATA");
  histo_freq->SetName("FREQUENCY");
  histo_cumfreq->SetName("CUMULATIVE_FREQUENCY");

  // Initialize
  int nDataPoints = z_values.size();
  double previousvalue = z_values[0] ; // minimum = first value from z_value array (sorted and cleaned from doublets)
  histo_value->InsertNextValue(previousvalue);
  histo_freq->InsertNextValue(0.0);
  double oldcumfreq = distribution->cdf(previousvalue);
  histo_cumfreq->InsertNextValue(oldcumfreq);

  // Recursion
  for (int ii=1; ii<nDataPoints; ++ii){
    double currentvalue = z_values[ii];
    double cumfreq = distribution->cdf(z_values[ii]);
    if (currentvalue==previousvalue)
    {
      std::cout<< "problem here";
    }
    double freq = (cumfreq - oldcumfreq)/(currentvalue-previousvalue);
    histo_value->InsertNextValue(currentvalue);
    histo_cumfreq->InsertNextValue(cumfreq);
    histo_freq->InsertNextValue(freq);
    oldcumfreq=cumfreq;
    previousvalue=currentvalue;
  }
  
  vtkSmartPointer<vtkTable> histo_table =  vtkSmartPointer<vtkTable>::New();
  histo_table->AddColumn(histo_value);
  histo_table->AddColumn(histo_freq);

  vtkSmartPointer<vtkTable> cumulative_histo_table =  vtkSmartPointer<vtkTable>::New();
  cumulative_histo_table->AddColumn(histo_value);
  cumulative_histo_table->AddColumn(histo_cumfreq);
  
  
  /* OLD CODE
  
  // Compute the histogram with n bins
  vtkSmartPointer<vtkFloatArray> histo_p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_vmid = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_vmean = vtkSmartPointer<vtkFloatArray>::New();
  histo_p->SetName("DATA");
  histo_vmid->SetName("Mid binning");
  histo_vmean->SetName("Mean binning");

  // the number of points is a function of the data spread
  double max_step = (desc_stats_array->GetValue(charts::MAX)-desc_stats_array->GetValue(charts::MIN))/1000;
  std::vector<Non_parametric_distribution::z_iterator::value_type  > resampled_z_values;
  std::vector<Non_parametric_distribution::p_iterator::value_type  > resampled_p_values;
  for (int ii=1; ii<z_values.size();++ii){
    Non_parametric_distribution::z_iterator::value_type spread = z_values[ii]-z_values[ii-1];
    if (spread>max_step){
      int number_of_entire_steps_fitting_in_the_initial_spread = spread/max_step;
      Non_parametric_distribution::z_iterator::value_type new_spread=spread/(number_of_entire_steps_fitting_in_the_initial_spread+1.0);
      for (int jj=0; jj<=number_of_entire_steps_fitting_in_the_initial_spread; ++jj){ // LEss OR EQUAL: to ensure that we have cut the interval
        Non_parametric_distribution::z_iterator::value_type z = z_values[ii-1]+(float)jj*new_spread;
        resampled_z_values.push_back(z);
        resampled_p_values.push_back(distribution->cdf(z));
      }
    }else{
      resampled_z_values.push_back(z_values[ii-1]);
      resampled_p_values.push_back(distribution->cdf(z_values[ii-1]));
    }
  }
  // insert last value
  resampled_z_values.push_back(z_values.back());
  resampled_p_values.push_back(1.0);

  int n_values = resampled_z_values.size();

  histo_p->SetNumberOfValues(n_values);
  histo_vmid->SetNumberOfValues(n_values);
  histo_vmean->SetNumberOfValues(n_values);

  for (int ii=0; ii < n_values; ++ii){
    histo_p->SetValue(ii,resampled_p_values[ii]);
    histo_vmean->SetValue(ii,resampled_z_values[ii]);
    histo_vmid->SetValue(ii,resampled_z_values[ii]);
  }

  vtkSmartPointer<vtkTable> histo_table =  vtkSmartPointer<vtkTable>::New();
  histo_table->AddColumn(histo_vmid);
  histo_table->AddColumn(histo_vmean);
  histo_table->AddColumn(histo_p);

  */

  // Set the table for the line plot
  // same as histo_p but with min and max prepended and appended
  int n_values=histo_value->GetNumberOfTuples();
  vtkSmartPointer<vtkFloatArray> histo_line_value = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_line_freq = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_line_cumfreq = vtkSmartPointer<vtkFloatArray>::New();
  histo_line_value->SetName("DATA");
  histo_line_freq->SetName("FREQUENCY_LINE");
  histo_line_cumfreq->SetName("CUMULATIVE_FREQUENCY_LINE");
  histo_line_value->SetNumberOfValues(2*n_values);
  histo_line_freq->SetNumberOfValues(2*n_values);
  histo_line_cumfreq->SetNumberOfValues(2*n_values);

  for(int i=0; i<n_values-1; ++i) {
    float mid_point  = histo_value->GetValue( i );
    float bin_width = ( histo_value->GetValue( i+1 ) - histo_value->GetValue( i ));
    histo_line_value->SetValue(2*i,mid_point - bin_width/2);
    histo_line_value->SetValue(2*i+1,mid_point + bin_width/2);
    histo_line_freq->SetValue(2*i,histo_freq->GetValue(i));
    histo_line_freq->SetValue(2*i+1,histo_freq->GetValue(i));
    histo_line_cumfreq->SetValue(2*i,histo_cumfreq->GetValue(i));
    histo_line_cumfreq->SetValue(2*i+1,histo_cumfreq->GetValue(i));
  }
  vtkSmartPointer<vtkTable> histo_line_table =  vtkSmartPointer<vtkTable>::New();
  histo_line_table->AddColumn(histo_line_value);
  histo_line_table->AddColumn(histo_line_freq);
  histo_line_table->AddColumn(histo_line_cumfreq);

  return new Continuous_statistics(desc_stats_array, quantile_stats_array, histo_table, histo_line_table, cumulative_histo_table);
   
}
