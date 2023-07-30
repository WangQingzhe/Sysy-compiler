/*
 * @Description:
 * @Author: 王清哲
 * @Date: 2023-07-30 11:32:04
 * @LastEditTime: 2023-07-30 12:04:14
 * @LastEditors: 王清哲
 */
int quick_read()
{
	int ch = getch();
	int x = 0, f = 0;
	while (ch < 48 || ch > 57)
	{
		if (ch == 45)
			f = 1;
		ch = getch();
	}
	while (ch >= 48 && ch <= 57)
	{
		x = x * 10 + ch - 48;
		ch = getch();
	}
	if (f)
		return -x;
	else
		return x;
}
int n, m;
// const int 1005 = 1005;
// const int 5005 = 5005;
int to[5005], next[5005], head[1005], cnt = 0;
void add_edge(int from, int To)
{
	to[cnt] = To;
	next[cnt] = head[from];
	head[from] = cnt;
	cnt = cnt + 1;
	to[cnt] = from;
	next[cnt] = head[To];
	head[To] = cnt;
	cnt = cnt + 1;
}
void init()
{
	int i = 0;
	while (i < 1005)
	{
		head[i] = -1;
		i = i + 1;
	}
}
int vis[1005];
void clear()
{
	int i = 1;
	while (i <= n)
	{
		vis[i] = 0;
		i = i + 1;
	}
}
int same(int x, int tar)
{
	vis[x] = 1;
	if (x == tar)
		return 1;
	int i = head[x];
	while (i != -1)
	{
		int y = to[i];
		if (!vis[y] && same(y, tar))
			return 1;
		i = next[i];
	}
	return 0;
}
int main()
{
	n = quick_read();
	m = quick_read();
	init();
	while (m)
	{
		int ch = getch();
		while (ch != 81 && ch != 85)
		{
			ch = getch();
		}
		if (ch == 81)
		{ // query
			int x = quick_read(), y = quick_read();
			clear();
			putint(same(x, y));
			putch(10);
		}
		else
		{ // union
			int x = quick_read(), y = quick_read();
			add_edge(x, y);
		}
		m = m - 1;
	}
	return 0;
}
