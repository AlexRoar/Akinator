//
// Created by Александр Дремов on 15.11.2020.
//
#include "BinaryTree.h"
#include "SwiftyList/SwiftyList.hpp"
#include "Algo/StringDistance.hpp"
#include <cctype>
#include <cstdarg>

#ifndef AKINATOR_AKINATOR_H
#define AKINATOR_AKINATOR_H

const int MAX_INPUT_LINE = 100;

class Akinator {
    BinaryTree       *head;
    BinaryTree       *currentNode;
    bool              speak;

    void printfsay(const char* format...) const {
        va_list argptr;
        va_start(argptr, format);

        char* buffer = static_cast<char*>(calloc(MAX_INPUT_LINE + 20, sizeof(char)));
        strcpy(buffer, "say ");
        vsprintf(buffer + sizeof("say"), format, argptr);

        printf("%s", buffer + sizeof("say"));

        fflush(stdout);
        if (speak)
            system(buffer);
        free(buffer);
        va_end(argptr);
    }

    static StringView* getAnswerText(){
        printf("> ");
        char* buffer = static_cast<char*>(calloc(MAX_INPUT_LINE, sizeof(char)));
        fgets(buffer, MAX_INPUT_LINE, stdin);
        auto* str = StringView::CreateNovel(buffer, true);
        for (char* end = str->getBuffer() + str->getLength() - 1; end > str->getBuffer(); --end) {
            if (!(*end == ' ' || *end == '\n')) {
                break;
            }
            *end = '\0';
        }
        return str;
    }

    void askUser() {
        printf("Is this true: %s?\n", currentNode->getNodeName()->getBuffer());
    }

    void startGuessing() {
        this->currentNode = this->head;
        while(!isTerminated()) {
            askUser();
            makeDecision();
        }
        checkCorrectness();
    }

    unsigned int traverseClosest(BinaryTree* node, StringView* string, SwiftyList<bool>* path, bool mutating = false){
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
        this->currentNode = this->head;
        printf("Who is the first character?\n");
        StringView* answerFirst = getAnswerText();
        printf("Who is the second character?\n");
        StringView* answerSecond = getAnswerText();

        auto* pathFirst = SwiftyList<bool>::CreateNovel(10, 0, nullptr, false);
        auto* pathSecond = SwiftyList<bool>::CreateNovel(10, 0, nullptr, false);

        this->traverseClosest(this->currentNode, answerFirst, pathFirst, true);
        this->traverseClosest(this->currentNode, answerSecond, pathSecond, true);

        size_t itFirst  = pathFirst->begin();
        size_t itSecond = pathSecond->begin();

        bool valFirst = false, valSecond = false;
        pathFirst->get(itFirst, &valFirst);
        pathSecond->get(itSecond, &valSecond);

        BinaryTree* lastElemFirst = theLastElem(this->head, pathFirst);
        BinaryTree* lastElemSecond = theLastElem(this->head, pathSecond);
        if (strcmp(lastElemFirst->getNodeName()->getBuffer(), answerFirst->getBuffer()) != 0){
            printf("As for the first character, I know only about \"%s\"\n", lastElemFirst->getNodeName()->getBuffer());
        }
        if (strcmp(lastElemSecond->getNodeName()->getBuffer(), answerSecond->getBuffer()) != 0){
            printf("As for the second character, I know only about \"%s\"\n", lastElemSecond->getNodeName()->getBuffer());
        }

        bool differenceFired = false;
        if (valFirst != valSecond){
            differenceFired = true;
            printfsay("Your characters have nothing in common. \n");
        } else {
            printfsay("Your characters both ");
        }

        for(;itFirst != 0 && itSecond != 0 && !differenceFired;
             pathFirst->nextIterator(&itFirst), pathSecond->nextIterator(&itSecond)){
            pathFirst->get(itFirst, &valFirst);
            pathSecond->get(itSecond, &valSecond);
            if (valFirst != valSecond) {
                break;
            }
            if (valFirst){
                printfsay("%s", this->currentNode->getNodeName()->getBuffer());
                this->currentNode = this->currentNode->getLeft();
            } else {
                printfsay("not %s", this->currentNode->getNodeName()->getBuffer());
                this->currentNode = this->currentNode->getRight();
            }
            printf(", ");
        }

        BinaryTree* commonNode = this->currentNode;
        size_t commonFirst = itFirst;
        if (itFirst != 0) {
            printfsay("but \"%s\" ", answerFirst->getBuffer());
//            itFirst = pathFirst.prevIterator(itFirst);
            for(;itFirst != 0; pathFirst->nextIterator(&itFirst)) {
                pathFirst->get(itFirst, &valFirst);
                if (valFirst){
                    printfsay("%s", this->currentNode->getNodeName()->getBuffer());
                    this->currentNode = this->currentNode->getLeft();
                } else {
                    printfsay("not %s", this->currentNode->getNodeName()->getBuffer());
                    this->currentNode = this->currentNode->getRight();
                }
                if (0 != pathFirst->nextIterator(itFirst))
                    printf(", ");
            }
        }
        if (itSecond != 0) {
            this->currentNode = commonNode;
            if (commonFirst != 0)
                printfsay(", while \"%s\" ", answerSecond->getBuffer());
            else
                printfsay("but \"%s\" ", answerSecond->getBuffer());
//            itSecond = pathSecond.prevIterator(itSecond);
            for (; itSecond != 0; pathSecond->nextIterator(&itSecond)) {
                pathSecond->get(itSecond, &valSecond);
                if (valSecond) {
                    printfsay("%s", this->currentNode->getNodeName()->getBuffer());
                    this->currentNode = this->currentNode->getLeft();
                } else {
                    printfsay("not %s", this->currentNode->getNodeName()->getBuffer());
                    this->currentNode = this->currentNode->getRight();
                }
                if (0 != pathSecond->nextIterator(itSecond))
                    printf(", ");
            }
        }
        pathFirst->DestructList();
        pathSecond->DestructList();
        answerFirst->DestructString();
        answerSecond->DestructString();
        free(answerFirst);
        free(answerSecond);
        free(pathFirst);
        free(pathSecond);
        printf("\n");
    }

