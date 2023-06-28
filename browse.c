#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

char *url_decode(const char *url_str);
void escape_html(char *str);
int cmp(const void *a, const void *b);
void read_dir(char *path);
void read_file(char *filename);
bool is_text_file(const char *filename);
bool is_png(const char *filename);
bool is_jpg(const char *filename);
bool is_mp3(const char *filename);
bool is_mp4(const char *filename);
bool is_pdf(const char *filename);

// 解码URL字符串
char *url_decode(const char *url_str)
{
    // 计算原始字符串的长度
    size_t len = strlen(url_str);
    // 为解码后的字符串分配空间
    char *decoded_str = (char *)malloc(len + 1);
    if (decoded_str == NULL)
    {
        return NULL;
    }

    int i, j;
    for (i = 0, j = 0; i < len; i++, j++)
    {
        // 如果遇到%符号，则将其后面的两个字符转换成一个16进制数字，并将其对应的ASCII码添加到解码后的字符串中
        if (url_str[i] == '%')
        {
            if (isxdigit(url_str[i + 1]) && isxdigit(url_str[i + 2]))
            {
                // 提取出16进制数字
                char hex_str[3] = {url_str[i + 1], url_str[i + 2], '\0'};
                // 将16进制数字转换成整数，并将其对应的ASCII码添加到解码后的字符串中
                decoded_str[j] = strtol(hex_str, NULL, 16);
                // 指针向后移动2位，跳过已经解码的两个字符
                i += 2; 
            }
            else
            {
                // 如果%后面不是两个有效的16进制数字，则将%符号直接添加到解码后的字符串中
                decoded_str[j] = '%';
            }
        }
        else if (url_str[i] == '+')
        {
            // 如果遇到+号，则将其替换成空格
            decoded_str[j] = ' ';
        }
        else
        {
            // 如果既不是%符号也不是+号，则直接将该字符添加到解码后的字符串中
            decoded_str[j] = url_str[i];
        }
    }

    decoded_str[j] = '\0';
    return decoded_str;
}

// 转义HTML中的特殊字符并输出
void escape_html(char *str)
{
    char *p = str;
    while (*p)
    {
        switch (*p)
        {
        case '<':
            printf("&lt;");
            break;
        case '>':
            printf("&gt;");
            break;
        case '&':
            printf("&amp;");
            break;
        default:
            putchar(*p);
            break;
        }
        p++;
    }
}

