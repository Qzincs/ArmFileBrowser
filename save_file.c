#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *url_decode(const char *url_str) {
    // 计算原始字符串的长度
    size_t len = strlen(url_str);
    // 为解码后的字符串分配空间
    char *decoded_str = (char *)malloc(len + 1);
    if (decoded_str == NULL) {
        return NULL;
    }

    int i, j;
    for (i = 0, j = 0; i < len; i++, j++) {
        // 如果遇到%符号，则将其后面的两个字符转换成一个16进制数字，并将其对应的ASCII码添加到解码后的字符串中
        if (url_str[i] == '%') {
            if (isxdigit(url_str[i+1]) && isxdigit(url_str[i+2])) {
                // 提取出16进制数字
                char hex_str[3] = { url_str[i+1], url_str[i+2], '\0' };
                // 将16进制数字转换成整数，并将其对应的ASCII码添加到解码后的字符串中
                decoded_str[j] = strtol(hex_str, NULL, 16);
                i += 2; // 指针向后移动2位，跳过已经解码的两个字符
            } else {
                // 如果%后面不是两个有效的16进制数字，则将%符号直接添加到解码后的字符串中
                decoded_str[j] = '%';
            }
        } else if (url_str[i] == '+') {
            // 如果遇到+号，则将其替换成空格
            decoded_str[j] = ' ';
        } else {
            // 如果既不是%符号也不是+号，则直接将该字符添加到解码后的字符串中
            decoded_str[j] = url_str[i];
        }
    }

    decoded_str[j] = '\0'; // 在解码后的字符串末尾添加NULL字符
    return decoded_str;
}

int main(void) {
    // 从POST请求参数中获取表单长度
    char *content_length_str = getenv("CONTENT_LENGTH");
    // 将字符串转换为整数
    int content_length = atoi(content_length_str);
    // 为缓存区分配内存
    char *buffer = (char*)malloc(content_length + 1);
    // 读取表单内容到缓冲区中
    fgets(buffer, content_length + 1, stdin);

    char *file_content = NULL;
    char *file_path = NULL;
    // 从缓冲区中解析出 file_content 和 file_path 的值
    // %m 表示动态分配字符串内存，[^&] 表示匹配除了 & 外的任意字符
    sscanf(buffer, "file_content=%m[^&]&file_path=%m[^&]", &file_content, &file_path);
    // url解码
    file_content = url_decode(file_content);
    file_path = url_decode(file_path);
    
    // 打开指定的文件以进行写入
    FILE* fp = fopen(file_path, "w");
    if (fp == NULL) {
        printf("Content-type:text/html\n\n");
        printf("<html>\n");
        printf("<head><title>Error</title></head>\n");
        printf("<body>\n");
        printf("<h2>Failed to open file for writing.</h2>\n");
        printf("</body>\n");
        printf("</html>\n");
        return 1;
    }

    // 将文件内容写入文件
    fprintf(fp, "%s", file_content);
    fclose(fp);

    // 显示成功保存的消息
    printf("Content-type:text/html\n\n");
    printf("<html>\n");
    printf("<head><title>File Saved</title></head>\n");
    printf("<body>\n");
    printf("<h2>File successfully saved.</h2>\n");
    printf("</body>\n");
    printf("</html>\n");

    free(file_content);
    free(file_path);
    free(buffer);
    return 0;
}
