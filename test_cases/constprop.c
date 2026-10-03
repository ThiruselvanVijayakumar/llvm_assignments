int constant_propagation(void)
{
    int a = 4;
    int b = 5;
    int c = a + b;
    int d = c * 2;
    return d;
}
