#include "SwiftyList/SwiftyList.hpp"
#include "Strings.h"


#ifndef AKINATOR_BINARYTREE_H
#define AKINATOR_BINARYTREE_H
class BinaryTree {
    BinaryTree* left;
    BinaryTree* right;
    StringView* node_name;
public:
    static BinaryTree* CreateNovel(StringView* node_name){
        auto* thou = static_cast<BinaryTree*>(calloc(1, sizeof(BinaryTree)));
        thou->left = thou->right = nullptr;
        thou->node_name = node_name;
        return thou;
    }

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

    StringView* getNodeName(){
        return node_name;
    }

    void setNodeName(StringView* name){
        this->node_name = name;
    }

    void DestructNode(){
        this->getNodeName()->DestructString();
        free(this->getNodeName());
        this->setNodeName(reinterpret_cast<StringView *>(0xBADF));
        if(getLeft() != nullptr) {
            getLeft()->DestructNode();
            free(getLeft());
        }
        if(getRight() != nullptr) {
            getRight()->DestructNode();
            free(getRight());
        }
    }
};

#endif //AKINATOR_BINARYTREE_H
