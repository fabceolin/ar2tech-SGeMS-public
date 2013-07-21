#include "utils.h"


Cartesian_grid * cartesian_grid(std::string _grid_name) 
{
  libGsTLAppli_math_init();
	libGsTLAppli_appli_init();
	libGsTLAppli_grid_init();

  // root
  Manager* root = Root::instance();
  if (root == NULL) return NULL;

  // create grid
  SmartPtr<Named_interface> ni = Root::instance()->new_interface( "cgrid://" + _grid_name,
                                        gridModels_manager + "/" + _grid_name );
  Cartesian_grid * grid = dynamic_cast<Cartesian_grid*>(ni.raw_ptr());
  
  return grid;
}

Reduced_grid * reduced_grid( std::string _grid_name)
{
  libGsTLAppli_math_init();
	libGsTLAppli_appli_init();
	libGsTLAppli_grid_init();

  // root
  Manager* root = Root::instance();
  if (root == NULL) return NULL;

  // create grid
  SmartPtr<Named_interface> ni = Root::instance()->new_interface( "reduced_grid://" + _grid_name,
                                        gridModels_manager + "/" + _grid_name );
  Reduced_grid * grid = dynamic_cast<Reduced_grid*>(ni.raw_ptr());
  
  return grid;
}

Point_set * 
point_set(std::string _point_set_name, int _point_set_size)
{
  libGsTLAppli_math_init();
	libGsTLAppli_appli_init();
	libGsTLAppli_grid_init();

  // root
  Manager* root = Root::instance();
  if (root == NULL) return NULL;

  // create point set
  std::string name_size_str = _point_set_name +"::"+ String_Op::to_string( _point_set_size );
  SmartPtr<Named_interface> ni = Root::instance()->new_interface( "point_set://" + name_size_str,
				  gridModels_manager + "/" + _point_set_name );
  Point_set* pset = dynamic_cast<Point_set*>( ni.raw_ptr() );
  
  return pset;
}

void delete_grid(std::string _grid_name)
{
  Root::instance()->delete_interface("cgrid://" + _grid_name);
}


Rgrid_window_neighborhood * six_face_rgrid_window_neighborhood(RGrid* _grid)
{
	Grid_template window_tpl;
	for(int i = -1; i<=1; i++) {
		for(int j = -1; j<=1; j++) {
			for(int k = -1; k<=1; k++) {
				// skip the center node
				if(i==0  && j ==0 && k==0) continue;
				if( std::abs(i) + std::abs(j) + std::abs(k) > 1 ) continue;
				window_tpl.add_vector(i,j,k);
			}
		}
	}
	return (Rgrid_window_neighborhood *) _grid->window_neighborhood(window_tpl);
}


MgridWindowNeighborhood * six_face_mgrid_window_neighborhood(Reduced_grid * _grid)
{
	Grid_template window_tpl;
	for(int i = -1; i<=1; i++) {
		for(int j = -1; j<=1; j++) {
			for(int k = -1; k<=1; k++) {
				// skip the center node
				if(i==0  && j ==0 && k==0) continue;
				if( std::abs(i) + std::abs(j) + std::abs(k) > 1 ) continue;
				window_tpl.add_vector(i,j,k);
			}
		}
	}
	
  return (MgridWindowNeighborhood *) _grid->window_neighborhood(window_tpl);
}


Rgrid_ellips_neighborhood * 
six_face_rgrid_ellips_neighborhood(RGrid* _grid)
{
	GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  Rgrid_ellips_neighborhood * neigh = (Rgrid_ellips_neighborhood *)_grid->neighborhood(ranges, angles);
  neigh->includes_center(false);
  neigh->max_size(6); // 6 faces
  return neigh;
}

Rgrid_ellips_neighborhood * 
rgrid_ellips_neighborhood(RGrid* _grid, const GsTLTriplet & _ranges, const GsTLTriplet & _angles)
{
  Rgrid_ellips_neighborhood * neigh = (Rgrid_ellips_neighborhood *)_grid->neighborhood(_ranges, _angles);
  neigh->includes_center(false);
  neigh->max_size(neigh->geometry().size());
  return neigh;
}

MgridNeighborhood * 
six_face_mgrid_neighborhood(Reduced_grid * _grid, Grid_continuous_property * _prop)
{
  GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  MgridNeighborhood * neigh = new MgridNeighborhood(_grid, _prop, ranges[0], ranges[1], ranges[2], angles[0], angles[1], angles[2]);
  neigh->includes_center(false);
  neigh->max_size(6); // 6 faces
  return neigh;
}

Rgrid_ellips_neighborhood_hd * 
six_face_rgrid_ellips_neighborhood_hd(RGrid * _grid, Grid_continuous_property * _prop)
{
  GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  Rgrid_ellips_neighborhood_hd * neigh = new Rgrid_ellips_neighborhood_hd(_grid, _prop, ranges[0], ranges[1], ranges[2], angles[0], angles[1], angles[2]);
  neigh->includes_center(false);
  neigh->max_size(6); // 6 faces
  return neigh;
}

MgridNeighborhood_hd * 
six_face_mgrid_neighborhood_hd(Reduced_grid * _grid, Grid_continuous_property * _prop)
{
  GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  MgridNeighborhood_hd * neigh = new MgridNeighborhood_hd(_grid, _prop, ranges[0], ranges[1], ranges[2], angles[0], angles[1], angles[2]);
  neigh->includes_center(false);
  neigh->max_size(6); // 6 faces
  return neigh;
}

Point_set_neighborhood * point_set_neighborhood(Point_set * _pset, const GsTLTriplet & _ranges, const GsTLTriplet & _angles)
{
  Point_set_neighborhood * neigh = (Point_set_neighborhood *) _pset->neighborhood(_ranges, _angles);
  neigh->includes_center(false);
  return neigh;
}

Point_set_rectangular_neighborhood * point_set_rectangular_neighborhood(Point_set * _pset, Grid_continuous_property * _prop, 
  const GsTLTriplet & _ranges, const GsTLTriplet & _angles)
{
  Point_set_rectangular_neighborhood * neigh = new Point_set_rectangular_neighborhood(
    _ranges[0], _ranges[1], _ranges[2], 
    _angles[0], _angles[1], _angles[2], 
    20, 
    _pset, _prop);
  neigh->includes_center(false);
  return neigh;
}

