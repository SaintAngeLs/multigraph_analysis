#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int debug_mode;
    const char *input_file;
    const char *metric_optional_file;
} Config;

Config* get_config();

#endif
