
#include "globals.h"
#include "tools.h"
#include "registries.h"
#include "worldgen.h"
#include "procedures.h"
#include "structures.h"

void setBlockIfReplaceable (short x, uint8_t y, short z, uint8_t block) {
  uint8_t target = getBlockAt(x, y, z);
  if (!isReplaceableBlock(target) && target != B_oak_leaves) return;
  makeBlockChange(x, y, z, block);
}


// Funkcja budująca prosty domek
void placeHouse(short x, uint8_t y, short z) {
    // Podłoga z desek dębowych (oak_planks - dodaj to do whitelist w JS, jeśli trzeba)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            makeBlockChange(x + i, y, z + j, B_OAK_PLANKS);
        }
    }
    
    // Ściany z bloków mchu (moss_carpet - z Twojej listy) lub innych dostępnych
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == 0 || i == 3 || j == 0 || j == 3) {
                makeBlockChange(x + i, y + 1, z + j, B_COBBLESTONE_SLAB);
                makeBlockChange(x + i, y + 2, z + j, B_COBBLESTONE_SLAB);
            }
        }
    }
}

// Funkcja tworząca ścieżkę
void placePath(short x, uint8_t y, short z, int length) {
    for (int i = 0; i < length; i++) {
        makeBlockChange(x + i, y - 1, z, B_GRASS_PATH); 
    }
}

#include "structures.h"

// Places a tree centered on the input coordinates
void placeTreeStructure (short x, uint8_t y, short z) {

  // Get a random number for tree height and leaf edges
  uint32_t r = fast_rand();
  uint8_t height = 4 + (r % 3);

  // Set tree base - replace sapling with log and put dirt below
  makeBlockChange(x, y - 1, z, B_dirt);
  makeBlockChange(x, y, z, B_oak_log);
  // Create tree stump
  for (int i = 1; i < height; i ++) {
    setBlockIfReplaceable(x, y + i, z, B_oak_log);
  }
  // Keep track of leaf corners, determines random number bit shift
  uint8_t t = 2;
  // First (bottom) leaf layer
  for (int i = -2; i <= 2; i ++) {
    for (int j = -2; j <= 2; j ++) {
      setBlockIfReplaceable(x + i, y + height - 3, z + j, B_oak_leaves);
      // Randomly skip some corners, emulating vanilla tree shape
      if ((i == 2 || i == -2) && (j == 2 || j == -2)) {
        t ++;
        if ((r >> t) & 1) continue;
      }
      setBlockIfReplaceable(x + i, y + height - 2, z + j, B_oak_leaves);
    }
  }
  // Second (top) leaf layer
  for (int i = -1; i <= 1; i ++) {
    for (int j = -1; j <= 1; j ++) {
      setBlockIfReplaceable(x + i, y + height - 1, z + j, B_oak_leaves);
      if ((i == 1 || i == -1) && (j == 1 || j == -1)) {
        t ++;
        if ((r >> t) & 1) continue;
      }
      setBlockIfReplaceable(x + i, y + height, z + j, B_oak_leaves);
    }
  }

}
