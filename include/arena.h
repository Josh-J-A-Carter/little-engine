#ifndef ARENA_H
#define ARENA_H

#include <cstddef>
#include <cstdlib>

struct arena {
    private:
        void* initial_loc { nullptr };
        void* next_loc { nullptr };
        void* final_loc { nullptr };

    public:
        arena(std::size_t size) {
            initial_loc = malloc(size);
            next_loc = initial_loc;
            final_loc = next_loc + size;
        }

        ~arena() {
            free(initial_loc);
        }

        template <typename T>
        T* allocate() {
            if (next_loc == nullptr) return nullptr;

            if (next_loc >= final_loc) return nullptr;

            if (next_loc + sizeof(T) >= final_loc) return nullptr;

            T* allocation = static_cast<T*>(next_loc);
            *allocation = {};

            next_loc = next_loc + sizeof(T);

            return allocation;
        }
};

#endif