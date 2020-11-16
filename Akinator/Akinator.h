//
// Created by Александр Дремов on 15.11.2020.
//
#include "BinaryTree.h"
#include "SwiftyList/SwiftyList.hpp"
#include "Algo/StringDistance.hpp"
#include <cctype>

#ifndef AKINATOR_AKINATOR_H
#define AKINATOR_AKINATOR_H

const int MAX_INPUT_LINE = 100;

class Akinator {
    BinaryTree       *head;
    BinaryTree       *currentNode;

    static StringView getAnswerText(){
        printf("> ");
        char* buffer = static_cast<char*>(calloc(MAX_INPUT_LINE, sizeof(char)));
        fgets(buffer, MAX_INPUT_LINE, stdin);
        auto str = StringView(buffer, true);
        for (char* end = str.getBuffer() + str.getLength() - 1; end > str.getBuffer(); --end) {
            if (!(*end == ' ' || *end == '\n')) {
                break;
            }
            *end = '\0';
        }
        return str;
    }

    void askUser() {
        printf("Is this true: %s?\n", currentNode->getNodeName().getBuffer());
    }

    void startGuessing() {
        while(!isTerminated()) {
            askUser();
            makeDecision();
        }
        checkCorrectness();
    }

    unsigned int traverseClosest(BinaryTree* node, StringView string, SwiftyList<bool>* path, bool mutating = false){
        if (node == nullptr)
            return -1;
        if (node->getLeft() == nullptr || node->getRight() == nullptr) {
            LevenshteinEdit* script;
            return levenshtein_distance(node->getNodeName(), string, &script);
        }

        if (!mutating){
            if (traverseClosest(node->getLeft(), string, path) < traverseClosest(node->getRight(), string, path))
                return traverseClosest(node->getLeft(), string, path);
            else
                return traverseClosest(node->getRight(), string, path);
        } else {
            if (traverseClosest(node->getLeft(), string, path) < traverseClosest(node->getRight(), string, path)) {
                path->pushBack(true);
                return traverseClosest(node->getLeft(), string, path, true);
            } else {
                path->pushBack(false);
                return traverseClosest(node->getRight(), string, path, true);
            }
        }
    }

    static BinaryTree* theLastElem(BinaryTree* start, SwiftyList<bool>* list) {
        BinaryTree* res = start;
        for(size_t it = list->begin(); it != 0; it = list->nextIterator(it)) {
            bool val = false;
            list->get(it, &val);
            if (val){
                res = res->getLeft();
            } else {
                res = res->getRight();
            }
        }
        return res;
    }

    void startDifference() {
        printf("Who is the first character?\n");
        StringView answerFirst = getAnswerText();
        printf("Who is the second character?\n");
        StringView answerSecond = getAnswerText();

        this->currentNode = this->head;
        auto pathFirst = SwiftyList<bool>(10, 0, nullptr, false);
        auto pathSecond = SwiftyList<bool>(10, 0, nullptr, false);

        this->traverseClosest(this->currentNode, answerFirst, &pathFirst, true);
        this->traverseClosest(this->currentNode, answerSecond, &pathSecond, true);

        size_t itFirst  = pathFirst.begin();
        size_t itSecond = pathSecond.begin();

        bool valFirst = false, valSecond = false;
        pathFirst.get(itFirst, &valFirst);
        pathSecond.get(itSecond, &valSecond);

        BinaryTree* lastElemFirst = theLastElem(this->head, &pathFirst);
        BinaryTree* lastElemSecond = theLastElem(this->head, &pathSecond);
        if (strcmp(lastElemFirst->getNodeName().getBuffer(), answerFirst.getBuffer()) != 0){
            printf("As for the first character, I know only about \"%s\"\n", lastElemFirst->getNodeName().getBuffer());
        }
        if (strcmp(lastElemSecond->getNodeName().getBuffer(), answerSecond.getBuffer()) != 0){
            printf("As for the second character, I know only about \"%s\"\n", lastElemSecond->getNodeName().getBuffer());
        }

        bool differenceFired = false;
        if (valFirst != valSecond){
            differenceFired = true;
            printf("Your characters have nothing in common. \n");
        } else {
            printf("Your characters both ");
        }

        for(;itFirst != 0 && itSecond != 0 && !differenceFired;
             pathFirst.nextIterator(&itFirst), pathSecond.nextIterator(&itSecond)){
            pathFirst.get(itFirst, &valFirst);
            pathSecond.get(itSecond, &valSecond);
            if (valFirst != valSecond) {
                break;
            }
            if (valFirst){
                printf("%s", this->currentNode->getNodeName().getBuffer());
                this->currentNode = this->currentNode->getLeft();
            } else {
                printf("not %s", this->currentNode->getNodeName().getBuffer());
                this->currentNode = this->currentNode->getRight();
            }
            printf(", ");
        }

        BinaryTree* commonNode = this->currentNode;
        size_t commonFirst = itFirst;
        if (itFirst != 0) {
            printf("but \"%s\" ", answerFirst.getBuffer());
//            itFirst = pathFirst.prevIterator(itFirst);
            for(;itFirst != 0; pathFirst.nextIterator(&itFirst)) {
                pathFirst.get(itFirst, &valFirst);
                if (valFirst){
                    printf("%s", this->currentNode->getNodeName().getBuffer());
                    this->currentNode = this->currentNode->getLeft();
                } else {
                    printf("not %s", this->currentNode->getNodeName().getBuffer());
                    this->currentNode = this->currentNode->getRight();
                }
                if (0 != pathFirst.nextIterator(itFirst))
                    printf(", ");
            }
        }
        if (itSecond != 0) {
            this->currentNode = commonNode;
            if (commonFirst != 0)
                printf(", while \"%s\" ", answerSecond.getBuffer());
            else
                printf("but \"%s\" ", answerSecond.getBuffer());
//            itSecond = pathSecond.prevIterator(itSecond);
            for (; itSecond != 0; pathSecond.nextIterator(&itSecond)) {
                pathSecond.get(itSecond, &valSecond);
                if (valSecond) {
                    printf("%s", this->currentNode->getNodeName().getBuffer());
                    this->currentNode = this->currentNode->getLeft();
                } else {
                    printf("not %s", this->currentNode->getNodeName().getBuffer());
                    this->currentNode = this->currentNode->getRight();
                }
                if (0 != pathSecond.nextIterator(itSecond))
                    printf(", ");
            }
        }
        pathFirst.destructList();
        pathSecond.destructList();
        answerFirst.DestructString();
        answerSecond.DestructString();
        printf("\n");
    }

