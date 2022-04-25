#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN 1024
#ifndef __testing
// #define MAT_SIZE 10 // Use the small graph
#define MAT_SIZE 400 // Use full-size graph
#endif

// The following are GLOBAL variables you can use
// anywhere in your code. They include the
// adjacency matrix, as well as an array of strings
// to store the ingredient names.
double AdjMat[MAT_SIZE][MAT_SIZE];
char ingredients[MAT_SIZE][MAX_STR_LEN];

typedef struct intNode_struct
{
    int x;
    struct intNode_struct *next;
} intNode;

intNode *insertInt(intNode *h, int x)
{
    // Inserts the int 'x' into the linked list
    // whose head is at the address in 'h'

    intNode *newNode;

    newNode = (intNode *)calloc(1, sizeof(intNode));
    newNode->x = x;
    newNode->next = NULL;

    if (h != NULL)
        newNode->next = h;

    return newNode;
}

int searchInt(intNode *h, int x)
{
    // Search the linked list for the value 'x', if
    // found, it returns 1, else it returns 0
    while (h != NULL)
    {
        if (h->x == x)
            return 1;
        h = h->next;
    }
    return 0;
}

intNode *deleteList(intNode *h)
{
    // Delete list and free memory
    intNode *q;
    if (h == NULL)
        return NULL;

    while (h != NULL)
    {
        q = h->next;
        free(h);
        h = q;
    }

    return NULL;
}

// Read the adjacency matrix from file, and read the ingredient
// names
void load_ingredients(void)
{
    // Loads the adjacency matrix and ingredients list from file,
    // N must be either 10 (small graph), or 400 (full-size).

    FILE *f;
    int n = MAT_SIZE;

    if (n == 10)
        f = fopen("AdjMat_small.dat", "rb");
    else
        f = fopen("AdjMat_full.dat", "rb");
    if (f == NULL)
    {
        printf("Can not open file with adjacency matrix. Please make sure it's in this directory\n");
        return;
    }
    fread(&AdjMat[0][0], n * n * sizeof(double), 1, f);
    fclose(f);

    if (n == 10)
        f = fopen("Ingredient_names_small.txt", "r");
    else
        f = fopen("Ingredient_names_full.txt", "r");
    if (f == NULL)
    {
        printf("Can not open file with ingredient names. Please make sure it's in this directory\n");
        return;
    }
    printf("Reading ingredient names. Available ingredient are:\n");
    for (int i = 0; i < n; i++)
    {
        fgets(&ingredients[i][0], MAX_STR_LEN, f);
        for (int j = 0; j < strlen(ingredients[i]); j++)
            if (ingredients[i][j] == 10 || ingredients[i][j] == 13)
                ingredients[i][j] = 0;
        printf("%s\n", ingredients[i]);
    }
    fclose(f);
}

void print_ingredients(intNode *h)
{
    /*
     * This function prints out all the ingredients corresponding
     * to the ingredient indexes stored in the linked list
     * whose head is 'h'.
     */
    intNode *new_node = NULL;
    new_node = h;
    while (new_node != NULL)
    {
        printf("%s\n", ingredients[new_node->x]);
        new_node = new_node->next;
    }
}

int ingredient_index(char source_ingredient[MAX_STR_LEN])
{
    /*
     * This function looks into the array with ingredient
     * names for one that matches the requested
     * 'source_ingredient'. If a match is found, it
     * returns the index at which the matching ingredient
     * name was found
     */
    for (int j = 0; j < MAT_SIZE; j++)
    {
        if (strcmp(ingredients[j], source_ingredient) == 0)
            return j;
    }
    return -1;
}

void related_ingredients(char source_ingredient[MAX_STR_LEN])
{
    /*
     * This function prints out all the ingredients related
     * to the source ingredient
     */
    int position = ingredient_index(source_ingredient);
    if (position >= 0)
    {
        for (int i = 0; i < MAT_SIZE; i++)
        {
            if (AdjMat[position][i] > 0 && position != i)
                printf("%s\n", ingredients[i]);
        }
    }
}
intNode *related_k_dist(intNode *h, char source_ingredient[MAX_STR_LEN], int k, int dist)
{
    /*
     * This function determines the ingredients related to the
     * specified source ingredient by a distance of at most k.
     * k >= 1 
     */
    if (dist >= k && dist < MAT_SIZE)
        return h;
    int position = ingredient_index(source_ingredient);
    for (int i = 0; i < MAT_SIZE; i++)
    {
        if (AdjMat[position][i] > 0 && i != position)
        {
            if (searchInt(h, i) == 0)
                h = insertInt(h, i);
            h = related_k_dist(h, ingredients[i], k, dist + 1);
        }
    }
    return h;
}

intNode *remove_specific(intNode *source_neighbours, int want_to_delete)
{
    if (source_neighbours == NULL)
        return source_neighbours;
    intNode *current = source_neighbours, *prev = NULL, *temp = NULL;
    while (current != NULL)
    {
        // Head Case
        if (current->x == want_to_delete && current == source_neighbours)
        {
            temp = current->next;
            free(current);
            return temp;
        }
        // middle case
        else if (current->x == want_to_delete && current->next != NULL)
        {
            prev->next = current->next;
            free(current);
            break;
        }
        // tail case
        else if (current->x == want_to_delete && current->next == NULL)
        {
            prev->next = NULL;
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }
    return source_neighbours;
}
intNode *related_with_restrictions(char source_ingredient[MAX_STR_LEN], char avoid[MAX_STR_LEN], int k_source, int k_avoid)
{
    /*
     * This function returns a linked list that contains the indexes of
     * all ingredients related to source_ingredient[] with a distance
     * of at most k_source
     */
    intNode *avoided_neighbours = NULL, *q = NULL, *source_neighbours = NULL;
    avoided_neighbours = related_k_dist(avoided_neighbours, avoid, k_avoid, 0);
    if (searchInt(avoided_neighbours, ingredient_index(avoid)) == 0)
        avoided_neighbours = insertInt(avoided_neighbours, ingredient_index(avoid));
    source_neighbours = related_k_dist(source_neighbours, source_ingredient, k_source, 0);
    q = avoided_neighbours;
    while (q != NULL)
    {
        if (searchInt(source_neighbours, q->x) == 1)
            source_neighbours = remove_specific(source_neighbours, q->x);
        q = q->next;
    }
    avoided_neighbours = deleteList(avoided_neighbours);
    return source_neighbours;
    
}
int search_in_recipe(int ingred, char recipe[10][MAX_STR_LEN])
{

    for (int i = 0; i < 10; i++)
    {
        if (ingred == ingredient_index(recipe[i]))
            return 1;
    }
    return 0;
}
void substitute_ingredient(char recipe[10][MAX_STR_LEN], char to_change[MAX_STR_LEN])
{
    int max = 0, count = 0, want_to_change_pos, ingred_pos;
    for (int i = 0; i < MAT_SIZE; i++)
    {
        if (search_in_recipe(i, recipe) == 0)
        {
            count = 0;
            for (int j = 0; j < 10; j++)
            {
                if (strcmp(recipe[j], "") != 0)
                {
                    if (ingredient_index(recipe[j]) != ingredient_index(to_change))
                        count += AdjMat[ingredient_index(recipe[j])][i];
                    if (ingredient_index(recipe[j]) == ingredient_index(to_change))
                        want_to_change_pos = j;
                }
            }
            if (count > max)
            {
                max = count;
                ingred_pos = i;
            }
        }
    }
    if (search_in_recipe(ingredient_index(to_change), recipe) == 1)
        strcpy(recipe[want_to_change_pos], ingredients[ingred_pos]);
}
