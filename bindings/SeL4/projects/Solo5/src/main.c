#include <solo5.h>
#include <string.h>

#define NSEC_PER_SEC	1000000000L

static void puts_(const char *s)
{
    solo5_console_write(s, strlen(s));
}


int solo5_app_main(const struct solo5_start_info *si __attribute__((unused)))
{
    puts_("\n**** Solo5 standalone test_time ****\n\n");

    /*
     * Verify that monotonic time is passing
     */
    solo5_time_t ta = 0, tb = 0;
    int iters = 5;
    ta = solo5_clock_monotonic();
    tb = solo5_clock_monotonic();
    /*
     * Adjacent calls to solo5_clock_monotonic() may return the same time count
     * on aarch64 systems, so give it a few extra iterations to change.
     */
    while (ta == tb && iters--)
        tb = solo5_clock_monotonic();
    if (!(tb > ta)) {
        puts_("ERROR: time is not passing\n");
        return SOLO5_EXIT_FAILURE;
    }

    /* 
     * The tender is configured with no I/O modules for this test so
     * solo5_yield() is equivalent to a sleep here.
     */
    ta = solo5_clock_monotonic();
    solo5_yield(ta + NSEC_PER_SEC);
    tb = solo5_clock_monotonic();
    /*
     * Verify that we did not sleep less than requested (see above).
     */
    if ((tb - ta) < NSEC_PER_SEC) {
        puts_("ERROR: slept too little\n");
        return SOLO5_EXIT_FAILURE;
    }
    /*
     * Verify that we did not sleep more than requested, within reason
     * (scheduling delays, general inaccuracy of the current timing code).
     */
    if ((tb - ta) > (NSEC_PER_SEC + 100000000ULL)) {
        puts_("ERROR: slept too much\n");
        return SOLO5_EXIT_FAILURE;
    }

    /*
     * Verify that wall time is 2017 or later
     */
    ta = solo5_clock_wall();
    if (ta < (1483228800ULL * NSEC_PER_SEC)) {
        puts_("ERROR: wall time is not 2017 or later\n");
        return SOLO5_EXIT_FAILURE;
    }

    puts_("SUCCESS\n");
    return SOLO5_EXIT_SUCCESS;

}
