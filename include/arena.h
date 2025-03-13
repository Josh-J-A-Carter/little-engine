#ifndef ARENA_H
#define ARENA_H

#include <cstddef>
#include <cstdlib>

struct arena {
    private:
        struct destructor_node {
            void (*destroy_fn) (void*);
            void* obj;
            destructor_node* next;
        };

        void* initial_loc { nullptr };
        void* next_loc { nullptr };
        void* final_loc { nullptr };

        destructor_node* destructors { nullptr };

    public:
        arena(std::size_t size) {
            initial_loc = malloc(size);
            next_loc = initial_loc;
            final_loc = next_loc + size;
        }

        ~arena() {
            while (destructors != nullptr) {
                destructors->destroy_fn(destructors->obj);
                destructor_node* old = destructors;
                destructors = destructors->next;

                delete old;
            }

            free(initial_loc);
        }

        template <typename T>
        inline T* allocate() {
            if (next_loc == nullptr) return nullptr;
            if (next_loc >= final_loc) return nullptr;
            if (next_loc + sizeof(T) >= final_loc) return nullptr;

            T* allocation = new (next_loc) T();

            destructor_node* next = new destructor_node {
                [](void* obj) { static_cast<T*>(obj)->~T(); },
                allocation,
                destructors
            };

            destructors = next;
            
            next_loc = next_loc + sizeof(T);

            return allocation;
        }
};

#endif