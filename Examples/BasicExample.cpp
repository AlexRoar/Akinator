#include "BinaryTree.h"
#include "Akinator.h"
#include "StandardFileHandler.h"

int main() {
    StandardFileHandler handler;

    FILE* inFile = fopen("akinator.db", "r");
    assert(inFile);

    char* commonBuffer = nullptr;
    SFHandlerResult res = handler.loadFromFile(inFile, commonBuffer);
    if (res != SF_SUCCESS){
        printf("Failed loading database\n");
        fclose(inFile);
        if (commonBuffer) free(commonBuffer);
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
    free(commonBuffer);
    return 0;
}
