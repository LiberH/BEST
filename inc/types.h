#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>
#include <lemon/list_graph.h>

using namespace lemon;

typedef ListDigraph          Graph;
typedef ListDigraph::Arc     Arc;
typedef ListDigraph::ArcIt   ArcIt;
typedef ListDigraph::Node    Node;
typedef ListDigraph::NodeIt  NodeIt;

typedef uint64_t u64;
typedef uint32_t u32;

#endif // _TYPES_H_