// 用于比较文件名的函数
int cmp(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

// 读取文件夹中的内容并输出HTML页面
void read_dir(char *path)
{
    // 打开文件夹
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path)) != NULL)
    {
        // 读取文件夹中的所有文件
        char **filenames = NULL;
        int count = 0;
        while ((ent = readdir(dir)) != NULL)
        {
            // 分配空间并保存文件名
            filenames = realloc(filenames, sizeof(char *) * (count + 1));
            filenames[count] = malloc(strlen(ent->d_name) + 1);
            strcpy(filenames[count], ent->d_name);
            count++;
        }
        closedir(dir);

        // 排序文件名
        qsort(filenames, count, sizeof(char *), cmp);

        int i;
        // 输出排序后的文件名
        for (i = 0; i < count; i++)
        {
            // 开始输出表格行
            printf("<tr><td>");
            // 文件的超链接
            char link[strlen(path) + strlen(filenames[i]) * 2 + 50];

            // 如果路径为“.”，则链接至当前路径
            if (strcmp(filenames[i], ".") == 0)
            {
                sprintf(link, "<a href=\"browse.cgi?path=%s\">.</a>", path);
            }
            // 如果路径为".."，则链接至上一级路径
            else if (strcmp(filenames[i], "..") == 0)
            {
                char *s = strchr(path, '/');
                char *p = strrchr(path, '/');
                // 如果在路径中只有一个"/"，则链接至根目录
                if (s == p)
                {
                    sprintf(link, "<a href=\"browse.cgi?path=/\">%s</a>", filenames[i]);
                }
                // 否则链接至上一级路径
                else
                {
                    *p = '\0';
                    sprintf(link, "<a href=\"browse.cgi?path=%s\">%s</a>", path, filenames[i]);
                    // 恢复路径
                    *p = '/';
                }
            }
            else
            {
                // 如果文件夹路径以"/"结尾，则不需要在超链接中添加"/"
                if (path[strlen(path) - 1] == '/')
                    sprintf(link, "<a href=\"browse.cgi?path=%s%s\">%s</a>", path, filenames[i], filenames[i]);
                else
                    sprintf(link, "<a href=\"browse.cgi?path=%s/%s\">%s</a>", path, filenames[i], filenames[i]);
            }
            printf("%s</td>", link);

            // 获取文件信息
            struct stat st;
            // 文件完整路径，+2是为了加上路径分隔符和终止符
            char filepath[strlen(path) + strlen(filenames[i]) + 2];
            sprintf(filepath, "%s/%s", path, filenames[i]);
            stat(filepath, &st);

            // 文件大小
            char size_str[16];
            // 如果是普通文件，计算文件大小并判断文件类型
            if (S_ISREG(st.st_mode))
            {
                if (st.st_size >= (1 << 30))
                {
                    sprintf(size_str, "%.2f GB", st.st_size / (float)(1 << 30));
                }
                else if (st.st_size >= (1 << 20))
                {
                    sprintf(size_str, "%.2f MB", st.st_size / (float)(1 << 20));
                }
                else if (st.st_size >= (1 << 10))
                {
                    sprintf(size_str, "%.2f KB", st.st_size / (float)(1 << 10));
                }
                else
                {
                    sprintf(size_str, "%ld Bytes", st.st_size);
                }

                if (is_text_file(filepath))
                    printf("<td>%s</td><td>Text</td></tr>", size_str);
                else if (is_png(filepath) || is_jpg(filepath))
                    printf("<td>%s</td><td>Image</td></tr>", size_str);
                else if (is_mp4(filepath))
                    printf("<td>%s</td><td>Video</td></tr>", size_str);
                else if (is_mp3(filepath))
                    printf("<td>%s</td><td>Audio</td></tr>", size_str);
                else
                    printf("<td>%s</td><td>File</td></tr>", size_str);
            }
            else
            { 
                sprintf(size_str, "-");
                printf("<td>%s</td><td>Dir</td></tr>", size_str);
            }

            free(filenames[i]);
        }
        free(filenames);
    }
    else
    {
        // 如果无法打开文件夹，则返回错误消息
        printf("<html><body><h2>Error: Unable to open directory</h2></body></html>\n");
    }
}

// 判断文件是否为文本文件
bool is_text_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        return false;
    }

    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    size_t i;
    for (i = 0; i < bytes_read; i++)
    {
        // 如果文件中有不可见字符，则判定为二进制文件
        if (buffer[i] < 9 || (buffer[i] > 13 && buffer[i] < 32) || buffer[i] == 127)
        {
            return false; 
        }
    }
    // 否则判定为文本文件
    return true; 
}

// 判断是否为PNG图片
bool is_png(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        return false;
    }

    unsigned char buffer[8];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    // 文件不足8个字节，不可能是PNG格式
    if (bytes_read != 8)
    {
        return false; 
    }

    // 文件头不匹配，不是PNG格式
    if (buffer[0] != 0x89 || buffer[1] != 'P' || buffer[2] != 'N' || buffer[3] != 'G' ||
        buffer[4] != 0x0D || buffer[5] != 0x0A || buffer[6] != 0x1A || buffer[7] != 0x0A)
    {
        return false; 
    }

    return true;
}

// 判断文件是否为JPG图片
bool is_jpg(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        return false;
    }

    unsigned char buffer[2];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    // 文件头不匹配，不是JPG格式
    if (bytes_read != 2 || buffer[0] != 0xFF || buffer[1] != 0xD8)
    {
        return false; 
    }

    return true;
}

// 判断文件是否为MP3文件
bool is_mp3(const char *filename)
{
    // MP3文件头
    const char *mp3_header = "ID3";

    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        return false;
    }

    char buffer[4];
    // 读取文件头
    fread(buffer, 1, 3, fp); 
    buffer[3] = '\0';
    fclose(fp);

    // 文件头匹配，则为MP3文件
    if (strcmp(buffer, mp3_header) == 0)
        return true;
    else
        return false;
}

// 判断文件是否为MP4文件
bool is_mp4(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        return false;
    }

    char buffer[12];
    size_t bytes_read = fread(buffer, 1, 12, fp);
    fclose(fp);

    if (bytes_read != 12)
    {
        return false;
    }
    // 用十六进制会将后面的字母一同当作转义字符，所以使用八进制转义
    if (memcmp(buffer, "\0\0\0\030ftypisom", 12) == 0 ||
        memcmp(buffer, "\0\0\0\030ftypmp42", 12) == 0 ||
        memcmp(buffer, "\0\0\0\030ftypMSNV", 12) == 0)
    {
        return true;
    }
    return false;
}

