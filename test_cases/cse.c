int common_subexpression(int a, int b)
{
    int x = a + b;
    int y = a + b;
    int z = x + y;

    return z;
}
