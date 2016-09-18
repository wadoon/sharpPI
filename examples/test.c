

int main(void) {
    char I1;
    char I2;
    char O;

    __CPROVER_assume(I1 == 0 || I1 == 1);

    if(I1==1) {
        O = I2 & 0b11;
    }
    else {
        O = I2 & 0b1;
    }

    assert(0);
    return 0;
}
