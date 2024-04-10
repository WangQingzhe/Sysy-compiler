int k;
int a[10];
int cal(int m){
    return m * k;
}
int main(){
    k = 2;
    int m = 1;
    a[0] = k;
    int temp = cal(m);
    putint(temp);
    return 0;
}