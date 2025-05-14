#include <stdio.h>

int main(){
    printf("Hello word!\n");
    printf("Enter flag:");
    fflush(stdout);
    char s[256];
    fgets(s, sizeof(s), stdin);
    printf("%s", s);
    return 0;
}
