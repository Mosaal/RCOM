#include <stdio.h>

int main()
{
  int lenght = 1065, output;
  unsigned char n[2];



  n[0] = (unsigned char) (lenght / 256);
  n[1] = (unsigned char) (lenght % 256);

  output = (int) (n[0] * 256) + (int) n[1];

  printf("0x%02x 0x%02x\n", n[0], n[1]);
  printf("%d\n", output);


  return 0;
}