    void startDefinition() {
        printf("Who is your character?\n");
        StringView answer = getAnswerText();

        this->currentNode = this->head;
        auto path = SwiftyList<bool>(10, 0, nullptr, false);
        this->traverseClosest(this->currentNode, answer, &path, true);

        BinaryTree* lastElemFirst = theLastElem(this->head, &path);
        if (strcmp(lastElemFirst->getNodeName().getBuffer(), answer.getBuffer()) != 0){
            printf("I know only about \"%s\"\n", lastElemFirst->getNodeName().getBuffer());
        }
        printf("I know, that \"%s\"... ", lastElemFirst->getNodeName().getBuffer());

        for(size_t it = path.begin(); it != 0 && this->currentNode != nullptr; it = path.nextIterator(it)){
            bool val = false;
            path.get(it, &val);
            if (val){
                printf("it %s", this->currentNode->getNodeName().getBuffer());
                this->currentNode = this->currentNode->getLeft();
            } else {
                printf("not %s", this->currentNode->getNodeName().getBuffer());
                this->currentNode = this->currentNode->getRight();
            }
            if (0 != path.nextIterator(it))
                printf(", ");
        }
        path.destructList();
        answer.DestructString();
        printf("\n");
    }

    void checkCorrectness() {
        if (this->currentNode == nullptr)
            printf("No character found!\n");
        printf("I guess it is... %s?\n", this->currentNode->getNodeName().getBuffer());
        if (askBoolAnswer()){
            printf("I knew it!\n");
            resetGuessing();
        } else {
            askForCorrectSolution();
        }
    }

    void askForCorrectSolution() {
        printf("Who's it then?\n");

        StringView naming = getAnswerText();

        printf("What \"%s\" has that \"%s\" doesn't has?\n", naming.getBuffer(), this->currentNode->getNodeName().getBuffer());

        StringView newTrait = getAnswerText();
        auto newRight = StringView(this->currentNode->getNodeName().getBuffer());

        this->currentNode->setNodeName(newTrait);
        this->currentNode->setLeft(new BinaryTree(naming));
        this->currentNode->setRight(new BinaryTree(newRight));
    }

    static bool askBoolAnswer() {
        StringView naming = getAnswerText();
        bool retVal = (naming.getBuffer()[0] == 'y' || naming.getBuffer()[0] == 'Y');
        naming.DestructString();
        return retVal;
    }

    void makeDecision() {
        if (this->currentNode == nullptr)
            printf("No decisions to make!\n");
        if (askBoolAnswer())
            this->currentNode = this->currentNode->getLeft();
        else
            this->currentNode = this->currentNode->getRight();
    }

public:
    explicit Akinator(BinaryTree *head): head(head), currentNode(head) {}

    void resetGuessing() {
        this->currentNode = this->head;
    }

    void startGame() {
        printf("What you want me to do?\n "
               "[1] - guess [2] - define [3] - difference\n");
        StringView answer = getAnswerText();
        switch (answer.getBuffer()[0]) {
            case '1':
                startGuessing();
                break;
            case '2':
                startDefinition();
                break;
            case '3':
                startDifference();
                break;
            default:
                printf("Unknown choice\n");
                startGame();
        }
        answer.DestructString();
    }

    bool isTerminated() {
        if(this->currentNode == nullptr)
            return true;
        if (this->currentNode->getRight() == nullptr && this->currentNode->getLeft() == nullptr)
            return true;
        return false;
    }
};

#endif //AKINATOR_AKINATOR_H
