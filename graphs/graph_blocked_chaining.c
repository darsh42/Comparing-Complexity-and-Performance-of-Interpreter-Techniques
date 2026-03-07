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
    block_t *blocks;

    block_t *current  = NULL;
    block_t *previous = NULL;

    for (;;) {
        if (current == NULL) {
            current = hash_find(blocks, pc);

            if (current == NULL) {
                current = decode(pc);
                hash_add(blocks, current, pc);
            }

            if (previous) {
                if (branched) { previous->jump = &current; } 
                else          { previous->cont = &current; }
            }
        }

        for (void **o = current.ops; *o != NULL; o++) {
            *o(); pc++;
        }

        previous = current;

        current = (branched) ? previous->jump:
                               previous->cont;
    }
}

