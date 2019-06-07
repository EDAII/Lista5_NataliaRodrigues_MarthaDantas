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
    fprintf(fp, "\n%s", rotation);
    fprintf(fp, "\nnode [fontname=\"Arial\"];");

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
}

Node* right_rotation(Node* t){
    Node* u = t->left;
    t->left = u->right;
    u->right = t;

    return u;
}

Node* left_rotation(Node* t){
    Node* u = t->right;
    t->right = u->left;
    u->left = t;

    return u;
}

Node* right_left_rotation(Node* t){
    t->right = right_rotation(t->right);
    return left_rotation(t);
}

Node* left_right_rotation(Node* t){
    t->left = left_rotation(t->left);
    return right_rotation(t);
}


int height(Node *r) {
   if (r == NULL)
      return 0;
   else {
      int hl = height(r->left);
      int hr = height(r->right);
      if (hl < hr)
        return hr + 1;
      else
        return hl + 1;
   }
}

Node *new_node() {
    Node *t = (Node *) malloc(sizeof(Node));
    return t;
}

Node* insert(int x, node* t){
    if(t == NULL) {
        t = new_node();
        t->value = x;
        t->left = t->right = NULL;
    }
    else if(x < t->value){
        t->left = insert(x, t->left);
        if(height(t->left) - height(t->right) == 2){
            if(x < t->left->value) {
                t = right_rotation(t);
            }
            else {
                t = left_right_rotation(t);
            }
        }
    }
    else if(x > t->value){
        t->right = insert(x, t->right);
        if(height(t->right) - height(t->left) == 2){
            if(x > t->right->value) {
                t = left_rotation(t);
            }
            else {
                t = right_left_rotation(t);
            }
        }
    }

    return t;
}


void insert_values(int size) {
    
    for(int i = 0; i < size; i++) {
        root = insert(elements[i], root);
        create_image("insert");
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

    // system("ffmpeg -framerate 1/1 -i %d.png -vf scale=1080:-2 avl.mp4");

    return 0;
}