    void startDefinition() {
        printfsay("Who is your character?\n");
        StringView* answer = getAnswerText();

        this->currentNode = this->head;
        auto* path = SwiftyList<bool>::CreateNovel(10, 0, nullptr, false);
        this->traverseClosest(this->currentNode, answer, path, true);

        BinaryTree* lastElemFirst = theLastElem(this->head, path);
        if (strcmp(lastElemFirst->getNodeName()->getBuffer(), answer->getBuffer()) != 0){
            printfsay("I know only about \"%s\"\n", lastElemFirst->getNodeName()->getBuffer());
        }
        printfsay("I know, that \"%s\"... ", lastElemFirst->getNodeName()->getBuffer());

        for(size_t it = path->begin(); it != 0 && this->currentNode != nullptr; it = path->nextIterator(it)){
            bool val = false;
            path->get(it, &val);
            if (val){
                printfsay("it %s", this->currentNode->getNodeName()->getBuffer());
                this->currentNode = this->currentNode->getLeft();
            } else {
                printfsay("not %s", this->currentNode->getNodeName()->getBuffer());
                this->currentNode = this->currentNode->getRight();
            }
            if (0 != path->nextIterator(it))
                printf(", ");
        }
        path->DestructList();
        answer->DestructString();
        free(answer);
        free(path);
        printf("\n");
    }

    void checkCorrectness() {
        if (this->currentNode == nullptr)
            printfsay("No character found!\n");
        printfsay("I guess it is... %s?\n", this->currentNode->getNodeName()->getBuffer());
        if (askBoolAnswer()){
            printfsay("I knew it!\n");
            resetGuessing();
        } else {
            askForCorrectSolution();
        }
    }

    void askForCorrectSolution() {
        printfsay("Who's it then?\n");

        StringView* naming = getAnswerText();

        printfsay("What \"%s\" has that \"%s\" doesn't has?\n", naming->getBuffer(), this->currentNode->getNodeName()->getBuffer());

        StringView* newTrait = getAnswerText();
        auto* newRight = StringView::CreateNovel(this->currentNode->getNodeName()->getBuffer());

        this->currentNode->setNodeName(newTrait);
        this->currentNode->setLeft(BinaryTree::CreateNovel(naming));
        this->currentNode->setRight(BinaryTree::CreateNovel(newRight));
    }

    static bool askBoolAnswer() {
        StringView* naming = getAnswerText();
        bool retVal = (naming->getBuffer()[0] == 'y' || naming->getBuffer()[0] == 'Y');
        naming->DestructString();
        free(naming);
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

    void dumpNode(FILE* out, BinaryTree* node){
        if (!node)
            return;
        if (node->getLeft() == nullptr && node->getRight() == nullptr)
            fprintf(out, "\tnode_%p [label=\"%s\" shape=\"invhouse\" fillcolor=\"aliceblue\" style=filled];\n", node, node->getNodeName()->getBuffer());
        else
            fprintf(out, "\tnode_%p [label=\"%s\" shape=\"box3d\"];\n", node, node->getNodeName()->getBuffer());
        if (node->getLeft())
            fprintf(out, "\tnode_%p->node_%p [color=\"green\" label=\"yes\" fontcolor=\"green\"];\n", node, node->getLeft());
        if (node->getRight())
            fprintf(out, "\tnode_%p->node_%p [color=\"darkred\" label=\"no\" fontcolor=\"darkred\"];\n", node, node->getRight());
        dumpNode(out, node->getLeft());
        dumpNode(out, node->getRight());
    }

    void buildGraph() {
        FILE* graph = fopen("tmp.gv", "w");

        fprintf(graph, "digraph akinator {\n");
        dumpNode(graph, head);
        fprintf(graph, "}\n");
        fclose(graph);
        system("dot -Tsvg tmp.gv -o graph.svg");
        system("dot -Tpng tmp.gv -o graph.png");
    }

public:
    static Akinator* CreateNovel(BinaryTree *head, bool speak=false){
        auto* thou = static_cast<Akinator*>(calloc(1, sizeof(Akinator)));
        thou->head        = head;
        thou->currentNode = head;
        thou->speak       = speak;
        return thou;
    }

    void resetGuessing() {
        this->currentNode = this->head;
    }

    void startGame() {
        printf("What you want me to do?\n "
               "\t[1] - guess\n\t[2] - define\n\t[3] - difference\n\t[4] - exit and save\n\t[5] - build graph\n");
        StringView* answer = getAnswerText();
        switch (answer->getBuffer()[0]) {
            case '1':
                startGuessing();
                break;
            case '2':
                startDefinition();
                break;
            case '3':
                startDifference();
                break;
            case '4': {
                answer->DestructString();
                free(answer);
                printfsay("Bye\n");
                return;
            }
            case '5': {
                buildGraph();
                break;
            }
            default:
                printf("Unknown choice\n");
                startGame();
        }
        answer->DestructString();
        free(answer);
        startGame();
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
