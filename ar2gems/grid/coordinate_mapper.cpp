#include <grid/coordinate_mapper.h>
#include <grid/geostat_grid.h>
#include <grid/rgrid.h>
#include <grid/structured_grid.h>

Coordinate_mapper::Coordinate_mapper(Geostat_grid* grid):grid_(grid) {}


GsTLPoint Coordinate_mapper::xyz_coords(int node_id) const
{
  return grid_->xyz_location(node_id);
}
GsTLPoint Coordinate_mapper::uvw_coords(int node_id)  const
{
  return grid_->location(node_id);
}