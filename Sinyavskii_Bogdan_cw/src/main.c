#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEXT_LENGTH 10000
#define MAX_LINE_LENGTH 1000
#define SUCCESS 0
#define ERROR_NULL_POINTER -1
#define ERROR_BUFFER_OVERFLOW -2
#define ERROR_INVALID_FORMAT -3
#define ERROR_INVALID_DATE -4

int count_substring(const char* substr, const char* str, int* result) {
    if (!substr || !str || !result) {
        return ERROR_NULL_POINTER;
    }
    
    int substr_len = strlen(substr);
    int str_len = strlen(str);
    
    if (substr_len == 0 || str_len == 0 || substr_len > str_len) {
        *result = 0;
        return SUCCESS;
    }
    
    *result = 0;
    for (int i = 0; i <= str_len - substr_len; i++) {
        int j;
        for (j = 0; j < substr_len; j++) {
            if (str[i + j] != substr[j]) {
                break;
            }
        }
        if (j == substr_len) {
            (*result)++;
        }
    }
    
    return SUCCESS;
}

int remove_extra_spaces(char* str, size_t size) {
    if (!str) {
        return ERROR_NULL_POINTER;
    }
    
    if (size == 0) {
        return ERROR_BUFFER_OVERFLOW;
    }
    
    int start = 0;
    while (str[start] == ' ') {
        start++;
    }
    
    if (str[start] == '\0') {
        str[0] = '\0';
        return SUCCESS;
    }
    
    int read = start;
    int write = 0;
    int last_non_space = -1;
    
    while (str[read] != '\0' && write < (int)size - 1) {
        if (str[read] != ' ') {
            str[write++] = str[read];
            last_non_space = write - 1;
        } else if (write > 0 && str[write-1] != ' ') {
            str[write++] = ' ';
        }
        read++;
    }
    
    if (write >= (int)size) {
        return ERROR_BUFFER_OVERFLOW;
    }
    
    if (last_non_space >= 0) {
        str[last_non_space + 1] = '\0';
    } else {
        str[0] = '\0';
    }
    
    return SUCCESS;
}

int remove_digits(char* str, size_t size) {
    if (!str) {
        return ERROR_NULL_POINTER;
    }
    
    if (size == 0) {
        return ERROR_BUFFER_OVERFLOW;
    }
    
    int read = 0;
    int write = 0;
    
    while (str[read] != '\0' && write < (int)size - 1) {
        if (str[read] < '0' || str[read] > '9') {
            str[write++] = str[read];
        }
        read++;
    }
    
    if (write >= (int)size) {
        return ERROR_BUFFER_OVERFLOW;
    }
    
    str[write] = '\0';
    return SUCCESS;
}

int days_to_nearest_leap_year(const char* date_str, int* result) {
    if (!date_str || !result) {
        return ERROR_NULL_POINTER;
    }
    
    int day, year;
    char month[4];
    
    if (sscanf(date_str, "%d %3s %d", &day, month, &year) != 3) {
        return ERROR_INVALID_FORMAT;
    }
    
    if (day < 1 || day > 31) {
        return ERROR_INVALID_DATE;
    }
    
    if (year < 1) {
        return ERROR_INVALID_DATE;
    }
    
    int month_num = month_to_number(month);
    if (month_num == -1) {
        return ERROR_INVALID_DATE;
    }
    
    if (day > days_in_month(month_num, year)) {
        return ERROR_INVALID_DATE;
    }
    
    int nearest_leap = find_nearest_leap_year(year);
    *result = abs(days_between_dates(day, month_num, year, 1, 1, nearest_leap));
    
    return SUCCESS;
}

