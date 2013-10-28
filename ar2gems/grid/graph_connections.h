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



//using namespace boost;

class GRID_DECL Graph_connection {
public:
	Graph_connection() {}
	virtual ~Graph_connection() {}

	// Reserve the memory : use first for faster initialization
	void set_number_of_cells(int ncells ); //{ vertex_vec.reserve(ncells); }
	void set_number_of_connections(int nconnections ); //{ edge_vec_.reserve(nconnections); }

	// graph structure
	void add_cell(int node_id, double x, double y, double z);
	bool add_connection(int node_id1, int node_id2,  double weight=1.0, double dum = 0.0);

	// set edge or vertex properties
	void set_edge_weight( int edge_index ); //{ no idea }

	// get information about the graph
	bool is_connected(int node_id1, int node_id2) const;
	void direct_connections(int nodeid, std::vector<int>& connections);

private:

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

	// Iterator for the vertices
	//std::pair<boost::graph_traits<cell_graph_t>::out_edge_iterator,boost::graph_traits<cell_graph_t>::out_edge_iterator> it_edgePair_;
	// obtain iterators (needs better documentation)
	//std::pair<boost::graph_traits<cell_graph_t>::out_edge_iterator,boost::graph_traits<cell_graph_t>::out_edge_iterator> get_iterator_pair_for_edges_from_vertex(int vertexidx);

	

};


#endif