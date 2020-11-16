//
// Created by Александр Дремов on 14.11.2020.
//
#include <cstdlib>
#include <cstring>

#ifndef AKINATOR_STRINGVIEW_H
#define AKINATOR_STRINGVIEW_H

class StringView {
    char*   buffer;
    size_t  len;
    bool    canBeFreed;
public:
    static StringView* CreateNovel(char *line, bool canBeFreed=false){
        auto* thou = static_cast<StringView*>(calloc(1, sizeof(StringView)));
        thou->buffer = line;
        thou->len = strlen(line);
        thou->canBeFreed = canBeFreed;
        return thou;
    }

    static StringView* CreateNovel(char * line, size_t len, bool canBeFreed=false){
        auto* thou = static_cast<StringView*>(calloc(1, sizeof(StringView)));
        thou->buffer = line;
        thou->len = len;
        thou->canBeFreed = canBeFreed;
        return thou;
    }

    [[nodiscard]] char* getBuffer() const {
        return this->buffer;
    }

    [[nodiscard]] size_t getLength() const {
        return this->len;
    }

    void updateLength() {
        this->len = strlen(this->buffer);
    }

    void DestructString() {
        if (canBeFreed)
            free(buffer);
    }

};
#endif //AKINATOR_STRINGVIEW_H