// 判断文件是否为PDF文件
bool is_pdf(const char *filename)
{
    // PDF文件头
    const char *pdf_header = "\x25\x50\x44\x46";

    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        return false;
    }

    char buffer[5];
    // 读取文件头
    fread(buffer, 1, 4, fp);
    buffer[4] = '\0';
    fclose(fp);

    // 文件头匹配，则为PDF文件
    if (strcmp(buffer, pdf_header) == 0)
        return true;
    else
        return false;
}

// 读取文件内容并输出HTML页面
void read_file(char *file_path)
{
    if (is_text_file(file_path))
    {
        // 打开文件
        FILE *fp = fopen(file_path, "r");
        if (fp == NULL)
        {
            printf("<html><body><h2>Error: Unable to open file %s</h2></body></html>\n", file_path);
            return;
        }

        // 获取文件大小
        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        // 将文件指针移回文件开头
        fseek(fp, 0, SEEK_SET);

        // 分配缓冲区
        char *buffer = (char *)malloc(file_size + 1);
        if (buffer == NULL)
        {
            printf("<html><body><h2>Error: Unable to allocate memory</h2></body></html>\n");
            fclose(fp);
            return;
        }

        // 读取文件内容到缓冲区
        fread(buffer, 1, file_size, fp);
        buffer[file_size] = '\0';

        // 输出HTML表单
        printf("<html>\n");
        printf("<head><title>File Editor</title></head>\n");
        printf("<body>\n");
        // 用save_file.cgi保存文件
        printf("<form method=\"POST\" action=\"save_file.cgi\">\n");
        printf("<textarea name=\"file_content\" cols=\"80\" rows=\"20\">");
        // 对文件内容进行转义
        escape_html(buffer);
        printf("</textarea> <br>\n");
        printf("<input type=\"hidden\" name=\"file_path\" value=\"%s\">\n", file_path);
        printf("<input type=\"submit\" value=\"Save\">\n");
        printf("<input type=\"button\" value=\"Cancel\" onclick=\"window.history.back();\">\n");
        printf("</form>\n");
        printf("</body></html>\n");

        // 释放缓冲区和文件句柄
        free(buffer);
        fclose(fp);
    }
    else if (is_jpg(file_path) || is_png(file_path))
    {
        printf("<img src=\"root%s\" alt=\"Image\" >\n", file_path);
        printf("</body></html>\n");
    }
    else if (is_mp3(file_path))
    {
        printf("<audio src=\"root%s\" controls=\"controls\">\n", file_path);
        printf("</body></html>\n");
    }
    else if (is_mp4(file_path))
    {
        printf("<video src=\"root%s\" controls=\"controls\">\n", file_path);
        printf("</body></html>\n");
    }
    else if (is_pdf(file_path))
    {
        printf("<iframe src=\"root%s\" width=\"100%%\" height=\"100%%\">\n", file_path);
        printf("</body></html>\n");
    }
    else
    {
        printf("<html><body><h2>Error: Unsupported file format: %s</h2></body></html>\n", file_path);
    }
}

int main()
{
    // 获取路径参数
    char *url_path = url_decode(getenv("QUERY_STRING"));
    char *path_start = strstr(url_path, "path=");
    if (path_start != NULL)
    {
        url_path += (path_start - url_path + strlen("path="));
    }
    else
    {
        strcpy(url_path, "/");
    }

    // 开始输出HTML页面
    printf("Content-type:text/html\n\n");
    printf("<html><body><h2>%s</h2>\n", url_path);

    // 获取路径数据
    struct stat path_stat;
    if (stat(url_path, &path_stat) != 0)
    {
        printf("<html><body><h2>Error: Unable to access %s</h2></body></html>\n", url_path);
        return;
    }

    // 判断是文件还是文件夹并进行相应处理
    if (S_ISREG(path_stat.st_mode))
    {
        read_file(url_path);
    }
    else if (S_ISDIR(path_stat.st_mode))
    {
        // 如果是文件夹，则输出文件列表
        printf("<table board=\"8\"><tr><th>Name</th><th>Size</th><th>Type</th></tr>");
        read_dir(url_path);
    }
    else
    {
        printf("<html><body><h2>Error: %s is not a file or directory</h2></body></html>\n", url_path);
    }

    return 0;
}