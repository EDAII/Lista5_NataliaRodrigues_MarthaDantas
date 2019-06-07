#include <iostream>
#include <vector>
#include <algorithm>
#include <string.h>

using namespace std;

typedef struct node {
    int value;
    struct node *dad;
    struct node *left;
    struct node *right;
} Node;

Node *root = NULL;
vector<int> elements;

int id;
int nulls;

void generate_vector(int size) {	
	for(int i = 1; i <= size; i++)
		elements.push_back(i);
		
	random_shuffle(++elements.begin(), elements.end());
}

void write_in_file(FILE *fp, Node *node) {

    if(node) {
        if(!node->left) {
            nulls++;
            fprintf(fp, "\nnull%d [shape=point];", nulls);
            fprintf(fp, "\n%d -> null%d;", node->value, nulls);
        }
        else
            fprintf(fp, "\n%d -> %d;", node->value, node->left->value);

        if(!node->right) {
            nulls++;
            fprintf(fp, "\nnull%d [shape=point];", nulls);
            fprintf(fp, "\n%d -> null%d;", node->value, nulls);
        }
        else
            fprintf(fp, "\n%d -> %d;", node->value, node->right->value);

        write_in_file(fp, node->left);
        write_in_file(fp, node->right);
    }
}

void create_image(const char *rotation) {
    id++;
    FILE *fp = fopen("file.dot", "w+");
    fprintf(fp, "digraph BST {");
    fprintf(fp, "\n%s [shape=box];", rotation);
    fprintf(fp, "\nnode [fontname=\"Arial\"];");

    write_in_file(fp, root);

    fprintf(fp, "\n}");
    fclose(fp);

    char sid[8];
    char command[46];

    sprintf(sid, "%d", id);

    strcpy(command, "dot -Tpng file.dot -o ");
    strcat(command, "image-");
    strcat(command, sid);
    strcat(command, ".png");

    system(command);
}

Node * right_rotation(Node *r){
    Node* u = r->left;

    if(r->dad) {
        if(r->value > r->dad->value)
            r->dad->right = u;
        else
            r->dad->left = u;
    }

    u->dad = r->dad;
    r->dad = u;

    if(u->right)
        u->right->dad = r;
    
    r->left = u->right;
    u->right = r;

    return u;
}

Node * left_rotation(Node *r){
    Node* u = r->right;

    if(r->dad) {
        if(r->value > r->dad->value)
            r->dad->right = u;
        else
            r->dad->left = u;
    }

    u->dad = r->dad;
    r->dad = u;

    if(u->left)
        u->left->dad = r;
    
    r->right = u->left;
    u->left = r;

    return u;
}

Node * left_right_rotation(Node *r){
    r->left = left_rotation(r->left);
    return right_rotation(r);
}

Node * right_left_rotation(Node *r){
    r->right = right_rotation(r->right);
    return left_rotation(r);
}

int height(Node *r) {
   if (!r)
      return -1;
   else {
      int hl = height(r->left);
      int hr = height(r->right);
      if (hl < hr)
        return hr + 1;
      else
        return hl + 1;
   }
}

int balancing_factor(Node *r){
    int factor;
    if(r){
        factor = ((height(r->right) + 1) - (height(r->left) + 1));
        return factor;
    }

    return -1;
}

Node * balance_node(Node *node, int factor, char *rotation){
    if(factor < -1){
        if(balancing_factor(node->left) < 0) {
            node = right_rotation(node);
            strcpy(rotation, "right_rotation");
        }
        else {
            node = left_right_rotation(node);
            strcpy(rotation, "left_right_rotation");
        }
    }
    else if(factor > 1){
        if(balancing_factor(node->right) > 0) {
            node = left_rotation(node);
            strcpy(rotation, "left_rotation");
        }
        else {
            node = right_left_rotation(node);
            strcpy(rotation, "right_left_rotation");
        }
    }

    return node;
}

Node * balance(Node *r, char *rotation) {
    Node *aux = r, *ret;
    int factor;

    while(aux) {
        factor = balancing_factor(aux);
        if(factor < 1 || factor > 1) {
            aux = balance_node(aux, factor, rotation);
        }
        
        if(!aux->dad)
            ret = aux;
        
        aux = aux->dad;
    }

    return ret;
}

Node * new_node(int value){
    Node *node = (Node*) malloc(sizeof(Node));

    node->value = value;
    node->dad = node->left = node->right = NULL;

    return node;
}

Node * insert(Node *r, Node *node){
    if(!r) {
        return node;
    }
    else if((!r->left) || (!r->right)) {
        node->dad = r;
    }
    
    if(node->value > r->value)
        r->right = insert(r->right, node);
    else
        r->left = insert(r->left, node);

    return r;
}

void insert_values(int size) {
    Node *node;
    char rotation[19];
    char insert_text[15];
    char insert_value[8];

    memset(rotation, 0, strlen(rotation));
    memset(insert_text, 0, strlen(insert_text));

    for(int i = 0; i < size; i++) {
        node = new_node(elements[i]);
        root = insert(root, node);

        sprintf(insert_value, "%d", elements[i]);
        strcpy(insert_text, "insert_");
        strcat(insert_text, insert_value);

        create_image(insert_text);

        memset(insert_text, 0, strlen(insert_text));

        root = balance(node, rotation);

        if(strlen(rotation) > 0) {
            create_image(rotation);
            memset(rotation, 0, strlen(rotation));
        }
    }
}

int main() {
    int size;
    char option;

    cout << "\nEnter the amount of elements you want in the tree: ";
    cin >> size;

    id = 0;
    nulls = 0;

    cout << "\nAutomatically generate numbers? (Y/N): ";
    cin >> option;

    if(option == 'Y')
        generate_vector(size);
    else {
        int aux;
        cout << "\nEnter the numbers: ";
        for(int i = 0; i < size; i++) {
            cin >> aux;
            elements.push_back(aux);
        }
    }

    insert_values(size);
    create_image("final");

    system("ffmpeg -framerate 1/2 -i image-%d.png -vf scale=1080:-2 avl.mp4");

    cout << endl << endl << "END";

    cout << endl << endl << "A set of images showing the steps of the insertion in AVL was generated in the folder, also watch the video 'avl.mp4'.";
    cout << endl << endl;

    return 0;
}