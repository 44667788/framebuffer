//
// Created by zeke on 17-8-16.
//


#include <wchar.h>
#include <string.h>
#include <stdio.h>

const char *get_post_fix(const char *src) {
    const char *res = NULL;
    if (src == NULL || strlen(src) == 0) {
        printf("src is empty");
    } else {
        int len = strlen(src);
        int i = len - 1;
        while (i >= 0) {
            if (src[i] == '.' && i + 1 != len) {
                res = &src[i+1];
                break;
            }
            --i;
        }
    }
    return res;
}
