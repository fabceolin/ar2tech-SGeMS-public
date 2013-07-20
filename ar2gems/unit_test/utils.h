#ifndef __UTILS_H__ 
#define __UTILS_H__ 

#include <grid/rgrid_neighborhood.h>
#include <grid/mgrid_neighborhood.h>
#include <grid/rgrid.h>
#include <grid/grid_categorical_property.h>
#include <math/library_math_init.h>
#include <appli/library_appli_init.h>
#include <grid/library_grid_init.h>
#include <appli/manager_repository.h>
#include <grid/cartesian_grid.h>
#include <grid/grid_property.h>
#include <grid/neighbors.h>
#include <grid/reduced_grid.h>
#include <grid/combined_neighborhood.h>
#include <grid/point_set_neighborhood.h>
#include <grid/grid_random_path.h>
#include <grid/grid_path.h>
#include <grid/grid_ordered_path.h>
#include <grid/multi_grid_path.h>

#include <boost/test/unit_test.hpp>

#include "parallelism_utils/computational_task.h"
#include "parallelism_utils/computational_task_manager.h"


Cartesian_grid * cartesian_grid(std::string _grid_name);
Reduced_grid * reduced_grid( std::string _grid_name);
Point_set * point_set(std::string _point_set_name, int _point_set_size);

void delete_grid(std::string _grid_name);

Rgrid_window_neighborhood * six_face_rgrid_window_neighborhood(RGrid* _grid);
MgridWindowNeighborhood * six_face_mgrid_window_neighborhood(Reduced_grid * _grid);

Rgrid_ellips_neighborhood * six_face_rgrid_ellips_neighborhood(RGrid* _grid);
Rgrid_ellips_neighborhood * rgrid_ellips_neighborhood(RGrid* _grid, const GsTLTriplet & _ranges, const GsTLTriplet & _angles);
MgridNeighborhood * six_face_mgrid_neighborhood(Reduced_grid * _grid, Grid_continuous_property * _prop);
Rgrid_ellips_neighborhood_hd * six_face_rgrid_ellips_neighborhood_hd(RGrid* _grid, Grid_continuous_property * _prop);
MgridNeighborhood_hd * six_face_mgrid_neighborhood_hd(Reduced_grid * _grid, Grid_continuous_property * _prop);

Point_set_neighborhood * point_set_neighborhood(Point_set * _pset, const GsTLTriplet & _ranges, const GsTLTriplet & _angles);
Point_set_rectangular_neighborhood * point_set_rectangular_neighborhood(Point_set * _pset, Grid_continuous_property * _prop, 
  const GsTLTriplet & _ranges, const GsTLTriplet & _angles);


template <typename Iterator_type1, typename Iterator_type2>
boost::test_tools::predicate_result two_geovalue_lists_equal(
  Iterator_type1 begin1, Iterator_type1 end1, 
  Iterator_type2 begin2, Iterator_type2 end2)
{
  for (; begin1 != end1 && begin2 != end2; begin1++, begin2++)
  {
    if (begin1->node_id() != begin2->node_id()) return false;
  }
  if (begin1 == end1 && begin2 == end2) return true;
  return false;
}

template <typename Iterator_type1, typename Iterator_type2>
boost::test_tools::predicate_result two_lists_equal(
  Iterator_type1 begin1, Iterator_type1 end1, 
  Iterator_type2 begin2, Iterator_type2 end2)
{
  for (; begin1 != end1 && begin2 != end2; begin1++, begin2++)
  {
    if (*begin1 != *begin2) return false;
  }
  if (begin1 == end1 && begin2 == end2) return true;
  return false;
}

