#include "TP.h"
#include<string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define OPTICK_ENABLE_GPU (0)
#include "optick.h"

#define HEAP_SIZE UINT32_MAX * UINT32_MAX * 9999999999
static uint8_t* heap = NULL;
static size_t heap_top = 0;

void* allocate(size_t size)
{
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

//Hypotese
// La structure de donnée avec la list des adjacant est beaucoup plus performant que
// celle avec la matrice de par l'utilisation en memoir, celle avec la matrice 
// nécecite une grandre quantitée de memoir afin detre initialisé. Ensuite elle est aussi
// plus performant en terme de lecture car la structure avec la matrice possede
// un lien potentielle avec tout les autre node donc pour lire tout cest voisin
// ceci nessesite de passer en boucle tout les node ce qui a un gros impact au niveau de la 
// performance. La structure avec la list elle chaque node possede des liens seulement pour
// ses voisin donc lors de l'iteration cela nessecite beaucoup moin de performance a lire car
// cela nessecite pas d'iterer sur tout les node.
//

int main()
{
	const int nbMatrice = 3;
	const int nbList = 8;

	OPTICK_APP("Profiling");
	heap = (uint8_t*)malloc(HEAP_SIZE);
	assert(heap != NULL);

	std::string test[] = { {"Image/31.bmp"}, {"Image/64.bmp"}, {"Image/128.bmp"}, {"Image/512.bmp"}, {"Image/45565.bmp"}, {"Image/braid2k.png"}, {"Image/combo400.png"}, {"Image/perfect2k.png"} };
	std::string test1[] = { {"Image/31Matrice.bmp"}, {"Image/64Matrice.bmp"}, {"Image/128Matrice.bmp"}, {"Image/31List.bmp"}, {"Image/64List.bmp"}, {"Image/128List.bmp"}, {"Image/512List.bmp"}, {"Image/45565List.bmp"}, {"Image/braid2kList.png"}, {"Image/combo400List.png"}, {"Image/perfect2kList.png"} };

	std::map<double, int> index;
	
	int count = 0;
	for (int i = 0; i < nbMatrice + nbList; i++)
	{
		Stack s = stack_init(1000000);
		int width;
		int height;
		int channels;
		unsigned char* image = stbi_load(test[count++].c_str(), &width, &height, &channels, 0);

		if (i == nbMatrice - 1)
		{
			count = 0;
		}

		if (image == NULL)
		{
			printf("Error in loading the image\n");
			return 1;
		}

		if (i < nbMatrice)
		{
			AdjMatrix* matrix = create_graph(GetNombreWithPixel(image, width, height, channels));

			SetNode(width, height, channels, image, matrix, index);

			SetEdge(width, height, channels, image, matrix, index);

			astar(matrix, 0, matrix->len - 1, &s);

			DrawChemin(&s);
		}
		else
		{
			NodeAdj* root = create_nodeRootAdj(GetNombreWithPixel(image, width, height, channels));

			SetNodeListAdj(width, height, channels, image, root, index);

			SetEdgeListAdj(width, height, channels, image, root, index);

			astarListAdj(root, 0, root->len - 1, &s);

			DrawCheminListAdj(&s);
		}

		stbi_write_bmp(test1[i].c_str(), width, height, channels, image);

		stbi_image_free(image);

		memset(heap, 0, sizeof(HEAP_SIZE));
		heap_top = 0;
		index.clear();
	}

	return 0;
}