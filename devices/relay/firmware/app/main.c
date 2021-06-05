#include <stdio.h>
#include "hdp.h"
#include "shell.h"
#include "shell_commands.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"

static const hdp_params_t hdp_params = HDP_PARAMS_INIT;
static hdp_ctx_t hdp_ctx;

int main(void)
{
    hdp_init(&hdp_ctx, &hdp_params);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
