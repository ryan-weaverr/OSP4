/*
 Program is set to run with file redirect
 gcc -g -Wall Iru.c
 ./a.out < test_1.in | diff - test_1.out

 Can be run with fopen for file input, just change the comment on lines 105 and 112
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PAGE_SIZE 10
#define VIRTUAL_PAGES 10
#define PHYSICAL_FRAMES 4

typedef struct
{
    int vpn;
    int pfn;
    bool valid;
} PageTableEntry;

typedef struct
{
    int vpn;
    bool valid;
} InvertedPageTableEntry;

void print_info(int virtual_addr, int physical_addr, bool page_fault, PageTableEntry *page_table, InvertedPageTableEntry *inverted_page_table, int *lru_order)
{
    printf(" %02d   %02d ", virtual_addr, physical_addr);

    if (page_fault)
    {
        printf("  fault    ");
    }
    else
    {
        printf("           ");
    }

    for (int i = 0; i < VIRTUAL_PAGES; i++)
    {
        if (page_table[i].valid)
        {
            if (page_table[i].vpn == virtual_addr / PAGE_SIZE)
            {
                printf("%d%c ", page_table[i].pfn, page_fault == true ? '*' : '+');
            }
            else
            {
                printf("%d  ", page_table[i].pfn);
            }
        }
        else
        {
            printf("   ");
        }
    }
    printf("    ");

    for (int i = 0; i < PHYSICAL_FRAMES; i++)
    {
        if (inverted_page_table[i].valid)
        {
            if (inverted_page_table[i].vpn == virtual_addr / PAGE_SIZE)
            {
                printf("%d%c ", inverted_page_table[i].vpn,
                       page_fault == true ? '*' : '+');
            }
            else
            {
                printf("%d  ", inverted_page_table[i].vpn);
            }
        }
        else
        {
            printf("   ");
        }
    }
    printf("   ");

    for (int i = 0; i < PHYSICAL_FRAMES; i++)
    {
        printf("%d", lru_order[i]);
        if (i != PHYSICAL_FRAMES - 1)
        {
            printf(",");
        }
    }

    printf("\n");
}

int main(int argc, char *argv[])
{
    PageTableEntry page_table[VIRTUAL_PAGES] = {0};
    InvertedPageTableEntry inverted_page_table[PHYSICAL_FRAMES] = {0};
    int lru_order[PHYSICAL_FRAMES] = {0, 1, 2, 3};
    int num_references = 0;
    int num_faults = 0;
    char line[100];

    //FILE *input = fopen(argv[1], "r");

    printf("\n address   page           page table entries          page frames   LRU order\n");
    printf("virt|phys  fault   | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9|   | 0| 1| 2| 3|  LRU...MRU\n");
    printf("---- ----  -----   |--|--|--|--|--|--|--|--|--|--|   |--|--|--|--|  ---------\n");
    printf("                                                                     %d,%d,%d,%d\n", lru_order[0], lru_order[1], lru_order[2], lru_order[3]);

    //while (fgets(line, sizeof(line), input))
    while (fgets(line, sizeof(line), stdin))
    {
        int virt_addr = atoi(line);
        int vpn = virt_addr / PAGE_SIZE;
        int offset = virt_addr % PAGE_SIZE;

        PageTableEntry *pte = &page_table[vpn];

        if (pte->valid)
        {
            int i;
            int pfn = pte->pfn;
            int phys_addr = pfn * PAGE_SIZE + offset;

            for (i = 0; i < PHYSICAL_FRAMES; i++)
            {
                if (lru_order[i] == pfn)
                {
                    break;
                }
            }

            for (; i < PHYSICAL_FRAMES - 1; i++)
            {
                lru_order[i] = lru_order[i + 1];
            }

            lru_order[PHYSICAL_FRAMES - 1] = pfn;
            print_info(virt_addr, phys_addr, false, page_table, inverted_page_table, lru_order);
        }

        else
        {
            num_faults++;

            int pfn = -1;
            for (int i = 0; i < PHYSICAL_FRAMES; i++)
            {
                if (!inverted_page_table[i].valid)
                {
                    pfn = i;
                    break;
                }
            }

            if (pfn == -1)
            {
                pfn = lru_order[0];
                int vpn_to_evict = -1;

                for (int i = 0; i < VIRTUAL_PAGES; i++)
                {
                    if (page_table[i].valid && page_table[i].pfn == pfn)
                    {
                        vpn_to_evict = i;
                        break;
                    }
                }

                page_table[vpn_to_evict].valid = false;
                inverted_page_table[pfn].valid = false;
            }

            pte->vpn = vpn;
            pte->pfn = pfn;
            pte->valid = true;
            inverted_page_table[pfn].vpn = vpn;
            inverted_page_table[pfn].valid = true;

            int phys_addr = pfn * PAGE_SIZE + offset;

            for (int i = 0; i < PHYSICAL_FRAMES - 1; i++)
            {
                lru_order[i] = lru_order[i + 1];
            }
            lru_order[PHYSICAL_FRAMES - 1] = pfn;
            print_info(virt_addr, phys_addr, true, page_table, inverted_page_table, lru_order);
        }
        num_references++;
    }

    printf("\ntrace length = %d addresses, causing %d page faults", num_references, num_faults);
    printf("\n\n");

    return 0;
}