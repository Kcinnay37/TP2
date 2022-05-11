#include "TP.h"
#include <math.h>
#include <stdlib.h>
#include<stdio.h>
#include <string.h>
#include <time.h>

#include "optick.h"

#include "stb_image.h"
#include "stb_image_write.h"

Stack stack_init(size_t max_size) 
{
	Stack s;
	s.max = max_size;
	s.top = -1;
	s.elements = (void**)allocate(sizeof(void*) * s.max);
	return s;
}

void stack_push(Stack* s, void* element) 
{
	if (s->max != s->top + 1) {
		s->top++;
		s->elements[s->top] = element;
	}
}

void* stack_pop(Stack* s) {
	if (s->top > -1) {
		void* elem = s->elements[s->top];
		s->elements[s->top] = NULL;
		--s->top;
		return elem;
	}
	return NULL;
}

void queue_init(Queue* q) 
{
	q->next = q->prev = NULL;
	q->count = 0;
}

void queue_push(Queue* q, void* data) 
{
	QNode* n = (QNode*)allocate(sizeof(Node));
	n->data = data;
	n->prev = n->next = NULL;
	if (q->prev == NULL) {
		q->prev = q->next = n;
	}
	else {
		QNode* temp = q->next;
		q->next = temp->prev = n;
		n->next = temp;
	}
}

void* queue_pop(Queue* q) 
{
	if (q->prev != NULL) {
		QNode* temp = q->prev;
		if (temp->prev != NULL)
			temp->prev->next = NULL;
		q->prev = temp->prev;
		q->count++;
		return temp->data;
	}
	return NULL;
}

NodeAdj* create_nodeRootAdj(int maxSizeAdj)
{
	OPTICK_EVENT("create_nodeRootAdj");
	NodeAdj* newNode = (NodeAdj*)allocate(sizeof(NodeAdj));
	
	newNode->adj = (NodeAdj**)allocate(sizeof(NodeAdj**) * maxSizeAdj);
	for (int i = 0; i < maxSizeAdj; i++)
	{
		newNode->adj[i] = (NodeAdj*)allocate(sizeof(NodeAdj*));
		memset(newNode->adj[i], 0, sizeof(NodeAdj*));
	}
	
	newNode->len = 0;
	newNode->cost = 0;
	newNode->index = 0;
	newNode->maxSize = maxSizeAdj;
	newNode->visited = 0;
	newNode->group = 0;
	newNode->position.x = 0;
	newNode->position.y = 0;
	newNode->data = NULL;
	newNode->path_from = 0;

	return newNode;
}

NodeAdj* create_nodeAdj(int maxSizeAdj, void* data)
{
	NodeAdj* newNode = (NodeAdj*)allocate(sizeof(NodeAdj));

	newNode->adj = (NodeAdj**)allocate(sizeof(NodeAdj**) * maxSizeAdj);
	for (int i = 0; i < maxSizeAdj; i++)
	{
		newNode->adj[i] = (NodeAdj*)allocate(sizeof(NodeAdj*));
		memset(newNode->adj[i], 0, sizeof(NodeAdj*));
	}

	newNode->len = 0;
	newNode->cost = 0;
	newNode->index = 0;
	newNode->maxSize = maxSizeAdj;
	newNode->visited = 0;
	newNode->group = 0;
	newNode->position.x = 0;
	newNode->position.y = 0;
	newNode->data = data;
	newNode->path_from = 0;

	return newNode;
}

void add_RootAdjacent_node(NodeAdj* root, NodeAdj* node, Vector2 position)
{
	if (root->len == root->maxSize)
	{
		return;
	}
	node->index = root->len;
	node->position = position;
	root->adj[root->len++] = node;
}

void add_edgeListAdj(NodeAdj* root, int fromNode, int toNode, uint8_t cost)
{
	NodeAdj* nodeToNode = create_nodeAdj(4, NULL);
	*nodeToNode = *root->adj[toNode];
	nodeToNode->cost = cost;

	NodeAdj* nodeFromNode = root->adj[fromNode];

	if (nodeFromNode->len == nodeFromNode->maxSize)
	{
		return;
	}

	nodeFromNode->adj[nodeFromNode->len++] = nodeToNode;

}

