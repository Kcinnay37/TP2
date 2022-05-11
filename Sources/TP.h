#pragma once
#include <stdint.h>
#include <assert.h>
#include <map>

void* allocate(size_t size);

typedef struct {
	void** elements;
	int top;
	size_t max;
} Stack;

Stack stack_init(size_t max_size);
void stack_push(Stack* s, void* element);
void* stack_pop(Stack* s);


typedef struct QNode QNode;
typedef struct QNode {
	void* data;
	QNode* prev;
	QNode* next;
	size_t count;
}Queue;

void queue_init(Queue* q);
void queue_push(Queue* q, void* data);
void* queue_pop(Queue* q);


typedef struct {
	double x;
	double y;
} Vector2;


typedef struct {
	int cost;
	Vector2 position;
	uint8_t graph_group;
	void* data;
	int path_from;
	uint8_t visited;
	int index;
} Node;

typedef struct AdjMatrix AdjMatrix;
struct AdjMatrix {
	int** adjGraph;
	Node* nodes;
	size_t len;
	size_t max_size;
};


typedef struct NodeAdj NodeAdj;

struct NodeAdj
{
	int len;
	int cost;
	int index;
	int maxSize;
	int path_from;
	uint8_t visited;
	uint8_t group;
	Vector2 position;
	void* data;
	NodeAdj** adj;
};

NodeAdj* create_nodeRootAdj(int maxSizeAdj);

NodeAdj* create_nodeAdj(int maxSizeAdj, void* data);

void add_RootAdjacent_node(NodeAdj* root, NodeAdj node, Vector2 position);

void add_edgeListAdj(NodeAdj* graph, int fromNode, int toNode, uint8_t cost);

AdjMatrix* create_graph(size_t max_nodes);

void add_node(AdjMatrix* graph, void* data, Vector2 pos);

void add_edge(AdjMatrix* graph, int fromNode, int toNode, uint8_t cost);

void build_groups(AdjMatrix* graph);

void astar(AdjMatrix* graph, int startNodeIndex, int endNodeIndex, Stack* solvedPath);

void astarListAdj(NodeAdj* root, int startNodeIndex, int endNodeIndex, Stack* solvedPath);

void ResetGroup(uint8_t lastGroup, uint8_t newGroup, AdjMatrix* graph);

int CheckDistance(Node* endNode, Node* currNode);

int CheckDistanceListAdj(NodeAdj* endNode, NodeAdj* currNode);

void SetNode(int width, int height, int channel, unsigned char* image, AdjMatrix* matrix, std::map<double, int>& index);

void CheckVoisin(int x, int y, AdjMatrix* matrix, std::map<double, int>& index);

void DrawChemin(Stack* s);

void DrawCheminListAdj(Stack* s);

int GetNombreWithPixel(unsigned char* image, int width, int height, int channel);

double GetCoordDouble(int x, int y);

void SetEdge(int width, int height, int channel, unsigned char* image, AdjMatrix* matrix, std::map<double, int>& index);

void CheckVoisin(int x, int y, AdjMatrix* matrix, std::map<float, int>& index);

void SetNodeListAdj(int width, int height, int channel, unsigned char* image, NodeAdj* root, std::map<double, int>& index);

void SetEdgeListAdj(int width, int height, int channel, unsigned char* image, NodeAdj* root, std::map<double, int>& index);

void CheckVoisinListAdj(int x, int y, NodeAdj* root, std::map<double, int>& index);