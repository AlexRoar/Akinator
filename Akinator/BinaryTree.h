#include "SwiftyList/SwiftyList.hpp"
#include "Strings.h"


#ifndef AKINATOR_BINARYTREE_H
#define AKINATOR_BINARYTREE_H
class BinaryTree {
    BinaryTree* left;
    BinaryTree* right;
    StringView  node_name;
public:
    explicit BinaryTree(StringView node_name): left(nullptr), right(nullptr), node_name(node_name){}

    void setLeft(BinaryTree* left) {
        this->left = left;
    }

    void setRight(BinaryTree* right) {
        this->right = right;
    }

    BinaryTree* getRight() {
        return right;
    }

    BinaryTree* getLeft() {
        return left;
    }

    StringView getNodeName(){
        return node_name;
    }

    void setNodeName(StringView name){
        this->node_name = name;
    }

    void DestructNode(){
        this->getNodeName().DestructString();
        if(getLeft() != nullptr)
            getLeft()->DestructNode();
        if(getRight() != nullptr)
            getRight()->DestructNode();
        delete this;
    }
};

#endif //AKINATOR_BINARYTREE_H
