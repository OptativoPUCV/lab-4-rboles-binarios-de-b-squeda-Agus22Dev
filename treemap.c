#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lt) (void* key1, void* key2)) {
    // Reservar memoria para el TreeMap
    TreeMap * map = (TreeMap *)malloc(sizeof(TreeMap));
    if (map == NULL) return NULL; // Si malloc falla, retornar NULL

    // Inicializar campos
    map->root = NULL;
    map->current = NULL;
    map->lower_than = lt;

    return map;
}


void insertTreeMap(TreeMap* tree, void* key, void* value) {
    // Verificar si la clave ya existe
    if (searchTreeMap(tree, key) != NULL) return;

    TreeNode* newNode = (TreeNode*) malloc(sizeof(TreeNode));
    if (newNode == NULL) return;

    newNode->pair = (Pair*) malloc(sizeof(Pair));
    if (newNode->pair == NULL) return;

    newNode->pair->key = key;
    newNode->pair->value = value;
    newNode->left = newNode->right = NULL;
    newNode->parent = NULL;

    TreeNode* parent = NULL;
    TreeNode* current = tree->root;

    // Buscar dónde insertar el nodo
    while (current != NULL) {
        parent = current;
        if (tree->lower_than(key, current->pair->key)) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    newNode->parent = parent;

    if (parent == NULL) {
        // El árbol estaba vacío
        tree->root = newNode;
    } else if (tree->lower_than(key, parent->pair->key)) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }

    // Apuntar el current al nuevo nodo
    tree->current = newNode;
}


TreeNode * minimum(TreeNode * x){
    if (x == NULL) return NULL;

    while (x->left != NULL) {
        x = x->left;
    }

    return x;
}


void removeNode(TreeMap * tree, TreeNode* node) {
    // Caso 1: Nodo sin hijos
    if (node->left == NULL && node->right == NULL) {
        if (node->parent == NULL) {
            tree->root = NULL;  // era la raíz
        } else if (node->parent->left == node) {
            node->parent->left = NULL;
        } else {
            node->parent->right = NULL;
        }
        free(node->pair);
        free(node);
        return;
    }

    // Caso 2: Nodo con un solo hijo
    if (node->left == NULL || node->right == NULL) {
        TreeNode* child = (node->left != NULL) ? node->left : node->right;

        if (node->parent == NULL) {
            tree->root = child;
        } else if (node->parent->left == node) {
            node->parent->left = child;
        } else {
            node->parent->right = child;
        }

        child->parent = node->parent;

        free(node->pair);
        free(node);
        return;
    }

    // Caso 3: Nodo con dos hijos
    TreeNode* successor = minimum(node->right);

    // Copiar key y value del sucesor al nodo actual
    node->pair->key = successor->pair->key;
    node->pair->value = successor->pair->value;

    // Eliminar el sucesor (que tendrá 0 o 1 hijo)
    removeNode(tree, successor);
}


void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}




Pair* searchTreeMap(TreeMap* tree, void* key) {
    TreeNode* node = tree->root;

    while (node != NULL) {
        // Si key es menor que node->pair->key, voy a la izquierda
        if (tree->lower_than(key, node->pair->key)) {
            node = node->left;
        }
        // Si key es mayor que node->pair->key, voy a la derecha
        else if (tree->lower_than(node->pair->key, key)) {
            node = node->right;
        }
        // Si no es menor ni mayor, entonces son iguales
        else {
            tree->current = node; // apunto al nodo encontrado
            return node->pair;
        }
    }

    // No se encontró
    return NULL;
}


Pair* upperBound(TreeMap* tree, void* key){
    TreeNode* current = tree->root;
    TreeNode* ub_node = NULL;

    while (current != NULL) {
        if (!tree->lower_than(current->pair->key, key)) {
            // current->key >= key → posible upper bound
            ub_node = current;
            current = current->left; // buscar una mejor (más pequeña)
        }
        else {
            // current->key < key → seguir buscando a la derecha
            current = current->right;
        }
    }

    if (ub_node != NULL) {
        tree->current = ub_node;
        return ub_node->pair;
    }

    return NULL;
}


Pair * firstTreeMap(TreeMap * tree) {
    if (tree == NULL || tree->root == NULL) return NULL;

    TreeNode* minNode = minimum(tree->root);
    tree->current = minNode;
    return minNode->pair;
}


Pair * nextTreeMap(TreeMap * tree) {
    return NULL;
}
