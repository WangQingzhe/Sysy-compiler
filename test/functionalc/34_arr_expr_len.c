/*
 * @Description:
 * @Author: 王清哲
 * @Date: 2023-07-18 21:20:29
 * @LastEditTime: 2023-07-23 19:37:01
 * @LastEditors: 王清哲
 */
const int N = -1;
int arr[6] = {1, 2, 33, 4, 5, 6};

int main()
{
  int i = 0, sum = 0;
  while (i < 6)
  {
    sum = sum + arr[i];
    i = i + 1;
  }
  return sum;
}
