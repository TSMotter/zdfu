#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include <zephyr/stats/stats.h>
#include <zephyr/mgmt/mcumgr/grp/stat_mgmt/stat_mgmt.h>

/***************************************************************************************************
 * LOGs
 ***************************************************************************************************/
#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(zdfu);

/***************************************************************************************************
 * SMP Server Statistics Management
 ***************************************************************************************************/
// Define an example stats group
STATS_SECT_START(smp_server_stats)
STATS_SECT_ENTRY(counter_stat)
STATS_SECT_END;

// Assign a name to the `counter_stat` stat.
STATS_NAME_START(smp_server_stats)
STATS_NAME(smp_server_stats, counter_stat)
STATS_NAME_END(smp_server_stats);

// Define an instance of the stats group.
STATS_SECT_DECL(smp_server_stats) smp_server_stats;

/***************************************************************************************************
 * LED definition and initialization
 ***************************************************************************************************/
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led0_dt_spec = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0});

/***************************************************************************************************
 * Choose to build one of the 2 binaries
 ***************************************************************************************************/
// #define Binary_One
#define Binary_Two


#ifdef Binary_One

#define BLINK_PERIOD 1000

#elif defined(Binary_Two)

#define BLINK_PERIOD 100

#endif


int main(void)
{
    int rc = 0;

    rc = STATS_INIT_AND_REG(smp_server_stats, STATS_SIZE_32, "smp_server_stats");
    if (rc < 0)
    {
        LOG_ERR("Error initializing stats system [%d]", rc);
        return 0;
    }

    rc = gpio_is_ready_dt(&led0_dt_spec);
    if (rc < 0)
    {
        LOG_ERR("Error initializing led0 [%d]", rc);
        return 0;
    }

    rc = gpio_pin_configure_dt(&led0_dt_spec, GPIO_OUTPUT_ACTIVE);
    if (rc < 0)
    {
        LOG_ERR("Error configuring led0 pin [%d]", rc);
        return 0;
    }

    while (1)
    {
        rc = gpio_pin_toggle_dt(&led0_dt_spec);
        if (rc < 0)
        {
            LOG_ERR("Error toggling led0 [%d]", rc);
            return 0;
        }
        k_msleep(BLINK_PERIOD);
        STATS_INC(smp_server_stats, counter_stat);
    }
    return 0;
}
