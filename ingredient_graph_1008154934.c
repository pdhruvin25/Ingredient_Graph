/*
 * CSC A48 - Assignment 3 - Ingredient networks
 *
 * In this assignment, you will practice and strengthen your
 * understanding of graph and recursion. The application
 * you're working on is a simple network for studying
 * how food ingredients interact.
 *
 * This kind of network is the basic model that very
 * powerful systems such as IMB's Watson use as the
 * basis for creating new combinations of ingredients
 * that are likely to go well together, but haven't been
 * used in previously printed recipes.
 *
 * Your work here has several components:
 *
 * - Understanding how the graph is represented by an
 *   adjacency matrix.
 * - Using the adjacency matrix to figure out, given
 *   an input ingredient, what other ingredients it is
 *   related to directly
 * - Using the adjacency matrix to determine, given
 *   a specific ingredient, which others are related
 *   to it directly *and* indirectly by a distance
 *   of k hops along the graph (k>=1).
 * - Get lits of ingredients that are related, but
 *   not including ingredients from a separate
 *   list of ingredients
 * - *crunchy* Using the adjacency matrix that, given
 *   a recipe (which a list of ingredients) and
 *   a target ingredient (part of the recipe) to
 *   replace, finds the best replacement from the
 *   remaining ingredients in the graph and updates
 *   the recipe.
 *
 * This file contains the functions that set up
 * the adjacency matrix and the list of ingredients
 * for you. We provide you with 2 graphs, a
 * small one with 10 ingredients (and an adjacency
 * matrix of size 10x10), and a full one with
 * 400 ingredients (adjacency matrix is 400x400).
 *
 * Use the 10x10 while implementing and testing your
 * solution, and only move to the full-size one once
 * you're confident your code works well.
 *
 * Select which matrix to use by uncommenting the
 * respective #define statement below.
 *
 * Sections where you have to implement code are
 * clearly marked
 * ********
 * TO DO:
 * ********
 *
 * (c) F. Estrada, March 2022
 */

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

// Bare-bones implementation of a linked-list of
// integers, you can use it to return the ingredient
// lists requested from functions you have to
// implement. DO NOT CHANGE THE IMPLEMENTATION
// OF THE LINKED LISTS.

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

/***************************************************************
 * The part of the assignment you need to implement starts below
 ***************************************************************/

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
    /*****
     * TO Do:
     * Complete this function
     *****/
}

int ingredient_index(char source_ingredient[MAX_STR_LEN])
{
    /*
     * This function looks into the array with ingredient
     * names for one that matches the requested
     * 'source_ingredient'. If a match is found, it
     * returns the index at which the matching ingredient
     * name was found.
     *
     * If the 'source_ingredient' is *not* in the ingredients
     * array, the function returns -1
     */
    for (int j = 0; j < MAT_SIZE; j++)
    {
        if (strcmp(ingredients[j], source_ingredient) == 0)
            return j;
    }
    return -1;
    /******
     * TO Do:
     * Implement this function
     *****/
    // return 0;
}

void related_ingredients(char source_ingredient[MAX_STR_LEN])
{
    /*
     * This function prints out all the ingredients related
     * to the source ingredient.
     *
     * One per line, with no additional characters or symbols.
     *
     * Format of the adjacency matrix:
     *
     * AdjMat[i][j] = 0 if two ingredients i and j are not directly linked
     *                (no edge in the graph between them, this means
     *                 they never appeared together in one recipe)
     * AdjMat[i][j] > 0 if two ingredients i and j are neighbours (they
     * 			appeared together in at least a recipe). The
     * 			actual value indicates how many times these two
     * 			ingredients appeared together in a recipe
     *
     * Make sure you understand what this is telling you about the
     * ingredients in your graph. You'll need it later
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

    /**********
     * TO DO:
     * Implement this function
     * ********/
}
intNode *related_k_dist(intNode *h, char source_ingredient[MAX_STR_LEN], int k, int dist)
{
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
    /*
     * This function determines the ingredients related to the
     * specified source ingredient by a distance of at most k.
     * k >= 1
     *
     * (for k=1, this produces the same list as related_ingredients)
     *
     * For instance, if k=2, the list should contain all the
     * ingredients who are neighbours of source_ingredient[]
     * as well as the ingredients that are neighbours of
     * the neighbours (distance=2).
     *
     * If k=3, then we want source_ingredients' neighbours, its
     * neighbours' neighbours, and its neighbours' neighbours'
     * neighbours. And so on.
     *
     * *****  This function MUST use recursion  ******
     *
     * Ingredients are returned as *indexes*, so, for instance,
     * if we find a related ingredient 'chicken' is stored at
     * index 7 in ingredients[][], then we store 7 in the
     * linked list of related ingredients.
     *
     * The returned list MUST CONTAIN NO DUPLICATES.
     *
     * And be smart about it, or you'll end up in an infinite
     * recursion! So think carefully about the base case(s)
     * and about what the recursive case must do.
     *
     * Example call:
     *
     * Our test code may call your function in this way:
     *
     * intNode *head=NULL;
     * head=related_k_dist(head,"rice",2,0);
     *
     * After that call, 'head' must point to a linked list with
     * all the ingredients related to 'rice' up to a distance of
     * 2 away.
     *
     * It's up to *you* to figure out what the 'dist' parameter
     * is for!
     *
     */
    /*******
     * TO DO:
     * Complete this function
     *******/

    // return NULL;
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
    /*
     * This function returns a linked list that contains the indexes of
     * all ingredients related to source_ingredient[] with a distance
     * of at most k_source.
     *
     * BUT, the list *must not contain* any ingredients related to avoid[]
     * (including avoid[] itself) by a distance of up to k_avoid.
     *
     * Example:
     *
     * intNode *head=NULL;
     * head=related_with_restrictions("rice", "nuts", 2, 0);
     * (yes, we know the function doesn't take the head of a linked list as a parameter,
     *  that's not a mistake)
     *
     * Should return a pointer to the head of a list of ingredients related to
     * 'rice' by a distance of up to 2, NOT INCLUDING 'nuts'.
     *
     * intNode *head=NULL;
     * head=related_with_restrictions("rice", "chicken", 2, 1);
     *
     * Should return a list of ingredients related to 'rice' by a distance
     * of up to 2, NOT INCLUDING 'nuts' and any ingredients related
     * to 'nuts' with a distance of up to 1.
     *
     */

    /****
     * TO DO:
     * Implement this function
     *****/
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
