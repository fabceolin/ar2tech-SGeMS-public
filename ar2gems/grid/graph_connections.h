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

#ifndef GRAPH_CONNECTIONS_H_ 
#define GRAPH_CONNECTIONS_H_ 

#include <grid/common.h>

// Boost libraries

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>

/*! \class GRID_DECL
    \brief Class containing the graph representation of the grid (neighbors, connections)
*/ 

class GRID_DECL Graph_connection {
public:
	Graph_connection() {}
	virtual ~Graph_connection() {}

  /// \brief Important: reserve memory BEFORE FIRST USE for all grid blocks
  /// \param ncells Number of cells, grid blocks
	void set_number_of_cells(int ncells ); //{ vertex_vec.reserve(ncells); }

  /// \brief Important: reserve memory BEFORE FIRST USE for all connections
  /// \param nconnections Number of connections between grid blocks
	void set_number_of_connections(int nconnections ); //{ edge_vec_.reserve(nconnections); }

  /// \brief Get number of connections
  int get_number_of_connections();


  /// \brief Add one vertex (ie. point) to the graph
	/// \param node_id integer with the node index
  /// \param double x : x-position of vertex (usually grid block center, but can be anything)
  /// \param double y : y-position of vertex (usually grid block center, but can be anything)
  /// \param double z : z-position of vertex (usually grid block center, but can be anything)
	void add_cell(int node_id, double x, double y, double z);

  /// \brief Add one connection (ie. edge linking two vertices) to the graph
	/// \param node_id1 integer with the first node index
	/// \param node_id2 integer with the second node index
  /// \param weight: a weight for the connection (could be used for anything)
  /// \param dum : for now, just a dummy variable
  /// \return bool: has the connection been added successfully?
	bool add_connection(int node_id1, int node_id2,  double weight=1.0, double dum = 0.0);

	/// \brief Set edge weight
  /// \param node_id1 integer index of the first node (ie. vertex)
  /// \param node_id2 integer index of the second node (ie. vertex)
  /// \param double weight value
	void set_edge_weight( int node_id1, int node_id2, double weight_value ); 

	/// \brief Check if two points are connected
  /// \param node_id1 integer index of the first node (ie. vertex)
  /// \param node_id2 integer index of the second node (ie. vertex)
  /// \return bool true (the two vertices are connected) or false
	bool is_connected(int node_id1, int node_id2) const;

	/// \brief Obtain a vector of connected vertices
  /// \param nodeid integer index of the node
  /// \param[out] vector of integer with the node index connected to modeid
	void direct_connections(int nodeid, std::vector<int>& connections);

private:

  int nEdges_;
  int nVertex_;
	struct Vertex_data { double xc; double yc; double zc; };
	struct Edge_data { double dum; double weight;};

	// Define the type of graph we are going to need
	typedef boost::adjacency_list <boost::vecS,boost::vecS,boost::undirectedS,Vertex_data,Edge_data> cell_graph_t;
	cell_graph_t graph_;

	//Some typedefs for simplicity
	typedef boost::graph_traits<cell_graph_t>::vertex_descriptor vertex_t;
	typedef boost::graph_traits<cell_graph_t>::edge_descriptor edge_t;
	std::vector<vertex_t> vertex_vec_;
	std::vector<edge_t> edge_vec_;

};

#endif