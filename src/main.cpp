#include <iostream>
#include <vector>
#include <algorithm>
#include <string.h>

using namespace std;

typedef struct node {
    int value;
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
            nulls--;
            fprintf(fp, "\n%d -- %c", node->value, nulls);
        }
        else
            fprintf(fp, "\n%d -- %d", node->value, node->left->value);

        if(!node->right) {
            nulls--;
            fprintf(fp, "\n%d -- %c", node->value, nulls);
        }
        else
            fprintf(fp, "\n%d -- %d", node->value, node->right->value);

        write_in_file(fp, node->left);
        write_in_file(fp, node->right);
    }
}

void create_image() {
    FILE *fp = fopen("file.dot", "w+");
    fprintf(fp, "graph {");

    write_in_file(fp, root);

    fprintf(fp, "\n}");
    fclose(fp);

    char sid[8];
    char command[40];

    sprintf(sid, "%d", id);

    strcpy(command, "dot -Tpng file.dot -o ");
    strcat(command, sid);
    strcat(command, ".png");

    system(command);

    system("mogrify -resize 640x480 *.png");
    system("convert -delay 20 -loop 0 *.png myimage.gif");
}

Node * right_rotation(Node *r){
    Node* u = r->left;
    r->left = u->right;
    u->right = r;

    return u;
}

Node * left_rotation(Node *r){
    Node* u = r->right;
    r->right = u->left;
    u->left = r;

    return u;
}


Node * left_right_rotation(Node *r){
    r = left_rotation(r->left);
    r = right_rotation(r);

    return r;
}

Node * right_left_rotation(Node *r){
    r = right_rotation(r->right);
    r = left_rotation(r);

    return r;
}


int height(Node *r) {
   if (r == NULL)
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

Node * balance_node(Node *node, int factor){
    if(node){
        if(factor < -1){
            if(balancing_factor(node->left) < 0)
                node = right_rotation(node);
            else
                node = left_right_rotation(node);
        }
        else if(factor > 1){
            if(balancing_factor(node->right) > 0)
                node = left_rotation(node);
            else
                node = right_left_rotation(node);
        }
    }

    return node;
}

Node * balance(Node *r) {
    if(!r)
        return r;
    
    int factor = balancing_factor(r);
    if(factor > 1 || factor < -1)
        r = balance_node(r, factor);

    r->left = balance(r->left);
    r->right = balance(r->right);

    return r;
}

Node * new_node(int value){
    Node *node = (Node*) malloc(sizeof(Node));

    node->value = value;
    node->left = NULL;
    node->right = NULL;

    return node;
}

Node * insert(Node *r, Node *node){
    if(!r)
        return node;
    else {
        if(node->value > r->value)
            r->right = insert(r->right, node);
        else
            r->left = insert(r->left, node);
    }

    return r;
}


void insert_values(int size) {
    
    for(int i = 0; i < size; i++) {
        root = insert(root, new_node(elements[i]));
        root = balance(root);
    }
}

int main() {
    int size;

    cout << "\nDigite a quantidade de elementos que você quer na árvore: ";
    cin >> size;

    id = 0;
    nulls = 0;
    generate_vector(size);
    insert_values(size);
    
    create_image();

    return 0;
}