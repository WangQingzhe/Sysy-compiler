int main()
{ /* scope test */
    putch(97);
    putch(10);
    int a = 1, putch = 0;
    {
        a = a + 2;
        int b = a + 3;
        b = b + 4;
        putch = putch + a + b;
        {
            b = b + 5;
            int main = b + 6;
            a = a + main;
            putch = putch + a + b + main;
            {
                b = b + a;
                int a = main + 7;
                a = a + 8;
                putch = putch + a + b + main;
                {
                    b = b + a;
                    int b = main + 9;
                    a = a + 10;
                    const int a = 11;
                    b = b + 12;
                    putch = putch + a + b + main;
                    {
                        main = main + b;
                        int main = b + 13;
                        main = main + a;
                        putch = putch + a + b + main;
                    }
                    putch = putch - main;
                }
                putch = putch - b;
            }
            putch = putch - a;
        }
    }
    return putch % 77;
}