int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month(int month, int year) {
    int days[] = {31, 28 + is_leap_year(year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return days[month - 1];
}

int month_to_number(const char* month) {
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; i++) {
        if (strncmp(month, months[i], 3) == 0) {
            return i + 1;
        }
    }
    return -1;
}

int days_between_dates(int day1, int month1, int year1, int day2, int month2, int year2) {
    int days = 0;
    
    while (year1 < year2) {
        days += is_leap_year(year1) ? 366 : 365;
        year1++;
    }
    while (year1 > year2) {
        year1--;
        days -= is_leap_year(year1) ? 366 : 365;
    }
    
    if (month1 < month2) {
        for (int m = month1; m < month2; m++) {
            days += days_in_month(m, year1);
        }
    } else if (month1 > month2) {
        for (int m = month2; m < month1; m++) {
            days -= days_in_month(m, year1);
        }
    }
    
    days += day2 - day1;
    
    return days;
}

int find_nearest_leap_year(int year) {
    int next = year;
    int prev = year;
    
    while (!is_leap_year(next)) {
        next++;
    }
    
    while (!is_leap_year(prev) && prev > 0) {
        prev--;
    }
    
    if (prev <= 0) {
        return next;
    }
    
    return (next - year < year - prev) ? next : prev;
}

void print_help(void) {
    printf("Available commands:\n");
    printf("0 - Display text after initial processing\n");
    printf("1 - Count substring occurrences in text\n");
    printf("2 - Calculate days to nearest leap year\n");
    printf("3 - Remove extra spaces from text\n");
    printf("4 - Remove all digits from text\n");
    printf("5 - Show this help message\n");
}

char* read_text(void) {
    char* text = malloc(MAX_TEXT_LENGTH * sizeof(char));
    char line[MAX_LINE_LENGTH];
    int pos = 0;
    int consecutive_newlines = 0;
    
    if (text == NULL) {
        return NULL;
    }
    
    text[0] = '\0';
    
    while (fgets(line, MAX_LINE_LENGTH, stdin) != NULL) {
        if (line[0] == '\n') {
            consecutive_newlines++;
        } else {
            consecutive_newlines = 0;
        }
        
        if (consecutive_newlines == 2) {
            break;
        }
        
        if (pos + strlen(line) >= MAX_TEXT_LENGTH) {
            free(text);
            return NULL;
        }
        
        strcat(text, line);
        pos += strlen(line);
    }
    
    return text;
}

int main(void) {
    printf("Course work for option 3.3, created by Bogdan Sinyavskii.\n");
    
    int command;
    if (scanf("%d", &command) != 1) {
        printf("Error: Invalid command\n");
        return 1;
    }
    getchar();
    
    char* text;
    int result;
    int error_code;
    
    switch (command) {
        case 0:
            text = read_text();
            if (!text) {
                printf("Error: Failed to allocate memory\n");
                return 1;
            }
            printf("%s", text);
            free(text);
            break;
            
        case 1:
            {
                char substr[MAX_LINE_LENGTH];
                text = read_text();
                if (!text) {
                    printf("Error: Failed to allocate memory\n");
                    return 1;
                }
                
                if (fgets(substr, MAX_LINE_LENGTH, stdin) == NULL) {
                    printf("Error: Failed to read substring\n");
                    free(text);
                    return 1;
                }
                substr[strcspn(substr, "\n")] = 0;
                
                error_code = count_substring(substr, text, &result);
                if (error_code == SUCCESS) {
                    printf("%d\n", result);
                } else if (error_code == ERROR_NULL_POINTER) {
                    printf("Error: Invalid input\n");
                }
                free(text);
            }
            break;
            
        case 2:
            text = read_text();
            if (!text) {
                printf("Error: Failed to allocate memory\n");
                return 1;
            }
            
            error_code = days_to_nearest_leap_year(text, &result);
            if (error_code == SUCCESS) {
                printf("%d\n", result);
            } else {
                switch (error_code) {
                    case ERROR_INVALID_FORMAT:
                        printf("Error: Invalid date format\n");
                        break;
                    case ERROR_INVALID_DATE:
                        printf("Error: Invalid date\n");
                        break;
                    case ERROR_NULL_POINTER:
                        printf("Error: Invalid input\n");
                        break;
                }
            }
            free(text);
            break;
            
        case 3:
            text = read_text();
            if (!text) {
                printf("Error: Failed to allocate memory\n");
                return 1;
            }
            
            error_code = remove_extra_spaces(text, MAX_TEXT_LENGTH);
            if (error_code == SUCCESS) {
                printf("%s\n", text);
            } else if (error_code == ERROR_NULL_POINTER) {
                printf("Error: Invalid input\n");
            } else if (error_code == ERROR_BUFFER_OVERFLOW) {
                printf("Error: Text too long\n");
            }
            free(text);
            break;
            
        case 4:
            text = read_text();
            if (!text) {
                printf("Error: Failed to allocate memory\n");
                return 1;
            }
            
            error_code = remove_digits(text, MAX_TEXT_LENGTH);
            if (error_code == SUCCESS) {
                printf("%s\n", text);
            } else if (error_code == ERROR_NULL_POINTER) {
                printf("Error: Invalid input\n");
            } else if (error_code == ERROR_BUFFER_OVERFLOW) {
                printf("Error: Text too long\n");
            }
            free(text);
            break;
            
        case 5:
            print_help();
            break;
            
        default:
            printf("Error: Invalid command\n");
            return 1;
    }
    
    return 0;
}
