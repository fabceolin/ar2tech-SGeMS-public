#include <grid/graph_connections.h>

#include <boost/graph/adjacency_iterator.hpp>
#include <boost/graph/adjacency_list.hpp>


void Graph_connection::set_number_of_cells(int ncells )
{ 
	vertex_vec_.reserve(ncells); 
}

void Graph_connection::set_number_of_connections(int nconnections )
{ 
	edge_vec_.reserve(nconnections); 
}

void Graph_connection::add_cell(int node_id, double x, double y, double z)  	{
	vertex_t vertex = boost::add_vertex(graph_);
	// Assign properties
	graph_[vertex].xc = x;
	graph_[vertex].yc = y;
	graph_[vertex].zc = z;
	vertex_vec_.push_back(vertex);
	// WARNING: THE node_id variable is never used:
	// this assumes that vertex indices are identical to the grid it is called from
};

bool Graph_connection::add_connection(int node_id1, int node_id2,  double weight, double dum)	{
	edge_t edge; bool success;

	boost::tie(edge,success) = boost::add_edge(vertex_vec_[node_id1],vertex_vec_[node_id2],graph_);
	// Add a property for the edge
	if(!success) return false;
	graph_[edge].dum = dum;
	graph_[edge].weight = weight;
	edge_vec_.push_back(edge);
  return success;
}

void Graph_connection::set_edge_weight( int edge_index ){

}


bool Graph_connection::is_connected(int node_id1, int node_id2) const{

	return false;
}

void Graph_connection::direct_connections(int nodeid, std::vector<int>& connections){
	connections.clear();
	// form a pair of iterators for the edges linked to the vertex at nodeid
	std::pair<boost::graph_traits<cell_graph_t>::out_edge_iterator,boost::graph_traits<cell_graph_t>::out_edge_iterator> it_edgePair;
	for (it_edgePair = boost::out_edges(vertex_vec_[nodeid], graph_); it_edgePair.first != it_edgePair.second; ++it_edgePair.first)
	{
	  // screen out: std::cout << "   [herve] (" << source(*it_edgePair.first, graph_) << "," << target(*it_edgePair.first, graph_) << ")" << std::endl;
    connections.push_back(target(*it_edgePair.first, graph_));
  }
}