AdjMatrix* create_graph(size_t max_nodes)
{
	OPTICK_EVENT("create_graph");
	AdjMatrix* newMatrix = (AdjMatrix*)allocate(sizeof(AdjMatrix));
	newMatrix->len = 0;
	newMatrix->max_size = max_nodes;
	newMatrix->nodes = (Node*)allocate(sizeof(Node) * newMatrix->max_size);
	for (int i = 0; i < newMatrix->max_size; i++)
	{
		newMatrix->nodes[i].cost = 0;
		newMatrix->nodes[i].path_from = 0;
		newMatrix->nodes[i].visited = 0;
		newMatrix->nodes[i].data = NULL;
		newMatrix->nodes[i].graph_group = 0;
		newMatrix->nodes[i].position.x = 0;
		newMatrix->nodes[i].position.y = 0;
	}

	newMatrix->adjGraph = (int**)allocate(sizeof(int*) * newMatrix->max_size);
	for (int i = 0; i < newMatrix->max_size; i++)
	{
		newMatrix->adjGraph[i] = (int*)allocate(sizeof(int) * newMatrix->max_size);
	}

	for (int a = 0; a < newMatrix->max_size; a++)
	{
		for (int b = 0; b < newMatrix->max_size; b++)
		{
			newMatrix->adjGraph[a][b] = 0;
		}
	}

	return newMatrix;
}

void add_node(AdjMatrix* graph, void* data, Vector2 pos)
{
	graph->nodes[graph->len++].data = data;
	graph->nodes[graph->len - 1].index = graph->len - 1;
	graph->nodes[graph->len - 1].position = pos;
}

void add_edge(AdjMatrix* graph, int fromNode, int toNode, uint8_t cost)
{
	graph->adjGraph[fromNode][toNode] = cost;
}

void build_groups(AdjMatrix* graph)
{
	int currGroup = 1;
	Queue* q = (Queue*)allocate(sizeof(Queue));
	queue_init(q);
	for (int i = 0; i < graph->len; i++)
	{
		Node* n = &graph->nodes[i];
		if (n->graph_group == 0)
		{
			queue_push(q, n);
			while (n != NULL)
			{
				n = (Node*)queue_pop(q);
				if (n == NULL)
				{
					currGroup++;
					break;
				}
				n->graph_group = currGroup;

				for (int a = 0; a < graph->len; a++)
				{
					if (graph->adjGraph[n->index][a] != 0 && graph->nodes[a].graph_group != currGroup)
					{
						if (graph->nodes[a].graph_group != 0)
						{
							ResetGroup(graph->nodes[a].graph_group, currGroup, graph);
						}
						else
						{
							queue_push(q, &graph->nodes[a]);
						}
					}
				}

			}
		}
	}
}

void ResetGroup(uint8_t lastGroup, uint8_t newGroup, AdjMatrix* graph)
{
	for (int i = 0; i < graph->len; i++)
	{
		if (graph->nodes[i].graph_group == lastGroup)
		{
			graph->nodes[i].graph_group == newGroup;
		}
	}
}

void astar(AdjMatrix* graph, int startNodeIndex, int endNodeIndex, Stack* solvedPath)
{
	OPTICK_EVENT("astar");
	while (solvedPath->top != -1)
	{
		stack_pop(solvedPath);
	}

	for (int i = 0; i < graph->len; i++)
	{
		graph->nodes[i].cost = 0;
		graph->nodes[i].path_from = 0;
		graph->nodes[i].visited = 0;
	}

	if (graph->nodes[startNodeIndex].graph_group != graph->nodes[endNodeIndex].graph_group)
	{
		return;
	}

	if (startNodeIndex < 0 || startNodeIndex >= graph->len || endNodeIndex < 0 || endNodeIndex >= graph->len)
	{
		return;
	}

	int good = 0;

	Queue* q = (Queue*)allocate(sizeof(Queue));
	queue_init(q);
	queue_push(q, &graph->nodes[startNodeIndex]);
	Node* n = &graph->nodes[startNodeIndex];

	while (n != NULL)
	{
		n = (Node*)queue_pop(q);
		if (n == NULL)
		{
			break;
		}
		n->visited = 1;
		if (n == &graph->nodes[endNodeIndex])
		{
			good = 1;
			continue;
		}

		//ici je passe tout les adjacant et si il a un lien et la consition et vrai je set sont pathfrom et sont cost avant de la push
		for (int i = 0; i < graph->len; i++)
		{
			if ((graph->adjGraph[n->index][i] != 0))
			{
				if ((graph->nodes[i].cost == 0 && graph->nodes[i].visited != 1) || graph->nodes[i].cost > graph->adjGraph[n->index][i] + graph->nodes[n->index].cost + CheckDistance(&graph->nodes[endNodeIndex], &graph->nodes[i]))
				{
					graph->nodes[i].cost = graph->nodes[n->index].cost + graph->adjGraph[n->index][i] + CheckDistance(&graph->nodes[endNodeIndex], &graph->nodes[i]);
					graph->nodes[i].path_from = n->index;

					queue_push(q, &graph->nodes[i]);
				}
			}
		}
	}

	if (good == 0)
	{
		return;
	}

	//ici je part de la derniere node et en passant par ses pathfrom je push le chemin dans la stack
	n = &graph->nodes[endNodeIndex];
	stack_push(solvedPath, n);
	while (n != &graph->nodes[startNodeIndex])
	{
		n = &graph->nodes[n->path_from];
		stack_push(solvedPath, n);
	}
}

