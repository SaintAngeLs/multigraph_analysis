#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int debug_mode;
    const char *input_file;
} Config;

Config* get_config();

#endif
