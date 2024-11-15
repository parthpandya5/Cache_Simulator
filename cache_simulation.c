#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define CACHE_SIZE 128
#define BLOCK_SIZE 16
#define NUM_BLOCKS (CACHE_SIZE / BLOCK_SIZE)

// Cache line structure
typedef struct CacheLine {
    unsigned int tag;
    bool valid;
    bool dirty;
    int lru_counter; // For LRU policy
} CacheLine;

// Cache structure
typedef struct Cache {
    CacheLine lines[NUM_BLOCKS];
    int lru_max; // For LRU policy
} Cache;

// Initialize cache
void initCache(Cache* cache) {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        cache->lines[i].valid = false;
        cache->lines[i].dirty = false;
        cache->lines[i].lru_counter = 0;
    }
    cache->lru_max = 0;
}

// Function to access cache
bool accessCache(Cache* cache, unsigned int address, bool write) {
    unsigned int index = (address / BLOCK_SIZE) % NUM_BLOCKS;
    unsigned int tag = address / CACHE_SIZE;

    CacheLine* line = &cache->lines[index];

    // Check for a cache hit
    if (line->valid && line->tag == tag) {
        // Update LRU counter
        line->lru_counter = ++cache->lru_max;
        if (write) {
            line->dirty = true;
        }
        return true; // Cache hit
    }

    // Cache miss
    if (line->valid && line->dirty) {
        // Write-back to memory
        printf("Write-back: Address 0x%X\n", (line->tag * CACHE_SIZE) + (index * BLOCK_SIZE));
    }

    // Update cache line
    line->tag = tag;
    line->valid = true;
    line->dirty = write;
    line->lru_counter = ++cache->lru_max;

    return false; // Cache miss
}

int main() {
    Cache cache;
    initCache(&cache);

    unsigned int addresses[] = {0x0, 0x1, 0x2, 0x3, 0x80, 0x81, 0x82, 0x83, 0x100, 0x180};
    bool writes[] = {false, false, true, false, false, true, false, true, false, true};
    int num_accesses = sizeof(addresses) / sizeof(addresses[0]);

    for (int i = 0; i < num_accesses; i++) {
        bool hit = accessCache(&cache, addresses[i], writes[i]);
        printf("Access address 0x%X: %s\n", addresses[i], hit ? "Hit" : "Miss");
    }

    return 0;
}
