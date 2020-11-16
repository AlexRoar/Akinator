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
    explicit StringView(char * line, bool canBeFreed=false): buffer(line), len(strlen(line)), canBeFreed(canBeFreed){}
    explicit StringView(char * line, size_t len, bool canBeFreed=false): buffer(line), len(len), canBeFreed(canBeFreed){}

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

class StringViewBuilder {
    char*       buffer;
    size_t      size;
    size_t      capacity;
public:
    StringViewBuilder(): capacity(0), buffer(nullptr), size(0){
        const size_t initialSize = 20;
        this->capacity = initialSize;
        this->buffer = static_cast<char *>(calloc(initialSize + 1, sizeof(char)));
    }

    void append(char symbol){
        this->reallocate();
        this->buffer[this->size++] = symbol;
    }

    void reallocate() {
        if (this->size >= this->capacity) {
            this->buffer = static_cast<char *>(realloc(this->buffer, this->size * 2 + 1));
            this->capacity = this->size * 2;
        }
    }

    StringView build() {
        return StringView(this->buffer, this->size);
    }
};

#endif //AKINATOR_STRINGVIEW_H
