void do_add(void);
void do_sub(void);
void do_mul(void);
void do_div(void);
void do_lw(void); 
void do_sw(void); 

int read(int address);

static const void *opcodes[] = {
    [0x1] &&add, [0x2] &&sub, [0x3] &&mul,
    [0x4] &&div, [0x5] &&lw,  [0x6] &&sw,
};

void **block decode(int pc) {
    void **block;

add:
    do_add();
    *block++ = opcodes[read(pc++)];

sub:
    do_sub();
    *block++ = opcodes[read(pc++)];

mul:
    do_mul();
    *block++ = opcodes[read(pc++)];

div:
    do_div();
    *block++ = opcodes[read(pc++)];

lw:
    do_lw();
    *block++ = opcodes[read(pc++)];

sw:
    do_sw();
    *block++ = opcodes[read(pc++)];
}

int main(void) {
    void ***blocks;

    for (;;) {
        void **block = NULL;

        block = hash_find(blocks, pc);

        if (block == NULL) {
            block = decode(pc);
            hash_add(blocks, pc, block);
        }
        
        for (void **b = block; *b != NULL; b++) {
            *b(); pc++;
        }
    }
}
