#include "BinaryTree.h"
#include "Akinator.h"
#include "StandardFileHandler.h"

int main() {
    StandardFileHandler handler;

    FILE* inFile = fopen("akinator.db", "r");
    assert(inFile);
    SFHandlerResult res = handler.loadFromFile(inFile);
    if (res != SF_SUCCESS){
        printf("Failed loading database\n");
        fclose(inFile);
        return EXIT_FAILURE;
    }
    fclose(inFile);

    BinaryTree* head = handler.getTreeHead();

    auto akinator = Akinator(head);

    akinator.startGame();

    FILE* outFile = fopen("akinator.db", "w");
    assert(outFile);
    handler.fileDump(outFile);
    fclose(outFile);
    head->DestructNode();
    delete head;
    return 0;
}
