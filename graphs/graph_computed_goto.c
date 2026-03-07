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

int main(void) {
    int pc;

add:
    do_add();
    opcodes[read(pc++)];

sub:
    do_sub();
    opcodes[read(pc++)];

mul:
    do_mul();
    opcodes[read(pc++)];

div:
    do_div();
    opcodes[read(pc++)];

lw:
    do_lw();
    opcodes[read(pc++)];

sw:
    do_sw();
    opcodes[read(pc++)];
}