template <typename Iterator_type1, typename Iterator_type2>
boost::test_tools::predicate_result two_geovalue_sets_equal(
  Iterator_type1 begin1, Iterator_type1 end1, 
  Iterator_type2 begin2, Iterator_type2 end2) 
{ 
  std::set<int> set1, set2;
  for (; begin1 != end1 && begin2 != end2; begin1++, begin2++)
  {
    set1.insert(begin1->node_id());
    set2.insert(begin2->node_id());
  }
  if (begin1 != end1 || begin2 != end2) return false;

  std::set<int>::iterator sb1 = set1.begin();
  std::set<int>::iterator sb2 = set2.begin();
  for (; sb1 != set1.end() && sb2 != set2.end(); sb1++, sb2++)
  {
    if (*sb1 != *sb2) return false;
  }
  return true;
}



class Find_neighbors_task : public Computational_task
{
public:
  typedef boost::shared_ptr<Find_neighbors_task> Ptr;

  int run()
  {
    for (int i = 0; i < 1000; i++) {
      if ( nbh_ ) nbh_->find_neighbors(center_, neighbors_);
    }
    return 0;
  }

  const Neighbors * neighbors() const
  {
    return &neighbors_;
  }

  void center_is(const Geovalue & _center)
  {
    center_ = _center;
  }

  void neighborhood_is(const Neighborhood * nbh)
  {
    nbh_ = const_cast<Neighborhood *>(nbh);
  }

  static Ptr Ptr_new()
  {
    Ptr ptr(new Find_neighbors_task());
    return ptr;
  }

  ~Find_neighbors_task()
  {
    nbh_ = NULL;
  }


protected:
  Geovalue center_;
  Neighborhood * nbh_;
  Neighbors neighbors_;


protected:
  Find_neighbors_task()
  {
    nbh_ = NULL;
  }
};


class Find_all_neighbors_task : public Computational_task
{
public:
  typedef boost::shared_ptr<Find_all_neighbors_task> Ptr;

  int run()
  {
    for (int i = 0; i < 1000; i++) {
      if ( nbh_ ) nbh_->find_all_neighbors(center_, neighbors_);
    }
    return 0;
  }

  const Neighbors * neighbors() const
  {
    return &neighbors_;
  }

  void center_is(const Geovalue & _center)
  {
    center_ = _center;
  }

  void neighborhood_is(const Window_neighborhood * nbh)
  {
    nbh_ = const_cast<Window_neighborhood *>(nbh);
  }

  static Ptr Ptr_new()
  {
    Ptr ptr(new Find_all_neighbors_task());
    return ptr;
  }

  ~Find_all_neighbors_task()
  {
    nbh_ = NULL;
  }


protected:
  Geovalue center_;
  Window_neighborhood * nbh_;
  Neighbors neighbors_;


protected:
  Find_all_neighbors_task()
  {
    nbh_ = NULL;
  }
};


template <typename Iterator_type>
class Grid_path_task : public Computational_task
{
public:
  typedef boost::shared_ptr<Grid_path_task> Ptr;

  int run()
  {
    for (int i = 0; i < 1000; i++) {
      nodes_.clear();
      node_ids_.clear();
      geovalues_.clear();

      Iterator_type it = path_->begin();
      for (; it != path_->end(); it++) {
        nodes_.push_back(it);
      }
      for (int i = 0; i < path_->size(); i++) {
        node_ids_.push_back( path_->node_id(i) );
        geovalues_.push_back( path_->geovalue(i) );
      }
    }
    return 0;
  }

  const std::vector<Iterator_type> & nodes() const { return nodes_; }
  const std::vector<GsTLInt> & node_ids() const { return node_ids_; }
  const std::vector<Geovalue> & geovalues() const { return geovalues_; }

  void grid_path_is(const Grid_path<Iterator_type> * path)
  {
    path_ = const_cast<Grid_path<Iterator_type> *>(path);
  }

  static Ptr Ptr_new()
  {
    Ptr ptr(new Grid_path_task());
    return ptr;
  }

  ~Grid_path_task()
  {
    path_ = NULL;
  }


protected:
  Grid_path<Iterator_type> * path_;
  std::vector<Iterator_type> nodes_;
  std::vector<GsTLInt> node_ids_;
  std::vector<Geovalue> geovalues_;


protected:
  Grid_path_task()
  {
    path_ = NULL;
  }
};


#endif