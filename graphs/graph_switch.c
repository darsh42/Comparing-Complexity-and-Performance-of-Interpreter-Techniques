void do_add(void);
void do_sub(void);
void do_mul(void);
void do_div(void);
void do_lw(void); 
void do_sw(void); 

int read(int address);

int main(void) {
    int pc, instruction;
    for (;;) {
        instruction = read(pc++);
        switch(instruction) {
        case 0x1: do_add(); break;
        case 0x2: do_sub(); break;
        case 0x3: do_mul(); break;
        case 0x4: do_div(); break;
        case 0x5: do_lw();  break;
        case 0x6: do_sw();  break;
        }
    }
}
