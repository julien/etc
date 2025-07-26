#+feature dynamic-literals
package clock

import "core:fmt"
import "core:os"
import "core:strings"
import "core:time"
import "core:time/datetime"
import "core:time/timezone"

main :: proc() {
	leds := map[string]string {
		"0" = " _  ,| | ,|_| ",
		"1" = "  ,| ,| ",
		"2" = " _  , _| ,|_  ",
		"3" = "_  ,_| ,_| ",
		"4" = "    ,|_| ,  | ",
		"5" = " _  ,|_  , _| ",
		"6" = " _  ,|_  ,|_| ",
		"7" = "_   , |  , |  ",
		"8" = " _  ,|_| ,|_| ",
		"9" = " _  ,|_| , _| ",
		":" = "    , ·  , .  ",
	}

	region, ok := timezone.region_load("Europe/Paris")
	if !ok {
		fmt.eprintln("failed to load timezone")
		os.exit(1)
	}

	for {
		fmt.printf("\x1b[2J\x1b[H")

		// Get current time in the current tz region and format it in HH:MM:SS
		// (there's probably a better way to do this)
		now := time.now()
		dt, ok := time.time_to_datetime(now)
		if !ok {
			continue
		}
		dt, ok = timezone.datetime_to_tz(dt, region)
		if !ok {
			continue
		}
		now, ok = time.datetime_to_time(dt)
		if !ok {
			continue
		}

		buf: [64]u8
		str := time.time_to_string_hms(now, buf[:])
		digits := strings.split(str, "")
		sb := strings.builder_make()

		for i in 0 ..= 2 {
			for d in digits {
				lines := strings.split(leds[d], ",")
				strings.write_string(&sb, lines[i])
			}
			strings.write_string(&sb, "\n")
		}
		fmt.printf("%s", strings.to_string(sb))
		time.sleep(time.Second)
	}
}