void astarListAdj(NodeAdj* root, int startNodeIndex, int endNodeIndex, Stack* solvedPath)
{
	OPTICK_EVENT("astarListAdj");
	while (solvedPath->top != -1)
	{
		stack_pop(solvedPath);
	}

	for (int i = 0; i < root->len; i++)
	{
		root->adj[i]->cost = 0;
		root->adj[i]->path_from = 0;
		root->adj[i]->visited = 0;
	}

	if (root->adj[startNodeIndex]->group != root->adj[endNodeIndex]->group)
	{
		return;
	}

	if (startNodeIndex < 0 || startNodeIndex >= root->len || endNodeIndex < 0 || endNodeIndex >= root->len)
	{
		return;
	}

	int good = 0;

	Queue* q = (Queue*)allocate(sizeof(Queue));
	queue_init(q);
	queue_push(q, root->adj[startNodeIndex]);
	NodeAdj* n = root->adj[startNodeIndex];

	while (n != NULL)
	{
		n = (NodeAdj*)queue_pop(q);
		if (n == NULL)
		{
			break;
		}
		n->visited = 1;
		if (n == root->adj[endNodeIndex])
		{
			good = 1;
			continue;
		}

		for (int i = 0; i < n->len; i++)
		{
			NodeAdj yo = *n->adj[i];

			if ((root->adj[yo.index]->cost == 0 && root->adj[yo.index]->visited != 1) || root->adj[yo.index]->cost > n->cost + yo.cost + CheckDistanceListAdj(root->adj[endNodeIndex], &yo))
			{
				root->adj[yo.index]->cost = n->cost + yo.cost + CheckDistanceListAdj(root->adj[endNodeIndex], &yo);
				root->adj[yo.index]->path_from = n->index;

				queue_push(q, root->adj[yo.index]);
			}
		}
	}

	if (good == 0)
	{
		return;
	}
	n = root->adj[endNodeIndex];
	stack_push(solvedPath, n);
	while (n != root->adj[startNodeIndex])
	{
		n = root->adj[n->path_from];
		stack_push(solvedPath, n);
	}
}

int CheckDistance(Node* endNode, Node* currNode)
{
	Vector2 vectDistance;
	vectDistance.x = endNode->position.x - currNode->position.x;
	vectDistance.y = endNode->position.y - currNode->position.y;

	int distance = (vectDistance.x * vectDistance.x) + (vectDistance.y * vectDistance.y);
	distance = sqrt(distance);
	return distance;
}

int CheckDistanceListAdj(NodeAdj* endNode, NodeAdj* currNode)
{
	Vector2 vectDistance;
	vectDistance.x = endNode->position.x - currNode->position.x;
	vectDistance.y = endNode->position.y - currNode->position.y;

	int distance = (vectDistance.x * vectDistance.x) + (vectDistance.y * vectDistance.y);
	distance = sqrt(distance);
	return distance;
}

void SetNode(int width, int height, int channel, unsigned char* image, AdjMatrix* matrix, std::map<double, int>& index)
{
	OPTICK_EVENT("SetNode");
	int x = 0;
	int y = 0;
	int i = 0;
	int indexCount = 0;
	//ici je passe mon image en boucle afin de ajouter les node dans la matrice et dans la map avec la position et index
	for (unsigned char* p = image; p < image + (width * height * channel); p += channel)
	{
		x = i % width;
		y = i / width;
		if (p[0] == 255 && p[1] == 255 && p[2] == 255)
		{
			Vector2 position;
			position.x = x * channel;
			position.y = y * channel;
			add_node(matrix, p, position);
			index.emplace(GetCoordDouble(x, y), indexCount++);
		}
		i++;
	}
}

void SetEdge(int width, int height, int channel, unsigned char* image, AdjMatrix* matrix, std::map<double, int>& index)
{
	OPTICK_EVENT("SetEdge");
	int x = 0;
	int y = 0;
	int i = 0;
	//ici je passe mon image en boucle et lorsque une pixel est blanc je regarde cest voisin pour ajouter des edges
	for (unsigned char* p = image; p < image + (width * height * channel); p += channel)
	{
		x = i % width;
		y = i / width;

		if (index.count(GetCoordDouble(x, y)))
		{
			CheckVoisin(x, y, matrix, index);
		}

		i++;
	}
}

