
#pragma safety enable

#include "ownership.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct TreeNode
{
    int data;
    struct TreeNode* _Opt left;
    struct TreeNode* _Opt right;

} TreeNode;

// Insertion
TreeNode* _Opt _Owner node_create(int value);
void node_add_right(TreeNode* _Opt node, TreeNode* _Opt root);
void node_add_left(TreeNode* _Opt node, TreeNode* _Opt root);
void bst_insert(TreeNode* _Opt node, TreeNode* _Opt root);

// DFS
void traverse_pre(TreeNode* _Opt root);
void traverse_inorder(TreeNode* _Opt root);
void traverse_post(TreeNode* _Opt root);
TreeNode* _Opt  bst_find(TreeNode* _Opt root, int value);

void node_delete(TreeNode* _Opt root, int value);

void bfs(TreeNode* _Opt root);



typedef struct DequeNode {
    TreeNode data;
    struct DequeNode* _Opt next;
} DequeNode;


DequeNode* _Opt _Owner deque_node_create(TreeNode* value);
void enqueue(DequeNode* _Owner  node, DequeNode** out_head, DequeNode** out_tail);
void dp(DequeNode* head);

DequeNode* _Opt deque(DequeNode** out_head);

int size = 0;

DequeNode* _Opt _Owner deque_node_create(TreeNode* value)
{
    DequeNode* _Opt  _Owner node = malloc(sizeof(DequeNode));
    if (node)
    {
        static_debug(value);
        node->data = *value;
        node->next = NULL;
    }

    return node;
}

void enqueue(DequeNode* _Owner node, DequeNode* _Owner* out_head, DequeNode** out_tail)
{

    if (*out_head == NULL && *out_tail == NULL)
    {
        *out_head = node;
        *out_tail = node;
        return;
    }

    (*out_tail)->next = node;
    *out_tail = node;

    size++;
    return;
}

DequeNode* _Opt deque(DequeNode* _Opt* out_head)
{

    if (size == 0)
        return NULL;

    if (*out_head == NULL)
    {
        return NULL;
    }
    *out_head = (*out_head)->next;
    size--;
    DequeNode* _Opt curr = *out_head;
    return curr;
}

void dp(DequeNode* head)
{
    DequeNode* _Opt tmp = head;
    while (tmp != NULL)
    {
        printf("%d->", tmp->data.data);
        tmp = tmp->next;
    }
    printf("\n");
    return;
}

TreeNode* _Opt _Owner node_create(int value)
{
    TreeNode* _Owner _Opt node = malloc(sizeof(TreeNode));
    if (node) //thiago
    {
        *node = (TreeNode){ value, NULL, NULL };
    }
    return node;
}

void bst_insert(TreeNode* node, TreeNode* _Opt root)
{
    if (root == NULL)
        return;

    if (node->data <= root->data)
        node_add_left(node, root);

    if (node->data >= root->data)
        node_add_right(node, root);

    return;
}

void node_add_right(TreeNode* node, TreeNode* _Opt root)
{


    if (root == NULL)
        return;

    TreeNode* _Opt cursor = root;

    while (cursor->right != NULL && cursor->left != NULL)
        cursor = cursor->right;

    if (cursor->left == NULL)
    {
        cursor->left = node;
        return;
    }
    if (cursor->right == NULL)
    {
        cursor->right = node;
        return;
    }

    return;
}
void node_add_left(TreeNode* node, TreeNode* _Opt root)
{
    if (root == NULL)
        return;

    TreeNode* cursor = root;

    while (cursor->left != NULL && cursor->right != NULL)
        cursor = cursor->left;

    if (cursor->left == NULL)
    {
        cursor->left = node;
        return;
    }

    if (cursor->right == NULL)
    {
        cursor->right = node;
        return;
    }
}

void traverse_pre(TreeNode* _Opt root)
{
    TreeNode* _Opt cursor = root;

    if (cursor == NULL)
        return;

    printf("%d -> ", cursor->data);
    traverse_pre(cursor->left);
    traverse_pre(cursor->right);
}
void traverse_inorder(TreeNode* _Opt root)
{
    if (root == NULL)
        return;

    TreeNode* cursor = root;

    if (cursor == NULL)
        return;

    traverse_inorder(cursor->left);
    printf("%d -> ", cursor->data);
    traverse_inorder(cursor->right);
}
void traverse_post(TreeNode* _Opt root)
{
    TreeNode* _Opt  cursor = root;

    if (cursor == NULL)
        return;

    traverse_post(cursor->left);
    traverse_post(cursor->right);
    printf("%d -> ", cursor->data);

}

TreeNode* _Opt bst_find(TreeNode* _Opt root, int value)
{
    if (root == NULL)
        return NULL;

    if (root->data == value)
        return root;

    if (root->data <= value)
        return bst_find(root->right, value);

    return bst_find(root->left, value);
}

void bfs(TreeNode* _Opt root)
{
    if (root == NULL)
    {
        return;
    }

    DequeNode* _Opt  _Owner node = malloc(sizeof(DequeNode));
    if (node == NULL)
        return;

    node->data = *root;
    node->next = NULL;

    DequeNode* head = node;
    DequeNode* tail = head;

    enqueue(node, &head, &tail);

    while (size > 0)
    {

        for (DequeNode* tmp = head; tmp != NULL; tmp = tmp->next)
        {
            DequeNode* _Opt curr = deque(&head);
            if (curr)
            {
                printf("%d -> ", tmp->data.data);

                if (curr->data.left)
                {
                    DequeNode* _Owner _Opt left_node = deque_node_create(curr->data.left);
                    if (left_node)
                        enqueue(left_node, &head, &tail);
                }
                if (curr->data.right)
                {
                    DequeNode* _Owner _Opt right_node = deque_node_create(curr->data.right);
                    if (right_node)
                        enqueue(right_node, &head, &tail);
                }
            }
        }
    }
    printf("\n");
}
