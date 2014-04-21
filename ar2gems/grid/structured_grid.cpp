#include <grid/structured_grid.h>

#include <vtkPoints.h>
#include <vtkCell.h>
#include <vtkGenericCell.h>

Named_interface* Structured_grid::create_new_interface(std::string& name){
  return new Structured_grid(name);
}


Structured_grid::Structured_grid()  : Cartesian_grid()
{
  sgrid_geom_ = vtkSmartPointer<vtkStructuredGrid>::New();
  cell_centers_filter_ = vtkSmartPointer<vtkCellCenters>::New();

  //cell_centers_filter_->SetInputConnection(sgrid_geom_->GetProducerPort());
  cell_centers_filter_->SetInputData(sgrid_geom_);

//  cellCentersFilter->SetInputData(imageData);

  
}  

Structured_grid::Structured_grid(std::string name) : Cartesian_grid(name)
{
  sgrid_geom_ = vtkSmartPointer<vtkStructuredGrid>::New();
  cell_centers_filter_ = vtkSmartPointer<vtkCellCenters>::New();
  cell_centers_filter_->SetInputData(sgrid_geom_);
}

Structured_grid::Structured_grid(std::string name, int nx, int ny, int nz) :
  Cartesian_grid(name)
{
  Cartesian_grid::set_dimensions( nx, ny, nz, (double)1.0/nx, (double)1.0/ny, (double)1.0/nz,0,0,0);
  sgrid_geom_ = vtkSmartPointer<vtkStructuredGrid>::New();
  cell_centers_filter_ = vtkSmartPointer<vtkCellCenters>::New();
  cell_centers_filter_->SetInputData(sgrid_geom_);
}


Structured_grid::~Structured_grid()
{
}


void Structured_grid::set_structured_points( std::vector<GsTLPoint>& corner_points){

  vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
  pts->SetDataTypeToDouble ();
  pts->SetNumberOfPoints( (this->nx()+1) * (this->ny()+1) * (this->nz()+1) );

  std::vector<GsTLPoint>::const_iterator it = corner_points.begin();
  for(int i=0 ; it!= corner_points.end(); ++it,++i) {
    pts->SetPoint(i,it->x(),it->y(),it->z());
  }
  
  sgrid_geom_->SetDimensions(this->nx()+1, this->ny()+1, this->nz()+1);
  sgrid_geom_->SetPoints(pts);

  cell_centers_filter_->Update();

  coord_mapper_ = new Structured_grid_coord_mapper(this);
  
}


GsTLPoint Structured_grid::get_corner_point_locations(int id) const {
  double d_coord[3];
  sgrid_geom_->GetPoint(id,d_coord);
  return GsTLPoint(d_coord[0],d_coord[1],d_coord[2]); 

}


Geostat_grid::location_type Structured_grid::xyz_location( int node_id ) const 
{
  
  appli_assert( node_id >= 0 && node_id <(signed int)( sgrid_geom_->GetNumberOfCells()) ); 

  double loc[3];
  cell_centers_filter_->GetOutput()->GetPoint(node_id,loc);
  return Geostat_grid::location_type(loc[0],loc[1],loc[2]);

} 

/*
double Structured_grid::get_support(int nodeid) const {
  vtkSmartPointer<vtkCell> cell = vtkSmartPointer<vtkCell>::New();
  sgrid_geom_->GetCell(nodeid)->Get
}
*/

/*
--------------------------
*/


Structured_grid_coord_mapper::Structured_grid_coord_mapper(Structured_grid* sgrid)
  :Coordinate_mapper(sgrid), sgrid_(sgrid)
{ 
  grid_cell_number_.x() = sgrid_->geometry()->dim(0);
  grid_cell_number_.y() = sgrid_->geometry()->dim(1);
  grid_cell_number_.z() = sgrid_->geometry()->dim(2);
}

GsTLPoint Structured_grid_coord_mapper::uvw_coords(GsTLPoint xyz)  {
  vtkSmartPointer<vtkStructuredGrid> geom = sgrid_->get_structured_geometry();


  // Need to get the ijk of the cell and add the parametric coordinate once normalized

  vtkSmartPointer<vtkGenericCell> gcell = vtkSmartPointer<vtkGenericCell>::New();
  
  double xyzcoord[3];
  xyzcoord[0] = xyz.x();
  xyzcoord[1] = xyz.y();
  xyzcoord[2] = xyz.z();

  double pcoord[3];
  double weights[8];
  int subid;

  int cellid = geom->FindCell(xyzcoord,0,gcell,-1,1e-4,subid,pcoord,weights);

  //If it outside the grid, ensure that it is so far away that it will get pickup within a neighborhood
  if(cellid <0 ) return GsTLPoint(xyzcoord[0] + 9e9,xyzcoord[1] + 9e9,xyzcoord[2] + 9e9);
  //geom->GetCell(cellid, gcell);  //may not be necessary if gcell is already stored in the FindCell
  int i,j,k;
  sgrid_->cursor()->coords(cellid,i,j,k);

  //Th u,v,w coordinates ranges from 0-1

  double u = (static_cast<double>(i) + pcoord[0])/grid_cell_number_.x();
  double v = (static_cast<double>(j) + pcoord[1])/grid_cell_number_.y();
  double w = (static_cast<double>(k) + pcoord[2])/grid_cell_number_.z();

  return GsTLPoint(u,v,w );

}


GsTLPoint Structured_grid_coord_mapper::xyz_coords(GsTLPoint uvw)  {
  vtkSmartPointer<vtkStructuredGrid> geom = sgrid_->get_structured_geometry();

  int i,j,k;
  int node_id = sgrid_->closest_node(uvw);
  sgrid_->cursor()->coords(node_id,i,j,k);

  //The coordinates wihtin the cell
  double pcoords[3];
  pcoords[0] = uvw.x() - static_cast<double>(i)/grid_cell_number_.x();
  pcoords[1] = uvw.y() - static_cast<double>(j)/grid_cell_number_.y();
  pcoords[2] = uvw.z() - static_cast<double>(k)/grid_cell_number_.z();

  double xyz[3];
  double* weights;
  vtkSmartPointer<vtkGenericCell> cell = vtkSmartPointer<vtkGenericCell>::New();
  geom->GetCell(node_id,cell);
  int subid=0;
  cell->EvaluateLocation(subid,pcoords,xyz,weights);
  return GsTLPoint(xyz[0],xyz[1],xyz[2]);


}