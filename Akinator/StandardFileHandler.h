//
// Created by Александр Дремов on 14.11.2020.
//
#include  <exception>
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
    SF_FILE_NO_MEM
};

class StandardFileException : public std::exception{
    SFHandlerResult result;
    int line;
    const char* func;
    char* why;
public:
    explicit StandardFileException(SFHandlerResult result, int line=0, const char* func= nullptr, char* why=nullptr): result(result), line(line), func(func), why(why){}
    SFHandlerResult what(){
        return this->result;
    }
};

class StandardFileHandler {
    BinaryTree *treeHead;

    static StringView retrievePlainText(char *start, size_t *skipLen) {
        assert(start != nullptr && skipLen != nullptr);

        char* foundStart = strchr(start, '"');
        if (foundStart == nullptr)
            throw StandardFileException(SF_FILE_WRONG_STRING, __LINE__, __PRETTY_FUNCTION__ );

        char* foundEnd = strchr(foundStart + 1, '"');
        if (foundEnd == nullptr)
            throw StandardFileException(SF_FILE_WRONG_STRING, __LINE__, __PRETTY_FUNCTION__ );

        *skipLen = foundEnd - start + 1;
        *foundEnd = '\0';
        return StringView(foundStart + 1);
    }

    static BinaryTree* retrieveNode(char*& data) {
        size_t skipDist = 0;
        StringView plainText = retrievePlainText(data, &skipDist);
        auto* subtree = new BinaryTree(plainText);

        data += skipDist;
        char* nextTextStart = strchr(data, '"');
        char* branchesStart = strchr(data, '[');
        char* branchesEnd   = strchr(data, ']');

        if (branchesStart == nullptr && (branchesEnd  != nullptr && branchesEnd < nextTextStart) ||
            branchesEnd   == nullptr && branchesStart != nullptr){
            throw StandardFileException(SF_FILE_WRONG_FORMAT, __LINE__, __PRETTY_FUNCTION__ );
        }

        if ((nextTextStart < branchesStart && nextTextStart != nullptr) || branchesStart == nullptr)
            return subtree;

        if (nextTextStart == nullptr)
            throw StandardFileException(SF_FILE_WRONG_FORMAT, __LINE__, __PRETTY_FUNCTION__ );

        if (branchesStart < branchesEnd) {
            subtree->setLeft(retrieveNode(data));
            subtree->setRight(retrieveNode(data));
        }
        return subtree;
    }

    static void expectInvisibleOnly(char* start, const char* end) {
        assert(start!= nullptr);
        while (start < end){
            if (*start != ' ' && *start != '\n' && *start != '\t')
                throw StandardFileException(SF_FILE_WRONG_FORMAT, __LINE__, __PRETTY_FUNCTION__, start);
            start++;
        }
    }

    void dumpAllNodes(FILE* file) const {
        dumpNode(file, this->treeHead);
    }

    void dumpNode(FILE* file, BinaryTree* node, int depthLevel = 0) const {
        if (node == nullptr)
            throw StandardFileException(SF_EMPTY, __LINE__, __PRETTY_FUNCTION__ );

        fprintf(file, "%*s", depthLevel * 4, "");
        StringView name = node->getNodeName();
        fprintf(file, "\"%s\"", (name.getBuffer() == nullptr)? "<unknown>": name.getBuffer());
        if (node->getLeft() != nullptr || node->getRight() != nullptr){
            fprintf(file, " [\n");
            if (node->getLeft() == nullptr || node->getRight() == nullptr)
                throw StandardFileException(SF_FILE_WRONG_FORMAT, __LINE__, __PRETTY_FUNCTION__ );
            dumpNode(file, node->getLeft(), depthLevel + 1);
            fprintf(file, "\n");
            dumpNode(file, node->getRight(), depthLevel + 1);
            fprintf(file, "\n%*s]", depthLevel * 4, "");
        }
    }

public:
    StandardFileHandler() : treeHead(nullptr) {}

    SFHandlerResult loadFromFile(FILE* input) noexcept {
        assert(input != nullptr);

        fseek(input, 0L, SEEK_END);
        size_t  numbytes = ftell(input);
        fseek(input, 0L, SEEK_SET);

        char* buffer = static_cast<char*>(calloc(numbytes + 1, sizeof(char)));
        char* unchangedBuffer = buffer;
        fread(buffer, sizeof(char), numbytes, input);

        try {
            this->treeHead = retrieveNode(buffer);
            free(unchangedBuffer);
        } catch (StandardFileException& parseException) {
            return parseException.what();
        }
        return SF_SUCCESS;
    }

    SFHandlerResult fileDump(FILE* output) const noexcept {
        assert(output != nullptr);
        try {
            dumpAllNodes(output);
        } catch (StandardFileException &parseException) {
            return parseException.what();
        }
        return SF_SUCCESS;
    }

    [[nodiscard]] BinaryTree *getTreeHead() const noexcept {
        return treeHead;
    }
};


#endif //AKINATOR_STANDARDFILEHANDLER_H
