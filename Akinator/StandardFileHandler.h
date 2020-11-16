//
// Created by Александр Дремов on 14.11.2020.
//
#include  <cassert>
#include  <cctype>
#include "BinaryTree.h"
#include "Strings.h"

#ifndef AKINATOR_STANDARDFILEHANDLER_H
#define AKINATOR_STANDARDFILEHANDLER_H

enum SFHandlerResult{
    SF_SUCCESS,
    SF_EMPTY,
    SF_FILE_WRONG_FORMAT,
    SF_FILE_WRONG_STRING,
};

class StandardFileHandler {
    BinaryTree *treeHead;

    static StringView* retrievePlainText(char *start, size_t *skipLen, SFHandlerResult& result) {
        assert(start != nullptr && skipLen != nullptr);

        char* foundStart = strchr(start, '"');
        if (foundStart == nullptr) {
            result = SF_FILE_WRONG_STRING;
            return nullptr;
        }

        char* foundEnd = strchr(foundStart + 1, '"');
        if (foundEnd == nullptr){
            result = SF_FILE_WRONG_STRING;
            return nullptr;
        }

        *skipLen = foundEnd - start + 1;
        *foundEnd = '\0';
        return StringView::CreateNovel(foundStart + 1);
    }

    static BinaryTree* retrieveNode(char*& data, SFHandlerResult& result) {
        size_t skipDist = 0;
        StringView* plainText = retrievePlainText(data, &skipDist, result);
        if (result != SF_SUCCESS)
            return nullptr;
        auto* subtree = BinaryTree::CreateNovel(plainText);

        data += skipDist;
        char* nextTextStart = strchr(data, '"');
        char* branchesStart = strchr(data, '[');
        char* branchesEnd   = strchr(data, ']');

        if (branchesStart == nullptr && (branchesEnd  != nullptr && branchesEnd < nextTextStart) ||
            branchesEnd   == nullptr && branchesStart != nullptr){
            result = SF_FILE_WRONG_FORMAT;
            return nullptr;
        }

        if ((nextTextStart < branchesStart && nextTextStart != nullptr) || branchesStart == nullptr)
            return subtree;

        if (nextTextStart == nullptr) {
            result = SF_FILE_WRONG_FORMAT;
            return nullptr;
        }

        if (branchesStart < branchesEnd) {
            subtree->setLeft(retrieveNode(data, result));
            subtree->setRight(retrieveNode(data, result));
        }
        return subtree;
    }

    void dumpAllNodes(FILE* file, SFHandlerResult& result) const {
        dumpNode(file, this->treeHead, result);
    }

    void dumpNode(FILE* file, BinaryTree* node, SFHandlerResult& result, int depthLevel=0) const {
        if (node == nullptr) {
            result = SF_FILE_WRONG_FORMAT;
            return;
        }
        fprintf(file, "%*s", depthLevel * 4, "");
        StringView* name = node->getNodeName();
        fprintf(file, "\"%s\"", (name->getBuffer() == nullptr)? "<unknown>": name->getBuffer());
        if (node->getLeft() != nullptr || node->getRight() != nullptr){
            fprintf(file, " [\n");
            if (node->getLeft() == nullptr || node->getRight() == nullptr) {
                result = SF_FILE_WRONG_FORMAT;
                return;
            }
            dumpNode(file, node->getLeft(), result, depthLevel + 1);
            fprintf(file, "\n");
            dumpNode(file, node->getRight(), result,depthLevel + 1);
            fprintf(file, "\n%*s]", depthLevel * 4, "");
        }
    }

public:
    StandardFileHandler() : treeHead(nullptr) {}

    SFHandlerResult loadFromFile(FILE* input, char*& commonBuffer) {
        assert(input != nullptr);

        fseek(input, 0L, SEEK_END);
        size_t  numbytes = ftell(input);
        fseek(input, 0L, SEEK_SET);

        char* buffer = static_cast<char*>(calloc(numbytes + 1, sizeof(char)));
        commonBuffer = buffer;
        fread(buffer, sizeof(char), numbytes, input);

        SFHandlerResult result = SF_SUCCESS;
        this->treeHead = retrieveNode(buffer, result);
        return result;
    }

    SFHandlerResult fileDump(FILE* output) const {
        assert(output != nullptr);
        SFHandlerResult result = SF_SUCCESS;
        dumpAllNodes(output, result);
        return result;
    }

    [[nodiscard]] BinaryTree *getTreeHead() const {
        return treeHead;
    }
};


#endif //AKINATOR_STANDARDFILEHANDLER_H