void CheckVoisin(int x, int y, AdjMatrix* matrix, std::map<double, int>& index)
{
	if (index.count(GetCoordDouble(x + 1, y)))
	{
		add_edge(matrix, index.at(GetCoordDouble(x, y)), index.at(GetCoordDouble(x + 1, y)), 1);
	}
	if (index.count(GetCoordDouble(x - 1, y)))
	{
		add_edge(matrix, index.at(GetCoordDouble(x, y)), index.at(GetCoordDouble(x - 1, y)), 1);
	}
	if (index.count(GetCoordDouble(x, y + 1)))
	{
		add_edge(matrix, index.at(GetCoordDouble(x, y)), index.at(GetCoordDouble(x, y + 1)), 1);
	}
	if (index.count(GetCoordDouble(x, y - 1)))
	{
		add_edge(matrix, index.at(GetCoordDouble(x, y)), index.at(GetCoordDouble(x, y - 1)), 1);
	}
}

void DrawChemin(Stack* s)
{
	OPTICK_EVENT("DrawChemin");
	//ici je pop les data du chemin que jai push et je color les pixel en rouge
	while (s->top != -1)
	{
		Node* n = (Node*)stack_pop(s);
		unsigned char* p = (unsigned char*)n->data;
		p[0] = 255;
		p[1] = 0;
		p[2] = 0;
	}
}

void DrawCheminListAdj(Stack* s)
{
	OPTICK_EVENT("DrawCheminListAdj");
	while (s->top != -1)
	{
		NodeAdj* n = (NodeAdj*)stack_pop(s);
		unsigned char* p = (unsigned char*)n->data;
		p[0] = 255;
		p[1] = 0;
		p[2] = 0;
	}
}

int GetNombreWithPixel(unsigned char* image, int width, int height, int channel)
{
	int i = 0;
	for (unsigned char* p = image; p < image + (width * height * channel); p += channel)
	{
		if (p[0] == 255)
		{
			i++;
		}
		
	}
	return i;
}

double GetCoordDouble(int x, int y)
{
	int divide = 10;
	while (divide <= y)
	{
		divide *= 10;
	}

	double tempX = (double)x;
	double tempY = (double)y / divide;
	divide *= 10;
	tempY += 1.0 / divide;

	return tempX + tempY;
}

void SetNodeListAdj(int width, int height, int channel, unsigned char* image, NodeAdj* root, std::map<double, int>& index)
{
	OPTICK_EVENT("SetNodeListAdj");
	int x = 0;
	int y = 0;
	int i = 0;
	int indexCount = 0;
	for (unsigned char* p = image; p < image + (width * height * channel); p += channel)
	{
		x = i % width;
		y = i / width;
		if (p[0] == 255 && p[1] == 255 && p[2] == 255)
		{
			Vector2 position;
			position.x = x * channel;
			position.y = y * channel;
			add_RootAdjacent_node(root, create_nodeAdj(4, p), position);
			index.emplace(GetCoordDouble(x, y), indexCount++);
		}
		i++;
	}
}

void SetEdgeListAdj(int width, int height, int channel, unsigned char* image, NodeAdj* root, std::map<double, int>& index)
{
	OPTICK_EVENT("SetEdgeListAdj");
	int x = 0;
	int y = 0;
	int i = 0;
	for (unsigned char* p = image; p < image + (width * height * channel); p += channel)
	{
		x = i % width;
		y = i / width;

		if (index.count(GetCoordDouble(x, y)))
		{
			CheckVoisinListAdj(x, y, root, index);
		}

		i++;
	}
}

void CheckVoisinListAdj(int x, int y, NodeAdj* root, std::map<double, int>& index)
{
	if (index.count(GetCoordDouble(x + 1, y)))
	{
		add_edgeListAdj(root, index.at(GetCoordDouble(x, y)), index.at(GetCoordDouble(x + 1, y)), 1);
	}
	if (index.count(GetCoordDouble(x - 1, y)))
	{
		add_edgeListAdj(root, index.at(GetCoordDouble(x, y)), index.at(GetCoordDouble(x - 1, y)), 1);
	}
	if (index.count(GetCoordDouble(x, y + 1)))
	{
		add_edgeListAdj(root, index.at(GetCoordDouble(x, y)), index.at(GetCoordDouble(x, y + 1)), 1);
	}
	if (index.count(GetCoordDouble(x, y - 1)))
	{
		add_edgeListAdj(root, index.at(GetCoordDouble(x, y)), index.at(GetCoordDouble(x, y - 1)), 1);
	}
}