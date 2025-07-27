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

	// We could pass the "region" via os.args or define a flag.
	region, ok := timezone.region_load("Europe/Madrid")
	if !ok {
		fmt.eprintln("failed to load timezone")
		os.exit(1)
	}

	for {
		fmt.printf("\x1b[2J\x1b[H")

		now, ok := current_time(region)
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

// Returns the current time given a tz region (there's probably a better way to do this).
current_time :: proc(region: ^datetime.TZ_Region) -> (time.Time, bool) {
	now := time.now()
	dt, ok := time.time_to_datetime(now)
	if !ok {
		return {}, false
	}
	dt, ok = timezone.datetime_to_tz(dt, region)
	if !ok {
		return {}, false
	}
	now, ok = time.datetime_to_time(dt)
	if !ok {
		return {}, false
	}
	return now, true
}
