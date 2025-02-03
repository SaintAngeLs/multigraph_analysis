#include <stdlib.h>
#include "config.h"

static Config config_instance = {0};

Config* get_config() {
    return &config_instance;
}
